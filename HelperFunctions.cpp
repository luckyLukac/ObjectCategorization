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

double difference(const double value1, const double value2) {
	return std::abs(value1 - value2);
}

double distance(const Pixel& p1, const Pixel& p2) {
	return std::sqrt(std::pow(p2.x - p1.x, 2) + std::pow(p2.y - p1.y, 2));
}

Pixel getEndPointForBresenham(const Pixel& start, const double angle, const int maxCoordinate) {
	// If the angle equals 45°, the solution is trivial.
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
	
	// Angles in range: (0°, 45°).
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
	// Angles in range: (45°, 90°).
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
	// Angles in range: (90°, 135°).
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
	// Angles in range: (135°, 180°).
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
	const int ax = 2 * std::abs(deltaX);
	const int ay = 2 * std::abs(deltaY);
	
	// Starting X and Y coordinates.
	int x = startPoint.x;
	int y = startPoint.y;
	int error;

	if (ax >= ay) {
		// Calculating the current error.
		error = ay - (ax / 2);

		for (int i = 0; i <= std::abs(deltaX); i++) {
			// Adding the new pixel to the vector.
			if (x >= 0 && x != pixelField.size() && y >= 0 && y != pixelField.size()) {
				pixels.push_back(Pixel(x, y, pixelField[y][x].position, pixelField[y][x].directionPrevious, pixelField[y][x].directionNext));
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
		error = ax - (ay / 2);

		for (int i = 0; i <= std::abs(deltaY); i++) {
			// Adding the new pixel to the vector.
			if (x != pixelField.size() && y != pixelField.size()) {
				pixels.push_back(Pixel(x, y, pixelField[y][x].position, pixelField[y][x].directionPrevious, pixelField[y][x].directionNext));
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

std::vector<Pixel> clearyWyvill(const Pixel& startPoint, const Pixel& endPoint, const PixelField& pixelField, const double angleOfRotation) {
	std::vector<Pixel> pixels;

	short pX = -1;
	short pY = -1;
	if (endPoint.x > startPoint.x) {
		pX = 1;
	}
	if (startPoint.y > startPoint.y) {
		pY = 1;
	}

	// Calculation of delta X and delta Y distances.
	double deltaX = std::abs(1 / std::sin(PI / 2 - angleOfRotation));
	double deltaY = std::abs(1 / std::cos(PI / 2 - angleOfRotation));

	double dX = deltaX;
	double dY = deltaY;

	Pixel currentPixel = startPoint;
	pixels.push_back(Pixel(currentPixel));

	while (
		currentPixel != Pixel(endPoint.x - 1, endPoint.y - 1) &&
		currentPixel != Pixel(endPoint.x, endPoint.y - 1) &&
		currentPixel != Pixel(endPoint.x + 1, endPoint.y - 1) &&
		currentPixel != Pixel(endPoint.x - 1, endPoint.y) &&
		currentPixel != Pixel(endPoint.x, endPoint.y) &&
		currentPixel != Pixel(endPoint.x + 1, endPoint.y) &&
		currentPixel != Pixel(endPoint.x - 1, endPoint.y + 1) &&
		currentPixel != Pixel(endPoint.x, endPoint.y + 1) &&
		currentPixel != Pixel(endPoint.x + 1, endPoint.y + 1)
	)
	{
		if (currentPixel.x >= static_cast<int>(pixelField.size()) || currentPixel.y >= static_cast<int>(pixelField.size())) {
			break;
		}

		if (dX <= dY) {
			currentPixel.x += pX;
			dX += deltaX;
			pixels.push_back(Pixel(currentPixel.x, currentPixel.y, pixelField[currentPixel.y][currentPixel.x].position, pixelField[currentPixel.y][currentPixel.x].directionPrevious, pixelField[currentPixel.y][currentPixel.x].directionNext));
		}
		else {
			currentPixel.y += pY;
			dY += deltaY;
			pixels.push_back(Pixel(currentPixel.x, currentPixel.y, pixelField[currentPixel.y][currentPixel.x].position, pixelField[currentPixel.y][currentPixel.x].directionPrevious, pixelField[currentPixel.y][currentPixel.x].directionNext));
		}
	}
	
	return pixels;
}

std::vector<Pixel> findEdgePixelsWithBresenham(const Pixel& startPoint, const Pixel& endPoint, const PixelField& pixelField) {
	std::vector<Pixel> pixels;

	// Calculating the difference between the both coordinates.
	const double deltaX = endPoint.x - startPoint.x;
	const double deltaY = endPoint.y - startPoint.y;
	const double coefficient = deltaY / deltaX;

	// Starting X and Y coordinates.
	double x = startPoint.x;
	double y = startPoint.y;
	double error;

	if (deltaX >= deltaY) {
		// Calculating the current error.
		error = deltaY / deltaX;

		for (int i = 0; i <= std::abs(deltaX); i++) {
			// Adding the new pixel to the vector.
			const int pixelY = static_cast<int>(y + (error - coefficient));
			if (x >= 0 && x < pixelField.size() && pixelY >= 0 && pixelY < pixelField.size() && pixelField[pixelY][x].position == Position::edge) {
				pixels.push_back(Pixel(x, y + (error - coefficient), pixelField[pixelY][x].position, pixelField[pixelY][x].directionPrevious, pixelField[pixelY][x].directionNext));
			}

			// Adding the correct values to X and Y coordinates.
			if (error > 0.5) {
				error -= 1;
				y += 1;
			}

			error += coefficient;
			x += 1;
		}
	}
	else {
		// Calculating the current error.
		error = deltaX / deltaY;

		for (int i = 0; i <= std::abs(deltaY); i++) {
			// Adding the new pixel to the vector.
			const int pixelX = static_cast<int>(x + (1 / coefficient - error));
			if (pixelX < pixelField.size() && y < pixelField.size() && pixelField[y][pixelX].position == Position::edge) {
				pixels.push_back(Pixel(x + (1 / coefficient - error), y, pixelField[y][pixelX].position, pixelField[y][pixelX].directionPrevious, pixelField[y][pixelX].directionNext));
			}

			// Adding the correct values to X and Y coordinates.
			if (error < -0.5) {
				error += 1;
				x += 1;
			}

			error -= (1 / coefficient);
			y += 1;
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

	if (distance > epsilon && index != 0) {
		std::vector<LineSegment> result1 = douglasPeucker(std::vector<Pixel>(pixels.begin(), pixels.begin() + index), LineSegment(lineSegment.p1, pixels[index]), epsilon);
		std::vector<LineSegment> result2 = douglasPeucker(std::vector<Pixel>(pixels.begin() + index, pixels.end()), LineSegment(pixels[index], lineSegment.p2), epsilon);

		lineSegments.insert(lineSegments.end(), result1.begin(), result1.end());
		lineSegments.insert(lineSegments.end(), result2.begin(), result2.end());
	}
	else {
		lineSegments.push_back(lineSegment);
		//for (uint i = 1; i < pixels.size(); i++) {
			//lineSegments.push_back(LineSegment(pixels[i - 1], pixels[i]));
		//}
	}

	return lineSegments;
}

std::tuple<uint, double> farthestPoint(const std::vector<Pixel>& pixels, const LineSegment& lineSegment) {
	const Pixel& p1 = lineSegment.p1;
	const Pixel& p2 = lineSegment.p2;
	
	uint maxIndex = 0;
	double maxDistance = 0.0;
	
	for (uint i = 0; i < pixels.size(); i++) {
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