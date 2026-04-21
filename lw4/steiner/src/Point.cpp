#include "Point.h"

#include <cmath>

namespace steiner
{

double Distance(const Point& a, const Point& b) noexcept
{
	const double dx = a.x - b.x;
	const double dy = a.y - b.y;
	return std::sqrt(dx * dx + dy * dy);
}

} // namespace steiner