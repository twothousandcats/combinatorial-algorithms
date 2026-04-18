#pragma once

#include "Geometry.h"

#include <vector>

namespace steiner
{

class TerminalSetValidator
{
public:
	[[nodiscard]] bool AreAllCoordinatesFinite(const std::vector<Point2D>& terminals) const noexcept;

	void ValidateAllCoordinatesFiniteOrThrow(const std::vector<Point2D>& terminals) const;
};

} // namespace steiner