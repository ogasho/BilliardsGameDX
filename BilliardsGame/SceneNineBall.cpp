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
#include "BilliardPocketAnim.h"
#include "NineBallRule.h"
#include "BilliardUI.h"

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
	Foul,		// ファウル処理中
	Control,	// 操作中
	Shot,		// ボールを打っている
	Finish		// 試合終了
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
	m_rule = nullptr;
	m_billiardUI = nullptr;
	m_pocketAnim = nullptr;

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
	SafeDelete(m_rule);
	SafeDelete(m_billiardUI);
	SafeDelete(m_pocketAnim);

	if (m_balls != nullptr)
	{
		for (int i = 0; i < NUM_BALL; i++)
		{
			SafeDelete(m_balls[i]);
		}
		SafeDeleteArr(m_balls);
	}
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
	result = objMesh->LoadOBJFile("data/pool_balls.obj");
	if (!result) return false;

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
	m_light->SetAmbientColor(0.2f, 0.2f, 0.2f, 1.0f);
	m_light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_light->SetDirection(0.0f, -2.0f, 1.0f);

	// テーブル初期化
	m_table = new Table;
	result = m_table->Init(m_dx3D, TABLE_WIDTH, TABLE_HEIGHT);
	if (!result) return false;

	// プレイヤー初期化
	m_player = new Player;
	result = m_player->Init(m_dx3D);
	if (!result) return false;

	// カウント初期化
	m_frameCount = new FrameCount;

	// ポケットアニメUI初期化
	m_pocketAnim = new BilliardPocketAnim;
	result = m_pocketAnim->Init(m_dx3D, NUM_BALL);
	if (!result) return false;

	// 汎用UI初期化
	m_billiardUI = new BilliardUI;
	result = m_billiardUI->Init(m_dx3D, NUM_BALL);
	if (!result) return false;

	// ルール初期化
	m_rule = new NineBallRule;
	result = m_rule->Init(NUM_BALL);
	if (!result) return false;

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
	case PlayState::Foul:
		UpdateFoul();
		break;
	case PlayState::Control:
		UpdateControl();
		break;
	case PlayState::Shot:
		UpdateShot();
		break;
	default:
		break;
	}

	// カメラ更新
	UpdateCamera();

	// エスケープが押されたらタイトルに戻る
	if (m_inputManager->IsFrameKeyDown(UseKeys::Esc))
	{
		result = SceneID::Title;
	}

	return result;
}

void SceneNineBall::UpdateFoul()
{
	// 初期化
	if (m_isStateChangeFrame)
	{
		m_motion = CameraMotion::Birdeye;

		// 白球が死んでいたら復活
		RestoreBall(0, XMFLOAT3(0, 0, 0));

		// ナインボールが死んでいたら復活
		RestoreBall(9, XMFLOAT3(TABLE_WIDTH * 0.25f, 0, 0));
	}

	m_player->InitShotState(m_balls[0]->GetPosition(), m_balls[m_rule->GetNextBallNum()]->GetPosition());

	// フリードロップ操作受け付け
	m_player->UpdateFreeDrop(m_inputManager, m_balls[0],
		m_cameraMotion->IsFlipPos(), m_motion == CameraMotion::Firstperson);

	// 置き場所が決まったらコントロールへ
	if (m_player->IsDecideShot())
	{
		m_player->ResetDecideShot();
		m_playState = PlayState::Control;
	}

	// ボール物理更新
	for (int i = 1; i < NUM_BALL; i++)
	{
		m_physics->UpdateHitBallAndBall(m_balls[0], m_balls[i], false);
	}

	m_physics->UpdateHitBallAndTable(m_balls[0], m_table->GetTableWidth(), m_table->GetTableHeight());

	m_balls[0]->UpdateMove();

}

void SceneNineBall::RestoreBall(int ballNum, const XMFLOAT3& restorePos)
{
	// ポケットされたボールを復活させる
	if (!m_balls[ballNum]->IsPockets())
		return;

	m_balls[ballNum]->Restore(restorePos);
	m_rule->SetBallPocket(ballNum, false);

	// 既に置いてあるボールと反発させて自然な位置に配置させる
	// (復活地点周辺にボールがあった場合のめり込み防止)
	bool isHit;
	do
	{
		isHit = false;
		for (int i = 0; i < NUM_BALL - 1; i++)
		{
			if (ballNum == i) continue;

			isHit = m_physics->UpdateHitBallAndBall(m_balls[ballNum], m_balls[i], false);
			if (!isHit) continue;

			m_balls[ballNum]->UpdateMove();
			m_balls[ballNum]->SetMoveVec(XMFLOAT3(0, 0, 0));

			// A,Bボール間のちょうど間に存在した場合、反発で往復して抜け出さない
			// その事故を防止するため、少しだけ座標をずらす
			if (ballNum % 2 == 0)
				m_balls[ballNum]->AddPosition(1, 0, 1);
			else
				m_balls[ballNum]->AddPosition(1, 0, -1);

			break;
		}
	} while (isHit);
}

void SceneNineBall::UpdateControl()
{
	// 初期化
	if (m_isStateChangeFrame)
	{
		m_rule->Refresh();

		m_player->InitShotState(m_balls[0]->GetPosition(), m_balls[m_rule->GetNextBallNum()]->GetPosition());
		if (m_motion == CameraMotion::ShotMove)
			m_motion = CameraMotion::Birdeye;
		m_pocketAnim->Clear();
	}

	m_player->UpdateInput(m_inputManager);

	// 打つ方向が決まったら打つ
	if (m_player->IsDecideShot())
	{
		m_player->ShotBall(m_balls[0]);
		m_playState = PlayState::Shot;
	}

}

