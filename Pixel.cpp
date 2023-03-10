#include <cmath>

#include "Pixel.hpp"


Pixel::Pixel() :
	x(0),
	y(0),
	position(Position::undefined),
	directionPrevious(-1),
	directionNext(-1)
{}

Pixel::Pixel(const double x, const double y, const Position& position, const short directionPrevious, const short directionNext) :
	x(x),
	y(y),
	position(position),
	directionPrevious(directionPrevious),
	directionNext(directionNext)
{}



bool Pixel::operator == (const Pixel& pixel) const {
	return x == pixel.x && y == pixel.y;
}

Pixel Pixel::operator + (const Pixel& pixel) const {
	return Pixel(x + pixel.x, y + pixel.y);
}

Pixel Pixel::operator / (const double factor) const {
	return Pixel(x / factor, y / factor);
}


Pixel Pixel::floor() const {
	Pixel pixel = *this;
	pixel.x = std::floor(pixel.x);
	pixel.y = std::floor(pixel.y);

	return pixel;
}