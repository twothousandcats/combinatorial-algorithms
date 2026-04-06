#pragma once

#include "ITreeBuilder.h"

class SteinerTree final : public ITreeBuilder
{
public:
	TreeResult Build(const std::vector<Point>& points) override;
};