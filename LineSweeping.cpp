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
		currentX = chainCode.startPoint.x * chainCode.scale;
		currentY = chainCode.startPoint.y * chainCode.scale;
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
			point.directionPrevious = (coordinates.back().directionNext + 4) % 8;
			point.directionNext = *(&chainElement + 1);
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
std::vector<Pixel> LineSweeping::findEdgePixels(const std::vector<Pixel>& rasterizedLine) const {
	std::vector<Pixel> pixels;

	for (uint i = 1; i < rasterizedLine.size(); i++) {
		Pixel previousPixel = rasterizedLine[i - 1];
		Pixel pixel = rasterizedLine[i];

		if (previousPixel.position == Position::outside && pixel.position == Position::edge && (pixels.empty() || pixel != pixels.back())) {
			pixels.emplace_back(pixel);
		}
		if (pixelField[previousPixel.y][previousPixel.x].position == Position::edge && pixelField[pixel.y][pixel.x].position == Position::inside && (pixels.empty() || previousPixel != pixels.back())) {
			pixels.emplace_back(previousPixel);
		}
		if (pixelField[previousPixel.y][previousPixel.x].position == Position::inside && pixelField[pixel.y][pixel.x].position == Position::edge && (pixels.empty() || pixel != pixels.back())) {
			pixels.emplace_back(pixel);
		}
		if (previousPixel.position == Position::edge && pixel.position == Position::outside && (pixels.empty() || previousPixel != pixels.back())) {
			pixels.emplace_back(previousPixel);
		}
	}

	return pixels;
}

// Iterative chain building.
void LineSweeping::buildChainsIteratively(const std::vector<Pixel>& edgePixels) {
	wxClientDC dc(drawWindow);

	const int vicinity = chainCodes[0].scale * 10;

	std::vector<Pixel> currentEdgePixels;

	// Finding matching pixels on the edge.
	for (const Pixel& currentEdgePixel : edgePixels) {
		for (const Pixel& previousEdgePixel : previousEdgePixels) {
			if (isEdgePixelInVicinity(vicinity, currentEdgePixel.floor(), previousEdgePixel.floor())) {
				currentEdgePixels.push_back(currentEdgePixel);
				break;
			}
		}
	}

	if (previousActualEdgePixels.empty()) {
		for (uint i = 1; i < edgePixels.size(); i++) {
			Pixel p((edgePixels[i - 1] + edgePixels[i]) / 2.0);
			
			if (pixelField[p.y][p.x].position == Position::inside) {
				Chain newChain;
				newChain.angle = toDegrees(sweepAngle);
				newChain.pixels.push_back(p);
				chains.push_back(newChain);

				if (currentEdgePixels.empty()) {
					currentEdgePixels = edgePixels;
				}
			}
		}
	}
	else {
		uint count = 1;
		for (int i = 1; i < currentEdgePixels.size(); i++) {
			if (currentEdgePixels.empty()) {
				break;
			}

			const Pixel midPixel = (currentEdgePixels[i - 1] + currentEdgePixels[i]) / 2.0;

			//if (distance(currentEdgePixels[i - 1], currentEdgePixels[i]) <= 2.1) {
			//	continue;
			//}

			if (count > previousActualEdgePixels.size()) {
				Chain newChain;
				newChain.angle = toDegrees(sweepAngle);
				newChain.pixels.push_back(midPixel);
				chains.push_back(newChain);
			}
			else {
				Pixel previousMidPixel;
				std::vector<Chain>::iterator it = chains.end();
				for (uint count2 = 1; count2 < previousActualEdgePixels.size(); count2 += 1) {
					previousMidPixel = (previousActualEdgePixels[count2 - 1] + previousActualEdgePixels[count2]) / 2.0;

					it = std::find_if(chains.begin(), chains.end(), [this, &previousMidPixel, &midPixel](const Chain& chain) {
						if ((sweepAngle == 0.0 && chain.pixels.back().y == midPixel.y) || (isInTolerance(sweepAngle, toRadians(90.0)) && chain.pixels.back().x == midPixel.x)) {
							return false;
						}

						return distance(chain.pixels.back(), previousMidPixel) < 10 * chainCodes[0].scale && isInTolerance(toRadians(chain.angle), sweepAngle);
					});

					if (it != chains.end() && distance(previousMidPixel, midPixel) < vicinity) {
						break;
					}
				}

				if (it == chains.end() || distance(previousMidPixel, midPixel) > vicinity) {
					Chain newChain;
					newChain.angle = toDegrees(sweepAngle);
					newChain.pixels.push_back(midPixel);
					chains.push_back(newChain);
				}
				else {
					count += 1;
					if (pixelField[midPixel.y][midPixel.x].position != Position::inside) {
						continue;
					}

					it->pixels.push_back(midPixel);
					//dc.SetPen(*wxRED_PEN);
					//dc.DrawLine(previousMidPixel.x * plotRatio, (maxCoordinate - previousMidPixel.y) * plotRatio, midPixel.x * plotRatio, (maxCoordinate - midPixel.y) * plotRatio);
					//dc.SetPen(*wxBLACK_PEN);
				}
			}
		}
	}

	// Current edge pixels become previous. Everyone gets old sometimes.
	previousEdgePixels = edgePixels;
	previousActualEdgePixels = currentEdgePixels;
}

