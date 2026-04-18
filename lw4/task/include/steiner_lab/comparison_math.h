#pragma once

#include "steiner_lab/constants.h"

#include <cmath>
#include <limits>

namespace steiner_lab
{

struct AlgorithmLengthRatios
{
	double mst_over_esmt = std::numeric_limits<double>::quiet_NaN();
	double esmt_save_percent_vs_mst = std::numeric_limits<double>::quiet_NaN();
	bool mst_over_esmt_valid = false;
	bool esmt_save_percent_valid = false;
};

inline AlgorithmLengthRatios ComputeAlgorithmLengthRatios(double mstLength, double esmtLength)
{
	AlgorithmLengthRatios result{};
	const double minLen = comparison_constants::kMinPositiveLength;

	if (esmtLength > minLen && std::isfinite(esmtLength))
	{
		if (mstLength > minLen && std::isfinite(mstLength))
		{
			result.mst_over_esmt = mstLength / esmtLength;
			result.mst_over_esmt_valid = true;
		}
	}

	if (mstLength > minLen && std::isfinite(mstLength) && esmtLength > minLen && std::isfinite(esmtLength))
	{
		result.esmt_save_percent_vs_mst =
			comparison_constants::kPercentScale * (1.0 - esmtLength / mstLength);
		result.esmt_save_percent_valid = true;
	}

	return result;
}

} // namespace steiner_lab
