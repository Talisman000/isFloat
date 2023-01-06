#pragma once
#include "CpMeshRenderer.h"

class CpNumMeshRenderer : public Component
{
	std::vector<std::shared_ptr<MeshRenderer>> m_rendererList;
	std::vector<int> m_indexes;
public:
	CpNumMeshRenderer(std::shared_ptr<GameObject> gameObject, const int priority = -999): Component(gameObject, priority){}
	void Init(Core* core, int digit = 5);
	void SetNumber(int num);
	void Update() override;
	void Draw() override;
};

