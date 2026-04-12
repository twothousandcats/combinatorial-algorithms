#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include "Geometry.h"
#include "Graph.h"
#include "MstBoruvka.h"
#include "SteinerTree.h"
#include "Visualizer.h"

bool LoadTerminalsFromFile(const std::string& filename, Graph& terminals)
{
	std::ifstream file(filename);
	if (!file.is_open())
	{
		std::cerr << "Error: Could not open file " << filename << std::endl;
		return false;
	}

	std::string line;
	while (std::getline(file, line))
	{
		// Пропускаем пустые строки
		if (line.empty())
			continue;

		std::istringstream iss(line);
		std::string label;
		double x, y;

		// Ожидаемый формат: Label X Y
		if (iss >> label >> x >> y)
		{
			terminals.AddNode(geometry::Point(x, y));
		}
		else
		{
			std::cerr << "Warning: Skipping invalid line: " << line << std::endl;
		}
	}

	file.close();
	return true;
}

int main()
{
	Graph terminals;

	// 1. Load Terminals from File
	// Замените "points.txt" на имя вашего файла
	if (!LoadTerminalsFromFile("points.txt", terminals))
	{
		return 1; // Exit if file loading failed
	}

	int N = terminals.GetNodeCount(); // Предполагаем, что есть метод получения количества узлов
	// Если метода GetNodeCount нет, можно использовать локальный счетчик в LoadTerminalsFromFile

	if (N < 2)
	{
		std::cerr << "Error: Need at least 2 points to compute MST/Steiner tree." << std::endl;
		return 1;
	}

	std::cout << "Loaded " << N << " terminals from file." << std::endl;

	// 2. Compute MST (Boruvka)
	Graph mst = MstBoruvka::Compute(terminals);
	double mstLen = mst.TotalWeight();

	// 3. Compute Steiner Tree (Heuristic)
	auto steinerRes = SteinerTreeSolver::Compute(terminals);
	double steinerLen = steinerRes.length;

	// 4. Output Results
	std::cout << "MST Length (Boruvka): " << mstLen << std::endl;
	std::cout << "Steiner Length (Heuristic): " << steinerLen << std::endl;

	if (mstLen > 0)
	{
		std::cout << "Improvement: " << ((mstLen - steinerLen) / mstLen * 100.0) << "%" << std::endl;
	}
	else
	{
		std::cout << "Improvement: N/A (MST length is 0)" << std::endl;
	}

	std::cout << "Steiner Points Added: " << steinerRes.steinerPointsCount << std::endl;

	// 5. Visualize
	Visualizer::SaveToHtml(mst, steinerRes.graph, "output.html");
	std::cout << "Visualization saved to output.html" << std::endl;

	return 0;
}