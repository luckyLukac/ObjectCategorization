#pragma once

#include <string>
#include <vector>

#include "Constants.hpp"
#include "Pixel.hpp"


/// <summary>
/// Structure for storing a chain code.
/// </summary>
struct ChainCode {
	std::vector<short> code;  // Sequence of chain code commands.
	bool clockwise;			  // Clockwise orientation (true or false).
	Pixel startPoint;	      // Start point of the object.

	/// <summary>
	/// Constructor of the structure.
	/// </summary>
	/// <param name="chainCode">: string that forms the F8 chain code</param>
	/// <param name="clockwise">: clockwise orientation if true, anti-clockwise otherwise</param>
	/// <param name="startPoint">: starting pixel in the raster space</param>
	/// <param name="rotation">: rotation on the level of the F8 chain code (0-7)</param>
	/// <param name="isF4">: true if the given code is F4, false otherwise</param>
	ChainCode(const std::string& chainCode, const bool clockwise, const Pixel& startPoint, const uint rotation = 0, const bool isF4 = false);

	/// <summary>
	/// Method for flipping the chain code orientation from clockwise to anti-clockwise and vice-versa.
	/// </summary>
	void flipOrientation();
};

