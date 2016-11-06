/* 入力処理クラス */
// 入力状態を保管、提供する
// 入力状態はウィンドウプロシージャで更新する

#pragma once

enum class UseKeys
{
	Up,
	Down,
	Left,
	Right,
	Enter,
};

class InputManager
{
public:
	InputManager();
	~InputManager();

	void Init();

	void KeyDown(unsigned int input);
	void KeyUp(unsigned int input);

	bool IsKeyDown(unsigned int key);
	bool IsKeyDown(UseKeys key);

private:
	bool m_keys[256];

};

