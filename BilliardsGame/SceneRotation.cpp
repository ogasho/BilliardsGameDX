#include "SceneRotation.h"
#include "DXUtil.h"

#include "Camera.h"
#include "TextureShader.h"
#include "Light.h"
#include "Ball.h"
#include "BilliardPhysics.h"
#include "Player.h"
#include "ObjMesh.h"
#include "Table.h"

const bool DEBUG_FRAME = false;
const UseKeys DEBUG_FRAME_UPDATE_KEY = UseKeys::Enter;
const char DEBUG_FRAME_PLAY_KEY = 'U';

// ローテーション初期座標
// 1/ 7 8/ 11 12 13/ 9 14 15 10/ 2 4 5 6 3 の三角配置になるように座標を等間隔にずらす
const XMFLOAT2 BSET{ 5.0f, 3.0f };
const XMFLOAT3 BALLS_INIT_POS[] =
{ XMFLOAT3(-50, 0, 0),
XMFLOAT3(50 + BSET.x * 0, 0, BSET.y * 0), XMFLOAT3(50 + BSET.x * 4, 0, BSET.y *-4), XMFLOAT3(50.0f + BSET.x * 4, 0, BSET.y * 4),
XMFLOAT3(50 + BSET.x * 4, 0, BSET.y *-2), XMFLOAT3(50 + BSET.x * 4, 0, BSET.y * 0), XMFLOAT3(50.0f + BSET.x * 4, 0, BSET.y * 2),
XMFLOAT3(50 + BSET.x * 1, 0, BSET.y *-1), XMFLOAT3(50 + BSET.x * 1, 0, BSET.y * 1), XMFLOAT3(50.0f + BSET.x * 3, 0, BSET.y *-3),
XMFLOAT3(50 + BSET.x * 3, 0, BSET.y * 3), XMFLOAT3(50 + BSET.x * 2, 0, BSET.y *-2), XMFLOAT3(50.0f + BSET.x * 2, 0, BSET.y * 0), 
XMFLOAT3(50 + BSET.x * 2, 0, BSET.y * 2), XMFLOAT3(50 + BSET.x * 3, 0, BSET.y *-1), XMFLOAT3(50.0f + BSET.x * 3, 0, BSET.y * 1)};
const int NUM_BALL = 16;
const float TABLE_WIDTH = 200.0f;
const float TABLE_HEIGHT = 100.0f;

enum class PlayState
{
	Control,	// 操作中
	Shot		// ボールを打っている
};

SceneRotation::SceneRotation(DX11Manager* dx3D, const InputManager* inputManager)
	:SceneBase(dx3D, inputManager)
{
	m_camera = nullptr;
	m_balls = nullptr;
	m_player = nullptr;
	m_textureShader = nullptr;
	m_light = nullptr;
	m_physics = nullptr;
	m_table = nullptr;

	m_playState = PlayState::Control;
}

SceneRotation::~SceneRotation()
{
	SafeDelete(m_camera);
	SafeDelete(m_player);
	SafeDelete(m_textureShader);
	SafeDelete(m_light);
	SafeDelete(m_physics);
	SafeDelete(m_table);

	for (int i = 0; i < NUM_BALL; i++)
	{
		SafeDelete(m_balls[i]);
	}
	SafeDeleteArr(m_balls);
}

bool SceneRotation::Init(HWND hWnd)
{
	bool result;

	// カメラ初期化
	m_camera = new Camera;
	m_camera->SetPosition(0.0f, TABLE_WIDTH*0.75f, -TABLE_WIDTH*0.25f);
	m_camera->SetRotation(75.0f, 0.0f, 0.0f);

	// ボールのモデルデータ読み込み
	ObjMesh* objMesh = new ObjMesh;
	objMesh->LoadOBJFile("data/pool_balls.obj");

	// ボール初期化
	m_balls = new Ball*[NUM_BALL];
	for (int i = 0; i < NUM_BALL; i++)
	{
		m_balls[i] = new Ball;
		result = m_balls[i]->Init(m_dx3D, BALLS_INIT_POS[i], i, objMesh);
		if (!result) return false;
	}

	// ボールのモデルデータ破棄
	SafeDelete(objMesh);

	// テクスチャシェーダ初期化
	m_textureShader = new TextureShader;
	result = m_textureShader->Init(m_dx3D->GetDevice(), hWnd);
	if (!result) return false;

	// ライト初期化
	m_light = new Light;
	m_light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_light->SetDirection(0.0f, -2.0f, 1.0f);

	// テーブル初期化
	m_table = new Table;
	result = m_table->Init(m_dx3D, TABLE_WIDTH, TABLE_HEIGHT);

	// プレイヤー初期化
	m_player = new Player;
	m_player->Init(m_dx3D);

	return true;
}

SceneID SceneRotation::Frame()
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
		result = SceneID::Reset;
	// Eキーが押されたらモードを変える
	if (m_inputManager->IsFrameKeyDown('E'))
		result = SceneID::G_NineBall;

	return result;
}

SceneID SceneRotation::UpdateControl()
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

SceneID SceneRotation::UpdateShot()
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
		if (m_balls[i]->IsPockets())
			continue;

		for (int j = i + 1; j < NUM_BALL; j++)
		{
			m_physics->UpdateHitBallAndBall(m_balls[i], m_balls[j]);
		}

		m_physics->UpdateHitBallAndTable(m_balls[i], m_table->GetTableWidth(), m_table->GetTableHeight());
		m_physics->UpdateHitBallAndPockets(m_balls[i], m_table);
	}

	// ボール移動
	for (int i = 0; i < NUM_BALL; i++)
	{
		if (m_balls[i]->IsPockets())
			m_balls[i]->UpdatePocket();
		else
			m_balls[i]->UpdateMove();
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

bool SceneRotation::Render()
{
	XMFLOAT4X4 viewMatrix, projectionMatrix;
	bool result;

	// カメラ
	m_camera->Render();

	// ビュー、射影行列取得
	m_camera->GetViewMatrix(&viewMatrix);
	m_dx3D->GetProjectionMatrix(&projectionMatrix);

	// ボール描画
	for (int i = 0; i < NUM_BALL; i++)
	{
		if (m_balls[i]->IsPockets() && m_balls[i]->IsStopBallMove())
			continue;

		result = m_balls[i]->Render(m_dx3D, m_textureShader, viewMatrix, projectionMatrix, m_light);
		if (!result) return false;
	}

	// テーブル描画
	m_table->Render(m_dx3D, m_textureShader, viewMatrix, projectionMatrix, m_light);

	// プレイヤー(打つ方向)描画
	if (m_playState == PlayState::Control)
	{
		m_player->Render(m_dx3D, m_textureShader, viewMatrix, projectionMatrix, m_light, m_balls[0]);
	}

	return true;
}
