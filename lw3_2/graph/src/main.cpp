#include "Graph.h"
#include <iostream>
#include <algorithm>

int main()
{
	try
	{
		Graph graph;
		graph.LoadFromFile("input.txt");

		auto sccs = graph.FindStronglyConnectedComponents();
		const auto& names = graph.GetVertexNames(); // getting map index -> name

		std::cout << "Found " << sccs.size() << " Strongly Connected Components:\n";

		// In directed graphs, dead ends are always distinct SCC (of size 1) unless they have a self-loop.
		for (size_t i = 0; i < sccs.size(); ++i)
		{
			std::cout << "SCC #" << (i + 1) << ":";
			std::ranges::sort(sccs[i]); // lexico
			for (const int idx : sccs[i])
			{
				if (idx >= 0 && idx < static_cast<int>(names.size()))
				{
					std::cout << " " << names[idx];
				}
				else
				{
					std::cout << " " << idx;
				}
			}
			std::cout << "\n";
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return 1;
	}
	return 0;
}