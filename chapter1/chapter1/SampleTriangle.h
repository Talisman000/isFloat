#pragma once
#include "TriangleRenderer.h"

class SampleTriangle
{
private:
	TriangleRenderer* triangleRenderer;
public:
	UniTransform transform;
	SampleTriangle(Core* core);
	void Draw();
};

