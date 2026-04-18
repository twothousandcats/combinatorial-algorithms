// CLI app: read user terminals and compare MST (Boruvka) with exact ESMT.
#include "Boruvka.h"
#include "Comparison.h"
#include "Exceptions.h"
#include "InputParser.h"
#include "InputValidator.h"
#include "SteinerSolver.h"
#include "Visualizer.h"

#include <chrono>
#include <iostream>
#include <string>
#include <vector>

namespace
{

// Читает точки из stdin и нужна для получения пользовательского набора терминалов.
std::vector<steiner::Point2D> ReadPointsFromStdin()
{
	std::cout << "Введите точки (по одной в строке: x y). Завершите ввод через Ctrl+D:\n";
	return steiner::TerminalSetParser{}.ParseLinesOfPairs(std::cin);
}

// Печатает сравнительные метрики длин и нужна для интерпретации результата двух алгоритмов.
void PrintComparison(double mstLength, double esmtLength)
{
	const steiner::AlgorithmLengthRatios ratios = steiner::ComputeAlgorithmLengthRatios(mstLength, esmtLength);
	std::cout << "\n--- Сравнение ---\n";
	std::cout << "Минимальное остовное дерево по Борувке: " << mstLength << "\n";
	std::cout << "Алгоритм Штейнера: " << esmtLength << "\n";
	if (ratios.mst_over_esmt_valid)
	{
		std::cout << "Отношение: остовное дерево по Борувке / алгоритм Штейнера: " << ratios.mst_over_esmt << "\n";
	}
	else
	{
		std::cout << "Отношение: остовное дерево по Борувке / алгоритм Штейнера: недоступно\n";
	}
	if (ratios.esmt_save_percent_valid)
	{
		std::cout << "Экономия длины алгоритма Штейнера относительно остовного дерева по Борувке: "
				  << ratios.esmt_save_percent_vs_mst << " %\n";
	}
	else
	{
		std::cout << "Экономия длины алгоритма Штейнера относительно остовного дерева по Борувке: недоступно\n";
	}
}

} // namespace

// Запускает ввод, вычисления и вывод и нужна как точка входа консольного приложения.
int main()
{
	try
	{
		const std::vector<steiner::Point2D> terminals = ReadPointsFromStdin();
		steiner::TerminalSetValidator{}.ValidateAllCoordinatesFiniteOrThrow(terminals);

		const auto mstStart = std::chrono::steady_clock::now();
		const steiner::MstResult mst = steiner::BoruvkaMstSolver{}.Build(terminals);
		const auto mstEnd = std::chrono::steady_clock::now();

		const auto esmtStart = std::chrono::steady_clock::now();
		const steiner::SteinerTreeResult esmt = steiner::SteinerSolver{}.Solve(terminals);
		const auto esmtEnd = std::chrono::steady_clock::now();

		std::cout << "\nРезультаты:\n";
		std::cout << "Минимальное остовное дерево по Борувке, длина: " << mst.total_length << "\n";
		std::cout << "Время построения остовного дерева по Борувке: "
				  << std::chrono::duration<double, std::milli>(mstEnd - mstStart).count() << " ms\n";
		std::cout << "Алгоритм Штейнера, длина: " << esmt.total_length << "\n";
		std::cout << "Время выполнения алгоритма Штейнера: "
				  << std::chrono::duration<double, std::milli>(esmtEnd - esmtStart).count() << " ms\n";

		PrintComparison(mst.total_length, esmt.total_length);

		const std::string reportPath = "result.html";
		steiner::Visualizer::SaveToHtml(terminals, mst, esmt, reportPath);
		std::cout << "HTML-отчёт сохранён: " << reportPath << "\n";
	}
	catch (const steiner::SteinerException& error)
	{
		std::cerr << "Ошибка: " << error.what() << '\n';
		return 1;
	}
	return 0;
}