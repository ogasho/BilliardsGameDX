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
}

void InputManager::KeyDown(unsigned int input)
{
	m_keys[input] = true;
}

void InputManager::KeyUp(unsigned int input)
{
	m_keys[input] = false;
}

bool InputManager::IsKeyDown(unsigned int key)
{
	return m_keys[key];
}

bool InputManager::IsKeyDown(UseKeys key)
{
	switch (key)
	{
	case UseKeys::Up:
		return m_keys[VK_UP];

	case UseKeys::Down:
		return m_keys[VK_DOWN];

	case UseKeys::Left:
		return m_keys[VK_LEFT];

	case UseKeys::Right:
		return m_keys[VK_RIGHT];

	case UseKeys::Enter:
		return m_keys[VK_RETURN];

	}

	return false;
}