// Checking whether a target pixel is in the vicinity of the current pixel on the object edge.
bool LineSweeping::isEdgePixelInVicinity(const uint vicinity, const Pixel& currentPixel, const Pixel& targetPixel) {
	Pixel iteratingPixel = currentPixel;
	
	for (uint i = 0; i < vicinity; i++) {
		// If the iterating and the target pixel match, we count success.
		if (iteratingPixel == targetPixel) {
			return true;
		}

		iteratingPixel = chainCodeMovePixel(iteratingPixel, iteratingPixel.directionPrevious);
	}

	iteratingPixel = currentPixel;

	for (uint i = 0; i < vicinity; i++) {
		// If the iterating and the target pixel match, we count success.
		if (iteratingPixel == targetPixel) {
			return true;
		}

		iteratingPixel = chainCodeMovePixel(iteratingPixel, iteratingPixel.directionNext);
	}

	return false;
}

// Obtaining a new pixel after chain code move.
Pixel LineSweeping::chainCodeMovePixel(const Pixel& currentPixel, const short direction) {
	Pixel transformedPixel = currentPixel;

	if (direction == 0) {
		transformedPixel.x++;
	}
	else if (direction == 1) {
		transformedPixel.x++;
		transformedPixel.y++;
	}
	else if (direction == 2) {
		transformedPixel.y++;
	}
	else if (direction == 3) {
		transformedPixel.x--;
		transformedPixel.y++;
	}
	else if (direction == 4) {
		transformedPixel.x--;
	}
	else if (direction == 5) {
		transformedPixel.x--;
		transformedPixel.y--;
	}
	else if (direction == 6) {
		transformedPixel.y--;
	}
	else if (direction == 7) {
		transformedPixel.x++;
		transformedPixel.y--;
	}

	return transformedPixel;
}



