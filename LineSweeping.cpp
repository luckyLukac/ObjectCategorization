#include <algorithm>
#include <fstream>
#include <limits>
#include <stack>

#include "Chain.hpp"
#include "ChainCode.hpp"
#include "HelperFunctions.hpp"
#include "LineSweeping.hpp"
#include "stdafx.h"


// PRIVATE HELPER METHODS
// Transforming chain code to coordinates.
void LineSweeping::calculateCoordinatesFromChainCode() {
	// Current point and X and Y coordinates.
	Pixel point;
	int currentX = 0;
	int currentY = 0;

	// Adding new coordinates according to the chain code.
	for (const ChainCode& chainCode : chainCodes) {
		// Setting the start point.
		currentX = chainCode.startPoint.x;
		currentY = chainCode.startPoint.y;
		point.x = currentX;
		point.y = currentY;
		point.directionNext = chainCode.code.front();
		point.directionPrevious = (chainCode.code.back() + 4) % 8;
		coordinates.push_back(point);

		// Iterating through the chain code.
		for (const short& chainElement : chainCode.code) {
			const short direction = chainElement;  // Casting the byte to char.

			// 0 means right.
			if (direction == 0) {
				currentX++;
			}
			// 1 means up and right.
			else if (direction == 1) {
				currentY++;
				currentX++;
			}
			// 2 means up.
			else if (direction == 2) {
				currentY++;
			}
			// 3 means up and left.
			else if (direction == 3) {
				currentY++;
				currentX--;
			}
			// 4 means left.
			else if (direction == 4) {
				currentX--;
			}
			// 5 means down and left.
			else if (direction == 5) {
				currentY--;
				currentX--;
			}
			// 6 means down.
			else if (direction == 6) {
				currentY--;
			}
			// 7 means down and right.
			else if (direction == 7) {
				currentY--;
				currentX++;
			}

			// Adding a new coordinate to the vector.
			point.x = currentX;
			point.y = currentY;
			point.directionPrevious = (point.directionNext + 4) % 8;
			point.directionNext = direction;
			coordinates.push_back(point);
		}

		coordinates.pop_back();
	}
}

