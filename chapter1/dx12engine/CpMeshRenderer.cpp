#include "CpMeshRenderer.h"

#include "GameObject.h"

void CpMeshRenderer::SetMesh(Core* core, Mesh mesh)
{
	m_meshRenderer = std::make_shared<MeshRenderer>(core, mesh);
	if (m_meshRenderer == nullptr)
	{
		OutputDebugStringA("mesh renderer init failed");
	}
}

void CpMeshRenderer::SetMesh(Core* core, std::vector<Mesh> meshes)
{
	m_meshRenderer = std::make_shared<MeshRenderer>(core, meshes);
	if (m_meshRenderer == nullptr)
	{
		OutputDebugStringA("mesh renderer init failed");
	}
}

void CpMeshRenderer::Update()
{
	if(m_meshRenderer == nullptr) return;
	m_meshRenderer->Update(m_parent.lock()->GetAbsoluteTransform());
}


void CpMeshRenderer::Draw()
{
	if(m_meshRenderer == nullptr) return;
	m_meshRenderer->Draw();
}
