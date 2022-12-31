#pragma once
#include "ConstantBuffer.h"
#include "Core.h"
#include "IndexBuffer.h"
#include "PipelineState.h"
#include "RootSignature.h"
#include "SharedStruct.h"
#include "VertexBuffer.h"
class SquareRenderer
{
private:
	Core* m_core = nullptr;
	VertexBuffer* m_vertexBuffer = nullptr;
	IndexBuffer* m_indexBuffer = nullptr;
	ConstantBuffer* m_constantBuffer[2];
	RootSignature* m_rootSignature = nullptr;
	PipelineState* m_pipelineState = nullptr;
public:
	SquareRenderer(Core* core);
	bool Init();
	void Update(RenderProperty tran);
	void Draw();
};

