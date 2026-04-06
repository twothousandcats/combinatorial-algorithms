#pragma once

#include "Point.h"

#include <vector>
#include <string>

using PointId = int;

struct Edge
{
	PointId u{};
	PointId v{};
	double weight{};
};

struct TreeResult
{
	std::vector<Edge> edges;
	double totalLength{};
};

// Structure to hold input data with names
struct Terminal
{
	std::string name;
	Point position;
};