#pragma once
#include "TriangleRenderer.h"

class SampleTriangle
{
private:
	TriangleRenderer* triangleRenderer;
public:
	RenderProperty transform;
	SampleTriangle(Core* core);
	void Draw();
};

