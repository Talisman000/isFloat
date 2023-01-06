#pragma once
#include "Core.h"
#include "KeyInput.h"
#include "Time.h"

class Scene
{
protected:
	Core* core = nullptr;
public:
	virtual ~Scene() = default;
	explicit Scene(Core* core) : core(core) {}
	virtual void Start() {}
	virtual void Update() {
		KeyInput::Update();
	}
	virtual void Draw() {};
	virtual void Remove() {};
};

