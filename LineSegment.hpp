#pragma once

#include <sstream>
#include <string>
#include <vector>

#include "Pixel.hpp"


// Line segment with two end points.
template <typename T>
struct LineSegmentT {
    // CLASS VARIABLES
    Pixel p1;      // First end point that limits the line segment.
    Pixel p2;      // Second end point that limits the line segment.
       
    // CONSTRUCTORS
    // Constructor with both end points.
    LineSegmentT(const Pixel& p1, const Pixel& p2) :
        p1(p1),
        p2(p2)
    {}


    // OBJECT METHODS
    // Line segment length getter.
    T length() const {
        return static_cast<T>(std::sqrt(std::pow(p1.x - p2.x, 2) + std::pow(p1.y - p2.y, 2)));
    }

    // Checking whether the two line segments are equal.
    bool operator == (const LineSegmentT& ls) const {
        return (
            (p1 == ls.p1 && p2 == ls.p2) ||
            (p1 == ls.p2 && p2 == ls.p1)
        );
    }

    // Checking if line segment is smaller than the other.
    bool operator < (const LineSegmentT& ls) const {
        return p1.x < ls.p1.x || p1.y < ls.p1.y || p2.x < ls.p2.x || p2.y < ls.p2.y;
    }

    // Calculating the center point of the line segment.
    Pixel getCenterPoint() const {
        return (
            Pixel(
                (p1.x + p2.x) / 2,
                (p1.y + p2.y) / 2,
            )
        );
    }
};


// ALIASES
using LineSegment = LineSegmentT<double>;