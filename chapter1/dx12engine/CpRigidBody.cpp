#include "CPRigidBody.h"

#include "GameObject.h"
#include "Util.h"


void CpRigidBody::Update()
{
	const auto delta = Time::DeltaTime();

	if(isGravity)
	{
		velocity.x += m_property.Gravity.x * delta;
		velocity.y += m_property.Gravity.y * delta;
		velocity.z += m_property.Gravity.z * delta;
	}

	m_parent->transform.Position.x += velocity.x * delta;
	m_parent->transform.Position.y += velocity.y * delta;
	m_parent->transform.Position.z += velocity.z * delta;
}
