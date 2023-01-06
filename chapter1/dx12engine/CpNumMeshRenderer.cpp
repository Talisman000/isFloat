#include "CpNumMeshRenderer.h"

#include <format>
#include <string>
#include "XMFLOATHelper.h"

void CpNumMeshRenderer::Init(Core* core, int digit)
{
	for (int i = 0; i < digit; i++)
	{
		std::vector<Mesh> meshes;
		for (int j = 0; j < 10; j++)
		{
			auto path = std::format(L"Assets/Num/num{}.png", j);
			auto mesh = SquareMesh({ 1,1,1,1 }, path);
			meshes.emplace_back(mesh);
		}
		m_rendererList.emplace_back(std::make_shared<MeshRenderer>(core, meshes));
	}
	m_indexes.assign(m_rendererList.size(), 0);
}

void CpNumMeshRenderer::SetNumber(int num)
{
	if (const auto max = static_cast<int>(std::pow(10, m_indexes.size())) - 1; num > max)
	{
		num = max;
	}
	for (int i = 0; i < m_indexes.size(); i++)
	{
		const auto dig = num % 10;
		m_indexes[i] = dig;
		num /= 10;
	}
	std::ranges::reverse(m_indexes);
}

void CpNumMeshRenderer::Update()
{
	if (m_rendererList.size() == 0) return;
	for (int i = 0; i < m_rendererList.size(); i++)
	{
		auto tran = m_parent.lock()->GetAbsoluteTransform();
		tran.Position += {i * m_parent.lock()->transform.Scale.x * 2, 0, 0};
		m_rendererList[i]->Update(tran);
	}
}

void CpNumMeshRenderer::Draw()
{
	for (int i = 0; i < m_indexes.size(); i++)
	{
		m_rendererList[i]->DrawIndexed(m_indexes[i]);
	}
}
