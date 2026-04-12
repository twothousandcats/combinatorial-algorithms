#pragma once
#include "Graph.h"
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>

class Visualizer
{
public:
	static void SaveToHtml(const Graph& mst, const Graph& steiner, const std::string& filename);
};