/* ビリヤードプレイヤークラス */
// ボールを打つなどの操作を受け付ける

#pragma once

class Ball;
class InputManager;

class Player
{
public:
	Player();
	~Player();

	void UpdateInput(InputManager* input);
	void ShotBall(Ball* b);

	void ResetDecideShot(){ m_isDecideShot = false; }
	bool IsDecideShot(){ return m_isDecideShot; }

private:
	float m_shotPower;
	float m_shotDirection;
	bool m_isDecideShot;
};

