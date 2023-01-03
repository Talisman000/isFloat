#pragma once
#include <DirectXMath.h>

#include "Component.h"

using namespace DirectX;

struct PhysicsProperty
{
	XMFLOAT3 Gravity = { 0, -9.8f, 0 };
};

class CpRigidBody : public Component
{
	const PhysicsProperty m_property = PhysicsProperty();
public:
	CpRigidBody(std::shared_ptr<GameObject> gameObject, const int priority = 0): Component(gameObject, priority){}
	XMFLOAT3 velocity = { 0,0,0 };
	bool isGravity = true;
	void Update() override;
};

