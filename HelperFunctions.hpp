#pragma once

#include <tuple>

#include "Constants.hpp"
#include "LineSegment.hpp"
#include "LineSweeping.hpp"


// SIMPLE FUNCTIONS
/// <summary>
/// Determining whether a number is positive, negative, or zero.
/// </summary>
/// <param name="value">: number</param>
/// <returns>1 if positive, -1 if negative, 0 otherwise</returns>
short sign(const int value);

/// <summary>
/// Converting degrees to radians.
/// </summary>
/// <param name="degrees">: value in degrees</param>
/// <returns>value in radians</returns>
double toRadians(const double degrees);

/// <summary>
/// Converting radians to degrees.
/// </summary>
/// <param name="radians">: value in radians</param>
/// <returns>value in degrees</returns>
double toDegrees(const double radians);

/// <summary>
/// Checking whether a number is within the vicinity of another number.
/// </summary>
/// <param name="value">: number</param>
/// <param name="desiredValue">: number representing the center of the vicinity</param>
/// <param name="tolerance">: size of the vicinity</param>
/// <returns>true if within the vicinity, false otherwise</returns>
bool isInTolerance(const double value, const double desiredValue, const double tolerance = 0.0001);

/// <summary>
/// Calculation of a difference between two values.
/// </summary>
/// <param name="value1">: first value</param>
/// <param name="value2">: second value</param>
/// <returns>difference</returns>
double difference(const double value1, const double value2);

/// <summary>
/// Calculation of a Euclidean distance between two pixels.
/// </summary>
/// <param name="p1">: start pixel</param>
/// <param name="p2">: end pixel</param>
/// <returns>Euclidean distance</returns>
double distance(const Pixel& p1, const Pixel& p2);



// RASTERIZATION
/// <summary>
/// Calculation of end pixel of the line segment (input for Bresenham algorithm).
/// </summary>
/// <param name="start">: start pixel of the line segment</param>
/// <param name="angle">: angle of the line segment in radians</param>
/// <param name="maxCoordinate">: maximum coordinate of the bounding box</param>
/// <returns>end pixel of the line segment</returns>
Pixel getEndPointForBresenham(const Pixel& start, const double angle, const int maxCoordinate);

/// <summary>
/// Bresenham rasterization algorithm.
/// </summary>
/// <param name="startPoint">: start pixel of the line segment</param>
/// <param name="endPoint">: end pixel of the line segment</param>
/// <param name="pixelField">: pixel field of the rasterized scene with an object</param>
/// <returns>list of pixels of the rasterized line segment</returns>
std::vector<Pixel> bresenham(const Pixel& startPoint, const Pixel& endPoint, const PixelField& pixelField);


/// <summary>
/// Cleary-Wyvill rasterization algorithm.
/// </summary>
/// <param name="startPoint">: start pixel of the line segment</param>
/// <param name="endPoint">: end pixel of the line segment</param>
/// <param name="pixelField">: pixel field of the rasterized scene with an object</param>
/// <param name="angleOfRotation">: angle of the line segment in radians</param>
/// <returns>list of pixels of the rasterized line segment</returns>
std::vector<Pixel> clearyWyvill(const Pixel& startPoint, const Pixel& endPoint, const PixelField& pixelField, const double angleOfRotation);



// ROTATION FUNCTIONS
/// <summary>
/// Rotation of a point around the reference point for a certain angle.
/// </summary>
/// <param name="point">: point to be rotated</param>
/// <param name="referencePoint">: reference point for the rotation</param>
/// <param name="angle">: angle of the rotation in radians</param>
/// <returns>rotated pixel</returns>
Pixel rotate2D(const Pixel& point, const Pixel& referencePoint, const double angle);



// DOUGLAS-PEUCKER ALGORITHM
/// <summary>
/// Douglas-Peucker algorithm for decimating a curve given with pixels.
/// </summary>
/// <param name="pixels">: list of pixels</param>
/// <param name="lineSegment">: current line segment</param>
/// <param name="epsilon">: tolerance</param>
/// <returns>list of constructed line segments</returns>
std::vector<LineSegment> douglasPeucker(const std::vector<Pixel>& pixels, const LineSegment& lineSegment, const double epsilon);

/// <summary>
/// Calculation of the farthest point from a line segment.
/// </summary>
/// <param name="pixels">: list of pixels</param>
/// <param name="lineSegment">: line segment for the distance calculation</param>
/// <returns>tuple (index of the farthest point, distance)</returns>
std::tuple<uint, double> farthestPoint(const std::vector<Pixel>& pixels, const LineSegment& lineSegment);