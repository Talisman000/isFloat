#include "CpAnimatedMeshRenderer.h"

#include "Time.h"

void CpFlipAnimatedMeshRenderer::SetMeshes(Core* core, const std::vector<Mesh> meshes)
{
	SetMesh(core, meshes);
}

void CpFlipAnimatedMeshRenderer::RegisterAnimation(const std::string& name, const FlipAnimation animation)
{
	m_animation[name] = animation;
}

void CpFlipAnimatedMeshRenderer::ChangeAnimation(const std::string& name)
{
	if (name == m_currentAnimationName) return;
	const auto it = m_animation.find(name);
	if (it == m_animation.end())
	{
		OutputDebugString(L"[Warning] animation not found");
		return;
	}
	m_currentAnimation = it->second;
	m_currentAnimationName = name;
	m_elapsedTime = 0;
}

bool CpFlipAnimatedMeshRenderer::IsAnimationEnd() const
{
	if (m_currentAnimation.isLoop == true) return false;
	if (m_currentAnimation.indexes.size() == 0)return true;
	auto index = static_cast<int>(std::floorf(m_elapsedTime / m_currentAnimation.freq));
	return index >= m_currentAnimation.indexes.size();
}

void CpFlipAnimatedMeshRenderer::Update()
{
	m_elapsedTime += Time::DeltaTime();
	CpMeshRenderer::Update();
}

void CpFlipAnimatedMeshRenderer::Draw()
{
	if (m_currentAnimation.indexes.size() == 0)return;
	auto index = static_cast<int>(std::floorf(m_elapsedTime / m_currentAnimation.freq));
	if (m_currentAnimation.isLoop)
	{
		index %= m_currentAnimation.indexes.size();
	}
	else if (index >= m_currentAnimation.indexes.size())
	{
		index = m_currentAnimation.indexes.size() - 1;
	}
	m_meshRenderer->DrawIndexed(m_currentAnimation.indexes[index]);
}
