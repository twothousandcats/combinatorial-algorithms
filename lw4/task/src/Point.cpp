#include "Point.h"
#include <cmath>

Point::Point(const double xVal, const double yVal)
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

// Find the Fermat-Torricelli point using Weiszfeld's algorithm
// For a triangle with all angles < 120°, this is the point where all three segments meet at 120°
// If any angle >= 120°, the Fermat point is at that vertex
Point GetFermatPoint(const Point& a, const Point& b, const Point& c)
{
	// is any angle is >= 120 degrees
	// cos to check angles
	const double ab = Distance(a, b);
	const double bc = Distance(b, c);
	const double ac = Distance(a, c);

	// Check angle at A (opposite to side BC)
	// cos(A) = (AB² + AC² - BC²) / (2 * AB * AC)
	if (ab > 1e-9 && ac > 1e-9)
	{
		const double cosA = (ab * ab + ac * ac - bc * bc) / (2.0 * ab * ac);
		if (cosA <= -0.5) // angle >= 120 degrees (cos(120°) = -0.5)
		{
			return a;
		}
	}

	// Check angle at B (opposite to side AC)
	if (ab > 1e-9 && bc > 1e-9)
	{
		const double cosB = (ab * ab + bc * bc - ac * ac) / (2.0 * ab * bc);
		if (cosB <= -0.5) // angle >= 120 degrees
		{
			return b;
		}
	}

	// Check angle at C (opposite to side AB)
	if (ac > 1e-9 && bc > 1e-9)
	{
		const double cosC = (ac * ac + bc * bc - ab * ab) / (2.0 * ac * bc);
		if (cosC <= -0.5) // angle >= 120 degrees
		{
			return c;
		}
	}

	// All angles < 120°, use Weiszfeld's algorithm to find Fermat point
	Point p = GetCentroid(a, b, c); // Start from centroid

	const int maxIterations = 1000;
	const double epsilon = 1e-9;

	for (int iter = 0; iter < maxIterations; ++iter)
	{
		const double da = Distance(p, a);
		const double db = Distance(p, b);
		const double dc = Distance(p, c);

		// Handle case when p coincides with one of the points
		if (da < epsilon)
			return a;
		if (db < epsilon)
			return b;
		if (dc < epsilon)
			return c;

		// Weiszfeld iteration: weighted average with weights = 1/distance
		const double wA = 1.0 / da;
		const double wB = 1.0 / db;
		const double wC = 1.0 / dc;

		const double sumWeights = wA + wB + wC;

		const double newX = (wA * a.x + wB * b.x + wC * c.x) / sumWeights;
		const double newY = (wA * a.y + wB * b.y + wC * c.y) / sumWeights;

		Point newP(newX, newY);

		if (Distance(p, newP) < epsilon)
		{
			break;
		}

		p = newP;
	}

	return p;
}
}