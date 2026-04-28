#include <iostream>
#include <fstream>
#include <random>
#include <string>

int main()
{
	const int N = 2000;
	const std::string filename = "input.txt";

	// Генератор случайных чисел
	std::random_device rd;
	std::mt19937 gen(rd());

	// Диапазоны значений
	std::uniform_int_distribution<> weight_dist(1, 100);
	std::uniform_int_distribution<> value_dist(1, 200);
	std::uniform_int_distribution<> capacity_dist(500, 5000);

	const int max_capacity = capacity_dist(gen);
	std::ofstream outFile(filename);
	if (!outFile.is_open())
	{
		std::cerr << "Ошибка открытия файла!" << std::endl;
		return 1;
	}

	outFile << N << " " << max_capacity << "\n";
	for (int i = 0; i < N; ++i)
	{
		const int weight = weight_dist(gen);
		const int value = value_dist(gen);
		outFile << weight << " " << value << "\n";
	}

	outFile.close();
	std::cout << "Файл '" << filename << "' успешно создан." << std::endl;

	return 0;
}