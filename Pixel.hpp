#pragma once

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

	Pixel();
	Pixel(const int x, const int y, const Position& position = Position::undefined);
};