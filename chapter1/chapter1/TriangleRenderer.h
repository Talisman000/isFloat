#pragma once
#include "ConstantBuffer.h"
#include "Core.h"
#include "PipelineState.h"
#include "RootSignature.h"
#include "SharedStruct.h"
#include "VertexBuffer.h"


class TriangleRenderer
{
private:
	Core* m_core = nullptr;
	VertexBuffer* m_vertexBuffer = nullptr;
	ConstantBuffer* m_constantBuffer[2];
	RootSignature* m_rootSignature = nullptr;
	PipelineState* m_pipelineState = nullptr;
public:
	TriangleRenderer(Core* core);
	bool Init();
	void Update(UniTransform tran);
	void Draw();
};