// Calculation of a bounding box according to point coordinates.
void LineSweeping::calculateBoundingBox() {
	// Initializing coordinates to max and min.
	int xMin = std::numeric_limits<int>::max();
	int yMin = std::numeric_limits<int>::max();
	int xMax = std::numeric_limits<int>::min();
	int yMax = std::numeric_limits<int>::min();
	uint yMaxIndex = 0;

	// Iterating through each point.
	for (uint i = 0; i < coordinates.size(); i++) {
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

	// Moving the bounding box to the left upper corner.
	std::transform(
		coordinates.begin(),
		coordinates.end(),
		coordinates.begin(),
		[&xMin, &yMin](Pixel point) {
			return Pixel(point.x - xMin, point.y - yMin, Position::undefined, point.directionPrevious, point.directionNext);
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
		[&magnifiedX, &magnifiedY](const Pixel& point) {
			return Pixel(point.x + magnifiedX, point.y + magnifiedY, Position::undefined, point.directionPrevious, point.directionNext);
		}
	);

	// Setting the max coordinates.
	maxCoordinate = magnifiedPivotCoordinate + 1;
	if (maxCoordinate % 2 == 1) {
		maxCoordinate++;
	}

	// Setting draw coordinates.
	plotRatio = 1000.0 / maxCoordinate;
	if (plotRatio > 1.0) {
		plotRatio = 1.0;
	}

	// Creating the pixel field.
	for (int y = 0; y < maxCoordinate; y++) {
		pixelField.push_back(std::vector<Pixel>());

		for (int x = 0; x < maxCoordinate; x++) {
			pixelField[y].push_back(Pixel(x, y, Position::undefined));
		}
	}

	for (const Pixel& coordinate : coordinates) {
		pixelField[coordinate.y][coordinate.x].position = Position::edge;
		pixelField[coordinate.y][coordinate.x].directionPrevious = coordinate.directionPrevious;
		pixelField[coordinate.y][coordinate.x].directionNext = coordinate.directionNext;
	}
}

// Filling a rectangle at X and Y coordinates.
void LineSweeping::fillRectangle(wxDC& dc, const int x, const int y, const int pixelSize, const int maxCoordinate, const wxPen& pen, const wxBrush& brush, const double ratio) const {
	// Creating a wxPoint for rendering.
	wxPoint P[4] = {
		wxPoint(static_cast<int>(x * ratio), static_cast<int>((maxCoordinate - y) * ratio)),
		wxPoint(static_cast<int>(x * ratio + pixelSize), static_cast<int>((maxCoordinate - y) * ratio)),
		wxPoint(static_cast<int>(x * ratio + pixelSize), static_cast<int>((maxCoordinate - y) * ratio + pixelSize)),
		wxPoint(static_cast<int>(x * ratio), static_cast<int>((maxCoordinate - y) * ratio + pixelSize))
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
std::vector<Pixel> LineSweeping::findEdgePixels(const std::vector<Pixel>& rasterizedLine, bool extended) const {
	std::vector<Pixel> pixels;

	for (uint i = 1; i < rasterizedLine.size(); i++) {
		Pixel previousPixel = rasterizedLine[i - 1];
		Pixel pixel = rasterizedLine[i];

		if (extended && previousPixel.position == Position::outside && pixel.position == Position::edge) {
			pixels.emplace_back(pixel);
		}
		if (pixelField[previousPixel.y][previousPixel.x].position == Position::edge && pixelField[pixel.y][pixel.x].position == Position::inside) {
			pixels.emplace_back(previousPixel);
		}
		if (pixelField[previousPixel.y][previousPixel.x].position == Position::inside && pixelField[pixel.y][pixel.x].position == Position::edge) {
			pixels.emplace_back(pixel);
		}
		if (extended && previousPixel.position == Position::edge && pixel.position == Position::outside) {
			pixels.emplace_back(previousPixel);
		}
	}

	return pixels;
}

// Iterative chain building.
void LineSweeping::buildChainsIteratively(const std::vector<Pixel>& rasterizedLine) {
	const std::vector<Pixel> edgePixels = findEdgePixels(rasterizedLine, true);  // Finding edge pixels on the rasterized line.
	const int vicinity = 3;

	// Finding matching pixels on the edge.
	for (int i = 0; i < edgePixels.size(); i++) {
		Pixel currentPixel = edgePixels[i];

		for (int j = 0; j < previousEdgePixels.size(); j++) {

		}
	}

	// Current edge pixels become previous. Everyone gets old sometimes.
	previousEdgePixels = edgePixels;
}

// Checking whether a target pixel is in the vicinity of the current pixel on the object edge.
bool LineSweeping::isEdgePixelInVicinity(const uint vicinity, const Pixel& currentPixel, const Pixel& targetPixel) {
	Pixel iteratingPixel = currentPixel;
	
	for (uint i = 0; i < vicinity; i++) {
		// If the iterating and the target pixel match, we count success.
		if (iteratingPixel == targetPixel) {
			return true;
		}

		iteratingPixel = pixelField[]
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
		fillRectangle(dc, point.x, point.y, 1, maxCoordinate, pen, brush, plotRatio);
	}
}

// Plotting the object bounding box.
void LineSweeping::plotBoundingBox(wxDC& dc) const {
	// Plotting the four lines of the bounding box.
	dc.DrawLine(0, 0, static_cast<int>(maxCoordinate * plotRatio), 0);
	dc.DrawLine(static_cast<int>(maxCoordinate * plotRatio), 0, static_cast<int>(maxCoordinate * plotRatio), static_cast<int>(maxCoordinate * plotRatio));
	dc.DrawLine(static_cast<int>(maxCoordinate * plotRatio), static_cast<int>(maxCoordinate * plotRatio), 0, static_cast<int>(maxCoordinate * plotRatio));
	dc.DrawLine(0, static_cast<int>(maxCoordinate * plotRatio), 0, 0);
}

// Plotting the Bresenham line.
void LineSweeping::plotBresenhamLine(wxDC& dc, const std::vector<Pixel>& rasterizedLine) const {
	// Plotting each pixel of the line.
	for (const Pixel& pixel : rasterizedLine) {
		fillRectangle(dc, pixel.x, pixel.y, 1, maxCoordinate, *wxLIGHT_GREY_PEN, *wxLIGHT_GREY_BRUSH, plotRatio);
	}
}

// Plotting the segments.
void LineSweeping::plotChains(wxDC& dc) const {
	for (const Chain& chain : chains) {
		for (uint i = 1; i < chain.pixels.size(); i++) {
			dc.DrawLine(chain.pixels[i - 1].x * plotRatio, (maxCoordinate - chain.pixels[i - 1].y) * plotRatio, chain.pixels[i].x * plotRatio, (maxCoordinate - chain.pixels[i].y) * plotRatio);
		}
	}
}



// GETTERS AND SETTERS
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
	chains.clear();
}

// Setting the angle of rotation (given in radians).
void LineSweeping::setAngleOfRotation(const double angle) {
	sweepAngle = angle;
}



// PUBLIC METHODS
// Reading an F4 chain code file.
bool LineSweeping::readFileF8(std::string file, const uint rotation) {
	// Opening a file.
	std::ifstream in(file);

	// If a file is not open, we do not panic but abort the process.
	if (!in.is_open()) {
		return false;
	}

	// Clearing the previous coordinates.
	chainCodes.clear();
	coordinates.clear();
	pixelField.clear();
	chains.clear();

	// Reading first line.
	std::string firstLine;
	std::getline(in, firstLine);
	if (firstLine != "CC Multi") {
		return false;
	}

	// Reading the chain code.
	bool clockwise = false;
	int startX = 0;
	int startY = 0;

	std::string value;
	while (std::getline(in, value, ';')) {
		// Reading clockwise or anti-clockwise orientation.
		std::getline(in, value, ';');
		if (value == "CW") {
			clockwise = true;
		}
		else {
			clockwise = false;
		}

		// Reading the starting point.
		std::getline(in, value, ',');
		startX = std::stoi(value);
		std::getline(in, value, ';');
		startY = -std::stoi(value);
		std::getline(in, value, ';');

		// Adding a new chain code.
		std::getline(in, value);
		ChainCode chainCode(value, clockwise, Pixel(startX, startY, Position::undefined), rotation);
		chainCodes.push_back(chainCode);
	}

	// Calculating the coordinates.
	calculateCoordinatesFromChainCode();
	calculateBoundingBox();

	return true;
}

// Filling the loaded shape.
void LineSweeping::fillShape() {
	wxClientDC dc(drawWindow);

	// Creating two stacks for filling the shape.
	std::stack<uint> leftStack;
	std::stack<uint> rightStack;
	long startCoordinate = 0;

	// Iterating through the chain codes and filling the shape.
	int j = 0;
	for (const ChainCode& chainCode : chainCodes) {
		for (uint i = 0; i < chainCode.code.size(); i++) {
			// Getting the chain code element and the X and Y coordinates of the pixel.
			const short code = chainCode.code[i];
			const uint x = coordinates[startCoordinate + i].x;
			const uint y = coordinates[startCoordinate + i].y;

			// If the instruction is to go up, the left stack is pushed to if the right stack is empty.
			if (code == 1 || code == 2 || code == 3) {
				if (!rightStack.empty()) {
					// Getting the top element from the right stack.
					const uint right = rightStack.top();
					rightStack.pop();

					// Setting undefined pixels to inside if left and right pixel coordinates are not flipped (left < right).
					if (right > x) {
						for (uint pixelX = x + 1; pixelX < right; pixelX++) {
							if (pixelField[y][pixelX].position == Position::inside) {
								pixelField[y][pixelX].position = Position::outside;
								//fillRectangle(dc, pixelX, y, 1, maxCoordinate, *wxWHITE_PEN, *wxWHITE_BRUSH, plotRatio);
							}
							else if (pixelField[y][pixelX].position != Position::edge) {
								pixelField[y][pixelX].position = Position::inside;
								//fillRectangle(dc, pixelX, y, 1, maxCoordinate, *wxGREEN_PEN, *wxGREEN_BRUSH, plotRatio);
							}
						}
					}
					// Setting pixels to outside if left and right pixel coordinates are flipped (left > right).
					else {
						for (uint pixelX = right + 1; pixelX < x; pixelX++) {
							if (pixelField[y][pixelX].position == Position::inside) {
								pixelField[y][pixelX].position = Position::outside;
								//fillRectangle(dc, pixelX, y, 1, maxCoordinate, *wxWHITE_PEN, *wxWHITE_BRUSH, plotRatio);
							}
							else if (pixelField[y][pixelX].position != Position::edge) {
								pixelField[y][pixelX].position = Position::inside;
								//fillRectangle(dc, pixelX, y, 1, maxCoordinate, *wxGREEN_PEN, *wxGREEN_BRUSH, plotRatio);
							}
						}
					}
				}
				else {
					leftStack.push(x);
					//fillRectangle(dc, x, y, 1, maxCoordinate, *wxRED_PEN, *wxRED_BRUSH, plotRatio);
				}
			}
			// If the instruction is to go down, the right stack is pushed to if the left stack is empty.
			else if (code == 5 || code == 6 || code == 7) {
				if (!leftStack.empty()) {
					// Getting the top element from the left stack.
					const uint left = leftStack.top();
					leftStack.pop();

					// Setting undefined pixels to inside if left and right pixel coordinates are not flipped (left < right).
					if (left < x) {
						for (uint pixelX = left + 1; pixelX < x; pixelX++) {
							if (pixelField[y - 1][pixelX].position == Position::inside) {
								pixelField[y - 1][pixelX].position = Position::outside;
								//fillRectangle(dc, pixelX, y - 1, 1, maxCoordinate, *wxWHITE_PEN, *wxWHITE_BRUSH, plotRatio);
							}
							else if (pixelField[y - 1][pixelX].position != Position::edge) {
								pixelField[y - 1][pixelX].position = Position::inside;
								//fillRectangle(dc, pixelX, y - 1, 1, maxCoordinate, *wxGREEN_PEN, *wxGREEN_BRUSH, plotRatio);
							}
						}
					}
					// Setting pixels to outside if left and right pixel coordinates are flipped (left > right).
					else {
						for (uint pixelX = x + 1; pixelX < left; pixelX++) {
							if (pixelField[y - 1][pixelX].position == Position::inside) {
								pixelField[y - 1][pixelX].position = Position::outside;
								//fillRectangle(dc, pixelX, y - 1, 1, maxCoordinate, *wxWHITE_PEN, *wxWHITE_BRUSH, plotRatio);
							}
							else if (pixelField[y - 1][pixelX].position != Position::edge) {
								pixelField[y - 1][pixelX].position = Position::inside;
								//fillRectangle(dc, pixelX, y - 1, 1, maxCoordinate, *wxGREEN_PEN, *wxGREEN_BRUSH, plotRatio);
							}
						}
					}
				}
				else {
					rightStack.push(x);
					//fillRectangle(dc, x, y, 1, maxCoordinate, *wxBLUE_PEN, *wxBLUE_BRUSH, plotRatio);
				}
			}

			// Setting the current pixel to edge.
			pixelField[y][x].position = Position::edge;
			pixelField[y][x].directionPrevious = coordinates[startCoordinate + i].directionPrevious;
			pixelField[y][x].directionNext = coordinates[startCoordinate + i].directionNext;
		}

		leftStack = std::stack<uint>();
		rightStack = std::stack<uint>();
		startCoordinate += chainCode.code.size();
		j++;
	}

	// Setting undefined pixels to outside.
	for (uint y = 0; y < pixelField.size(); y++) {
		for (uint x = 0; x < pixelField[y].size(); x++) {
			if (pixelField[y][x].position == Position::undefined) {
				pixelField[y][x].position = Position::outside;
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
	if (isInTolerance(sweepAngle, 0.0)) {
		// Creating the starting line segment points.
		for (int i = 0; i < maxCoordinate; i++) {
			bresenhamPixels[i] = Pixel(i, 0, pixelField[0][i].position, pixelField[0][i].directionPrevious, pixelField[0][i].directionNext);
		}

		// Moving the rasterized line segment vertically.
		for (int i = 0; i < maxCoordinate - 1; i++) {
			buildChainsIteratively(bresenhamPixels);  // Iterative chain building.

			// Increasing each pixel Y coordinate.
			std::transform(
				bresenhamPixels.begin(),
				bresenhamPixels.end(),
				bresenhamPixels.begin(),
				[this](const Pixel& pixel) {
					return Pixel(pixel.x, pixel.y + 1, pixelField[pixel.y + 1][pixel.x].position, pixelField[pixel.y + 1][pixel.x].directionPrevious, pixelField[pixel.y + 1][pixel.x].directionNext);
				}
			);
		}
	}
	// If the line is vertical, there is no need for sophisticated rasterization method.
	else if (isInTolerance(sweepAngle, toRadians(90))) {
		// Creating the starting line segment points.
		for (int i = 0; i < maxCoordinate; i++) {
			bresenhamPixels[i] = Pixel(0, i, pixelField[i][0].position, pixelField[i][0].directionPrevious, pixelField[i][0].directionNext);
		}

		// Moving the rasterized line segment vertically.
		for (int i = 0; i < maxCoordinate - 1; i++) {
			buildChainsIteratively(bresenhamPixels);  // Iterative chain building.

			// Increasing each pixel X coordinate.
			std::transform(
				bresenhamPixels.begin(),
				bresenhamPixels.end(),
				bresenhamPixels.begin(),
				[this](const Pixel& pixel) {
					return Pixel(pixel.x + 1, pixel.y, pixelField[pixel.y][pixel.x + 1].position, pixelField[pixel.y][pixel.x + 1].directionPrevious, pixelField[pixel.y][pixel.x + 1].directionNext);
				}
			);
		}
	}
	// If the line is neither horizontal nor vertical, we have to reach for Bresenham rasterization algorithm.
	else {
		if (toDegrees(sweepAngle) < 90.0) {
			for (int y = 0; y < maxCoordinate; y += 1) {
				// Bresenham rasterization algorithm.
				Pixel startPoint(Pixel(0, y));
				Pixel endPoint = getEndPointForBresenham(startPoint, sweepAngle, maxCoordinate);
				std::vector<Pixel> rasterizedLine = bresenham(startPoint, endPoint, pixelField);  // Rasterization method.
				//std::vector<Pixel> rasterizedLine = clearyWyvill(startPoint, endPoint, pixelField, angleOfRotation);  // Rasterization method.
				buildChainsIteratively(rasterizedLine);  // Iterative chain building.
			}
			for (int x = 0; x < maxCoordinate; x += 1) {
				// Bresenham rasterization algorithm.
				Pixel startPoint = Pixel(x, maxCoordinate);
				Pixel endPoint = getEndPointForBresenham(Pixel(x, maxCoordinate), sweepAngle, maxCoordinate);
				std::vector<Pixel> rasterizedLine = bresenham(startPoint, endPoint, pixelField);  // Rasterization method.
				//std::vector<Pixel> rasterizedLine = clearyWyvill(startPoint, endPoint, pixelField, PI - angleOfRotation);  // Rasterization method.
				//addCurrentMidpointFromSweepLine(rasterizedLine);  // Adding midpoints according to the sweep line.
				buildChainsIteratively(rasterizedLine);  // Iterative chain building.
			}
		}
		else if (toDegrees(sweepAngle) < 180.0) {
			for (int x = 0; x < maxCoordinate; x += 1) {
				// Bresenham rasterization algorithm.
				Pixel startPoint(Pixel(x, maxCoordinate));
				Pixel endPoint = getEndPointForBresenham(Pixel(x, maxCoordinate), sweepAngle, maxCoordinate);
				std::vector<Pixel> rasterizedLine = bresenham(startPoint, endPoint, pixelField);  // Rasterization method.
				//std::vector<Pixel> rasterizedLine = clearyWyvill(startPoint, endPoint, pixelField, angleOfRotation);  // Rasterization method.
				//addCurrentMidpointFromSweepLine(rasterizedLine);  // Adding midpoints according to the sweep line.
				buildChainsIteratively(rasterizedLine);  // Iterative chain building.
			}
			for (int y = maxCoordinate; y >= 0; y -= 1) {
				// Bresenham rasterization algorithm.
				auto startPoint(Pixel(maxCoordinate, y));
				auto endPoint = getEndPointForBresenham(startPoint, sweepAngle, maxCoordinate);
				std::vector<Pixel> rasterizedLine = bresenham(startPoint, endPoint, pixelField);  // Rasterization method.
				//std::vector<Pixel> rasterizedLine = clearyWyvill(startPoint, endPoint, pixelField, angleOfRotation);  // Rasterization method.
				//addCurrentMidpointFromSweepLine(rasterizedLine);  // Adding midpoints according to the sweep line.
				buildChainsIteratively(rasterizedLine);  // Iterative chain building.
			}
		}
	}
}

// Calculation of a feature vector.
FeatureVector LineSweeping::calculateFeatureVector() const {
	std::vector<Chain> chainVector(chains);  // Feature vector of the object.

	// Sorting the feature vector according to the chain lengths.
	std::sort(
		chainVector.begin(),
		chainVector.end(),
		[](const Chain& s1, const Chain& s2) {
			return s1.pixels.size() > s2.pixels.size();
		}
	);

	// Creating a feature vector.
	FeatureVector featureVector(chainVector);

	// Sorting the feature vector according to the chain lengths.
	std::sort(
		featureVector.chainLengths.begin(),
		featureVector.chainLengths.end(),
		[](const double s1, const double s2) {
			return s1 > s2;
		}
	);

	if (!featureVector.chainLengths.empty()) {
		const double maxLen = featureVector.chainLengths[0];
		std::transform(
			featureVector.chainLengths.begin(),
			featureVector.chainLengths.end(),
			featureVector.chainLengths.begin(),
			[&maxLen](double d) {
				return d / maxLen;
			}
		);
	}

	return featureVector;
}