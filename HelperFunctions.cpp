#include <numbers>

#include "HelperFunctions.hpp"


// SIMPLE FUNCTIONS
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
std::pair<Point, Point> getCoordinatesForBresenham(const Point& start, const double angle, const int maxCoordinate) {
	// If the angle equals 45°, the solution is trivial.
	if (isInTolerance(angle, std::numbers::pi / 2)) {
		return std::make_pair(start, Point(start.x, maxCoordinate));
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
			return std::make_pair(start, Point(xCandidate, maxCoordinate));
		}
		// Otherwise, the X coordinate is bound by the maximum coordinate.
		else {
			return std::make_pair(start, Point(maxCoordinate, yRef + (maxCoordinate - xRef) * angle));
		}
	}
	// If the angle is greater than 45°.
	else {
		const int newXCandidate = xCandidate - xRef;

		if (newXCandidate < 0) {
			return std::make_pair(start, Point(0, yRef - angle * xRef));
		}
		else {
			return std::make_pair(start, Point(newXCandidate, maxCoordinate));
		}
	}
}