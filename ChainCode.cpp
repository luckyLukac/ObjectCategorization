#include <algorithm>

#include "ChainCode.hpp"


ChainCode::ChainCode(const std::string& chainCode, const bool clockwise, const Pixel& startPoint, const uint rotation) :
	clockwise(clockwise),
	startPoint(startPoint)
{
	// Reading the chain code character by character.
	for (const char& ch : chainCode) {
		// If a value is 0, 1, 2 or 3, the value is added to the chain code vector.
		if (ch == '0' || ch == '1' || ch == '2' || ch == '3' || ch == '4' || ch == '5' || ch == '6' || ch == '7') {
			code.push_back(((ch - '0') + rotation) % 8);
		}
	}

	// If the orientation is CCW, we flip it to CW.
	if (!clockwise) {
		flipOrientation();
	}
}

void ChainCode::flipOrientation() {
	// In order to reverse a chain code, the vector must be reversed along with the chain code instructions.
	std::reverse(code.begin(), code.end());
	std::transform(
		code.begin(),
		code.end(),
		code.begin(),
		[](const char& element) {
			return static_cast<short>(((element + 4) % 8));
		}
	);

	// Flipping orientation property.
	clockwise = !clockwise;
}