// PLOT METHODS
// Plotting the F4 chain code as an image.
void LineSweeping::plotInput(wxDC& dc) const {
	// Setting the color to green.
	const wxPen pen(*wxBLACK_PEN);
	const wxBrush brush(*wxBLACK_BRUSH);

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

std::vector<wxColor> colors({
	wxColor(255,   0,   0),  // Red (0°)
	wxColor(  0,   0, 255),  // Blue (15°)
	wxColor(34, 139,  34),  // Green (30°)
	wxColor(255,  0,  255),  // Magenta (45°)
	wxColor(255, 140,  80),  // Orange (60°)
	wxColor(0, 0, 0),		// Black (75°)
	wxColor(76, 0, 153),  // Purple (90°)

	wxColor(255,  0,  255),  // Magenta (105°)
	wxColor(255, 140,  80),  // Orange (120°)
	wxColor(0, 0, 0),		// Black (135°)
	wxColor(76, 0, 153),  // Purple (150°)
	wxColor(34, 139,  34),  // Green (165°)

});

// Plotting the segments.
void LineSweeping::plotChains(wxDC& dc) const {
	for (const Chain& chain : chains) {
		const uint index = static_cast<uint>((chain.angle + 1.0) / 15);
		dc.SetPen(wxPen(colors[index], 2));
		
		if (chain.pixels.size() < 10) {
			continue;
		}

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
bool LineSweeping::readFileF8(std::string file, const uint rotation, const uint scale) {
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
	bool isF4 = false;
	while (std::getline(in, value, ';')) {
		if (value == "F4") {
			isF4 = true;
		}

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
		Pixel startPixel(startX, startY);
		startPixel = rotate2D(startPixel, Pixel(0, 0), rotation * (PI / 4));

		// Adding a new chain code.
		std::getline(in, value);
		ChainCode chainCode(value, clockwise, startPixel, rotation, scale, isF4);
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
			const std::vector<Pixel> edgePixels = findEdgePixels(bresenhamPixels);  // Finding edge pixels on the rasterized line.
			if (!edgePixels.empty()) {
				buildChainsIteratively(edgePixels);  // Iterative chain building.
			}

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
			const std::vector<Pixel> edgePixels = findEdgePixels(bresenhamPixels);  // Finding edge pixels on the rasterized line.
			if (!edgePixels.empty()) {
				buildChainsIteratively(edgePixels);  // Iterative chain building.
			}

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
				//dc.DrawLine(startPoint.x * plotRatio, (maxCoordinate - startPoint.y) * plotRatio, endPoint.x * plotRatio, (maxCoordinate - endPoint.y) * plotRatio);

				std::vector<Pixel> edgePixels = findEdgePixelsWithBresenham(startPoint, endPoint, pixelField);
				//std::vector<Pixel> rasterizedLine = bresenham(startPoint, endPoint, pixelField);  // Rasterization method.
				//std::vector<Pixel> rasterizedLine = clearyWyvill(startPoint, endPoint, pixelField, sweepAngle);  // Rasterization method.
				//const std::vector<Pixel> edgePixels = findEdgePixels(rasterizedLine);  // Finding edge pixels on the rasterized line.
				if (!edgePixels.empty()) {
					buildChainsIteratively(edgePixels);  // Iterative chain building.
				}
			}
			for (int x = 0; x < maxCoordinate; x += 1) {
				// Bresenham rasterization algorithm.
				Pixel startPoint = Pixel(x, maxCoordinate);
				Pixel endPoint = getEndPointForBresenham(Pixel(x, maxCoordinate), sweepAngle, maxCoordinate);
				std::vector<Pixel> edgePixels = findEdgePixelsWithBresenham(startPoint, endPoint, pixelField);
				//std::vector<Pixel> rasterizedLine = bresenham(startPoint, endPoint, pixelField);  // Rasterization method.
				//std::vector<Pixel> rasterizedLine = clearyWyvill(startPoint, endPoint, pixelField, PI - sweepAngle);  // Rasterization method.
				//const std::vector<Pixel> edgePixels = findEdgePixels(rasterizedLine);  // Finding edge pixels on the rasterized line.
				if (!edgePixels.empty()) {
					buildChainsIteratively(edgePixels);  // Iterative chain building.
				}
			}
		}
		else if (toDegrees(sweepAngle) < 180.0) {
			for (int x = 0; x < maxCoordinate; x += 1) {
				// Bresenham rasterization algorithm.
				Pixel startPoint(Pixel(x, maxCoordinate));
				Pixel endPoint = getEndPointForBresenham(Pixel(x, maxCoordinate), sweepAngle, maxCoordinate);
				std::vector<Pixel> edgePixels = findEdgePixelsWithBresenham(startPoint, endPoint, pixelField);
				//std::vector<Pixel> rasterizedLine = bresenham(startPoint, endPoint, pixelField);  // Rasterization method.
				//std::vector<Pixel> rasterizedLine = clearyWyvill(startPoint, endPoint, pixelField, sweepAngle);  // Rasterization method.
				//const std::vector<Pixel> edgePixels = findEdgePixels(rasterizedLine);  // Finding edge pixels on the rasterized line.
				if (!edgePixels.empty()) {
					buildChainsIteratively(edgePixels);  // Iterative chain building.
				}
			}
			for (int y = maxCoordinate; y >= 0; y -= 1) {
				// Bresenham rasterization algorithm.
				auto startPoint(Pixel(maxCoordinate, y));
				auto endPoint = getEndPointForBresenham(startPoint, sweepAngle, maxCoordinate);
				std::vector<Pixel> edgePixels = findEdgePixelsWithBresenham(startPoint, endPoint, pixelField);
				//std::vector<Pixel> rasterizedLine = bresenham(startPoint, endPoint, pixelField);  // Rasterization method.
				//std::vector<Pixel> rasterizedLine = clearyWyvill(startPoint, endPoint, pixelField, sweepAngle);  // Rasterization method.
				//const std::vector<Pixel> edgePixels = findEdgePixels(rasterizedLine);  // Finding edge pixels on the rasterized line.
				if (!edgePixels.empty()) {
					buildChainsIteratively(edgePixels);  // Iterative chain building.
				}
			}
		}
	}

	previousEdgePixels.clear();
	previousActualEdgePixels.clear();
}

// Calculation of a feature vector.
FeatureVector LineSweeping::calculateFeatureVector() {
	std::vector<double> distances(chains.size());
	for (uint i = 0; i < chains.size(); i++) {
		distances[i] = distance(chains[i].pixels.front(), chains[i].pixels.back());
	}

	std::sort(distances.begin(), distances.end());
	std::reverse(distances.begin(), distances.end());
	const double maxLength = distances[0];
	for (uint i = 0; i < distances.size(); i++) {
		distances[i] /= maxLength;
	}
	distances.erase(std::remove_if(distances.begin(), distances.end(), [](const double distance) { return distance < 0.2; }), distances.end());


	//std::sort(chains.begin(), chains.end(), [](const Chain& chain1, const Chain& chain2) { return chain1.pixels.size() > chain2.pixels.size(); });
	//const double maxLength = chains[0].totalLength();
	//chains.erase(std::remove_if(chains.begin(), chains.end(), [maxLength](const Chain& chain) { return (chain.totalLength() / maxLength) < 0.2; }), chains.end());

	//std::vector<Chain> chainVector(chains);  // Feature vector of the object.

	//// Sorting the feature vector according to the chain lengths.
	//std::sort(
	//	chainVector.begin(),
	//	chainVector.end(),
	//	[](const Chain& s1, const Chain& s2) {
	//		return s1.pixels.size() > s2.pixels.size();
	//	}
	//);

	//// Creating a feature vector.
	FeatureVector featureVector(distances);

	//// Sorting the feature vector according to the chain lengths.
	//std::sort(
	//	featureVector.chainLengths.begin(),
	//	featureVector.chainLengths.end(),
	//	[](const double s1, const double s2) {
	//		return s1 > s2;
	//	}
	//);

	//if (!featureVector.chainLengths.empty()) {
	//	const double maxLen = featureVector.chainLengths[0];
	//	std::transform(
	//		featureVector.chainLengths.begin(),
	//		featureVector.chainLengths.end(),
	//		featureVector.chainLengths.begin(),
	//		[&maxLen](double d) {
	//			return d / maxLen;
	//		}
	//	);
	//}

	//featureVector.chainLengths.erase(
	//	std::remove_if(featureVector.chainLengths.begin(), featureVector.chainLengths.end(), [](const double len) { return len < 0.05; }),
	//	featureVector.chainLengths.end()
	//);

	return featureVector;
}

FeatureVector calculateFeatureVector(const std::vector<LineSweeping>& sweepVector) {
	uint sum = 0;

	std::vector<double> distances;
	for (uint i = 0; i < sweepVector.size(); i++) {
		for (uint j = 0; j < sweepVector[i].chains.size(); j++) {
			//sum += sweepVector[i].chains[j].pixels.size();
			//
			//double euclideanDistance = 0.0;
			//for (uint k = 1; k < sweepVector[i].chains[j].pixels.size(); k++) {
			//	euclideanDistance += distance(sweepVector[i].chains[j].pixels[k - 1], sweepVector[i].chains[j].pixels[k]);
			//}

			//distances.push_back(euclideanDistance);

			distances.push_back(distance(sweepVector[i].chains[j].pixels.front(), sweepVector[i].chains[j].pixels.back()));
		}
	}

	std::sort(distances.begin(), distances.end());
	std::reverse(distances.begin(), distances.end());
	const double maxLength = distances.empty() ? 1.0 : distances[0];
	for (uint i = 0; i < distances.size(); i++) {
		distances[i] /= maxLength;
		//distances[i] /= sweepVector[0].maxCoordinate;
		//distances[i] /= sum;
	}
	distances.erase(std::remove_if(distances.begin(), distances.end(), [](const double distance) { return distance < 0.2; }), distances.end());

	// Creating a feature vector.
	FeatureVector featureVector(distances);

	return featureVector;
}