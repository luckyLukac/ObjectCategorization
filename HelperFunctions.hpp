#pragma once

#include <tuple>

#include "LineSweeping.hpp"


// SIMPLE FUNCTIONS
double toRadians(const double degrees);																 // Converting degrees to radians.
double toDegrees(const double radians);																 // Converting radians to degrees.
bool isInTolerance(const double value, const double desiredValue, const double tolerance = 0.0001);  // Checking whether the two double value are in tolerance.

// BRESENHAM
std::pair<Point, Point> getCoordinatesForBresenham(const Point& start, const double angle, const int maxCoordinate);  // Getting start and end coordinates for Bresenham algorithm.