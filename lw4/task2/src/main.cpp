#include "BoruvkaMST.h"
#include "Graph.h"
#include "InputReader.h"
#include "SteinerTree.h"
#include "Visualizer.h"

#include <exception>
#include <iomanip>
#include <iostream>

namespace
{

void PrintEdges(const std::vector<steiner::Point>& vertices,
	const std::vector<steiner::Edge>& edges, std::ostream& out)
{
	for (const auto& e : edges)
	{
		out << "    " << vertices[e.from].name << " -- " << vertices[e.to].name
			<< "  (length = " << e.weight << ")\n";
	}
}

} // namespace

int main(int argc, char* argv[])
{
	if (argc < 2 || argc > 3)
	{
		std::cerr << "Usage: " << (argc > 0 ? argv[0] : "steiner")
			<< " <input-file> [output.html]\n";
		return 1;
	}

	try
	{
		const steiner::InputReader reader;
		const auto terminals = reader.ReadFromFile(argv[1]);
		if (terminals.size() < 2)
		{
			std::cout << "Need at least two terminals.\n";
			return 0;
		}

		std::cout << std::fixed << std::setprecision(6);

		const auto completeGraph = steiner::Graph::BuildComplete(terminals);
		const steiner::BoruvkaMST mstBuilder;
		const auto mstEdges = mstBuilder.Build(completeGraph);
		const double mstLength = steiner::TotalWeight(mstEdges);

		std::cout << "Terminals: " << terminals.size() << "\n\n";
		std::cout << "Boruvka MST length: " << mstLength << "\n";
		std::cout << "MST edges:\n";
		PrintEdges(terminals, mstEdges, std::cout);

		const steiner::SteinerTreeBuilder steinerBuilder;
		const auto steinerResult = steinerBuilder.Build(terminals);
		const std::size_t steinerPointCount =
			steinerResult.vertices.size() - terminals.size();

		std::cout << "\nSteiner tree length: " << steinerResult.totalLength << "\n";
		std::cout << "Steiner points added: " << steinerPointCount << "\n";
		std::cout << "Steiner tree edges:\n";
		PrintEdges(steinerResult.vertices, steinerResult.edges, std::cout);

		const double savings = mstLength - steinerResult.totalLength;
		const double ratio = mstLength > 0.0
			? steinerResult.totalLength / mstLength
			: 1.0;
		std::cout << "\nSavings (MST - Steiner): " << savings << "\n";
		std::cout << "Steiner / MST ratio:     " << ratio << "\n";

		if (argc == 3)
		{
			steiner::Visualizer::SaveToHtml(terminals, mstEdges, steinerResult, argv[2]);
			std::cout << "\nVisualization written to " << argv[2] << "\n";
		}
	}
	catch (const std::exception& ex)
	{
		std::cerr << "Error: " << ex.what() << "\n";
		return 1;
	}
	return 0;
}