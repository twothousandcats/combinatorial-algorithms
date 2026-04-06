#pragma once

#include "ITreeBuilder.h"

// MST
class BoruvkaTree final : public ITreeBuilder
{
public:
	TreeResult Build(const std::vector<Point>& points) override;
};