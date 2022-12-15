#include <numbers>

#include "HelperFunctions.hpp"


// SIMPLE FUNCTIONS
// Getting the sign of the value.
short sign(const int value) {
	// +
	if (value > 0) {
		return 1;
	}
	// 0
	else if (value == 0) {
		return 0;
	}
	// -
	else {
		return -1;
	}
}
 
// Converting degrees to radians.
double toRadians(const double degrees) {
	return degrees * std::numbers::pi / 180;
}

// Converting radians to degrees.
double toDegrees(const double radians) {
	return 180 * radians / std::numbers::pi;
}

// Checking whether the two double value are in tolerance.
bool isInTolerance(const double value, const double desiredValue, const double tolerance) {
	// If the absolute value of the difference is bigger than the allowed tolerance,
	// the value is not in tolerance with the desired value.
	if (std::abs(value - desiredValue) > tolerance) {
		return false;
	}

	return true;
}



// BRESENHAM
// Getting start and end coordinates for Bresenham algorithm.
Pixel getEndPointForBresenham(const Pixel& start, const double angle, const int maxCoordinate) {
	// If the angle equals 45°, the solution is trivial.
	if (isInTolerance(angle, std::numbers::pi / 2)) {
		return Pixel(start.x, maxCoordinate);
	}
	
	// Otherwise, some calculations have to be done.
	// Getting the reference coordinates.
	const int xRef = start.x;
	const int yRef = start.y;
	const int xCandidate = xRef + (maxCoordinate - yRef) / angle;

	// If the angle is smaller than 45°.
	if (angle < std::numbers::pi / 2) {
		// If the X candidate coordinate is smaller than the maximal coordinate, we are all good.
		if (xCandidate < maxCoordinate) {
			return Pixel(xCandidate, maxCoordinate);
		}
		// Otherwise, the X coordinate is bound by the maximum coordinate.
		else {
			return Pixel(maxCoordinate, yRef + (maxCoordinate - xRef) * angle);
		}
	}
	// If the angle is greater than 45°.
	else {
		const int newXCandidate = xCandidate - xRef;

		// If the new X coordinate is greater than 0, we are all good.
		if (newXCandidate < 0) {
			return Pixel(0, yRef - angle * xRef);
		}
		// Otherwise, the X coordinate is bound by the minimum coordinate.
		else {
			return Pixel(newXCandidate, maxCoordinate);
		}
	}
}

// Bresenham rasterization algorithm.
std::vector<Pixel> bresenham(const Pixel& startPoint, const Pixel& endPoint) {
	std::vector<Pixel> pixels;
	
	// Calculating the difference between the both coordinates.
	const int deltaX = endPoint.x - startPoint.x;
	const int deltaY = endPoint.y - startPoint.y;
	
	// Getting the signs of both deltas.
	const short signX = sign(deltaX);
	const short signY = sign(deltaY);

	// Helper values for coordinate calculation.
	const int ax = std::abs(deltaX) << 1;
	const int ay = std::abs(deltaY) << 1;
	
	// Starting X and Y coordinates.
	int x = startPoint.x;
	int y = startPoint.y;
	int error;

	if (ax >= ay) {
		// Calculating the current error.
		error = ay - (ax >> 1);

		for (int i = 0; i <= std::abs(deltaX); i++) {
			// Adding the new pixel to the vector.
			pixels.push_back(Pixel(x, y));

			// Adding the correct values to X and Y coordinates.
			if (error >= 0) {
				error -= ax;
				y += signY;
			}

			error += ay;
			x += signX;
		}
	}
	else {
		// Calculating the current error.
		error = ax - (ay >> 1);

		for (int i = 0; i <= std::abs(deltaY); i++) {
			// Adding the new pixel to the vector.
			pixels.push_back(Pixel(x, y));

			// Adding the correct values to X and Y coordinates.
			if (error > 0) {
				error -= ay;
				x += signX;
			}

			error += ax;
			y += signY;
		}
	}

	return pixels;
}