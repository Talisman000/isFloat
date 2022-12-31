#pragma once
#include <memory>

#include "ConstantBuffer.h"
#include "Core.h"
#include "DescriptorHeap.h"
#include "IndexBuffer.h"
#include "Mesh.h"
#include "PipelineState.h"
#include "RootSignature.h"
#include "SharedStruct.h"
#include "VertexBuffer.h"

class MeshRenderer
{
	std::vector<Mesh> m_meshes = {};
	Core* m_core = nullptr;
	std::vector<std::shared_ptr<VertexBuffer>> m_vertexBuffers = {};
	std::vector<std::shared_ptr<IndexBuffer>> m_indexBuffers = {};
	std::shared_ptr<DescriptorHeap> m_descriptorHeap = nullptr;
	std::vector<std::shared_ptr<DescriptorHandle>> m_materialHandles = {};
	int m_indexCount = 0;
	ConstantBuffer* m_constantBuffer[2];
	RootSignature* m_rootSignature = nullptr;
	PipelineState* m_pipelineState = nullptr;
public:
	MeshRenderer(Core* core, Mesh mesh);
	MeshRenderer(Core* core, std::vector<Mesh> meshes);
	bool Init();
	void Update(RenderProperty tran);
	void Draw();
};

