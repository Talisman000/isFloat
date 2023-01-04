#pragma once
#include <memory>
#include <utility>
#include  <Windows.h>
#include "GameObject.h"

class GameObject;


// éQçl: https://qiita.com/harayuu10/items/bf6d73353efa45212200
class Component
{
private:
	bool isActive = true;
protected:
	std::weak_ptr<GameObject> m_parent;
public:
	virtual ~Component() { OutputDebugString(L"Component Removed\n"); }
	Component(std::shared_ptr<GameObject> gameObject, const int priority = 0) : m_parent(std::move(gameObject)),
		m_priority(priority) {}
	int m_priority = 0;
	int GetPriority() const;
	virtual void Start() {}
	virtual void Update() {}
	virtual void Draw() {}
	void SetActive(const bool active) { isActive = active; }
	[[nodiscard]] bool GetActive() const { return isActive; }
};

