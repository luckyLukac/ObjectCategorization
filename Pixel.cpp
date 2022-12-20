#include "Pixel.hpp"

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