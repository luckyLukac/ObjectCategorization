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
	inside,
	undefined
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
	std::vector<Point> coordinates;      // Point coordinates.
	PixelField pixelField;				 // Pixel field with pixel positions according to the object (edge, outside or inside).
	int maxCoordinate = 0;				 // Maximum coordinate.

	double angleOfRotation;				 // Sweep line angle of rotation [0°-180°].
	std::vector<Point> midPoints;		 // Middle points found while sweeping.
	std::vector<Segment> segments;		 // Vector of segments.

	// PRIVATE HELPER METHODS
	void calculateCoordinatesFromChainCode();																					// Transforming chain code to coordinates.
	void calculateBoundingBox();																							    // Calculation of a bounding box according to point coordinates.
	void fillRectangle(wxDC& dc, const int x, const int y, const int pixelSize, const wxPen& pen, const wxBrush& brush) const;  // Filling a rectangle at X and Y coordinates.
	void addCurrentMidpointFromSweepLine(const std::vector<Point>& rasterizedLine);												// Adding the current midpoint and plotting the Bresenham line.

public:
	// PLOT METHODS
	void plotInput(wxDC& dc) const;													   // Plotting the F4 chain code as an image.
	void plotBoundingBox(wxDC& dc) const;											   // Plotting the object bounding box.
	void plotBresenhamLine(wxDC& dc, const std::vector<Point>& rasterizedLine) const;  // Plotting the Bresenham line.

	// GETTERS AND SETTERS
	void setF4(const std::vector<std::byte>& chainCode);      // Setting F4 chain code.
	void setDrawPanel(wxWindow* drawWindow);        		  // Setting draw panel.
	bool isChainCodeSet() const;							  // Returning true if a chain code is set.
	void clearSegments();									  // Clearing previous segments.
	void setAngleOfRotation(const double angle);			  // Setting the angle of rotation (given in radians).

	// PUBLIC METHODS
	std::vector<std::byte> readFile(std::string file) const;  // Reading a file.
	void fillShape();										  // Filling the loaded shape.
	void sweep();											  // Sweeping the object.
};


