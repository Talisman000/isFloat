#include "KeyInput.h"
#include "Windows.h"


void KeyInput::Update()
{
	for (auto& [keyCode, keyState] : m_listeningKeys)
	{
		const auto state = GetKeyState(keyCode);
		// ƒL[‚ª‰Ÿ‚³‚ê‚Ä‚¢‚é
		if (state & 0x8000)
		{
			keyState.isPressed = true;
			keyState.isDown = keyState.pressedFrame == 0;
			keyState.pressedFrame++;
		}
		else
		{
			keyState.isPressed = false;
			keyState.isDown = false;
			if (keyState.pressedFrame != 0)
			{
				keyState.isUp = true;
			}
			else
			{
				keyState.isUp = false;
			}
			keyState.pressedFrame = 0;
		}
	}
}

void KeyInput::AddListen(const int keyCode)
{
	m_listeningKeys[keyCode] = KeyState();
}

bool KeyInput::OnKeyDown(int keyCode)
{
	const auto it = m_listeningKeys.find(keyCode);
	if (it == m_listeningKeys.end())
	{
		OutputDebugString(L"Warning: keycode not found\n");
		return false;
	}
	return it->second.isDown;
}

bool KeyInput::OnKeyPress(int keyCode)
{
	const auto it = m_listeningKeys.find(keyCode);
	if (it == m_listeningKeys.end())
	{
		OutputDebugString(L"Warning: keycode not found\n");
		return false;
	}
	return it->second.isPressed;
}

bool KeyInput::OnKeyUp(int keyCode)
{
	const auto it = m_listeningKeys.find(keyCode);
	if (it == m_listeningKeys.end())
	{
		OutputDebugString(L"Warning: keycode not found\n");
		return false;
	}
	return it->second.isUp;
}
