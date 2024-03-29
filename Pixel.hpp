#pragma once

#include <vector>


// Position enum class.
enum class Position {
	edge,
	outside,
	inside,
	undefined
};


// Point struct.
struct Pixel {
	double x;
	double y;
	Position position;
	short directionPrevious;
	short directionNext;

	Pixel();
	Pixel(const double x, const double y, const Position& position = Position::undefined, const short directionPrevious = -1, const short directionNext = -1);

	bool operator == (const Pixel& pixel) const;
	Pixel operator + (const Pixel& pixel) const;
	Pixel operator / (const double factor) const;

	Pixel floor() const;
};



// ALIASES
using PixelField = std::vector<std::vector<Pixel>>;