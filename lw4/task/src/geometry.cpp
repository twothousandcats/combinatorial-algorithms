#include "steiner_lab/geometry.h"
#include "steiner_lab/constants.h"

#include <algorithm>
#include <array>
#include <cmath>

namespace steiner_lab
{

using namespace geometry_constants;

// Возвращает евклидово расстояние и нужен как базовая метрика всех алгоритмов.
double EuclideanGeometry::Distance(const Point2D& pointA, const Point2D& pointB)
{
	return std::hypot(pointA.x - pointB.x, pointA.y - pointB.y);
}

// Возвращает квадрат расстояния и нужен для дешевой проверки сходимости без sqrt.
double EuclideanGeometry::DistanceSquared(const Point2D& pointA, const Point2D& pointB)
{
	const double deltaX = pointA.x - pointB.x;
	const double deltaY = pointA.y - pointB.y;
	return deltaX * deltaX + deltaY * deltaY;
}

// Считает косинус угла при вершине B и нужен для определения тупого случая в Ферма-точке.
double EuclideanGeometry::CosineAngleAtB(const Point2D& pointA, const Point2D& vertexB, const Point2D& pointC)
{
	const double vector1X = pointA.x - vertexB.x;
	const double vector1Y = pointA.y - vertexB.y;
	const double vector2X = pointC.x - vertexB.x;
	const double vector2Y = pointC.y - vertexB.y;
	const double dotProduct = vector1X * vector2X + vector1Y * vector2Y;
	const double norm1 = std::max(kVectorNormEpsilon, std::sqrt(vector1X * vector1X + vector1Y * vector1Y));
	const double norm2 = std::max(kVectorNormEpsilon, std::sqrt(vector2X * vector2X + vector2Y * vector2Y));
	return std::clamp(dotProduct / (norm1 * norm2), kCosineClampMin, kCosineClampMax);
}

// Считает угол при вершине B и нужен для геометрических условий задачи Штейнера.
double EuclideanGeometry::AngleAtB(const Point2D& pointA, const Point2D& vertexB, const Point2D& pointC)
{
	return std::acos(CosineAngleAtB(pointA, vertexB, pointC));
}

// Строит третью вершину равностороннего треугольника и нужен как вспомогательная геометрия.
Point2D EuclideanGeometry::EquilateralThirdPoint(const Point2D& pointA, const Point2D& pointB, bool rotateCounterClockwise)
{
	const double vectorX = pointB.x - pointA.x;
	const double vectorY = pointB.y - pointA.y;
	const double cosine = kOneHalf;
	const double sine =
		(rotateCounterClockwise ? kOne : kMinusOne) * (std::sqrt(kThree) * kOneHalf);
	const double rotatedX = cosine * vectorX - sine * vectorY;
	const double rotatedY = sine * vectorX + cosine * vectorY;
	return {pointA.x + rotatedX, pointA.y + rotatedY};
}

// Возвращает порог 120 градусов в радианах и нужен для ветвления формулы Ферма-точки.
double EuclideanGeometry::FermatObtuseAngleLimitRadians()
{
	return kTwo * std::acos(kMinusOne) / kFermatObtuseAngleDivisor;
}

// Выполняет один шаг Вайцфельда и нужен для численного поиска геометрической медианы.
Point2D EuclideanGeometry::WeiszfeldIterationStep(const Point2D& current, const Point2D& pointA, const Point2D& pointB,
	const Point2D& pointC)
{
	double weightSum = 0.0;
	double numeratorX = 0.0;
	double numeratorY = 0.0;
	const std::array<Point2D, 3> corners = {pointA, pointB, pointC};
	for (const Point2D& corner : corners)
	{
		const double distanceValue = std::max(kWeiszfeldDistanceFloor, Distance(current, corner));
		const double weight = kOne / distanceValue;
		weightSum += weight;
		numeratorX += weight * corner.x;
		numeratorY += weight * corner.y;
	}
	return {numeratorX / weightSum, numeratorY / weightSum};
}

// Ищет геометрическую медиану трех точек и нужен как численный подалгоритм Ферма-Торричелли.
Point2D EuclideanGeometry::WeiszfeldGeometricMedianThree(const Point2D& pointA, const Point2D& pointB, const Point2D& pointC)
{
	Point2D current{(pointA.x + pointB.x + pointC.x) / kCentroidDivisor,
		(pointA.y + pointB.y + pointC.y) / kCentroidDivisor};
	for (int iteration = 0; iteration < kWeiszfeldMaxIterations; ++iteration)
	{
		const Point2D next = WeiszfeldIterationStep(current, pointA, pointB, pointC);
		if (DistanceSquared(current, next) < kWeiszfeldConvergenceEpsilonSq)
		{
			break;
		}
		current = next;
	}
	return current;
}

// Возвращает точку Ферма-Торричелли и нужен для локальной оптимизации узлов Штейнера.
Point2D EuclideanGeometry::FermatTorricelliPoint(const Point2D& pointA, const Point2D& pointB, const Point2D& pointC)
{
	const double obtuseLimit = FermatObtuseAngleLimitRadians();
	if (AngleAtB(pointB, pointA, pointC) >= obtuseLimit)
	{
		return pointA;
	}
	if (AngleAtB(pointA, pointB, pointC) >= obtuseLimit)
	{
		return pointB;
	}
	if (AngleAtB(pointB, pointC, pointA) >= obtuseLimit)
	{
		return pointC;
	}
	return WeiszfeldGeometricMedianThree(pointA, pointB, pointC);
}

} // namespace steiner_lab
