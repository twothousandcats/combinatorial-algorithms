#pragma once
#include <vector>
#include <cmath>
#include <algorithm>
#include <numeric>
#include <limits>

namespace geometry
{

struct Point
{
	double x{};
	double y{};

	Point() = default;

	Point(double x, double y)
		: x(x), y(y)
	{
	}

	bool operator==(const Point& other) const;
};

double Distance(const Point& a, const Point& b);

// Torricelli point (Fermat point) for a triangle if all angles < 120 degrees
// Otherwise returns the vertex with the obtuse angle
Point GetTorricelliPoint(const Point& a, const Point& b, const Point& c);

} // namespace geometry