#pragma once

#include <vector>
#include <wx/dcbuffer.h>

#include "ChainCode.hpp"
#include "Pixel.hpp"


// CONSTANTS
const double MAGNIFY_FACTOR = 1.5;


// STRUCTURES
struct MidPoint {
	Pixel point;
	//Pixel leftOffset;
	//Pixel rightOffset;
	double angle;
	bool valid;
	bool used;

	MidPoint() : angle(0.0), valid(false), used(false) {}
	MidPoint(const Pixel& point, /*const Pixel& leftOffset, const Pixel& rightOffset,*/ const double angle, const bool valid, const bool used) : point(point), /*leftOffset(leftOffset), rightOffset(rightOffset),*/ angle(angle), valid(valid), used(used) {}
};

// Segment struct.
struct Chain {
	std::vector<MidPoint> midPoints;    // The obtained polyline.
	double angle = 0.0;					// Sweep-line angle.
};

// Object bounding box structure.
struct BoundingBox {
	int deltaX;
	int deltaY;

	BoundingBox() : deltaX(0), deltaY(0) {}
	BoundingBox(const int deltaX, const int deltaY) : deltaX(deltaX), deltaY(deltaY) {}
};


// ALIASES
using PixelField = std::vector<std::vector<Pixel>>;


// MAIN CLASS
// Line sweeping class.
class LineSweeping {
public:
	// PRIVATE VARIABLES
	wxWindow* drawWindow = nullptr;		 // Draw window.
	std::vector<ChainCode> chainCodes;   // F4 chain code.
	BoundingBox boundingBox;			 // Bounding box of the object.
	std::vector<Pixel> coordinates;      // Point coordinates.
	PixelField pixelField;				 // Pixel field with pixels and their positions according to the object (edge, outside or inside).
	int maxCoordinate = 0;				 // Maximum coordinate.
	double plotRatio = 1.0;			     // Ratio factor for drawing.

	double angleOfRotation = 0.0;			       // Sweep line angle of rotation [0°-180°].
	std::vector<std::vector<MidPoint>> midPoints;  // Middle points found while sweeping.
	std::vector<Pixel> midPointPixels;			   // List of midpoints.
	std::vector<Chain> chains;		           // Vector of segments.

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
	void plotSegments(wxDC& dc) const;												   // Plotting the segments.

	// GETTERS AND SETTERS
	void setDrawPanel(wxWindow* drawWindow);        		  // Setting draw panel.
	bool isChainCodeSet() const;							  // Returning true if a chain code is set.
	void clearSegments();									  // Clearing previous segments.
	void setAngleOfRotation(const double angle);			  // Setting the angle of rotation (given in radians).

	// PUBLIC METHODS
	bool readFile(std::string file);					      // Reading a file.
	void fillShape();										  // Filling the loaded shape.
	void rotateObject(const double angle);					  // Rotation of the object for a certain angle.
	void sweep();											  // Sweeping the object.
	void extractChains();									  // Extracting segments from the swept object.
	std::vector<Chain> calculateFeatureVector() const;		  // Calculation of a feature vector.
};