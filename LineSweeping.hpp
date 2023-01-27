#pragma once

#include <queue>
#include <vector>

#include "ChainCode.hpp"
#include "FeatureVector.hpp"
#include "Pixel.hpp"
#include "stdafx.h"


// CONSTANTS
const double MAGNIFY_FACTOR = 1.5;


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
	std::vector<Chain> chains;		               // Vector of segments.

	// PRIVATE HELPER METHODS
	void calculateCoordinatesFromChainCode();																																	   // Transforming chain code to coordinates.
	void calculateBoundingBox();																																				   // Calculation of a bounding box according to point coordinates.
	void fillRectangle(wxDC& dc, const int x, const int y, const int pixelSize, const int maxCoordinate, const wxPen& pen, const wxBrush& brush, const double ratio = 1.0) const;  // Filling a rectangle at X and Y coordinates.
	std::vector<Pixel> findEdgePixels(const std::vector<Pixel>& rasterizedLine) const;																							   // Finding edge pixel pairs.
	void addCurrentMidpointFromSweepLine(const std::vector<Pixel>& rasterizedLine);																								   // Adding the current midpoint and plotting the Bresenham line.
	void chainExtractionPixelCheck(std::queue<MidPoint>& queue, const int x, const int y);																						   // Checking whether the pixel should belong to the current chain.


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
	bool readFileF8(std::string file, const uint rotation = 0);		// Reading an F4 chain code file.
	void fillShape();												// Filling the loaded shape.
	void rotateObject(const double angle);							// Rotation of the object for a certain angle.
	void sweep();													// Sweeping the object.
	void extractChains();											// Extracting segments from the swept object.
	FeatureVector calculateFeatureVector() const;				    // Calculation of a feature vector.
	double calculateScore(const std::vector<Chain>& chains) const;  // Calculation of the object score.
};