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

// Approximates a Euclidean Steiner tree for the given terminals by
// iteratively inserting Fermat points into a Boruvka MST of the current
// vertex set whenever such an insertion shortens the total length.
class SteinerTreeBuilder
{
public:
	SteinerTreeResult Build(const std::vector<Point>& terminals) const;
};

} // namespace steiner