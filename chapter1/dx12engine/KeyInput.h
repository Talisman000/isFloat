#pragma once
#include <map>


class KeyInput
{
	struct KeyState
	{
		int pressedFrame = 0;
		bool isDown = false;
		bool isPressed = false;
	};

	inline static std::map<int, KeyState> m_listeningKeys = std::map<int,KeyState>();
public:
	static void Update();
	static void AddListen(int keyCode);
	static bool OnKeyDown(int keyCode);
	static bool OnKeyPress(int keyCode);
};

