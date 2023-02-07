#pragma once

#include <vector>

#include "Constants.hpp"
#include "LineSegment.hpp"
#include "Pixel.hpp"


/// <summary>
/// Chain structure.
/// </summary>
struct Chain {
	std::vector<Pixel> pixels;  // The obtained pixels.
	double angle = 0.0;			// Sweep-line angle.

	/// <summary>
	/// Calculation of the total length of all line segments between the pixel pairs.
	/// </summary>
	/// <returns>total length of the chain</returns>
	double totalLength() const {
		double length = 0.0;

		for (uint i = 1; i < pixels.size(); i++) {
			length += LineSegment(pixels[i - 1], pixels[i]).length();
		}

		return length;
	}
};
