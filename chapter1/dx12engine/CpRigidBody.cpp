#include "CPRigidBody.h"

#include "GameObject.h"
#include "Time.h"
#include "XMFLOATHelper.h"


void CpRigidBody::Update()
{
	const auto delta = Time::DeltaTime();

	// ‰Á‘¬“xŒvŽZ
	if (isGravity)
	{
		m_force += m_property.Gravity;
	}
	const auto forceDelta = m_force * delta;
	velocity += forceDelta;
	m_force *= 0;

	auto parent = m_parent.lock();
	parent->transform.Position += velocity * delta;
}

void CpRigidBody::AddForce(XMFLOAT3 force)
{
	m_force += force;
}
