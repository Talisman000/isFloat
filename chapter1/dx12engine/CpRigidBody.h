#pragma once
#include <DirectXMath.h>

#include "Component.h"

using namespace DirectX;

struct PhysicsProperty
{
	XMFLOAT3 Gravity = { 0, -9.8f * 0.4f, 0 };
	XMFLOAT3 GroundPosition = { 0, -5, 0 };
	float RestitutionCoefficient = 0.5f;
};

class CpRigidBody : public Component
{
	const PhysicsProperty m_property = PhysicsProperty();
private:
	void ApplyAcceleration();
	XMFLOAT3 m_force = { 0,0,0 };
public:
	CpRigidBody(std::shared_ptr<GameObject> gameObject, const int priority = 0) : Component(gameObject, priority) {}
	XMFLOAT3 velocity = { 0,0,0 };
	bool isGravity = true;
	bool isGroundCollision = true;
	bool isRestitution = true;
	void Update() override;
	void AddForce(XMFLOAT3 force);
};

