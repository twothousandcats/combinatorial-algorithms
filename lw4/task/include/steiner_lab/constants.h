#pragma once

namespace steiner_lab
{

namespace scalar_constants
{
inline constexpr double kZero = 0.0;
} // namespace scalar_constants

namespace geometry_constants
{
inline constexpr double kVectorNormEpsilon = 1e-18;
inline constexpr double kWeiszfeldDistanceFloor = 1e-18;
inline constexpr int kWeiszfeldMaxIterations = 200;
inline constexpr double kWeiszfeldConvergenceEpsilonSq = 1e-24;
inline constexpr double kOneHalf = 0.5;
inline constexpr double kOne = 1.0;
inline constexpr double kMinusOne = -1.0;
inline constexpr double kTwo = 2.0;
inline constexpr double kThree = 3.0;
inline constexpr double kCosineClampMin = -1.0;
inline constexpr double kCosineClampMax = 1.0;
inline constexpr double kFermatObtuseAngleDivisor = 3.0;
inline constexpr double kCentroidDivisor = 3.0;
} // namespace geometry_constants

namespace smith_constants
{
inline constexpr double kRelaxationTolerance = 1e-10;
inline constexpr int kMaxIterations = 5000;
inline constexpr int kNeighborCountAtSteinerVertex = 3;
} // namespace smith_constants

namespace graphviz_constants
{
inline constexpr double kCoordinateMatchEpsilon = 1e-6;
inline constexpr double kDotNodeCoordinateScale = 1000.0;
} // namespace graphviz_constants

namespace demo_constants
{
inline constexpr int kDemoTerminalCount = 6;
inline constexpr int kRandomSampleSizeSmall = 10;
inline constexpr int kRandomSampleSizeLarge = 100;
inline constexpr double kInvalidLengthMarker = -1.0;
inline constexpr double kRandomUniformLow = 0.0;
inline constexpr double kRandomUniformHigh = 1.0;
} // namespace demo_constants

namespace comparison_constants
{
inline constexpr double kMinPositiveLength = 1e-15;
inline constexpr double kPercentScale = 100.0;
} // namespace comparison_constants

} // namespace steiner_lab
