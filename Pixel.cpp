#include "Pixel.hpp"

Pixel::Pixel() :
	x(0),
	y(0),
	position(Position::undefined)
{}

Pixel::Pixel(const int x, const int y, const Position& position) :
	x(x),
	y(y),
	position(position)
{}