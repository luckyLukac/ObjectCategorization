#include <algorithm>
#include <fstream>
#include <limits>
#include <stack>

#include "HelperFunctions.hpp"
#include "LineSweeping.hpp"


// PRIVATE HELPER METHODS
// Transforming chain code to coordinates.
void LineSweeping::calculateCoordinatesFromChainCode() {
	coordinates.clear();  // Clearing the previous coordinates.

	// Current point and X and Y coordinates.
	Pixel point;
	int currentX = 0;
	int currentY = -0;

	// Adding new coordinates according to the chain code.
	for (const std::byte& chainElement : chainCode) {
		const int direction = static_cast<int>(chainElement) - 48;  // Casting the byte to char.

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
}

// Calculation of a bounding box according to point coordinates.
void LineSweeping::calculateBoundingBox() {
	// Initializing coordinates to max and min.
	int xMin = std::numeric_limits<int>::max();
	int yMin = std::numeric_limits<int>::max();
	int xMax = std::numeric_limits<int>::min();
	int yMax = std::numeric_limits<int>::min();
	unsigned int yMaxIndex = 0;

	// Iterating through each point.
	for (unsigned int i = 0; i < coordinates.size(); i++) {
		// If a point X coordinate is smaller than the global
		// minimum, a new global minimum is set.
		if (coordinates[i].x < xMin) {
			xMin = coordinates[i].x;
		}

		// If a point X coordinate is larger than the global
		// maximum, a new global maximum is set.
		if (coordinates[i].x > xMax) {
			xMax = coordinates[i].x;
		}

		// If a point Y coordinate is smaller than the global
		// minimum, a new global minimum is set.
		if (coordinates[i].y < yMin) {
			yMin = coordinates[i].y;
		}

		// If a point Y coordinate is larger than the global
		// maximum, a new global maximum is set.
		if (coordinates[i].y > yMax) {
			yMax = coordinates[i].y;
			yMaxIndex = i;
		}
	}

	// Checking whether the chain code is anti-clockwise.
	const char topYChainCode = static_cast<char>(chainCode[yMaxIndex]);
	const char nextTopYChainCode = static_cast<char>(chainCode[(yMaxIndex + 1) % chainCode.size()]);
	const bool isAntiClockwise = !(topYChainCode == '0' && nextTopYChainCode == '3') && !(topYChainCode == '0' && nextTopYChainCode == '0') && !(topYChainCode == '1' && nextTopYChainCode == '0');
	
	// As we programmers do not tend to overcomplicate our lives, the chain
	// code is reversed if it is written in anti-clockwise orientation.
	if (isAntiClockwise) {
		// In order to reverse a chain code, the vector must be reversed along with the chain code instructions.
		std::reverse(chainCode.begin(), chainCode.end());
		std::transform(
			chainCode.begin(),
			chainCode.end(),
			chainCode.begin(),
			[](const std::byte& element) {
				const int ch = static_cast<int>(element) - 48;
				return static_cast<std::byte>(((ch + 2) % 4) + 48);
			}
		);

		// Calculation of new coordinates and the bounding box.
		calculateCoordinatesFromChainCode();
		calculateBoundingBox();
		return;
	}

	// Moving the bounding box to the left upper corner.
	std::transform(
		coordinates.begin(),
		coordinates.end(),
		coordinates.begin(),
		[&xMin, &yMin](Pixel point) {
			return Pixel(point.x - xMin, point.y - yMin);
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
		[&magnifiedX, &magnifiedY](Pixel point) {
			return Pixel(point.x + magnifiedX, point.y + magnifiedY);
		}
	);

	// Setting the max coordinates.
	this->maxCoordinate = magnifiedPivotCoordinate + 1;

	// Creating the pixel field.
	pixelField = std::vector<std::vector<Position>>(this->maxCoordinate, std::vector<Position>(this->maxCoordinate, Position::undefined));
}

// Filling a rectangle at X and Y coordinates.
void LineSweeping::fillRectangle(wxDC& dc, const int x, const int y, const int pixelSize, const wxPen& pen, const wxBrush& brush) const {
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

// Finding edge pixel pairs.
std::vector<Pixel> LineSweeping::findEdgePixels(const std::vector<Pixel>& rasterizedLine) const {
	std::vector<Pixel> pixels;

	for (unsigned int i = 0; i < rasterizedLine.size(); i++) {
		Pixel pixel = rasterizedLine[i];

		// If the pixel is outside of the bounding box, it is not added to the vector.
		if (pixel.x < 0 || pixel.x >= maxCoordinate || pixel.y < 0 || pixel.y >= maxCoordinate) {
			continue;
		}

		if (pixelField[pixel.y][pixel.x] == Position::edge) {
			pixels.emplace_back(pixel);
		}
	}

	return pixels;
}

// Adding the current midpoint.
void LineSweeping::addCurrentMidpointFromSweepLine(const std::vector<Pixel>& rasterizedLine) {
	// Plotting the rasterized line.
	wxClientDC dc(drawWindow);
	plotBresenhamLine(dc, rasterizedLine);

	// Finding the edge pixels that lie on the current rasterized line.
	std::vector<Pixel> edgePixel = findEdgePixels(rasterizedLine);

	// Finding the middle points between the edge pixels.
	for (unsigned int i = 1; i < edgePixel.size(); i++) {
		// Calculating middle X and Y coordinates.
		const int x = static_cast<int>((edgePixel[i - 1].x + edgePixel[i].x) / 2);
		const int y = static_cast<int>((edgePixel[i - 1].y + edgePixel[i].y) / 2);
		
		// If the center point lies inside of the object, a red rectangle is drawn there.
		if (pixelField[y][x] == Position::inside) {
			fillRectangle(dc, x, maxCoordinate - y, 1, *wxRED_PEN, *wxRED_BRUSH);
		}
	}
}


// PLOT METHODS
// Plotting the F4 chain code as an image.
void LineSweeping::plotInput(wxDC& dc) const {
	// Setting the color to green.
	const wxPen pen(*wxGREEN_PEN);
	const wxBrush brush(*wxGREEN_BRUSH);

	// Plotting each point as a pixel.
	for (const Pixel& point : coordinates) {
		fillRectangle(dc, point.x, -point.y + maxCoordinate, 1, pen, brush);
	}
}

// Plotting the object bounding box.
void LineSweeping::plotBoundingBox(wxDC& dc) const {
	// Plotting the four lines of the bounding box.
	dc.DrawLine(0, 0, maxCoordinate, 0);
	dc.DrawLine(maxCoordinate, 0, maxCoordinate, maxCoordinate);
	dc.DrawLine(maxCoordinate, maxCoordinate, 0, maxCoordinate);
	dc.DrawLine(0, maxCoordinate, 0, 0);
}

// Plotting the Bresenham line.
void LineSweeping::plotBresenhamLine(wxDC& dc, const std::vector<Pixel>& rasterizedLine) const {
	// Plotting each pixel of the line.
	for (const Pixel& pixel : rasterizedLine) {
		fillRectangle(dc, pixel.x, -pixel.y + maxCoordinate, 1, *wxLIGHT_GREY_PEN, *wxLIGHT_GREY_BRUSH);
	}
}



// GETTERS AND SETTERS
// Setting F4 chain code.
void LineSweeping::setF4(const std::vector<std::byte>& chainCode) {
	this->chainCode.clear();  // Clearing the previous chain code.
	coordinates.clear();      // Clearing the previous coordinates.

	// Adding the chain code to the object.
	this->chainCode = chainCode;

	// Calculation of coordinates and the bounding box.
	calculateCoordinatesFromChainCode();
	calculateBoundingBox();
}

// Setting draw panel.
void LineSweeping::setDrawPanel(wxWindow* drawWindow) {
	this->drawWindow = drawWindow;
}

// Returning true if a chain code is set.
bool LineSweeping::isChainCodeSet() const {
	return !coordinates.empty();
}

// Clearing previous segments.
void LineSweeping::clearSegments() {
	segments.clear();
}

// Setting the angle of rotation (given in radians).
void LineSweeping::setAngleOfRotation(const double angle) {
	angleOfRotation = angle;
}



// PUBLIC METHODS
// Reading a file.
std::vector<std::byte> LineSweeping::readFile(std::string file) const {
	std::vector<std::byte> chainCode;

	// Opening a file.
	std::ifstream in(file);

	// If a file is not open, we do not panic but abort the process.
	if (!in.is_open()) {
		return chainCode;
	}

	// Reading a file character by character.
	char c;
	while (in.read(&c, 1)) {
		// If a value is 0, 1, 2 or 3, the value is added to the chain code vector.
		if (c == '0' || c == '1' || c == '2' || c == '3') {
			chainCode.push_back(static_cast<std::byte>(c));
		}
	}

	return chainCode;
}

// Filling the loaded shape.
void LineSweeping::fillShape() {
	// Creating two stacks for filling the shape.
	std::stack<unsigned int> leftStack;
	std::stack<unsigned int> rightStack;

	// Iterating through the chain code and filling the shape.
	for (unsigned int i = 0; i < chainCode.size(); i++) {
		// Getting the chain code elemenz and the X and Y coordinates of the pixel.
		const char code = static_cast<char>(chainCode[i]);
		const unsigned int x = coordinates[i].x;
		const unsigned int y = coordinates[i].y;

		// Setting the current pixel to edge.
		pixelField[y][x] = Position::edge;

		// If the instruction is to go up, the left stack is pushed to if the right stack is empty.
		if (code == '1') {
			if (!rightStack.empty()) {
				// Getting the top element from the right stack.
				const unsigned int right = rightStack.top();
				rightStack.pop();

				// Setting undefined pixels to inside if left and right pixel coordinates are not flipped (left < right).
				if (right > x) {
					for (unsigned int pixelX = x + 1; pixelX < right; pixelX++) {
						if (pixelField[y][pixelX] == Position::undefined) {
							pixelField[y][pixelX] = Position::inside;
						}
					}
				}
				// Setting pixels to outside if left and right pixel coordinates are flipped (left > right).
				else {
					for (unsigned int pixelX = right + 1; pixelX < x; pixelX++) {
						pixelField[y][pixelX] = Position::outside;
					}
				}
			}
			else {
				leftStack.push(x);
			}
		}
		// If the instruction is to go down, the right stack is pushed to if the left stack is empty.
		else if (code == '3') {
			if (!leftStack.empty()) {
				// Getting the top element from the left stack.
				const unsigned int left = leftStack.top();
				leftStack.pop();

				// Setting undefined pixels to inside if left and right pixel coordinates are not flipped (left < right).
				if (left < x) {
					for (unsigned int pixelX = left + 1; pixelX < x; pixelX++) {
						if (pixelField[y][pixelX] == Position::undefined) {
							pixelField[y][pixelX] = Position::inside;
						}
					}
				}
				// Setting pixels to outside if left and right pixel coordinates are flipped (left > right).
				else {
					for (unsigned int pixelX = x + 1; pixelX < left; pixelX++) {
						pixelField[y][pixelX] = Position::outside;
					}
				}
			}
			else {
				rightStack.push(x);
			}
		}
	}
}

// Sweeping the object.
void LineSweeping::sweep() {
	// Creating the object for drawing the Bresenham line.
	wxClientDC dc(drawWindow);

	// Creating a Bresenham point vector.
	std::vector<Pixel> bresenhamPixels(maxCoordinate);

	// If the line is horizontal, there is no need for sophisticated rasterization method.
	if (isInTolerance(angleOfRotation, 0.0)) {
		// Creating the starting line segment points.
		for (int i = 0; i < maxCoordinate; i++) {
			bresenhamPixels[i] = Pixel(i, 0);
		}

		// Moving the rasterized line segment vertically.
		for (int i = 0; i < maxCoordinate; i++) {
			plotBresenhamLine(dc, bresenhamPixels);
			addCurrentMidpointFromSweepLine(bresenhamPixels);  // Adding midpoints according to the sweep line.

			// Increasing each pixel Y coordinate.
			std::transform(
				bresenhamPixels.begin(),
				bresenhamPixels.end(),
				bresenhamPixels.begin(),
				[](const Pixel& pixel) {
					return Pixel(pixel.x, pixel.y + 1);
				}
			);
		}
	}
	// If the line is vertical, there is no need for sophisticated rasterization method.
	else if (isInTolerance(angleOfRotation, toRadians(90))) {
		// Creating the starting line segment points.
		for (int i = 0; i < maxCoordinate; i++) {
			bresenhamPixels[i] = Pixel(0, i);
		}

		// Moving the rasterized line segment vertically.
		for (int i = 0; i < maxCoordinate; i++) {
			plotBresenhamLine(dc, bresenhamPixels);
			addCurrentMidpointFromSweepLine(bresenhamPixels);

			// Increasing each pixel X coordinate.
			std::transform(
				bresenhamPixels.begin(),
				bresenhamPixels.end(),
				bresenhamPixels.begin(),
				[](const Pixel& pixel) {
					return Pixel(pixel.x + 1, pixel.y);
				}
			);
		}
	}
	// If the line is neither horizontal nor vertical, we have to reach for Bresenham rasterization algorithm.
	else {
		for (int x = maxCoordinate - 2; x >= 0; x--) {
			// Bresenham rasterization algorithm.
			auto startPoint(Pixel(x, 0));
			auto endPoint = getEndPointForBresenham(Pixel(x, 0), angleOfRotation, maxCoordinate);
			std::vector<Pixel> rasterizedLine = bresenham(Pixel(x, 0), endPoint);  // Rasterization method.
			addCurrentMidpointFromSweepLine(rasterizedLine);  // Adding midpoints according to the sweep line.
		}
		for (int y = 0; y < maxCoordinate; y++) {
			// Bresenham rasterization algorithm.
			std::vector<Pixel> rasterizedLine = bresenham(Pixel(0, y), getEndPointForBresenham(Pixel(0, y), angleOfRotation, maxCoordinate));  // Rasterization method.
			addCurrentMidpointFromSweepLine(rasterizedLine);  // Adding midpoints according to the sweep line.
		}
	}
}


/******************************************************************************************************************************/
/******************************************************************************************************************************/
/******************************************************************************************************************************/
/******************************************************************************************************************************/
/******************************************************************************************************************************/
// POINT TYPE
Pixel::Pixel() :
	x(0),
	y(0),
	segmentID(-1)
{}

Pixel::Pixel(const int x, const int y, const int segmentID) :
	x(x),
	y(y),
	segmentID(segmentID)
{}