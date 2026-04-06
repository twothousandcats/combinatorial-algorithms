#include "Point.h"
#include <cmath>

Point::Point(double xVal, double yVal)
	: x(xVal), y(yVal)
{
}

namespace Geometry
{
double Distance(const Point& a, const Point& b)
{
	const double dx = a.x - b.x;
	const double dy = a.y - b.y;
	return std::sqrt(dx * dx + dy * dy);
}

Point GetCentroid(const Point& a, const Point& b, const Point& c)
{
	return Point(
		(a.x + b.x + c.x) / 3.0,
		(a.y + b.y + c.y) / 3.0
		);
}
}