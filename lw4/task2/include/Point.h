#pragma once

#include <string>

namespace steiner
{

struct Point
{
	std::string name;
	double x = 0.0;
	double y = 0.0;
};

double Distance(const Point& a, const Point& b) noexcept;

} // namespace steiner