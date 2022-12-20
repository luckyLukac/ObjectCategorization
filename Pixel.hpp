#pragma once

// Point struct.
struct Pixel {
	int x;
	int y;
	int segmentID;

	Pixel();
	Pixel(const int x, const int y, const int segmentID = -1);
};