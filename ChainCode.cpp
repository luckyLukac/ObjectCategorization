#include <algorithm>

#include "ChainCode.hpp"


ChainCode::ChainCode(const std::string& chainCode, const bool clockwise, const Pixel& startPoint, const uint rotation, const bool isF4) :
	clockwise(clockwise),
	startPoint(startPoint)
{
	const uint scale = 1;

	// Reading the chain code character by character.
	for (const char& ch : chainCode) {
		for (uint i = 0; i < scale; i++) {
			uint order = ch - '0';
			if (isF4) {
				order *= 2;
			}
			order = (order + rotation) % 8;
		
			// If a value is 0, 1, 2 or 3, the value is added to the chain code vector.
			if (order == 0 || order == 2 || order == 4 || order == 6) {
				code.push_back(order);
			}
			else if (order == 1) {
				code.push_back(0);
				code.push_back(2);
			}
			else if (order == 3) {
				code.push_back(2);
				code.push_back(4);
			}
			else if (order == 5) {
				code.push_back(4);
				code.push_back(6);
			}
			else if (order == 7) {
				code.push_back(6);
				code.push_back(0);
			}
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