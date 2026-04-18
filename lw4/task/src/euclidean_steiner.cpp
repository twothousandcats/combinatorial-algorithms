#include "steiner_lab/euclidean_steiner.h"
#include "steiner_lab/constants.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <vector>

namespace steiner_lab
{

using smith_constants::kMaxIterations;
using smith_constants::kNeighborCountAtSteinerVertex;
using smith_constants::kRelaxationTolerance;
using scalar_constants::kZero;

// Возвращает пустое дерево нулевой длины и нужен для тривиальных случаев 0-1 терминал.
EuclideanSteinerTree EuclideanSteinerSolver::TrivialEmptyOrSingle()
{
	EuclideanSteinerTree tree{};
	tree.total_length = kZero;
	return tree;
}

// Строит единственный отрезок для двух терминалов и нужен как точный базовый случай.
EuclideanSteinerTree EuclideanSteinerSolver::TrivialTwoTerminals(const std::vector<Point2D>& terminals)
{
	EuclideanSteinerTree best{};
	best.total_length = EuclideanGeometry::Distance(terminals[0], terminals[1]);
	best.segments.push_back({terminals[0], terminals[1]});
	return best;
}

// Строит решение через одну Ферма-точку и нужен как точный базовый случай для трех терминалов.
EuclideanSteinerTree EuclideanSteinerSolver::TrivialThreeTerminals(const std::vector<Point2D>& terminals)
{
	EuclideanSteinerTree best{};
	const Point2D fermatPoint =
		EuclideanGeometry::FermatTorricelliPoint(terminals[0], terminals[1], terminals[2]);
	best.steiner_points.push_back(fermatPoint);
	best.total_length = EuclideanGeometry::Distance(terminals[0], fermatPoint) +
						EuclideanGeometry::Distance(terminals[1], fermatPoint) +
						EuclideanGeometry::Distance(terminals[2], fermatPoint);
	best.segments.push_back({terminals[0], fermatPoint});
	best.segments.push_back({terminals[1], fermatPoint});
	best.segments.push_back({terminals[2], fermatPoint});
	return best;
}

// Нормализует ориентацию ребра и нужен для каноничного хранения топологий в set.
EuclideanSteinerSolver::NormalizedEdge EuclideanSteinerSolver::CanonEdge(int endpointA, int endpointB)
{
	if (endpointA > endpointB)
	{
		return {endpointB, endpointA};
	}
	return {endpointA, endpointB};
}

// Добавляет каноничное ребро в топологию и нужен для единообразной сборки графа.
void EuclideanSteinerSolver::InsertTopologyEdge(TopologyEdgeSet& graph, int endpointA, int endpointB)
{
	graph.insert(CanonEdge(endpointA, endpointB));
}

// Формирует единственную топологию для двух вершин и нужен как база рекурсии перебора.
void EuclideanSteinerSolver::CollectTwoVertexTopology(const std::vector<int>& vertices, TopologyCollection& output)
{
	TopologyEdgeSet graph;
	InsertTopologyEdge(graph, vertices[0], vertices[1]);
	output.insert(std::move(graph));
}

// Формирует звездную топологию для трех вершин и нужен как база рекурсии со Штейнер узлом.
void EuclideanSteinerSolver::CollectThreeVertexTopology(const std::vector<int>& vertices, int nextSteinerId,
	TopologyCollection& output)
{
	const int steinerId = nextSteinerId;
	TopologyEdgeSet graph;
	InsertTopologyEdge(graph, vertices[0], steinerId);
	InsertTopologyEdge(graph, vertices[1], steinerId);
	InsertTopologyEdge(graph, vertices[2], steinerId);
	output.insert(std::move(graph));
}

// Собирает уменьшенный список вершин после схлопывания пары и нужен для шага cherry-расширения.
std::vector<int> EuclideanSteinerSolver::MakeReducedVertexListWithSteiner(const std::vector<int>& vertices,
	std::size_t skipFirstIndex, std::size_t skipSecondIndex, int steinerId)
{
	std::vector<int> reduced;
	reduced.reserve(vertices.size() - 1);
	for (std::size_t vertexIndex = 0; vertexIndex < vertices.size(); ++vertexIndex)
	{
		if (vertexIndex == skipFirstIndex || vertexIndex == skipSecondIndex)
		{
			continue;
		}
		reduced.push_back(vertices[vertexIndex]);
	}
	reduced.push_back(steinerId);
	return reduced;
}

// Находит соседа листа-Штейнера и нужен для корректного раскрытия схлопнутой пары.
bool EuclideanSteinerSolver::FindSteinerLeafNeighbor(const TopologyEdgeSet& subgraph, int steinerId, int& outNeighbor)
{
	int incidentDegree = 0;
	int neighbor = -1;
	for (const NormalizedEdge& edge : subgraph)
	{
		const int left = edge.first;
		const int right = edge.second;
		if (left != steinerId && right != steinerId)
		{
			continue;
		}
		++incidentDegree;
		neighbor = (left == steinerId) ? right : left;
	}
	if (incidentDegree != 1 || neighbor < 0)
	{
		return false;
	}
	outNeighbor = neighbor;
	return true;
}

// Раскрывает cherry-операцию в подграфе и нужен для восстановления полной топологии.
EuclideanSteinerSolver::TopologyEdgeSet EuclideanSteinerSolver::TransformSubgraphWithCherry(
	const EuclideanSteinerSolver::TopologyEdgeSet& subgraph, int steinerId, int oldNeighbor, int endpointA, int endpointB)
{
	TopologyEdgeSet extended = subgraph;
	extended.erase(CanonEdge(steinerId, oldNeighbor));
	InsertTopologyEdge(extended, steinerId, endpointA);
	InsertTopologyEdge(extended, steinerId, endpointB);
	InsertTopologyEdge(extended, steinerId, oldNeighbor);
	return extended;
}

// Добавляет расширения для выбранной пары вершин и нужен для полного перечисления топологий.
void EuclideanSteinerSolver::AppendCherryExtensionsForPair(const std::vector<int>& vertices, std::size_t skipFirstIndex,
	std::size_t skipSecondIndex, int nextSteinerId, TopologyCollection& output)
{
	const int endpointA = vertices[skipFirstIndex];
	const int endpointB = vertices[skipSecondIndex];
	const int steinerId = nextSteinerId;
	const std::vector<int> reduced =
		MakeReducedVertexListWithSteiner(vertices, skipFirstIndex, skipSecondIndex, steinerId);
	TopologyCollection childTopologies;
	EnumerateFullTopologiesRec(reduced, nextSteinerId + 1, childTopologies);
	for (const TopologyEdgeSet& child : childTopologies)
	{
		int neighborId = 0;
		if (!FindSteinerLeafNeighbor(child, steinerId, neighborId))
		{
			continue;
		}
		TopologyEdgeSet extended = TransformSubgraphWithCherry(child, steinerId, neighborId, endpointA, endpointB);
		output.insert(std::move(extended));
	}
}

// перечисляет все полные топологии и нужен для точного перебора структуры дерева.
void EuclideanSteinerSolver::EnumerateFullTopologiesRec(const std::vector<int>& vertices, int nextSteinerId,
	TopologyCollection& output)
{
	if (vertices.size() == 2)
	{
		CollectTwoVertexTopology(vertices, output);
		return;
	}
	if (vertices.size() == 3)
	{
		CollectThreeVertexTopology(vertices, nextSteinerId, output);
		return;
	}
	for (std::size_t skipFirstIndex = 0; skipFirstIndex < vertices.size(); ++skipFirstIndex)
	{
		for (std::size_t skipSecondIndex = skipFirstIndex + 1; skipSecondIndex < vertices.size(); ++skipSecondIndex)
		{
			AppendCherryExtensionsForPair(vertices, skipFirstIndex, skipSecondIndex, nextSteinerId, output);
		}
	}
}

// Запускает перечисление полных топологий по числу терминалов и нужен как вход в точный метод.
EuclideanSteinerSolver::TopologyCollection EuclideanSteinerSolver::BuildAllFullTopologies(int terminalCount)
{
	TopologyCollection output;
	std::vector<int> vertexLabels(static_cast<std::size_t>(terminalCount));
	for (int label = 0; label < terminalCount; ++label)
	{
		vertexLabels[static_cast<std::size_t>(label)] = label;
	}
	EnumerateFullTopologiesRec(vertexLabels, terminalCount, output);
	return output;
}

// Строит список смежности топологии и нужен для последующей релаксации Смита.
void EuclideanSteinerSolver::BuildAdjacency(const TopologyEdgeSet& edges, int maxVertexId,
	std::vector<std::vector<int>>& adjacency)
{
	adjacency.assign(static_cast<std::size_t>(maxVertexId) + 1, {});
	for (const NormalizedEdge& edge : edges)
	{
		const int left = edge.first;
		const int right = edge.second;
		adjacency[static_cast<std::size_t>(left)].push_back(right);
		adjacency[static_cast<std::size_t>(right)].push_back(left);
	}
}

// Считает суммарную длину дерева и нужен для сравнения кандидатов по целевой функции.
double EuclideanSteinerSolver::TotalTreeLength(const TopologyEdgeSet& edges, const std::vector<Point2D>& positionsById)
{
	double sum = kZero;
	for (const NormalizedEdge& edge : edges)
	{
		const int left = edge.first;
		const int right = edge.second;
		sum += EuclideanGeometry::Distance(positionsById[static_cast<std::size_t>(left)],
			positionsById[static_cast<std::size_t>(right)]);
	}
	return sum;
}

// Делает один шаг и нужен для улучшения координат точек Штейнера.
double EuclideanSteinerSolver::SmithRelaxationStep(std::vector<Point2D>& positionsById,
	const std::vector<std::vector<int>>& adjacency, int terminalCount)
{
	double maxMove = kZero;
	for (int vertexId = terminalCount; vertexId < static_cast<int>(positionsById.size()); ++vertexId)
	{
		const std::vector<int>& neighbors = adjacency[static_cast<std::size_t>(vertexId)];
		if (static_cast<int>(neighbors.size()) != kNeighborCountAtSteinerVertex)
		{
			return std::numeric_limits<double>::infinity();
		}
		const Point2D& neighbor0 = positionsById[static_cast<std::size_t>(neighbors[0])];
		const Point2D& neighbor1 = positionsById[static_cast<std::size_t>(neighbors[1])];
		const Point2D& neighbor2 = positionsById[static_cast<std::size_t>(neighbors[2])];
		const Point2D newFermat = EuclideanGeometry::FermatTorricelliPoint(neighbor0, neighbor1, neighbor2);
		const double move = EuclideanGeometry::Distance(positionsById[static_cast<std::size_t>(vertexId)], newFermat);
		maxMove = std::max(maxMove, move);
		positionsById[static_cast<std::size_t>(vertexId)] = newFermat;
	}
	return maxMove;
}

// Выполняет итерации релаксации Смита до сходимости и нужен для геометрической оптимизации топологии.
bool EuclideanSteinerSolver::RunSmithRelaxation(const TopologyEdgeSet& topology, int terminalCount, int maxVertexId,
	std::vector<Point2D>& positionsById, double tolerance, int maxIterations)
{
	std::vector<std::vector<int>> adjacency;
	BuildAdjacency(topology, maxVertexId, adjacency);
	for (int iteration = 0; iteration < maxIterations; ++iteration)
	{
		const double maxMove = SmithRelaxationStep(positionsById, adjacency, terminalCount);
		if (!std::isfinite(maxMove))
		{
			return false;
		}
		if (maxMove < tolerance)
		{
			break;
		}
	}
	return true;
}

// Находит максимальный id вершины в топологии и нужен для размеров рабочих массивов.
int EuclideanSteinerSolver::MaxVertexId(const TopologyEdgeSet& edges)
{
	int maximum = -1;
	for (const NormalizedEdge& edge : edges)
	{
		maximum = std::max(maximum, edge.first);
		maximum = std::max(maximum, edge.second);
	}
	return maximum;
}

// Считает центроид терминалов и нужен как стартовое приближение для Штейнер-точек.
Point2D EuclideanSteinerSolver::TerminalCentroid(const std::vector<Point2D>& terminals)
{
	Point2D centroid{kZero, kZero};
	for (const Point2D& point : terminals)
	{
		centroid.x += point.x;
		centroid.y += point.y;
	}
	const double count = static_cast<double>(terminals.size());
	centroid.x /= count;
	centroid.y /= count;
	return centroid;
}

// Копирует терминалы в массив позиций по id и нужен для инициализации геометрии топологии.
void EuclideanSteinerSolver::PlaceTerminalsInPositionArray(std::vector<Point2D>& positions, int terminalCount,
	const std::vector<Point2D>& terminals)
{
	for (int index = 0; index < terminalCount; ++index)
	{
		positions[static_cast<std::size_t>(index)] = terminals[static_cast<std::size_t>(index)];
	}
}

// Инициализирует все Штейнер-вершины центроидом и нужен для старта численной релаксации.
void EuclideanSteinerSolver::InitSteinerAtCentroid(std::vector<Point2D>& positions, int terminalCount, int maxVertexId,
	const Point2D& centroid)
{
	for (int vertexId = terminalCount; vertexId <= maxVertexId; ++vertexId)
	{
		positions[static_cast<std::size_t>(vertexId)] = centroid;
	}
}

// Принимает кандидат как лучший при меньшей длине и нужен для хранения текущего оптимума.
void EuclideanSteinerSolver::AdoptIfShorter(const TopologyEdgeSet& topology, int terminalCount, int maxVertexId,
	const std::vector<Point2D>& positions, double length, double& bestLength, EuclideanSteinerTree& bestTree)
{
	if (length >= bestLength)
	{
		return;
	}
	bestLength = length;
	bestTree.total_length = length;
	bestTree.steiner_points.clear();
	bestTree.segments.clear();
	for (int steinerIndex = terminalCount; steinerIndex <= maxVertexId; ++steinerIndex)
	{
		bestTree.steiner_points.push_back(positions[static_cast<std::size_t>(steinerIndex)]);
	}
	for (const NormalizedEdge& edge : topology)
	{
		const Point2D& pointA = positions[static_cast<std::size_t>(edge.first)];
		const Point2D& pointB = positions[static_cast<std::size_t>(edge.second)];
		bestTree.segments.push_back({pointA, pointB});
	}
}

// Оценивает одну топологию целиком и нужен как атомарный шаг глобального перебора.
void EuclideanSteinerSolver::EvaluateTopology(const TopologyEdgeSet& topology, const std::vector<Point2D>& terminals,
	int terminalCount, double& bestLength, EuclideanSteinerTree& bestTree)
{
	const int maxVertex = MaxVertexId(topology);
	std::vector<Point2D> positionsById(static_cast<std::size_t>(maxVertex) + 1); //массивчик
	PlaceTerminalsInPositionArray(positionsById, terminalCount, terminals);
	const Point2D centroid = TerminalCentroid(terminals); //координаты для терминалов
	InitSteinerAtCentroid(positionsById, terminalCount, maxVertex, centroid);
	if (!RunSmithRelaxation(topology, terminalCount, maxVertex, positionsById, kRelaxationTolerance, kMaxIterations))
	{
		return;
	}
	const double treeLength = TotalTreeLength(topology, positionsById);
	AdoptIfShorter(topology, terminalCount, maxVertex, positionsById, treeLength, bestLength, bestTree);
}

// Строит точное дерево Штейнера перебором топологий
EuclideanSteinerTree EuclideanSteinerSolver::Solve(const std::vector<Point2D>& terminals) const
{
	const int terminalCount = static_cast<int>(terminals.size());
	if (terminalCount == 0)
	{
		return EuclideanSteinerTree{};
	}
	if (terminalCount == 1)
	{
		return TrivialEmptyOrSingle();
	}
	if (terminalCount == 2)
	{
		return TrivialTwoTerminals(terminals);
	}
	if (terminalCount == 3)
	{
		return TrivialThreeTerminals(terminals);
	}
	double bestLength = std::numeric_limits<double>::infinity();
	EuclideanSteinerTree bestTree{};
	const TopologyCollection allTopologies = BuildAllFullTopologies(terminalCount);
	for (const TopologyEdgeSet& topology : allTopologies)
	{
		EvaluateTopology(topology, terminals, terminalCount, bestLength, bestTree);
	}
	return bestTree;
}

}

