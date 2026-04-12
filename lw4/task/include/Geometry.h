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

// Fermat point
Point GetFermatPoint(const Point& a, const Point& b, const Point& c);

} // namespace geometry