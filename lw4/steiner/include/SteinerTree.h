#pragma once

#include "Edge.h"
#include "Point.h"

#include <vector>

namespace steiner
{

struct SteinerTreeResult
{
	std::vector<Point> vertices; // terminals followed by generated Steiner points
	std::vector<Edge> edges;
	double totalLength = 0.0;
};

class SteinerTreeBuilder
{
public:
	static SteinerTreeResult Build(const std::vector<Point>& terminals);
};

} // namespace steiner