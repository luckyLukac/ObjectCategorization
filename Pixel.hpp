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
	int x;
	int y;
	Position position;
	short directionPrevious;
	short directionNext;

	Pixel();
	Pixel(const int x, const int y, const Position& position = Position::undefined, const short directionPrevious = -1, const short directionNext = -1);

	bool operator == (const Pixel& pixel) const;
};


// ALIASES
using PixelField = std::vector<std::vector<Pixel>>;