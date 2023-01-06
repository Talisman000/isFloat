#pragma once
#include <memory>
#include "Mesh.h"
#include "Engine.h"

class MeshRenderer
{
	std::vector<Mesh> m_meshes = {};
	Core* m_core = nullptr;
	std::shared_ptr<VertexBuffer> m_vertexBuffer = nullptr;
	std::vector<std::shared_ptr<VertexBuffer>> m_vertexBuffers = {};
	std::vector<std::shared_ptr<IndexBuffer>> m_indexBuffers = {};
	std::shared_ptr<DescriptorHeap> m_descriptorHeap = nullptr;
	std::vector<std::shared_ptr<DescriptorHandle>> m_materialHandles = {};
	int m_indexCount = 0;
	std::shared_ptr<ConstantBuffer> m_constantBuffer[2];
	std::shared_ptr<RootSignature> m_rootSignature = nullptr;
	std::shared_ptr<PipelineState> m_pipelineState = nullptr;
public:
	MeshRenderer(Core* core, Mesh mesh);
	MeshRenderer(Core* core, std::vector<Mesh> meshes);
	bool Init();
	void Update(Transform tran) const;
	void Draw();
	void DrawIndexed(int n);
};

