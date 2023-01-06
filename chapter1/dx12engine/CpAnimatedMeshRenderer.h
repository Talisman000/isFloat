#pragma once
#include <map>

#include "CpMeshRenderer.h"

struct FlipAnimation
{
	float freq = 0.1f;
	bool isLoop = true;
	std::vector<int> indexes = {};
};

// 簡易アニメーションコンポーネント
// 時間に応じてMeshRendererを切り替えてるだけ
// 名前を指定してアニメーションを切り替えたり出来ます
class CpFlipAnimatedMeshRenderer : public  CpMeshRenderer
{
private:
	float m_elapsedTime = 0;
	std::map<std::string, FlipAnimation> m_animation = {};
	FlipAnimation m_currentAnimation = {};
	std::string m_currentAnimationName = "";
public:
	CpFlipAnimatedMeshRenderer(std::shared_ptr<GameObject> gameObject, const int priority = -999): CpMeshRenderer(gameObject, priority){}
	void SetMeshes(Core* core, std::vector<Mesh> meshes);
	void RegisterAnimation(const std::string& name, FlipAnimation animation);
	void ChangeAnimation(const std::string& name);
	bool IsAnimationEnd() const;
	void Update() override;
	void Draw() override;
};

