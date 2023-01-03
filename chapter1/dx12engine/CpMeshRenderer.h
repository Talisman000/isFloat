#pragma once
#include "Component.h"
#include "MeshRenderer.h"

class CpMeshRenderer : public Component
{
	std::shared_ptr<MeshRenderer> m_meshRenderer = nullptr;
public:
	CpMeshRenderer(std::shared_ptr<GameObject> gameObject, const int priority = -999) : Component(gameObject, priority) {}
	void SetMesh(Core* core, Mesh mesh);
	void SetMesh(Core* core, std::vector<Mesh> meshes);
	void Update() override;
	void Draw() override;
};

