#pragma once
#include <algorithm>
#include <memory>
#include <vector>

#include "Component.h"
#include "SharedStruct.h"

class Component;

enum class Tag
{
	Default,
};

class GameObject : public std::enable_shared_from_this<GameObject>
{
	// AddComponent����this�|�C���^���g�p�������̂ŁAstd::enable_shared_from_this���p������
	// ���̍ہA���|�C���^��GameObject���쐬���邱�Ƃ��Ȃ��悤�ɁA�R���X�g���N�^���B������
	// �Q�l:https://www.kabuku.co.jp/developers/cpp_enable_shared_from_this
	std::vector<std::shared_ptr<Component>> m_components;
	Tag m_tag = Tag::Default;
public:
	GameObject() = default;
	Transform transform;
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
			auto buff = dynamic_cast<std::shared_ptr<T>>(component);
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

	// shared_ptr��GameObject�𐶐�����
	static std::shared_ptr<GameObject> Create()
	{
		return std::make_shared<GameObject>();
	}
};
