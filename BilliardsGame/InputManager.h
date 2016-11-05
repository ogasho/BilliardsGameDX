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

	void KeyDown(unsigned int);
	void KeyUp(unsigned int);

	bool IsKeyDown(unsigned int);

private:
	bool m_keys[256];

};

