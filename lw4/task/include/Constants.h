#pragma once

namespace steiner
{

// Generic scalar constants.
inline constexpr double kZero = 0.0;
inline constexpr double kOneHalf = 0.5;
inline constexpr double kOne = 1.0;
inline constexpr double kMinusOne = -1.0;
inline constexpr double kTwo = 2.0;
inline constexpr double kThree = 3.0;

// Geometry-related constants.
inline constexpr double kVectorNormEpsilon = 1e-18;
inline constexpr double kWeiszfeldDistanceFloor = 1e-18;
inline constexpr int kWeiszfeldMaxIterations = 200;
inline constexpr double kWeiszfeldConvergenceEpsilonSq = 1e-24;
inline constexpr double kCosineClampMin = -1.0;
inline constexpr double kCosineClampMax = 1.0;
inline constexpr double kFermatObtuseAngleDivisor = 3.0;
inline constexpr double kCentroidDivisor = 3.0;

// Smith relaxation constants.
inline constexpr double kSmithRelaxationTolerance = 1e-10;
inline constexpr int kSmithMaxIterations = 5000;
inline constexpr int kSteinerVertexNeighborCount = 3;

// Comparison constants.
inline constexpr double kMinPositiveLength = 1e-15;
inline constexpr double kPercentScale = 100.0;

} // namespace steiner