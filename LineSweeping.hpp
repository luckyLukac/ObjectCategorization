#pragma once

#include <vector>
#include <wx/dcbuffer.h>


// CONSTANTS
const double MAGNIFY_FACTOR = 1.5;


// STRUCTURES
// Point struct.
struct Point {
	int x;
	int y;
	int segmentID;

	Point();
	Point(const int x, const int y, const int segmentID = -1);
};

// Segment struct.
struct Segment {
	double angle;						// Sweep-line angle.
	double averageAngle;				// Average angle of the obtained polyline.
	std::vector<Point> middlePolyline;  // The obtained polyline.
	std::vector<Point> offset1;		    //
	std::vector<Point> offset2;			//
	bool isValid;						// Validity flag.
};


// Position enum class.
enum class Position {
	edge,
	outside,
	inside
};


// ALIASES
using PixelField = std::vector<std::vector<Position>>;


// MAIN CLASS
// Line sweeping class.
class LineSweeping {
private:
	// PRIVATE VARIABLES
	wxWindow* drawWindow = nullptr;		 // Draw window.
	std::vector<std::byte> chainCode;    // F4 chain code.
	bool clockwiseOrientation = false;
	std::vector<Point> coordinates;      // Point coordinates.
	std::vector<Point> testFillcoordinatesTEMP;
	int maxX = 0;						 // Maximum X coordinate.
	int maxY = 0;						 // Maximum Y coordinate.
	PixelField pixelField;				 // Pixel field with pixel positions according to the object (edge, outside or inside).
	std::vector<Segment> segments;		 // Vector of segments.

	// PRIVATE HELPER METHODS
	void calculateBoundingBox();																							    // Calculation of a bounding box according to point coordinates.
	void fillRectangle(wxDC& dc, const int x, const int y, const int pixelSize, const wxPen& pen, const wxBrush& brush) const;  // Filling a rectangle at X and Y coordinates.

public:
	// PLOT METHODS
	void plotInput(wxDC& dc) const;							  // Plotting the F4 chain code as an image.
	void plotBoundingBox(wxDC& dc) const;					  // Plotting the object bounding box.

	// GETTERS AND SETTERS
	void setF4(const std::vector<std::byte>& chainCode);      // Setting F4 chain code.
	void setDrawPanel(wxWindow* drawWindow);        		  // Setting draw panel.
	bool isChainCodeSet() const;							  // Returning true if a chain code is set.
	void clearSegments();									  // Clearing previous segments.

	// PUBLIC METHODS
	std::vector<std::byte> readFile(std::string file) const;  // Reading a file.
	void fillShape();										  // Filling the loaded shape.
};


