#pragma once

#include <vector>

// CONSTANTS
const double MAGNIFY_FACTOR = 1.5;


// STRUCTURES
// Point type struct.
struct Point {
	int x;
	int y;
	int segmentID;

	Point();
	Point(const int x, const int y, const int segmentID = -1);
};


// MAIN CLASS
// Line sweeping class.
class LineSweeping {
private:
	std::vector<Point> coordinates;      // Point coordinates.
	int maxX;							 // Maximum X coordinate.
	int maxY;							 // Maximum Y coordinate.

	void calculateBoundingBox();		 // Calculation of a bounding box according to point coordinates.

public:
	void setF4(const std::vector<std::byte>& chainCode);  // Setting F4 chain code.
};


