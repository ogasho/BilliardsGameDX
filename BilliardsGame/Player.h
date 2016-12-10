/* ビリヤードプレイヤークラス */
// ボールを打つなどの操作受け付け、打つ方向の描画

#pragma once

class Ball;
class InputManager;
class Model;
class DX11Manager;
class ShaderManager;
class Light;

#include <DirectXMath.h>
using namespace DirectX;

class Player
{
public:
	Player();
	~Player();

	bool Init(DX11Manager* dx3D);

	void UpdateInput(const InputManager* input);
	void ShotBall(Ball* b);
	bool Render(DX11Manager* dx3D, const ShaderManager* shaderManager,
		const XMFLOAT4X4& view, const XMFLOAT4X4& projection, const Light* light, const Ball* ball);

	void ResetDecideShot(){ m_isDecideShot = false; }
	bool IsDecideShot()const{ return m_isDecideShot; }

private:
	float m_shotPower;
	float m_shotDirection;
	bool m_isDecideShot;

	Model* m_guideModel;
};

