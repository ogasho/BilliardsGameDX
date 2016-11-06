/* 入力処理クラス */
// 入力状態を保管、提供する
// 入力状態はウィンドウプロシージャで更新する

#pragma once

class InputManager
{
public:
	InputManager();
	~InputManager();

	void Init();

	void KeyDown(unsigned int input);
	void KeyUp(unsigned int input);

	bool IsKeyDown(unsigned int key);

private:
	bool m_keys[256];

};

