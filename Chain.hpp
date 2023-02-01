#pragma once

#include <vector>

#include "LineSegment.hpp"
#include "Pixel.hpp"


// STRUCTURES
struct MidPoint {
	Pixel point;
	double angle;
	bool valid;
	bool used;

	MidPoint() : angle(0.0), valid(false), used(false) {}
	MidPoint(const Pixel& point, const double angle, const bool valid, const bool used) : point(point), angle(angle), valid(valid), used(used) {}
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


/// <summary>
/// Chain structure.
/// </summary>
struct Chain {
	std::vector<LineSegment> lineSegments;  // The obtained polyline.
	double angle = 0.0;						// Sweep-line angle.

	// Getting the total length of all line segments.
	double totalLength() const {
		double length = 0.0;

		for (const LineSegment& ls : lineSegments) {
			length += ls.length();
		}

		return length;
	}
};
