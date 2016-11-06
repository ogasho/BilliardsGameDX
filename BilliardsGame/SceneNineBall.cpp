#include "SceneNineBall.h"
#include "DXUtil.h"

#include "Camera.h"
#include "TextureShader.h"
#include "Light.h"
#include "ObjMesh.h"
#include "Ball.h"
#include "Model.h"


// ナインボール初期座標(キュー,123456789)
// 1,56,492,87,3 のひし形配置になるように座標を等間隔にずらす
const XMFLOAT2 BSET{ 5.2f, 3.1f };
const XMFLOAT3 BALLS_INIT_POS[] =
{ XMFLOAT3(-50.0f, 0, 0),
XMFLOAT3(50 + BSET.x * 0, 0, BSET.y * 0), XMFLOAT3(50 + BSET.x * 2, 0, BSET.y *-2), XMFLOAT3(50.0f + BSET.x * 4, 0, BSET.y * 0),
XMFLOAT3(50 + BSET.x * 2, 0, BSET.y * 2), XMFLOAT3(50 + BSET.x * 1, 0, BSET.y *-1), XMFLOAT3(50.0f + BSET.x * 1, 0, BSET.y * 1),
XMFLOAT3(50 + BSET.x * 3, 0, BSET.y *-1), XMFLOAT3(50 + BSET.x * 3, 0, BSET.y * 1), XMFLOAT3(50.0f + BSET.x * 2, 0, BSET.y * 0) };
const int NUM_BALL = 10;

SceneNineBall::SceneNineBall()
{
	m_camera = nullptr;
	m_textureShader = nullptr;
	m_light = nullptr;
	m_ballMesh = nullptr;
	m_balls = nullptr;
}

SceneNineBall::~SceneNineBall()
{
	SafeDelete(m_camera);
	SafeDelete(m_textureShader);
	SafeDelete(m_light);
	SafeDelete(m_ballMesh);

	for (int i = 0; i < NUM_BALL; i++)
	{
		SafeDelete(m_balls[i]);
	}
	SafeDeleteArr(m_balls);
}

bool SceneNineBall::Init(DX11Manager* dx3D, HWND hWnd)
{
	bool result;

	// カメラ初期化
	m_camera = new Camera;
	m_camera->SetPosition(0.0f, 250.0f, 0.0f);
	m_camera->SetRotation(90.0f, 0.0f, 0.0f);

	// ボール用モデルデータのロード
	m_ballMesh = new ObjMesh;
	result = m_ballMesh->LoadOBJFile("data/pool_balls.obj");
	if (!result) return false;

	// ボール初期化
	m_balls = new Ball*[NUM_BALL];
	for (int i = 0; i < NUM_BALL; i++)
	{
		m_balls[i] = new Ball;
		result = m_balls[i]->Init(dx3D, m_ballMesh, BALLS_INIT_POS[i], i);
		if (!result) return false;
	}

	// テクスチャシェーダ初期化
	m_textureShader = new TextureShader;
	result = m_textureShader->Init(dx3D->GetDevice(), hWnd);
	if (!result) return false;

	// ライト初期化
	m_light = new Light;
	m_light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_light->SetDirection(0.0f, -1.0f, 1.0f);

	return true;
}

SceneID SceneNineBall::Frame()
{
	// ボール更新
	for (int i = 0; i < NUM_BALL; i++)
	{
		m_balls[i]->Frame();
	}

	return SceneID::Keep;
}

bool SceneNineBall::Render(DX11Manager* dx3D)
{
	XMFLOAT4X4 worldMatrix, viewMatrix, projectionMatrix;
	bool result;

	// カメラ
	m_camera->Render();

	// ビュー、射影行列取得
	m_camera->GetViewMatrix(&viewMatrix);
	dx3D->GetProjectionMatrix(&projectionMatrix);

	// ボールモデル描画
	for (int i = 0; i < NUM_BALL; i++)
	{
		m_balls[i]->GetWorldMatrix(&worldMatrix);

		Model* ballModel = m_balls[i]->GetModelPtr();
		m_balls[i]->Render(dx3D->GetDeviceContext());
		result = m_textureShader->Render(dx3D->GetDeviceContext(), ballModel->GetIndexCount(),
			worldMatrix, viewMatrix, projectionMatrix, ballModel->GetTexture(),
			m_light->GetDirection(), m_light->GetDiffuseColor());
	}
	if (!result) return false;

	return true;
}
