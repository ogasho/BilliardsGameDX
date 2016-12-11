#include "SceneNineBall.h"
#include "DXUtil.h"

#include "MainCameraMotion.h"
#include "LightShader.h"
#include "Light.h"
#include "Ball.h"
#include "BilliardPhysics.h"
#include "Player.h"
#include "ObjMesh.h"
#include "Table.h"
#include "FrameCount.h"

const bool DEBUG_FRAME = false;
const UseKeys DEBUG_FRAME_UPDATE_KEY = UseKeys::Enter;
const char DEBUG_FRAME_PLAY_KEY = 'U';

// ナインボール初期座標(キュー,123456789)
// 1,56,492,87,3 のひし形配置になるように座標を等間隔にずらす
const XMFLOAT2 BSET{ 5.0f, 3.0f };
const XMFLOAT3 BALLS_INIT_POS[] =
{ XMFLOAT3(-50, 0, 0),
XMFLOAT3(50 + BSET.x * 0, 0, BSET.y * 0), XMFLOAT3(50 + BSET.x * 2, 0, BSET.y *-2), XMFLOAT3(50.0f + BSET.x * 4, 0, BSET.y * 0),
XMFLOAT3(50 + BSET.x * 2, 0, BSET.y * 2), XMFLOAT3(50 + BSET.x * 1, 0, BSET.y *-1), XMFLOAT3(50.0f + BSET.x * 1, 0, BSET.y * 1),
XMFLOAT3(50 + BSET.x * 3, 0, BSET.y *-1), XMFLOAT3(50 + BSET.x * 3, 0, BSET.y * 1), XMFLOAT3(50.0f + BSET.x * 2, 0, BSET.y * 0) };
const int NUM_BALL = 10;
const float TABLE_WIDTH = 200.0f;
const float TABLE_HEIGHT = 100.0f;

enum class PlayState
{
	Control,	// 操作中
	Shot,		// ボールを打っている
};

SceneNineBall::SceneNineBall(DX11Manager* dx3D, const InputManager* inputManager, const ShaderManager* shaderManager)
	:SceneBase(dx3D, inputManager, shaderManager)
{
	m_cameraMotion = nullptr;
	m_balls = nullptr;
	m_player = nullptr;
	m_light = nullptr;
	m_physics = nullptr;
	m_table = nullptr;
	m_frameCount = nullptr;

	m_isStateChangeFrame = true;

	m_playState = PlayState::Control;
	m_oldPlayState = m_playState;
}

SceneNineBall::~SceneNineBall()
{
	SafeDelete(m_cameraMotion);
	SafeDelete(m_player);
	SafeDelete(m_light);
	SafeDelete(m_physics);
	SafeDelete(m_table);
	SafeDelete(m_frameCount);

	for (int i = 0; i < NUM_BALL; i++)
	{
		SafeDelete(m_balls[i]);
	}
	SafeDeleteArr(m_balls);
}

bool SceneNineBall::Init()
{
	bool result;

	// カメラ初期化
	m_cameraMotion = new MainCameraMotion;
	m_cameraMotion->Init(m_dx3D, TABLE_WIDTH, TABLE_HEIGHT);
	m_cameraMotion->SetBirdeyeCamera();
	m_motion = CameraMotion::Birdeye;

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

	// カウント初期化
	m_frameCount = new FrameCount;

	return true;
}

SceneID SceneNineBall::Frame()
{
	SceneID result = SceneID::Keep;

	// シーン状態が変わったらフラグをオン
	if (m_playState != m_oldPlayState)
		m_isStateChangeFrame = true;
	else
		m_isStateChangeFrame = false;
	m_oldPlayState = m_playState;

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

	// カメラ更新
	UpdateCamera();

	// Rキーが押されたらリセットする
	if (m_inputManager->IsFrameKeyDown('R'))
		result = SceneID::Reset;
	// Eキーが押されたらモードを変える
	if (m_inputManager->IsFrameKeyDown('E'))
		result = SceneID::G_Rotation;

	return result;
}

SceneID SceneNineBall::UpdateControl()
{
	// 初期化
	if (m_isStateChangeFrame)
	{
		m_player->InitShotState(m_balls[0]->GetPosition(), m_balls[1]->GetPosition());
		m_motion = CameraMotion::Birdeye;
	}

	m_player->UpdateInput(m_inputManager);

	// 白球が死んでいたら復活(場所指定実装は後で)
	if (m_balls[0]->IsPockets())
	{
		m_balls[0]->Restore(XMFLOAT3(0, 0, 0));
	}

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
	// 初期化
	if (m_isStateChangeFrame)
	{
		m_frameCount->Reset();
		m_motion = CameraMotion::Firstperson;
	}

	// 少し経ってからショット
	if (m_frameCount->GetCountFrame() < 30)
	{
		m_frameCount->CountingFrame();
		return SceneID::Keep;
	}
	m_motion = CameraMotion::ShotMove;

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

void SceneNineBall::UpdateCamera()
{
	if (m_playState == PlayState::Control)
	{
		// スペースが押されたら切り替え
		if (m_inputManager->IsFrameKeyDown(UseKeys::Space))
		{
			if (m_motion < CameraMotion::Firstperson)
			{
				m_motion = (CameraMotion)((int)m_motion + 1);
			}
			else
			{
				m_motion = CameraMotion::Birdeye;
			}
		}
	}

	switch (m_motion)
	{
	case CameraMotion::Birdeye:
		m_cameraMotion->SetBirdeyeCamera();
		break;
	case CameraMotion::Lookdown:
		m_cameraMotion->SetLookdownCamera();
		break;
	case CameraMotion::Firstperson:
		m_cameraMotion->SetFirstpersonCamera(m_balls[0]->GetPosition(), m_player->GetShotDir());
		break;
	case CameraMotion::ShotMove:
		m_cameraMotion->SetShotMoveCamera(m_player->GetShotDir());
		break;
	}
}

bool SceneNineBall::Render()
{
	XMFLOAT4X4 viewMatrix, projectionMatrix;
	XMFLOAT4X4 screenViewMatrix, orthoMatrix, screenWorldMatrix;
	bool result;

	// カメラ
	m_cameraMotion->Render();

	// ビュー行列、射影行列を取得
	m_cameraMotion->GetViewMatrix(&viewMatrix);
	if (m_cameraMotion->IsOrthoMode())
	{
		m_cameraMotion->GetOrthoMatrix(&projectionMatrix);
	}
	else
	{
		m_dx3D->GetProjectionMatrix(&projectionMatrix);
	}

	// UI描画用の正射影、汎用ワールド、ビュー行列を取得
	m_dx3D->GetOrthoMatrix(&orthoMatrix);
	m_dx3D->GetWorldMatrix(&screenWorldMatrix);
	m_dx3D->GetScreenViewMatrix(&screenViewMatrix);

	// ボール描画
	for (int i = 0; i < NUM_BALL; i++)
	{
		if (m_balls[i]->IsPockets() && m_balls[i]->IsStopBallMove())
			continue;

		result = m_balls[i]->Render(m_dx3D, m_shaderManager, viewMatrix, projectionMatrix, m_light);
		if (!result) return false;
	}

	// テーブル描画
	m_table->Render(m_dx3D, m_shaderManager, viewMatrix, projectionMatrix, m_light);

	// プレイヤー(打つ方向)描画
	if (m_oldPlayState == PlayState::Control)
	{
		m_player->Render(m_dx3D, m_shaderManager, viewMatrix, projectionMatrix, m_light, m_balls[0]);
	}
	
	return true;
}
