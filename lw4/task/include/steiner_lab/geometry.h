#pragma once

namespace steiner_lab
{

struct Point2D
{
	double x = 0.0;
	double y = 0.0;
};

class EuclideanGeometry
{
public:
	[[nodiscard]] static double Distance(const Point2D& pointA, const Point2D& pointB);
	[[nodiscard]] static double DistanceSquared(const Point2D& pointA, const Point2D& pointB);
	[[nodiscard]] static double AngleAtB(const Point2D& pointA, const Point2D& vertexB, const Point2D& pointC);
	[[nodiscard]] static Point2D EquilateralThirdPoint(const Point2D& pointA, const Point2D& pointB, bool rotateCounterClockwise);
	[[nodiscard]] static Point2D FermatTorricelliPoint(const Point2D& pointA, const Point2D& pointB, const Point2D& pointC);

private:
	[[nodiscard]] static double FermatObtuseAngleLimitRadians();
	[[nodiscard]] static double CosineAngleAtB(const Point2D& pointA, const Point2D& vertexB, const Point2D& pointC);
	[[nodiscard]] static Point2D WeiszfeldGeometricMedianThree(const Point2D& pointA, const Point2D& pointB,
		const Point2D& pointC);
	[[nodiscard]] static Point2D WeiszfeldIterationStep(const Point2D& current, const Point2D& pointA,
		const Point2D& pointB, const Point2D& pointC);
};

} // namespace steiner_lab