void SceneNineBall::UpdateShot()
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
		return;
	}
	m_motion = CameraMotion::ShotMove;

	// コマ送りデバッグ
	if (DEBUG_FRAME)
	{
		if (!m_inputManager->IsFrameKeyDown(DEBUG_FRAME_UPDATE_KEY) &&
			!m_inputManager->IsKeyDown(DEBUG_FRAME_PLAY_KEY))
			return;
	}

	// ボール物理更新
	for (int i = 0; i < NUM_BALL; i++)
	{
		if (m_balls[i]->IsPockets())
			continue;

		for (int j = i + 1; j < NUM_BALL; j++)
		{
			bool isHit;
			isHit = m_physics->UpdateHitBallAndBall(m_balls[i], m_balls[j], true);

			// ファウル判定
			if (isHit && i == 0)
			{
				m_rule->CheckHitFoul(j);
			}
		}

		m_physics->UpdateHitBallAndTable(m_balls[i], m_table->GetTableWidth(), m_table->GetTableHeight());

		bool isPocket;
		isPocket = m_physics->UpdateHitBallAndPockets(m_balls[i], m_table);

		// ポケットフラグ更新&ファウル判定
		if (isPocket)
		{
			m_rule->SetBallPocket(i, true);
			m_pocketAnim->StartPocketAnim(i);
		}
	}
	
	// ボール移動
	for (int i = 0; i < NUM_BALL; i++)
	{
		if (m_balls[i]->IsPockets())
			m_balls[i]->UpdatePocket();
		else
			m_balls[i]->UpdateMove();
	}

	// UI更新
	m_pocketAnim->UpdateAnim();
	if (m_rule->IsFoul())
	{
		m_pocketAnim->StartPocketAnim(-1); // ファウル文字
	}

	// 全てのボールが止まったか
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
		// ファウル判定
		m_rule->CheckTurnEndFoul();

		// ターン更新
		m_rule->ChangeTurn();

		// ゲーム終了ならfinishへ、
		// ファウルしていたらfoulのステートへ、そうでないならcontrolへ
		if (m_rule->IsGameEnd())
		{
			m_playState = PlayState::Finish;
		}
		else if (m_rule->IsFoul())
		{
			m_playState = PlayState::Foul;
		}
		else
		{
			m_playState = PlayState::Control;
		}

		m_player->ResetDecideShot();
	}
}

void SceneNineBall::UpdateFinish()
{
	// 初期化
	if (m_isStateChangeFrame)
	{
		m_pocketAnim->Clear();
	}
}

void SceneNineBall::UpdateCamera()
{
	if (m_playState == PlayState::Foul || m_playState == PlayState::Control)
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

	// UI描画
	m_dx3D->SpriteBegin();
	result = RenderUI();
	m_dx3D->SpriteEnd();

	if (!result) return false;
	
	return true;
}

bool SceneNineBall::RenderUI()
{
	bool result;
	XMFLOAT4X4 screenViewMatrix, orthoMatrix, screenWorldMatrix;

	// UI描画用の正射影、汎用ワールド、ビュー行列を取得
	m_dx3D->GetOrthoMatrix(&orthoMatrix);
	m_dx3D->GetWorldMatrix(&screenWorldMatrix);
	m_dx3D->GetScreenViewMatrix(&screenViewMatrix);

	// ポケット時のアニメーション描画
	if (m_playState == PlayState::Shot)
	{
		result = m_pocketAnim->Render(m_dx3D, m_shaderManager, screenWorldMatrix, screenViewMatrix, orthoMatrix);
		if (!result)return false;
	}

	// 残りボールの描画
	result = m_billiardUI->RenderBallListUI(m_rule->GetBallPocketsFlag(), m_dx3D, m_shaderManager,
		screenWorldMatrix, screenViewMatrix, orthoMatrix);
	if (!result)return false;

	// 次に打つべきボールUIの描画
	result = m_billiardUI->RenderNextBallUI(m_rule->GetNextBallNum(), m_rule->GetCurrentPlayer() ,m_dx3D, m_shaderManager,
		screenWorldMatrix, screenViewMatrix, orthoMatrix);
	if (!result)return false;

	// 左上のEXIT表記の描画
	result = m_billiardUI->RenderExitUI(m_dx3D, m_shaderManager,
		screenWorldMatrix, screenViewMatrix, orthoMatrix);
	if (!result)return false;

	// 操作方法描画
	if (m_playState == PlayState::Control)
	{
		result = m_billiardUI->RenderHelperUI(0, m_dx3D, m_shaderManager,
			screenWorldMatrix, screenViewMatrix, orthoMatrix);
	}
	else if(m_playState == PlayState::Foul)
	{
		result = m_billiardUI->RenderHelperUI(1, m_dx3D, m_shaderManager,
			screenWorldMatrix, screenViewMatrix, orthoMatrix);
	}
	if (!result)return false;

	// フリードロップ文字描画
	if (m_playState == PlayState::Foul)
	{
		result = m_billiardUI->RenderFreeDropUI(m_dx3D, m_shaderManager,
			screenWorldMatrix, screenViewMatrix, orthoMatrix);
		if (!result)return false;
	}

	// 勝者描画
	if (m_playState == PlayState::Finish)
	{
		result = m_billiardUI->RenderWinnerUI(m_rule->GetCurrentPlayer(), m_dx3D, m_shaderManager,
			screenWorldMatrix, screenViewMatrix, orthoMatrix);
		if (!result)return false;
	}

	return true;
}