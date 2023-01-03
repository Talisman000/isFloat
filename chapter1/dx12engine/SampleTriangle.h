#pragma once
#include "MeshRenderer.h"

class SampleTriangle
{
private:
	MeshRenderer* meshRenderer;
public:
	Transform transform;
	SampleTriangle(Core* core);
	void Draw();
};

