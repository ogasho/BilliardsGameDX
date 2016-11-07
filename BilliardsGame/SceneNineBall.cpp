#include "SceneNineBall.h"
#include "DXUtil.h"

#include "Camera.h"
#include "TextureShader.h"
#include "Light.h"
#include "Ball.h"
#include "Model.h"
#include "BilliardPhysics.h"
#include "Player.h"

#define DEBUG_FRAME false
#define DEBUG_FRAME_UPDATE_KEY UseKeys::Enter
#define DEBUG_FRAME_PLAY_KEY 'U'

// ナインボール初期座標(キュー,123456789)
// 1,56,492,87,3 のひし形配置になるように座標を等間隔にずらす
const XMFLOAT2 BSET{ 5.0f, 3.0f };
const XMFLOAT3 BALLS_INIT_POS[] =
{ XMFLOAT3(-50, 0, 0),
XMFLOAT3(50 + BSET.x * 0, 0, BSET.y * 0), XMFLOAT3(50 + BSET.x * 2, 0, BSET.y *-2), XMFLOAT3(50.0f + BSET.x * 4, 0, BSET.y * 0),
XMFLOAT3(50 + BSET.x * 2, 0, BSET.y * 2), XMFLOAT3(50 + BSET.x * 1, 0, BSET.y *-1), XMFLOAT3(50.0f + BSET.x * 1, 0, BSET.y * 1),
XMFLOAT3(50 + BSET.x * 3, 0, BSET.y *-1), XMFLOAT3(50 + BSET.x * 3, 0, BSET.y * 1), XMFLOAT3(50.0f + BSET.x * 2, 0, BSET.y * 0) };
const int NUM_BALL = 10;

enum class PlayState
{
	Control,	// 操作中
	Shot		// ボールを打っている
};

SceneNineBall::SceneNineBall()
{
	m_camera = nullptr;
	m_balls = nullptr;
	m_player = nullptr;
	m_textureShader = nullptr;
	m_light = nullptr;
	m_physics = nullptr;
	m_ballModel = nullptr;

	m_playState = PlayState::Control;
}

SceneNineBall::~SceneNineBall()
{
	SafeDelete(m_camera);
	SafeDelete(m_player);
	SafeDelete(m_textureShader);
	SafeDelete(m_light);
	SafeDelete(m_physics);
	SafeDelete(m_ballModel);

	for (int i = 0; i < NUM_BALL; i++)
	{
		SafeDelete(m_balls[i]);
	}
	SafeDeleteArr(m_balls);
}

bool SceneNineBall::Init(DX11Manager* dx3D, HWND hWnd, InputManager* inputManager)
{
	bool result;

	// マネージャ参照保管
	m_inputManager = inputManager;
	m_dx3D = dx3D;

	// カメラ初期化
	m_camera = new Camera;
	m_camera->SetPosition(0.0f, 150.0f, 0.0f);
	m_camera->SetRotation(90.0f, 0.0f, 0.0f);

	// ボールモデル初期化
	m_ballModel = new Model;
	result = m_ballModel->Init(dx3D->GetDevice(), dx3D->GetDeviceContext(), "data/pool_balls.obj", nullptr);

	// ボール初期化
	m_balls = new Ball*[NUM_BALL];
	for (int i = 0; i < NUM_BALL; i++)
	{
		m_balls[i] = new Ball;
		result = m_balls[i]->Init(dx3D, BALLS_INIT_POS[i], i);
		if (!result) return false;
	}

	// テクスチャシェーダ初期化
	m_textureShader = new TextureShader;
	result = m_textureShader->Init(dx3D->GetDevice(), hWnd);
	if (!result) return false;

	// ライト初期化
	m_light = new Light;
	m_light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_light->SetDirection(0.0f, -2.0f, 1.0f);

	// プレイヤー初期化
	m_player = new Player;

	return true;
}

SceneID SceneNineBall::Frame()
{
	SceneID result = SceneID::Keep;

	switch (m_playState)
	{
	case PlayState::Control:
		result = UpdateControl();
		break;
	case PlayState::Shot:
		result = UpdateShot();
		break;
	default:
		break;
	}

	// Rキーが押されたらリセットする
	if (m_inputManager->IsFrameKeyDown('R'))
		result = SceneID::G_NineBall;

	return result;
}

SceneID SceneNineBall::UpdateControl()
{

	m_player->UpdateInput(m_inputManager);

	// 打つ方向が決まったら打つ
	if (m_player->IsDecideShot())
	{
		m_player->ShotBall(m_balls[0]);
		m_playState = PlayState::Shot;
	}

	return SceneID::Keep;

}

SceneID SceneNineBall::UpdateShot()
{
	// コマ送りデバッグ
	if (DEBUG_FRAME)
	{
		if (!m_inputManager->IsFrameKeyDown(DEBUG_FRAME_UPDATE_KEY) &&
			!m_inputManager->IsKeyDown(DEBUG_FRAME_PLAY_KEY))
			return SceneID::Keep;
	}

	// ボール物理更新
	for (int i = 0; i < NUM_BALL; i++)
	{
		m_physics->UpdateHitBallAndTable(m_balls[i]);

		for (int j = i + 1; j < NUM_BALL; j++)
		{
			m_physics->UpdateHitBallAndBall(m_balls[i], m_balls[j]);
		}

		m_balls[i]->Frame();
	}

	// 全てのボールが止まったら操作に戻る
	bool isAllStoped = true;
	for (int i = 0; i < NUM_BALL; i++)
	{
		if (!m_balls[i]->IsStopBallMove())
		{
			isAllStoped = false;
			break;
		}
	}
	if (isAllStoped)
	{
		m_playState = PlayState::Control;
		m_player->ResetDecideShot();
		OutputDebugString(L"\n*****Finished Rolling Balls*****\n");
	}

	return SceneID::Keep;
}

bool SceneNineBall::Render()
{
	XMFLOAT4X4 worldMatrix, viewMatrix, projectionMatrix;
	bool result;

	// カメラ
	m_camera->Render();

	// ビュー、射影行列取得
	m_camera->GetViewMatrix(&viewMatrix);
	m_dx3D->GetProjectionMatrix(&projectionMatrix);

	// ボールモデル描画
	m_ballModel->Render(m_dx3D->GetDeviceContext());
	for (int i = 0; i < NUM_BALL; i++)
	{
		// それぞれのボールのワールドとテクスチャを取得、既存のボールモデルに流す
		m_balls[i]->GetCurrentWorldMatrix(&worldMatrix);

		result = m_textureShader->Render(m_dx3D->GetDeviceContext(), m_ballModel->GetIndexCount(),
			worldMatrix, viewMatrix, projectionMatrix, m_balls[i]->GetTexture(),
			m_light->GetDirection(), m_light->GetDiffuseColor());
	}
	if (!result) return false;
	
	return true;
}
