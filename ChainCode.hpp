#pragma once

#include <string>
#include <vector>

#include "Pixel.hpp"


struct ChainCode {
	std::vector<short> code;  // Sequence of chain code commands.
	bool clockwise;			  // Clockwise orientation (true or false).
	Pixel startPoint;	      // Start point of the object.

	ChainCode(const std::string& chainCode, const bool clockwise, const Pixel& startPoint);  // Main constructor.
	void flipOrientation();																     // Method for changing the CW to CCW orientation and vice-versa.
};

