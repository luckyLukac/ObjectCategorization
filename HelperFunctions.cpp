#include <numbers>

#include "HelperFunctions.hpp"


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
 
double toRadians(const double degrees) {
	return degrees * std::numbers::pi / 180;
}

double toDegrees(const double radians) {
	return 180 * radians / std::numbers::pi;
}

bool isInTolerance(const double value, const double desiredValue, const double tolerance) {
	// If the absolute value of the difference is bigger than the allowed tolerance,
	// the value is not in tolerance with the desired value.
	if (std::abs(value - desiredValue) > tolerance) {
		return false;
	}

	return true;
}

uint difference(const int value1, const int value2) {
	return std::abs(value1 - value2);
}

double distance(const Pixel& p1, const Pixel& p2) {
	return std::sqrt(std::pow(p2.x - p1.x, 2) + std::pow(p2.y - p1.y, 2));
}

Pixel getEndPointForBresenham(const Pixel& start, const double angle, const int maxCoordinate) {
	// If the angle equals 45�, the solution is trivial.
	if (isInTolerance(angle, std::numbers::pi / 2)) {
		return Pixel(start.x, maxCoordinate);
	}
	else if (isInTolerance(angle, std::numbers::pi / 4)) {
		if (start.y != maxCoordinate) {
			return Pixel(start.y, 0);
		}
		else {
			return Pixel(maxCoordinate, start.x);
		}
	}
	else if (isInTolerance(angle, 3 * std::numbers::pi / 4)) {
		if (start.x != maxCoordinate) {
			return Pixel(0, maxCoordinate - start.x);
		}
		else {
			return Pixel(maxCoordinate - start.y, 0);
		}
	}
	
	// Angles in range: (0�, 45�).
	if (angle < toRadians(45.0)) {
		const int x = static_cast<int>(start.y * std::tan(toRadians(90) - angle));
		const int y = static_cast<int>((x - maxCoordinate) * std::tan(angle));
		const int y2 = static_cast<int>(maxCoordinate - (maxCoordinate - start.x) * std::tan(angle));
	
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
	// Angles in range: (45�, 90�).
	else if (angle < toRadians(90.0)) {
		const int x = static_cast<int>(start.y * std::tan(toRadians(90) - angle));
		const int x2 = static_cast<int>(start.x + maxCoordinate * std::tan(toRadians(90) - angle));
		const int y = static_cast<int>(maxCoordinate - (maxCoordinate - start.x) / std::tan(toRadians(90) - angle));

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
	// Angles in range: (90�, 135�).
	else if (angle < toRadians(135.0)) {
		const int y = static_cast<int>(maxCoordinate - (start.x / std::tan(angle - toRadians(90))));
		const int x = static_cast<int>(-y * std::tan(angle - toRadians(90)));
		const int x2 = static_cast<int>(maxCoordinate - (start.y / std::tan(toRadians(90) - (angle - toRadians(90)))));

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
	// Angles in range: (135�, 180�).
	else if (angle < toRadians(180.0)) {
		const int y = static_cast<int>(maxCoordinate - (start.x / std::tan(angle - toRadians(90))));
		const int y2 = static_cast<int>(start.y - maxCoordinate * std::tan(toRadians(90) - (angle - toRadians(90))));
		const int x = static_cast<int>(maxCoordinate - (start.y / std::tan(toRadians(90) - (angle - toRadians(90)))));

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

std::vector<Pixel> bresenham(const Pixel& startPoint, const Pixel& endPoint, const PixelField& pixelField) {
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
			if (x >= 0 && x != pixelField.size() && y >= 0 && y != pixelField.size()) {
				pixels.push_back(Pixel(x, y, pixelField[y][x].position));
			}

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
			if (x != pixelField.size() && y != pixelField.size()) {
				pixels.push_back(Pixel(x, y));
			}

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

Pixel rotate2D(const Pixel& point, const Pixel& referencePoint, const double angle) {
	// Translating the point to reference point local coordinate system.
	const int x = point.x - referencePoint.x;
	const int y = point.y - referencePoint.y;

	// Rotating the point.
	const double rotatedX = x * std::cos(angle) - y * std::sin(angle);
	const double rotatedY = x * std::sin(angle) + y * std::cos(angle);

	// Translating the point back to the global coordinate system.
	const int newX = static_cast<int>(std::round(rotatedX + referencePoint.x));
	const int newY = static_cast<int>(std::round(rotatedY + referencePoint.y));

	return Pixel(newX, newY, point.position);
}

std::vector<LineSegment> douglasPeucker(const std::vector<Pixel>& pixels, const LineSegment& lineSegment, const double epsilon) {
	std::vector<LineSegment> lineSegments;

	const auto [index, distance] = farthestPoint(pixels, lineSegment);  // Detection of the farthest pixel.

	if (distance > epsilon) {
		std::vector<LineSegment> result1 = douglasPeucker(std::vector<Pixel>(pixels.begin() + 1, pixels.begin() + index), LineSegment(lineSegment.p1, pixels[index]), epsilon);
		std::vector<LineSegment> result2 = douglasPeucker(std::vector<Pixel>(pixels.begin() + index, pixels.end() - 1), LineSegment(pixels[index], lineSegment.p2), epsilon);

		lineSegments.insert(lineSegments.end(), result1.begin(), result1.end());
		lineSegments.insert(lineSegments.end(), result2.begin(), result2.end());
	}
	else {
		for (uint i = 1; i < pixels.size(); i++) {
			lineSegments.push_back(LineSegment(pixels[i - 1], pixels[i]));
		}
	}

	return lineSegments;
}

std::tuple<uint, double> farthestPoint(const std::vector<Pixel>& pixels, const LineSegment& lineSegment) {
	const Pixel& p1 = lineSegment.p1;
	const Pixel& p2 = lineSegment.p2;
	
	uint maxIndex = 0;
	double maxDistance = 0.0;
	
	for (int i = 0; i < pixels.size(); i++)	{
		const double currentDistance = (
			std::abs((p2.x - p1.x) * (p1.y - pixels[i].y) - (p1.x - pixels[i].x) * (p2.y - p1.y)) /
			std::sqrt(std::pow(p2.x - p1.x, 2) + std::pow(p2.y - p1.y, 2))
		);
		
		if (currentDistance > maxDistance) {
			maxIndex = i;
			maxDistance = currentDistance;
		}
	}

	return std::make_tuple(maxIndex, maxDistance);
}