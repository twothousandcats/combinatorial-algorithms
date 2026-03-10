#include <iostream>
#include "Graph.h"

int main()
{
	try
	{
		Graph graph;
		graph.LoadFromFile("input.txt");

		// check adapter
		const auto edges = graph.ConvertMatrixToEdgeList();
		const auto sccs = graph.FindStronglyConnectedComponents();
		std::cout << "\nНайдено компонент сильной связности: " << sccs.size() << std::endl;
		const auto& names = graph.GetVertexNames();
		for (size_t i = 0; i < sccs.size(); ++i)
		{
			std::cout << "#" << (i + 1) << ": ";
			for (const int idx : sccs[i])
			{
				std::cout << names[idx] << " ";
			}
			std::cout << std::endl;
		}

	}
	catch (const std::exception& e)
	{
		std::cerr << "Ошибка: " << e.what() << std::endl;
		return 1;
	}

	return 0;
}