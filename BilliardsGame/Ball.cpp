#include "Ball.h"
#include "DXUtil.h"

#include "DX11Manager.h"
#include "ShaderManager.h"
#include "Model.h"
#include "Light.h"

#include <random>

static const float DECLINE_VEC_RATE = 0.995f; // 移動ベクトルの減退
static const float STOP_VEC_VALUE = 0.01f; // 移動量がこの値以下になったら停止する
static const float BRAKE_VEC_RATE = 0.95f; // ブレーキ時の移動ベクトルの減退
static const float BRAKE_VEC_VALUE = 0.1f; // 移動量がこの値以下になったらブレーキする
static const float INIT_POS_DEV = 0.005f; // 初期座標にこの値*(+-DEV_NUM)の誤差が生まれる
static const int DEV_NUM = 20;

Ball::Ball()
{
	m_model = nullptr;

	m_scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
	m_radius = 2.8f;
	m_isStop = true;

	XMStoreFloat4x4(&m_rotateMatrix, XMMatrixTranspose(XMMatrixIdentity()));
}

Ball::~Ball()
{
	SafeDelete(m_model);
}

bool Ball::Init(DX11Manager* dx3D, XMFLOAT3 position, int ballNumber, const ObjMesh* ballMesh)
{
	bool result;

	// ロードするテクスチャの名前を確定 "data/b(番号).tga" 0はキュー
	char texName[20];
	if (ballNumber == 0)
		strcpy_s(texName, 20, "data/bq.tga");
	else
		sprintf_s(texName, 20, "data/b%d.tga", ballNumber);

	// モデル初期化
	m_model = new Model;
	result = m_model->Init(dx3D->GetDevice(), dx3D->GetDeviceContext(), ballMesh, texName);
	if (!result) return false;

	// 位置初期化 (ほんの少しだけブレる)
	std::random_device rand;
	m_position = position;
	m_position.x += (rand() % DEV_NUM * 2 + 1) * INIT_POS_DEV - INIT_POS_DEV * DEV_NUM;
	m_position.z += (rand() % DEV_NUM * 2 + 1) * INIT_POS_DEV - INIT_POS_DEV * DEV_NUM;
	m_position.y += m_radius;

	// 数字の絵が上を向くように
	AddRotation(XMFLOAT3(XMConvertToRadians(90), 0.0f, 0.0f));
	AddRotation(XMFLOAT3(0.0f, XMConvertToRadians(90), 0.0f));

	return true;
}

bool Ball::Render(DX11Manager* dx3D, const ShaderManager* shaderManager,
	const XMFLOAT4X4& view, const XMFLOAT4X4& projection, const Light* light)
{
	bool result;
	XMFLOAT4X4 worldMatrix;

	GetCurrentWorldMatrix(&worldMatrix);

	m_model->Render(dx3D->GetDeviceContext());

	result = shaderManager->RenderLightShader(dx3D->GetDeviceContext(), m_model->GetIndexCount(),
		worldMatrix, view, projection, m_model->GetTexture(),
		light->GetDirection(), light->GetDiffuseColor());

	return result;
}

bool Ball::UpdateMove()
{
	// 移動ベクトル分移動
	AddPosition(m_move);

	// 移動ベクトル分回転
	AddRotation(XMFLOAT3(m_move.z / m_radius, 0.0f, -m_move.x / m_radius));

	// 移動ベクトルが一定以下になったら停止 
	// (毎フレーム大きさを求めるのも重たいので成分量のみ)
	if (abs(m_move.x) < STOP_VEC_VALUE && abs(m_move.z) < STOP_VEC_VALUE)
	{
		m_move = XMFLOAT3(0, 0, 0);
		m_isStop = true;
	}
	else if (abs(m_move.x) < BRAKE_VEC_VALUE && abs(m_move.z) < BRAKE_VEC_VALUE)
	{
		MulMoveVec(BRAKE_VEC_RATE);
		m_isStop = false;
	}
	else
	{
		// 移動ベクトル減退
		MulMoveVec(DECLINE_VEC_RATE);
		m_isStop = false;
	}
	return true;
}

bool Ball::UpdatePocket()
{
	// 落下
	m_move.y = (m_move.y - 0.01f) * 1.01f;

	// 移動ベクトル分移動
	AddPosition(m_move);

	// 一定まで落下したら停止
	if (m_position.y < -m_radius)
	{
		m_isStop = true;
	}

	return true;
}

void Ball::Restore(const XMFLOAT3& pos)
{
	m_position.x = pos.x;
	m_position.y = m_radius;
	m_position.z = pos.z;

	// 復活
	m_isPocket = false;
}

void Ball::GetCurrentWorldMatrix(XMFLOAT4X4 *worldMatrix)
{
	XMMATRIX matrix;
	matrix = XMMatrixIdentity();

	// スケール
	matrix *= XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z);

	// 回転
	matrix *= XMLoadFloat4x4(&m_rotateMatrix);

	// 移動
	matrix *= XMMatrixTranslation(m_position.x, m_position.y, m_position.z);

	XMStoreFloat4x4(worldMatrix, XMMatrixTranspose(matrix));
}

ID3D11ShaderResourceView* Ball::GetTexture()
{
	return m_model->GetTexture();
}

int Ball::GetIndexCount()const
{
	return m_model->GetIndexCount();
}

void Ball::AddPosition(const XMFLOAT3& addPos)
{
	m_position.x += addPos.x;
	m_position.y += addPos.y;
	m_position.z += addPos.z;
}

void Ball::AddPosition(float x, float y, float z)
{
	m_position.x += x;
	m_position.y += y;
	m_position.z += z;
}

void Ball::MulMoveVec(float t)
{
	m_move.x *= t;
	m_move.y *= t;
	m_move.z *= t;
}

void Ball::AddRotation(const XMFLOAT3& rotate)
{
	XMMATRIX matrix = XMLoadFloat4x4(&m_rotateMatrix);

	matrix *= XMMatrixRotationRollPitchYaw(rotate.x, rotate.y, rotate.z);

	XMStoreFloat4x4(&m_rotateMatrix, matrix);
}