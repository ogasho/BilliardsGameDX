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
	void UpdateKeyState(); // フレーム更新の際に呼ぶ

	bool IsKeyDown(unsigned int key);
	bool IsKeyDown(UseKeys key);
	bool IsFrameKeyDown(unsigned int key);
	bool IsFrameKeyDown(UseKeys key);

private:
	unsigned int TransUseKeys(UseKeys key);

	bool m_keys[256];
	bool m_oldKeys[256];
};

