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
	
	// Angles in range: (0°, 45°).
	if (angle < toRadians(45.0)) {
		const int x = start.y * std::tan(toRadians(90) - angle);
		const int y = (x - maxCoordinate) * std::tan(angle);
		const int y2 = maxCoordinate - (maxCoordinate - start.x) * std::tan(angle);
	
		// If the X candidate coordinate is smaller than the maximal coordinate, we are all good.
		if (x < maxCoordinate) {
			return Pixel(x, 0);
		}
		else if (start.y < maxCoordinate) {
			return Pixel(maxCoordinate, y);
		}
		else {
			return Pixel(maxCoordinate, y2);
		}
	}
	// Angles in range: (45°, 90°).
	else if (angle < toRadians(90.0)) {
		const int x = start.y * std::tan(toRadians(90) - angle);
		const int x2 = start.x + maxCoordinate * std::tan(toRadians(90) - angle);
		const int y = maxCoordinate - (maxCoordinate - start.x) / std::tan(toRadians(90) - angle);

		// If the X candidate coordinate is smaller than the maximal coordinate, we are all good.
		if (start.y < maxCoordinate) {
			return Pixel(x, 0);
		}
		else if (x2 < maxCoordinate) {
			return Pixel(x2, 0);
		}
		else {
			return Pixel(maxCoordinate, y);
		}
	}
	// Angles in range: (90°, 135°).
	else if (angle < toRadians(135.0)) {
		const int y = maxCoordinate - (start.x / std::tan(angle - toRadians(90)));
		const int x = -y * std::tan(angle - toRadians(90));
		const int x2 = maxCoordinate - (start.y / std::tan(toRadians(90) - (angle - toRadians(90))));

		if (y > 0) {
			return Pixel(0, y);
		}
		else if (start.x < maxCoordinate) {
			return Pixel(x, 0);
		}
		else {
			return Pixel(x2, 0);
		}
	}
	// Angles in range: (135°, 180°).
	else if (angle < toRadians(180.0)) {
		const int y = maxCoordinate - (start.x / std::tan(angle - toRadians(90)));
		const int y2 = start.y - maxCoordinate * std::tan(toRadians(90) - (angle - toRadians(90)));
		const int x = maxCoordinate - (start.y / std::tan(toRadians(90) - (angle - toRadians(90))));

		if (start.x < maxCoordinate) {
			return Pixel(0, y);
		}
		else if (y2 > 0) {
			return Pixel(0, y2);
		}
		else {
			return Pixel(x, 0);
		}
	}

	return Pixel();
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

// Rotation of a point around the reference point for a certain angle.
Pixel rotate2D(const Pixel& point, const Pixel& referencePoint, const double angle) {
	// Translating the point to reference point local coordinate system.
	const int x = point.x - referencePoint.x;
	const int y = point.y - referencePoint.y;

	// Rotating the point.
	const double rotatedX = x * std::cos(angle) - y * std::sin(angle);
	const double rotatedY = x * std::sin(angle) + y * std::cos(angle);

	// Translating the point back to the global coordinate system.
	const int newX = std::round(rotatedX + referencePoint.x);
	const int newY = std::round(rotatedY + referencePoint.y);

	return Pixel(newX, newY, point.position);
}