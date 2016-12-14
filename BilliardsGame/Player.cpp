#include "Player.h"
#include "DXUtil.h"

#include "InputManager.h"
#include "Ball.h"
#include "Model.h"
#include "DX11Manager.h"
#include "ShaderManager.h"
#include "Texture.h"
#include "Light.h"

#include <string>

#define MAX_SHOT_POWER 5.5f
#define SPEED_ROTATE_DIR 1.0f
#define SPEED_CHANGE_POW 0.1f
#define SPEED_MOVE_BALL 1.0f;

Player::Player()
{
	m_shotDirection = 0;
	m_shotPower = 2.0f;
	m_isDecide = false;

	m_guideModel = nullptr;
}

Player::~Player()
{
	SafeDelete(m_guideModel);
}

bool Player::Init(const DX11Manager* dx3D)
{
	bool result = false;

	m_guideModel = new Model;
	result = m_guideModel->Init(dx3D->GetDevice(), dx3D->GetDeviceContext(),
		"data/triangle.obj", "data/red.tga");

	return result;
}

void Player::InitShotState(const XMFLOAT3& myBall, const XMFLOAT3& targetBall)
{
	// 自動で次に打つべき球の方向に向く
	float dirRad = atan2(targetBall.z - myBall.z, targetBall.x - myBall.x);
	m_shotDirection = XMConvertToDegrees(dirRad);

	// 強さ初期化
	m_shotPower = 2.0f;
}

void Player::UpdateInput(const InputManager* input)
{
	bool isUpdated = false;

	// 打つ方向
	if (input->IsKeyDown(UseKeys::Left))
	{
		isUpdated = true;
		m_shotDirection += SPEED_ROTATE_DIR;
	}
	else if (input->IsKeyDown(UseKeys::Right))
	{
		isUpdated = true;
		m_shotDirection -= SPEED_ROTATE_DIR;
	}

	// 打つ強さ
	if (input->IsKeyDown(UseKeys::Up))
	{
		isUpdated = true;
		m_shotPower += SPEED_CHANGE_POW;
	}
	else if (input->IsKeyDown(UseKeys::Down))
	{
		isUpdated = true;
		m_shotPower -= SPEED_CHANGE_POW;
	}

	// 決定
	if (input->IsFrameKeyDown(UseKeys::Enter))
		m_isDecide = true;

	// 値補正
	if (m_shotPower < 0.5f)
		m_shotPower = 0.5f;
	else if (m_shotPower > MAX_SHOT_POWER)
		m_shotPower = MAX_SHOT_POWER;
	if (m_shotDirection > 180.0f)
		m_shotDirection -= 360.0f;
	else if (m_shotDirection < -180.0f)
		m_shotDirection += 360.0f;
}

bool Player::UpdateFreeDrop(const InputManager* input, Ball* b, bool cameraFlip, bool isFirstperson)
{
	//ボールを移動させる
	bool moving = false;

	// カメラモードが一人称かどうかで挙動を変える
	if (isFirstperson)
	{
		moving = InputFreeDropFirstperson(input, b);
	}
	else
	{
		moving = InputFreeDropBirdeye(input, b, cameraFlip);
	}

	// 決定
	if (!moving && input->IsFrameKeyDown(UseKeys::Enter))
		m_isDecide = true;

	return moving;
}

bool Player::InputFreeDropBirdeye(const InputManager* input, Ball* b, bool cameraFlip)
{
	bool moving = false;

	// 俯瞰視点でのボール移動処理
	XMFLOAT3 ballVec(0, 0, 0);
	float flip = 1.0f;
	if (cameraFlip)
		flip = -1.0f;

	if (input->IsKeyDown(UseKeys::Left))
	{
		ballVec.x -= flip * SPEED_MOVE_BALL;
		moving = true;
	}
	else if (input->IsKeyDown(UseKeys::Right))
	{
		ballVec.x += flip * SPEED_MOVE_BALL;
		moving = true;
	}
	if (input->IsKeyDown(UseKeys::Up))
	{
		ballVec.z += flip * SPEED_MOVE_BALL;
		moving = true;
	}
	else if (input->IsKeyDown(UseKeys::Down))
	{
		ballVec.z -= flip * SPEED_MOVE_BALL;
		moving = true;
	}

	b->SetMoveVec(ballVec);

	return moving;
}

bool Player::InputFreeDropFirstperson(const InputManager* input, Ball* b)
{
	bool moving = false;
	// 一人称視点でのボール移動処理
	XMFLOAT3 ballVec(0, 0, 0);
	float dirRad = XMConvertToRadians(m_shotDirection);
	float dirPerRad = XMConvertToRadians(m_shotDirection + 90.0f);

	if (input->IsKeyDown(UseKeys::Left))
	{
		ballVec.x += cos(dirPerRad) * SPEED_MOVE_BALL;
		ballVec.z += sin(dirPerRad) * SPEED_MOVE_BALL;
		moving = true;
	}
	else if (input->IsKeyDown(UseKeys::Right))
	{
		ballVec.x -= cos(dirPerRad) * SPEED_MOVE_BALL;
		ballVec.z -= sin(dirPerRad) * SPEED_MOVE_BALL;
		moving = true;
	}
	if (input->IsKeyDown(UseKeys::Up))
	{
		ballVec.x += cos(dirRad) * SPEED_MOVE_BALL;
		ballVec.z += sin(dirRad) * SPEED_MOVE_BALL;
		moving = true;
	}
	else if (input->IsKeyDown(UseKeys::Down))
	{
		ballVec.x -= cos(dirRad) * SPEED_MOVE_BALL;
		ballVec.z -= sin(dirRad) * SPEED_MOVE_BALL;
		moving = true;
	}

	b->SetMoveVec(ballVec);

	return moving;
}

void Player::ShotBall(Ball* b)
{
	// 指定の角度/強さで打つ
	float dirRadian = XMConvertToRadians(m_shotDirection);
	float xPower = cos(dirRadian) * m_shotPower;
	float zPower = sin(dirRadian) * m_shotPower;

	b->SetMoveVec(XMFLOAT3(xPower, 0.0f, zPower));
}

void Player::ShotBallMaxPower(Ball* b)
{
	// (オートプレイ用)一番の強さで打つ
	m_shotPower = MAX_SHOT_POWER;
	ShotBall(b);
}

bool Player::Render(const DX11Manager* dx3D, const ShaderManager* shaderManager,
	const XMFLOAT4X4& view, const XMFLOAT4X4& projection, const Light* light, const Ball* ball)
{
	// 打つ方向/強さのガイドを描画
	bool result;
	XMFLOAT4X4 worldMatrix;

	XMFLOAT3 ballPos;
	ball->GetPosition(&ballPos); // 座標は白球の位置

	float modelRadian = XMConvertToRadians(-m_shotDirection);// 角度は打つ角度
	float modelScale = (m_shotPower / MAX_SHOT_POWER) * 25.0f + 5.0f; // 大きさは打つ強さ

	m_guideModel->UpdateWorldMatrix(ballPos, XMFLOAT3(0, modelRadian, 0), XMFLOAT3(modelScale, 1, 2));
	m_guideModel->GetWorldMatrix(&worldMatrix);

	m_guideModel->Render(dx3D->GetDeviceContext());

	result = shaderManager->RenderLightShader(dx3D->GetDeviceContext(), m_guideModel->GetIndexCount(),
		worldMatrix, view, projection, m_guideModel->GetTexture(),
		light->GetDirection(), light->GetDiffuseColor());

	return result;
}