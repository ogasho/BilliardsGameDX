#include "Player.h"
#include "DXUtil.h"

#include "InputManager.h"
#include "Ball.h"
#include "Model.h"
#include "DX11Manager.h"
#include "Texture.h"
#include "Light.h"
#include "TextureShader.h"

#include <string>

#define MAX_SHOT_POWER 5.0f
#define SPEED_ROTATE_DIR 2.0f
#define SPEED_CHANGE_POW 0.1f


Player::Player()
{
	m_shotDirection = 0;
	m_shotPower = 2.0f;
	m_isDecideShot = false;

	m_guideModel = nullptr;
}

Player::~Player()
{
	SafeDelete(m_guideModel);
}

bool Player::Init(DX11Manager* dx3D)
{
	bool result = false;

	m_guideModel = new Model;
	result = m_guideModel->Init(dx3D->GetDevice(), dx3D->GetDeviceContext(),
		"data/triangle.obj", "data/red.tga");

	return result;
}

void Player::UpdateInput(const InputManager* input)
{
	bool isUpdated = false;

	// �ł���
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

	// �ł���
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

	// ����
	if (input->IsKeyDown(UseKeys::Enter))
		m_isDecideShot = true;

	// �l�␳
	if (m_shotPower < 0.5f)
		m_shotPower = 0.5f;
	else if (m_shotPower > MAX_SHOT_POWER)
		m_shotPower = MAX_SHOT_POWER;
	if (m_shotDirection > 180.0f)
		m_shotDirection -= 360.0f;
	else if (m_shotDirection < -180.0f)
		m_shotDirection += 360.0f;


	/* �e�X�g�o�� */
	if (isUpdated)
	{
		MyOutputDebugString(L"\n pow=%f, dir=%f\n", m_shotPower, m_shotDirection);
	}
}

void Player::ShotBall(Ball* b)
{
	// �w��̊p�x/�����őł�
	float dirRadian = XMConvertToRadians(m_shotDirection);
	float xPower = cos(dirRadian) * m_shotPower;
	float zPower = sin(dirRadian) * m_shotPower;

	b->SetMoveVec(XMFLOAT3(xPower, 0.0f, zPower));
}

bool Player::Render(DX11Manager* dx3D, TextureShader* textureShader,
	const XMFLOAT4X4& view, const XMFLOAT4X4& projection, const Light* light, const Ball* ball)
{
	// �ł���/�����̃K�C�h��`��
	bool result;
	XMFLOAT4X4 worldMatrix;

	XMFLOAT3 ballPos;
	ball->GetPosition(&ballPos); // ���W�͔����̈ʒu

	float modelRadian = XMConvertToRadians(-m_shotDirection);// �p�x�͑łp�x
	float modelScale = (m_shotPower / MAX_SHOT_POWER) * 15.0f + 1.0f; // �傫���͑ł���

	m_guideModel->UpdateWorldMatrix(ballPos, XMFLOAT3(0, modelRadian, 0), XMFLOAT3(modelScale, 1, 2));
	m_guideModel->GetWorldMatrix(&worldMatrix);

	m_guideModel->Render(dx3D->GetDeviceContext());

	result = textureShader->Render(dx3D->GetDeviceContext(), m_guideModel->GetIndexCount(),
		worldMatrix, view, projection, m_guideModel->GetTexture(),
		light->GetDirection(), light->GetDiffuseColor());

	return result;
}