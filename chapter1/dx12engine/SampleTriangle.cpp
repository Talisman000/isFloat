#include "SampleTriangle.h"

SampleTriangle::SampleTriangle(Core* core)
{
	TriangleMesh mesh = TriangleMesh();
	meshRenderer = new MeshRenderer(core, mesh);
}

void SampleTriangle::Draw()
{
	meshRenderer->Update(transform);
	meshRenderer->Draw();
}
