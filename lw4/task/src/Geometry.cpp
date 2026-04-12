#include "Geometry.h"

namespace geometry
{

bool Point::operator==(const Point& other) const
{
	return std::abs(x - other.x) < 1e-9 && std::abs(y - other.y) < 1e-9;
}

double Distance(const Point& a, const Point& b)
{
	double dx = a.x - b.x;
	double dy = a.y - b.y;
	return std::sqrt(dx * dx + dy * dy);
}

Point GetTorricelliPoint(const Point& a, const Point& b, const Point& c)
{
	double ab2 = Distance(a, b) * Distance(a, b);
	double bc2 = Distance(b, c) * Distance(b, c);
	double ca2 = Distance(c, a) * Distance(c, a);

	// Check if any angle >= 120 degrees using cosine rule
	// If A >= 120, cos(A) <= -0.5 => a^2 >= b^2 + c^2 + bc
	if (bc2 >= ab2 + ca2 + std::sqrt(ab2 * ca2))
		return a;
	if (ca2 >= ab2 + bc2 + std::sqrt(ab2 * bc2))
		return b;
	if (ab2 >= bc2 + ca2 + std::sqrt(bc2 * ca2))
		return c;

	// Exact calculation for Fermat point using rotation method
	auto rotate = [](const Point& p, const Point& center, double angle) {
		double s = std::sin(angle);
		double c = std::cos(angle);
		double dx = p.x - center.x;
		double dy = p.y - center.y;
		return Point(
			center.x + dx * c - dy * s,
			center.y + dx * s + dy * c
			);
	};

	// Rotate B by -60 degrees around A to get B'
	// Rotate C by +60 degrees around A to get C'
	// Intersection of lines BB' and CC' gives the Fermat point
	Point bRot = rotate(b, a, -M_PI / 3.0);
	Point cRot = rotate(c, a, M_PI / 3.0);

	// Line intersection: line through (b, bRot) and line through (c, cRot)
	// Using parametric form: P = b + t*(bRot-b) = c + u*(cRot-c)
	double dx1 = bRot.x - b.x;
	double dy1 = bRot.y - b.y;
	double dx2 = cRot.x - c.x;
	double dy2 = cRot.y - c.y;

	double det = dx1 * dy2 - dy1 * dx2;
	if (std::abs(det) < 1e-12)
	{
		// Lines are nearly parallel, fallback to centroid
		return Point((a.x + b.x + c.x) / 3.0, (a.y + b.y + c.y) / 3.0);
	}

	double t = ((c.x - b.x) * dy2 - (c.y - b.y) * dx2) / det;
	return Point(b.x + t * dx1, b.y + t * dy1);
}

} // namespace geometry