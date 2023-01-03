#include "GameObject.h"

#include <algorithm>

#include "Component.h"

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

void GameObject::Update() const
{
	for (const auto& component : m_components)
	{
		component->Update();
	}
}

void GameObject::Draw() const
{
	for (const auto& component : m_components)
	{
		component->Draw();
	}
}
