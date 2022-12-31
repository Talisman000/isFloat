#pragma once
#include <memory>

#include "ConstantBuffer.h"
#include "Core.h"
#include "IndexBuffer.h"
#include "Mesh.h"
#include "PipelineState.h"
#include "RootSignature.h"
#include "SharedStruct.h"
#include "VertexBuffer.h"

class MeshRenderer
{
	Mesh m_mesh;
	Core* m_core = nullptr;
	VertexBuffer* m_vertexBuffer = nullptr;
	IndexBuffer* m_indexBuffer = nullptr;
	int m_indexCount = 0;
	ConstantBuffer* m_constantBuffer[2];
	RootSignature* m_rootSignature = nullptr;
	PipelineState* m_pipelineState = nullptr;
public:
	MeshRenderer(Core* core, Mesh mesh);
	bool Init();
	void Update(RenderProperty tran);
	void Draw();
};

