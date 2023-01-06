#pragma once
#include "Core.h"
#include "KeyInput.h"
#include "Time.h"

enum SceneState
{
	Running,
	Reload
};

class Scene
{
protected:
	Core* core = nullptr;
public:
	virtual ~Scene() = default;
	explicit Scene(Core* core) : core(core) {}
	virtual void Start() {}
	virtual SceneState Update() {
		KeyInput::Update();
		return SceneState::Running;
	}
	virtual void Draw() {};
	virtual void Remove() {};
};

