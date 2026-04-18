// CLI app: read user terminals and compare MST (Boruvka) with exact ESMT.
#include "steiner_lab/boruvka.h"
#include "steiner_lab/comparison_math.h"
#include "steiner_lab/euclidean_steiner.h"
#include "steiner_lab/exceptions.h"
#include "steiner_lab/file_io.h"
#include "steiner_lab/graphviz_export.h"
#include "steiner_lab/input_parser.h"
#include "steiner_lab/input_validator.h"

#include <chrono>
#include <iostream>
#include <string>
#include <vector>

namespace
{

// Читает точки из stdin и нужна для получения пользовательского набора терминалов.
std::vector<steiner_lab::Point2D> ReadPointsFromStdin()
{
	std::cout << "Введите точки (по одной в строке: x y). Завершите ввод через Ctrl+D:\n";
	return steiner_lab::TerminalSetParser{}.ParseLinesOfPairs(std::cin);
}

// Печатает сравнительные метрики длин и нужна для интерпретации результата двух алгоритмов.
void PrintComparison(double mstLength, double esmtLength)
{
	const steiner_lab::AlgorithmLengthRatios ratios = steiner_lab::ComputeAlgorithmLengthRatios(mstLength, esmtLength);
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

// Сохраняет DOT-файлы и нужна для визуализации решений в Graphviz.
void SaveDotFiles(const std::vector<steiner_lab::Point2D>& terminals, const steiner_lab::MstResult& mst,
	const steiner_lab::EuclideanSteinerTree& esmt)
{
	steiner_lab::GraphvizDotExporter exporter{};
	steiner_lab::WriteTextFile("result_mst.dot", exporter.ExportMst(terminals, mst, "BoruvkaMst"));
	steiner_lab::WriteTextFile("result_steiner.dot", exporter.ExportSteinerTree(terminals, esmt, "SteinerTree"));
	std::cout << "DOT-файлы сохранены: result_mst.dot, result_steiner.dot\n";
}

// Печатает DOT-код в консоль и нужен для быстрого копирования в Graphviz Online.
void PrintDotToConsole(const std::vector<steiner_lab::Point2D>& terminals, const steiner_lab::MstResult& mst,
	const steiner_lab::EuclideanSteinerTree& esmt)
{
	steiner_lab::GraphvizDotExporter exporter{};
	const std::string mstDot = exporter.ExportMst(terminals, mst, "BoruvkaMst");
	const std::string steinerDot = exporter.ExportSteinerTree(terminals, esmt, "SteinerTree");
	std::cout << "\n--- DOT: минимальное остовное дерево по Борувке ---\n";
	std::cout << mstDot;
	std::cout << "\n--- DOT: алгоритм Штейнера ---\n";
	std::cout << steinerDot;
}

} // namespace

// Запускает ввод, вычисления и вывод и нужна как точка входа консольного приложения.
int main()
{
	try
	{
		const std::vector<steiner_lab::Point2D> terminals = ReadPointsFromStdin();
		steiner_lab::TerminalSetValidator{}.ValidateAllCoordinatesFiniteOrThrow(terminals);

		const auto mstStart = std::chrono::steady_clock::now();
		const steiner_lab::MstResult mst = steiner_lab::BoruvkaMstSolver{}.Build(terminals);
		const auto mstEnd = std::chrono::steady_clock::now();

		const auto esmtStart = std::chrono::steady_clock::now();
		const steiner_lab::EuclideanSteinerTree esmt = steiner_lab::EuclideanSteinerSolver{}.Solve(terminals);
		const auto esmtEnd = std::chrono::steady_clock::now();

		std::cout << "\nРезультаты:\n";
		std::cout << "Минимальное остовное дерево по Борувке, длина: " << mst.total_length << "\n";
		std::cout << "Время построения остовного дерева по Борувке: "
				  << std::chrono::duration<double, std::milli>(mstEnd - mstStart).count() << " ms\n";
		std::cout << "Алгоритм Штейнера, длина: " << esmt.total_length << "\n";
		std::cout << "Время выполнения алгоритма Штейнера: "
				  << std::chrono::duration<double, std::milli>(esmtEnd - esmtStart).count() << " ms\n";

		PrintComparison(mst.total_length, esmt.total_length);
		SaveDotFiles(terminals, mst, esmt);
		PrintDotToConsole(terminals, mst, esmt);
	}
	catch (const steiner_lab::SteinerLabException& error)
	{
		std::cerr << "Ошибка: " << error.what() << '\n';
		return 1;
	}
	return 0;
}
