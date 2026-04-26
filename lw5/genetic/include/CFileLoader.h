#pragma once

#include "ISolver.h"
#include <string>
#include <optional>
#include <vector>
#include <iostream>

namespace knapsack
{

class CFileLoader
{
public:
	struct Header
	{
		size_t itemCount;
		int maxWeight;
	};

	static std::optional<Header> LoadHeader(std::istream& stream);

	static bool LoadItems(std::istream& stream, size_t count, std::vector<Item>& outItems);
};

void PrintResult(std::ostream& out, const KnapsackResult& result);

} // namespace knapsack