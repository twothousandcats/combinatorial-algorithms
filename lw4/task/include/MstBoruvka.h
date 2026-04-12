#pragma once
#include "Graph.h"
#include <vector>
#include <numeric>
#include <algorithm>

class MstBoruvka
{
public:
	static Graph Compute(const Graph& inputGraph);
};