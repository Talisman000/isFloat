#pragma once
#include <algorithm>
#include <memory>
#include <vector>
#include  <Windows.h>

#include "Random.h"
#include "Component.h"
#include "SharedStruct.h"

class Component;

enum class Tag
{
	Default,
};

class GameObject : public std::enable_shared_from_this<GameObject>
{
	// AddComponent時にthisポインタを使用したいので、std::enable_shared_from_thisを継承する

	std::vector<std::shared_ptr<Component>> m_components;
	Tag m_tag = Tag::Default;
	std::weak_ptr<GameObject> parent;
public:
	GameObject() = default;
	~GameObject(){OutputDebugString(L"GameObject Removed\n");}
	Transform transform;
	void Clear();
	Transform GetAbsoluteTransform() const;
	void SetParent(const std::shared_ptr<GameObject>& obj);
	void Update() const;
	void Draw() const;
	template <class T>
	std::shared_ptr<T> AddComponent(int priority = 0)
	{
		std::shared_ptr<T> buff = std::make_shared<T>(shared_from_this(), priority);
		std::sort(m_components.begin(), m_components.end(), [](const auto& a, const auto& b)
			{
				return a->m_priority > b->m_priority;
			});

		m_components.emplace_back(buff);
		return buff;
	}

	template <class T>
	std::shared_ptr<T> GetComponent()
	{
		for (auto& component : m_components)
		{
			auto buff = std::dynamic_pointer_cast<T>(component);
			if (buff != nullptr)
			{
				return buff;
			}
		}
#if _DEBUG
		printf("[Warning : Component] Component Not Found");
#endif
		return nullptr;
	}

	// shared_ptrなGameObjectを生成する
	static std::shared_ptr<GameObject> Create()
	{
		return std::make_shared<GameObject>();
	}
};

