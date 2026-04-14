#pragma once

#include "Point.h"

namespace steiner
{

// Returns the Fermat (Torricelli) point of triangle (a, b, c) - the point
// minimizing the sum of Euclidean distances to all three vertices.
Point ComputeFermatPoint(const Point& a, const Point& b, const Point& c);

} // namespace steiner