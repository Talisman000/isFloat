#include "TriangleApp.h"
#include <stdexcept>

void TriangleApp::Prepare()
{
	Vertex triangleVertices[] = {
		{{0.0f, 0.25f, 0.5f}, {1.0f, 0.0f, 0.0f, 1.0f}},
		{{0.25f, -0.25f, 0.5f}, {0.0f, 1.0f, 0.0f, 1.0f}},
		{{-0.25f, -0.25f, 0.5f}, {0.0f, 0.0f, 1.0f, 1.0f}},
	};
	uint32_t indices[] = {0, 1, 2};
}

void TriangleApp::Cleanup()
{
}

void TriangleApp::MakeCommand(ComPtr<ID3D12GraphicsCommandList>& command)
{
}
