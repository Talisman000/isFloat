#include "SampleTriangle.h"

SampleTriangle::SampleTriangle(Core* core)
{
	triangleRenderer = new TriangleRenderer(core);
	triangleRenderer->Init();
}

void SampleTriangle::Draw()
{
	triangleRenderer->Update(transform);
	triangleRenderer->Draw();
}
