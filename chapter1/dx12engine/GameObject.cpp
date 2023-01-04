#include "GameObject.h"

#include <algorithm>

#include "Component.h"
#include "XMFLOATHelper.h"

//template <class T>
//std::shared_ptr<T> GameObject::AddComponent(int priority)
//{
//	std::shared_ptr<T> buff = std::make_shared<T>(shared_from_this(), priority);
//	std::sort(m_components.begin(), m_components.end(), [](const auto& a, const auto& b)
//		{
//			return a->GetPrioriry() > b->GetPriority();
//		});
//	return buff;
//}
//
//template <class T>
//std::shared_ptr<T> GameObject::GetComponent()
//{
//	for (auto& component : m_components)
//	{
//		auto buff = dynamic_cast<std::shared_ptr<T>>(component);
//		if (buff != nullptr)
//		{
//			return buff;
//		}
//	}
//#if _DEBUG
//		printf("[Warning : Component] Component Not Found");
//#endif
//		return nullptr;
//}

void GameObject::Clear()
{
	//for(auto& com : m_components)
	//{
	//	com.reset();
	//}
	//m_components.clear();
}

Transform GameObject::GetAbsoluteTransform() const
{
	if (parent.lock())
	{
		const auto parentTran = parent.lock()->GetAbsoluteTransform();
		auto tran = transform;
		tran.Position += parentTran.Position;
		tran.Rotation += parentTran.Rotation;
		tran.Scale *= parentTran.Scale;
		return tran;
	}
	return transform;
}

void GameObject::SetParent(const std::shared_ptr<GameObject>& obj)
{
	parent = obj;
}

void GameObject::Update() const
{
	for (const auto& component : m_components)
	{
		if (component->GetActive()) 
			component->Update();
	}
}

void GameObject::Draw() const
{
	for (const auto& component : m_components)
	{
		if (component->GetActive()) 
			component->Draw();
	}
}
