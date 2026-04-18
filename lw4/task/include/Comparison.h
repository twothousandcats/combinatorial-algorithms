#pragma once

#include "Constants.h"

#include <cmath>
#include <limits>

namespace steiner
{

struct AlgorithmLengthRatios
{
	double mst_over_esmt = std::numeric_limits<double>::quiet_NaN();
	double esmt_save_percent_vs_mst = std::numeric_limits<double>::quiet_NaN();
	bool mst_over_esmt_valid = false;
	bool esmt_save_percent_valid = false;
};

// Считает отношения длин MST и ESMT и нужен для интерпретации выигрыша Штейнера.
inline AlgorithmLengthRatios ComputeAlgorithmLengthRatios(double mstLength, double esmtLength)
{
	AlgorithmLengthRatios result{};
	const bool mstUsable = std::isfinite(mstLength) && mstLength > kMinPositiveLength;
	const bool esmtUsable = std::isfinite(esmtLength) && esmtLength > kMinPositiveLength;

	if (mstUsable && esmtUsable)
	{
		result.mst_over_esmt = mstLength / esmtLength;
		result.mst_over_esmt_valid = true;

		result.esmt_save_percent_vs_mst = kPercentScale * (kOne - esmtLength / mstLength);
		result.esmt_save_percent_valid = true;
	}

	return result;
}

} // namespace steiner