#include "InputManager.h"

#include <Windows.h>

InputManager::InputManager()
{
}


InputManager::~InputManager()
{
}


void InputManager::Init()
{
	for (int i = 0; i < 256; i++)
	{
		m_keys[i] = false;
	}

	for (int i = 0; i < 256; i++)
	{
		m_oldKeys[i] = false;
	}
}

void InputManager::KeyDown(unsigned int input)
{
	m_keys[input] = true;
}

void InputManager::KeyUp(unsigned int input)
{
	m_keys[input] = false;
}

void InputManager::UpdateKeyState()
{
	for (int i = 0; i < 256; i++)
	{
		m_oldKeys[i] = m_keys[i];
	}
}

bool InputManager::IsKeyDown(unsigned int key)const
{
	return m_keys[key];
}

bool InputManager::IsFrameKeyDown(unsigned int key)const
{
	return m_keys[key] && !m_oldKeys[key];
}

bool InputManager::IsKeyDown(UseKeys key)const
{
	return IsKeyDown(TransUseKeys(key));
}

bool InputManager::IsFrameKeyDown(UseKeys key)const
{
	return IsFrameKeyDown(TransUseKeys(key));
}

unsigned int InputManager::TransUseKeys(UseKeys key)const
{
	switch (key)
	{
	case UseKeys::Up:
		return VK_UP;
	case UseKeys::Down:
		return VK_DOWN;
	case UseKeys::Left:
		return VK_LEFT;
	case UseKeys::Right:
		return VK_RIGHT;
	case UseKeys::Enter:
		return VK_RETURN;
	}

	return 0;
}