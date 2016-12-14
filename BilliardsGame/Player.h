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

	bool Init(const DX11Manager* dx3D);

	void InitShotState(const XMFLOAT3& myBall, const XMFLOAT3& targetBall);
	void UpdateInput(const InputManager* input);
	bool UpdateFreeDrop(const InputManager* input, Ball* b, bool cameraFlip, bool isFirstperson);
	void ShotBall(Ball* b);
	void ShotBallMaxPower(Ball* b);
	bool Render(const DX11Manager* dx3D, const ShaderManager* shaderManager,
		const XMFLOAT4X4& view, const XMFLOAT4X4& projection, const Light* light, const Ball* ball);

	void ResetDecideShot(){ m_isDecide = false; }
	bool IsDecideShot()const{ return m_isDecide; }
	float GetShotDir()const{ return m_shotDirection; }

private:
	bool InputFreeDropBirdeye(const InputManager* input, Ball* b, bool cameraFlip);
	bool InputFreeDropFirstperson(const InputManager* input, Ball* b);

	float m_shotPower;
	float m_shotDirection;
	bool m_isDecide;

	Model* m_guideModel;
};

