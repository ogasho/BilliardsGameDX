/* �r�����[�h�v���C���[�N���X */
// �{�[����łȂǂ̑���󂯕t���A�ł����̕`��

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

	void InitShotState(const XMFLOAT3& myBall, const XMFLOAT3& targetBall);
	void UpdateInput(const InputManager* input);
	void ShotBall(Ball* b);
	bool Render(DX11Manager* dx3D, const ShaderManager* shaderManager,
		const XMFLOAT4X4& view, const XMFLOAT4X4& projection, const Light* light, const Ball* ball);

	void ResetDecideShot(){ m_isDecideShot = false; }
	bool IsDecideShot()const{ return m_isDecideShot; }
	float GetShotDir()const{ return m_shotDirection; }

private:
	float m_shotPower;
	float m_shotDirection;
	bool m_isDecideShot;

	Model* m_guideModel;
};

