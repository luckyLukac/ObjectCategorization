#include <algorithm>
#include <limits>

#include "LineSweeping.hpp"


// Calculation of a bounding box according to point coordinates.
void LineSweeping::calculateBoundingBox() {
	// Initializing coordinates to max and min.
	int xMin = std::numeric_limits<int>::max();
	int yMin = std::numeric_limits<int>::max();
	int xMax = std::numeric_limits<int>::min();
	int yMax = std::numeric_limits<int>::min();

	// Iterating through each point.
	for (const Point& point : coordinates) {
		// If a point X coordinate is smaller than the global
		// minimum, a new global minimum is set.
		if (point.x < xMin) {
			xMin = point.x;
		}

		// If a point X coordinate is larger than the global
		// maximum, a new global maximum is set.
		if (point.x > xMax) {
			xMax = point.x;
		}

		// If a point Y coordinate is smaller than the global
		// minimum, a new global minimum is set.
		if (point.y < yMin) {
			yMin = point.y;
		}

		// If a point Y coordinate is larger than the global
		// maximum, a new global maximum is set.
		if (point.y > yMax) {
			yMax = point.y;
		}
	}

	// Moving the bounding box to the left upper corner.
	std::transform(
		coordinates.begin(),
		coordinates.end(),
		coordinates.begin(),
		[&xMin, &yMin](Point point) {
			return Point(point.x - xMin, point.y - yMin);
		}
	);

	// Calculation of the difference between the largest and smallest coordinates.
	const int deltaX = xMax - xMin;
	const int deltaY = yMax - yMin;

	// Fitting the bounding box (magnified by parameter MAGNIFY_FACTOR).
	int magnifiedPivotCoordinate = 0;
	int magnifiedX = 0;
	int magnifiedY = 0;

	// Finding the magnified coordinates.
	if (deltaX < deltaY) {
		magnifiedPivotCoordinate = static_cast<int>(MAGNIFY_FACTOR * deltaY);
		magnifiedX = static_cast<int>(0.5 * (deltaY - deltaX));
		magnifiedX = magnifiedX + static_cast<int>(0.25 * (magnifiedPivotCoordinate * MAGNIFY_FACTOR - magnifiedPivotCoordinate));
		magnifiedY = static_cast<int>(0.25 * (magnifiedPivotCoordinate * MAGNIFY_FACTOR - magnifiedPivotCoordinate));
	}
	else {
		magnifiedPivotCoordinate = static_cast<int>(MAGNIFY_FACTOR * deltaX);
		magnifiedY = static_cast<int>(0.5 * (deltaX - deltaY));
		magnifiedY = magnifiedY + static_cast<int>(0.25 * (magnifiedPivotCoordinate * MAGNIFY_FACTOR - magnifiedPivotCoordinate));
		magnifiedX = static_cast<int>(0.25 * (magnifiedPivotCoordinate * MAGNIFY_FACTOR - magnifiedPivotCoordinate));
	}

	// Transforming the points according to the magnified coordinates.
	std::transform(
		coordinates.begin(),
		coordinates.end(),
		coordinates.begin(),
		[&magnifiedX, &magnifiedY](Point point) {
			return Point(point.x + magnifiedX, point.y + magnifiedY);
		}
	);

	// Setting the max coordinates.
	maxX = magnifiedPivotCoordinate + 1;
	maxY = magnifiedPivotCoordinate + 1;
}

// Filling a rectangle at X and Y coordinates.
void LineSweeping::fillRectangle(wxDC& dc, const int x, const int y, const int pixelSize, const wxPen& pen, const wxBrush& brush) {
	// Creating a wxPoint for rendering.
	wxPoint P[4] = {
		wxPoint(x, y),
		wxPoint(x + pixelSize, y),
		wxPoint(x + pixelSize, y + pixelSize),
		wxPoint(x, y + pixelSize)
	};

	// Drawing the point.
	dc.SetPen(pen);
	dc.SetBrush(brush);
	dc.DrawPolygon(4, P);

	// Resetting the pen and the brush.
	dc.SetPen(*wxBLACK_PEN);
	dc.SetBrush(*wxWHITE_BRUSH);
}




// Setting F4 chain code.
void LineSweeping::setF4(const std::vector<std::byte>& chainCode) {
	// Adding the first point to coordinates.
	Point point;
	coordinates.push_back(point);

	// Current X and Y coordinates.
	int currentX = 0;
	int currentY = 0;

	// Adding new coordinates according to the chain code.
	for (const std::byte& chainElement : chainCode) {
		const int direction = to_integer<int>(chainElement) - 48;  // Casting the byte to char.

		// 0 means right.
		if (direction == 0) {
			currentX++;
		}
		// 1 means up.
		else if (direction == 1) {
			currentY++;
		}
		// 2 means left.
		else if (direction == 2) {
			currentX--;
		}
		// 3 means down.
		else if (direction == 3) {
			currentY--;
		}

		// Adding a new coordinate to the vector.
		point.x = currentX;
		point.y = currentY;
		point.segmentID = -1;
		coordinates.push_back(point);
	}

	// Calculation of the bounding box.
	calculateBoundingBox();
}

// Plotting the F4 chain code as an image.
void LineSweeping::plotInput(wxDC& dc) {
	// Setting the color to green.
	const wxPen pen(*wxGREEN_PEN);
	const wxBrush brush(*wxGREEN_BRUSH);

	// Plotting each point as a pixel.
	for (const Point& point : coordinates) {
		fillRectangle(dc, point.x, point.y, 1, pen, brush);
	}
}

// Plotting the object bounding box.
void LineSweeping::plotBoundingBox(wxDC& dc) {
	// Plotting the four lines of the bounding box.
	dc.DrawLine(0, 0, maxX, 0);
	dc.DrawLine(maxX, 0, maxX, maxY);
	dc.DrawLine(maxX, maxY, 0, maxY);
	dc.DrawLine(0, maxY, 0, 0);
}


/******************************************************************************************************************************/
/******************************************************************************************************************************/
/******************************************************************************************************************************/
/******************************************************************************************************************************/
/******************************************************************************************************************************/
// POINT TYPE
Point::Point() :
	x(0),
	y(0),
	segmentID(-1)
{}

Point::Point(const int x, const int y, const int segmentID) :
	x(x),
	y(y),
	segmentID(segmentID)
{}