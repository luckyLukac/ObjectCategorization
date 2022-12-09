#pragma once

#include <vector>
#include <wx/dcbuffer.h>


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

	void calculateBoundingBox();																						  // Calculation of a bounding box according to point coordinates.
	void fillRectangle(wxDC& dc, const int x, const int y, const int pixelSize, const wxPen& pen, const wxBrush& brush);  // Filling a rectangle at X and Y coordinates.

public:
	void setF4(const std::vector<std::byte>& chainCode);  // Setting F4 chain code.
	void plotInput(wxDC& dc);							  // Plotting the F4 chain code as an image.
	void plotBoundingBox(wxDC& dc);						  // Plotting the object bounding box.
};


