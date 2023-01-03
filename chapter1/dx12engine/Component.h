#pragma once
#include <memory>
#include <utility>

#include "GameObject.h"

class GameObject;

// éQçl: https://qiita.com/harayuu10/items/bf6d73353efa45212200
class Component
{
protected:
	std::shared_ptr<GameObject> m_parent;
public:
	virtual ~Component() = default;
	Component(std::shared_ptr<GameObject> gameObject, const int priority = 0) : m_priority(priority),
	                                                                                     m_parent(std::move(gameObject)) {}
	int m_priority = 0;
	int GetPriority() const;
	virtual void Start() {}
	virtual void Update() {}
	virtual void Draw() {}
};

