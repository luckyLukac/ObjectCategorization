#pragma once

#include <vector>
#include <wx/dcbuffer.h>

#include "ChainCode.hpp"
#include "Pixel.hpp"


// CONSTANTS
const double MAGNIFY_FACTOR = 1.5;


// STRUCTURES
// Segment struct.
struct Segment {
	double angle;						// Sweep-line angle.
	double averageAngle;				// Average angle of the obtained polyline.
	std::vector<Pixel> middlePolyline;  // The obtained polyline.
	std::vector<Pixel> offset1;		    //
	std::vector<Pixel> offset2;			//
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
	std::vector<ChainCode> chainCodes;   // F4 chain code.
	std::vector<Pixel> coordinates;      // Point coordinates.
	PixelField pixelField;				 // Pixel field with pixel positions according to the object (edge, outside or inside).
	int maxCoordinate = 0;				 // Maximum coordinate.
	double plotRatio = 1.0;			     // Ratio factor for drawing.

	double angleOfRotation = 0.0;		 // Sweep line angle of rotation [0°-180°].
	std::vector<Pixel> midPoints;		 // Middle points found while sweeping.
	std::vector<Segment> segments;		 // Vector of segments.

	// PRIVATE HELPER METHODS
	void calculateCoordinatesFromChainCode();																											  // Transforming chain code to coordinates.
	void calculateBoundingBox();																														  // Calculation of a bounding box according to point coordinates.
	void fillRectangle(wxDC& dc, const int x, const int y, const int pixelSize, const wxPen& pen, const wxBrush& brush, const double ratio = 1.0) const;  // Filling a rectangle at X and Y coordinates.
	std::vector<Pixel> findEdgePixels(const std::vector<Pixel>& rasterizedLine) const;																	  // Finding edge pixel pairs.
	void addCurrentMidpointFromSweepLine(const std::vector<Pixel>& rasterizedLine);																		  // Adding the current midpoint and plotting the Bresenham line.

public:
	// PLOT METHODS
	void plotInput(wxDC& dc) const;													   // Plotting the F4 chain code as an image.
	void plotBoundingBox(wxDC& dc) const;											   // Plotting the object bounding box.
	void plotBresenhamLine(wxDC& dc, const std::vector<Pixel>& rasterizedLine) const;  // Plotting the Bresenham line.

	// GETTERS AND SETTERS
	void setDrawPanel(wxWindow* drawWindow);        		  // Setting draw panel.
	bool isChainCodeSet() const;							  // Returning true if a chain code is set.
	void clearSegments();									  // Clearing previous segments.
	void setAngleOfRotation(const double angle);			  // Setting the angle of rotation (given in radians).

	// PUBLIC METHODS
	bool readFile(std::string file);					      // Reading a file.
	void fillShape();										  // Filling the loaded shape.
	void sweep();											  // Sweeping the object.
};