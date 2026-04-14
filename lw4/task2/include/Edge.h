#pragma once

#include <cstddef>

namespace steiner
{

struct Edge
{
	std::size_t from = 0;
	std::size_t to = 0;
	double weight = 0.0;
};

} // namespace steiner