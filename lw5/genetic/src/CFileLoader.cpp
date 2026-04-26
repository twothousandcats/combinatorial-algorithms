#include "CFileLoader.h"

#include <fstream>

namespace knapsack
{

std::optional<CFileLoader::Header> CFileLoader::LoadHeader(std::istream& stream)
{
	Header h{};
	if (stream >> h.itemCount >> h.maxWeight)
	{
		return h;
	}
	return std::nullopt;
}

bool CFileLoader::LoadItems(std::istream& stream, size_t count, std::vector<Item>& outItems)
{
	outItems.clear();
	outItems.reserve(count);
	for (size_t i = 0; i < count; ++i)
	{
		Item item;
		if (!(stream >> item.weight >> item.cost))
		{
			return false;
		}
		if (item.weight <= 0 || item.cost <= 0)
		{
			return false;
		}
		outItems.push_back(item);
	}
	return true;
}

void PrintResult(std::ostream& out, const KnapsackResult& result)
{
	if (!result.found)
	{
		out << "No valid combination found.\n";
		return;
	}

	out << "Max Cost: " << result.maxCost << "\n";
	out << "Selected Items Count: " << result.bestIndices.size() << "\n";
}

} // namespace knapsack