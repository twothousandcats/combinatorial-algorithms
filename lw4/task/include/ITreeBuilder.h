#pragma once

#include "TreeTypes.h"
#include <vector>

class ITreeBuilder
{
public:
	virtual ~ITreeBuilder() = default;

	// Takes only coordinates for calculation.
	// Indices in result correspond to indices in input vector
	virtual TreeResult Build(const std::vector<Point>& points) = 0;
};