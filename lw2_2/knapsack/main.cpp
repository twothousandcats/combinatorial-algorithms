#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <ranges>
#include <optional>

struct Item
{
	int weight;
	int cost;
};

struct KnapsackResult
{
	int maxCost;
	std::vector<size_t> bestOccurrence;
	bool found;
};

/**
 * @brief Преобразует индикаторный вектор в следующий (лексикографически).
 * Реализует логику инкремента бинарного числа.
 * @param occurrence Вектор состояний (0/1).
 * @return true, если следующий вектор существует; false, если перебор завершен(вектор единц).
 */
bool NextOccurrence(std::vector<size_t>& occurrence)
{
	if (occurrence.empty())
	{
		return false;
	}

	// Используем reverse_view для прохода с конца к началу
	for (size_t& bit : std::views::reverse(occurrence))
	{
		if (bit == 0)
		{
			bit = 1;
			return true;
		}
		bit = 0;
	}

	// все биты были единицами
	return false;
}

/**
 * @brief Вычисляет вес и стоимость для текущей комбинации
 */
void CalculateCurrentState(const std::vector<Item>& items,
	const std::vector<size_t>& occurrence,
	int& outWeight,
	int& outCost)
{
	outWeight = 0;
	outCost = 0;
	for (size_t i = 0; i < items.size(); ++i)
	{
		if (occurrence[i] == 1)
		{
			outWeight += items[i].weight;
			outCost += items[i].cost;
		}
	}
}

KnapsackResult SolveKnapsack(const std::vector<Item>& items, const int maxWeight)
{
	const size_t itemCount = items.size();
	if (itemCount == 0)
	{
		return { -1, {}, false };
	}

	std::vector<size_t> occurrence(itemCount, 0);
	int globalMaxCost = -1;
	std::vector<size_t> bestOccurrence;
	bool isFound = false;
	do
	{
		int currentWeight = 0;
		int currentCost = 0;
		CalculateCurrentState(items, occurrence, currentWeight, currentCost);

		if (currentWeight <= maxWeight)
		{
			if (currentCost > 0)
			{
				if (!isFound || currentCost > globalMaxCost)
				{
					globalMaxCost = currentCost;
					bestOccurrence = occurrence;
					isFound = true;
				}
			}
		}
	} while (NextOccurrence(occurrence));

	return { globalMaxCost, bestOccurrence, isFound };
}

/**
 * @brief Загружает данные о предметах из файла.
 * @return std::optional<pair<count, maxWeight>> или std::nullopt при ошибке.
 */
std::optional<std::pair<size_t, int> > ReadHeader(std::ifstream& file)
{
	size_t count{};
	int limit{};
	if (file >> count >> limit)
	{
		return std::make_pair(count, limit);
	}
	return std::nullopt;
}

bool LoadItems(std::ifstream& file, const size_t count, std::vector<Item>& items)
{
	items.reserve(count);
	for (size_t i = 0; i < count; ++i)
	{
		int w, c;
		if (!(file >> w >> c))
		{
			std::cerr << "Error: Failed to read item #" << (i + 1) << ".\n";
			return false;
		}

		if (w <= 0.0 || c <= 0.0)
		{
			std::cerr << "Error: Item #" << (i + 1)
				<< " has non-positive weight (" << w
				<< ") or cost (" << c << ").\n";
			return false;
		}

		items.push_back({ w, c });
	}
	return true;
}

void PrintHelp()
{
	std::cout << "Usage:\n";
	std::cout << "	WinOS: knapsack.exe <input_file>\n";
	std::cout << "	Unix: ./knapsack <input_file>\n";
	std::cout << "File format:\n";
	std::cout << "  Line 1: n max_weight\n";
	std::cout << "  Lines 2..n+1: weight_i cost_i\n";
}

void PrintResult(const KnapsackResult& result)
{
	if (!result.found)
	{
		std::cout << "No valid combination found.\n";
		return;
	}

	std::cout << "Max Cost: " << result.maxCost << "\n";
	std::cout << "Selection: [";

	const size_t size = result.bestOccurrence.size();
	for (size_t i = 0; i < size; ++i)
	{
		std::cout << result.bestOccurrence[i];
		if (i + 1 < size)
		{
			std::cout << ", ";
		}
	}
	std::cout << "]\n";
}

int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		std::cerr << "Error: Expected exactly one argument (input file).\n";
		PrintHelp();
		return 1;
	}

	const std::string filePath = argv[1];
	if (filePath == "-h" || filePath == "--help")
	{
		PrintHelp();
		return 0;
	}

	std::ifstream inputFile(filePath);
	if (!inputFile.is_open())
	{
		std::cerr << "Error: Cannot open file '" << filePath << "'\n";
		return 1;
	}

	// Чтение заголовка
	const auto headerOpt = ReadHeader(inputFile);
	if (!headerOpt)
	{
		std::cerr << "Error: Invalid header in file. Expected 'n max_weight'.\n";
		return 1;
	}

	const size_t itemCount = headerOpt->first;
	const int maxWeight = headerOpt->second;
	if (itemCount == 0)
	{
		std::cout << "No items to process.\n";
		return 0;
	}

	// Чтение предметов
	std::vector<Item> items;
	if (!LoadItems(inputFile, itemCount, items))
	{
		return 1;
	}

	// Проверка на лишние данные
	if (int dummy; inputFile >> dummy)
	{
		std::cerr << "Warning: Extra data found in file after " << itemCount << " items.\n";
	}

	PrintResult(SolveKnapsack(items, maxWeight));
	return 0;
}