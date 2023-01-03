#pragma once

#include <tuple>

#include "LineSweeping.hpp"


// SIMPLE FUNCTIONS
short sign(const int value);																		 // Getting the sign of the value.
double toRadians(const double degrees);																 // Converting degrees to radians.
double toDegrees(const double radians);																 // Converting radians to degrees.
bool isInTolerance(const double value, const double desiredValue, const double tolerance = 0.0001);  // Checking whether the two double value are in tolerance.

// BRESENHAM
Pixel getEndPointForBresenham(const Pixel& start, const double angle, const int maxCoordinate);      // Getting start and end coordinates for Bresenham algorithm.
std::vector<Pixel> bresenham(const Pixel& startPoint, const Pixel& endPoint);						 // Bresenham rasterization algorithm.

// ROTATION FUNCTIONS
Pixel rotate2D(const Pixel& point, const Pixel& referencePoint, const double angle);				 // Rotation of a point around the reference point for a certain angle.