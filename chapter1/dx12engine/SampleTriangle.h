#pragma once
#include "MeshRenderer.h"

class SampleTriangle
{
private:
	MeshRenderer* meshRenderer;
public:
	RenderProperty transform;
	SampleTriangle(Core* core);
	void Draw();
};

