#pragma once

struct Point
{
	double x{};
	double y{};

	Point() = default;

	Point(double xVal, double yVal);
};

// geometry calculations helpers
namespace Geometry
{
double Distance(const Point& a, const Point& b);

Point GetCentroid(const Point& a, const Point& b, const Point& c);
}