#include "Geometry.h"

#include <cmath>

namespace steiner
{
namespace
{

constexpr double kObtuseAngleCosine = -0.5; // cos(120 degrees)
constexpr double kEpsilon = 1e-12;
constexpr int kMaxIterations = 500;

double AngleCosine(const Point& vertex, const Point& left, const Point& right) noexcept
{
	const double ux = left.x - vertex.x;
	const double uy = left.y - vertex.y;
	const double vx = right.x - vertex.x;
	const double vy = right.y - vertex.y;
	const double uNorm = std::sqrt(ux * ux + uy * uy);
	const double vNorm = std::sqrt(vx * vx + vy * vy);
	if (uNorm < kEpsilon || vNorm < kEpsilon)
	{
		return 1.0;
	}
	return (ux * vx + uy * vy) / (uNorm * vNorm);
}

} // namespace

Point ComputeFermatPoint(const Point& a, const Point& b, const Point& c)
{
	// If any interior angle of triangle (a, b, c) is >= 120 degrees,
	// the Fermat point coincides with the corresponding vertex.
	if (AngleCosine(a, b, c) <= kObtuseAngleCosine)
	{
		return a;
	}
	if (AngleCosine(b, a, c) <= kObtuseAngleCosine)
	{
		return b;
	}
	if (AngleCosine(c, a, b) <= kObtuseAngleCosine)
	{
		return c;
	}

	// Otherwise, approximate the Fermat point with Weiszfeld iterations
	// starting from the centroid.
	Point current{};
	current.x = (a.x + b.x + c.x) / 3.0;
	current.y = (a.y + b.y + c.y) / 3.0;

	for (int i = 0; i < kMaxIterations; ++i)
	{
		const double da = Distance(current, a);
		const double db = Distance(current, b);
		const double dc = Distance(current, c);
		if (da < kEpsilon || db < kEpsilon || dc < kEpsilon)
		{
			break;
		}

		const double wa = 1.0 / da;
		const double wb = 1.0 / db;
		const double wc = 1.0 / dc;
		const double wSum = wa + wb + wc;

		Point next{};
		next.x = (wa * a.x + wb * b.x + wc * c.x) / wSum;
		next.y = (wa * a.y + wb * b.y + wc * c.y) / wSum;

		if (Distance(next, current) < kEpsilon)
		{
			current = next;
			break;
		}
		current = next;
	}
	return current;
}

} // namespace steiner