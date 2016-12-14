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

// �i�C���{�[���������W(�L���[,123456789)
// 1,56,492,87,3 �̂Ђ��`�z�u�ɂȂ�悤�ɍ��W�𓙊Ԋu�ɂ��炷
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
	Foul,		// �t�@�E��������
	Control,	// ���쒆
	Shot,		// �{�[����ł��Ă���
	Finish		// �����I��
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

	// �J����������
	m_cameraMotion = new MainCameraMotion;
	m_cameraMotion->Init(m_dx3D, TABLE_WIDTH, TABLE_HEIGHT);
	m_cameraMotion->SetBirdeyeCamera();
	m_motion = CameraMotion::Birdeye;

	// �{�[���̃��f���f�[�^�ǂݍ���
	ObjMesh* objMesh = new ObjMesh;
	result = objMesh->LoadOBJFile("data/pool_balls.obj");
	if (!result) return false;

	// �{�[��������
	m_balls = new Ball*[NUM_BALL];
	for (int i = 0; i < NUM_BALL; i++)
	{
		m_balls[i] = new Ball;
		result = m_balls[i]->Init(m_dx3D, BALLS_INIT_POS[i], i, objMesh);
		if (!result) return false;
	}

	// �{�[���̃��f���f�[�^�j��
	SafeDelete(objMesh);

	// ���C�g������
	m_light = new Light;
	m_light->SetAmbientColor(0.2f, 0.2f, 0.2f, 1.0f);
	m_light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_light->SetDirection(0.0f, -2.0f, 1.0f);

	// �e�[�u��������
	m_table = new Table;
	result = m_table->Init(m_dx3D, TABLE_WIDTH, TABLE_HEIGHT);
	if (!result) return false;

	// �v���C���[������
	m_player = new Player;
	result = m_player->Init(m_dx3D);
	if (!result) return false;

	// �J�E���g������
	m_frameCount = new FrameCount;

	// �|�P�b�g�A�j��UI������
	m_pocketAnim = new BilliardPocketAnim;
	result = m_pocketAnim->Init(m_dx3D, NUM_BALL);
	if (!result) return false;

	// �ėpUI������
	m_billiardUI = new BilliardUI;
	result = m_billiardUI->Init(m_dx3D, NUM_BALL);
	if (!result) return false;

	// ���[��������
	m_rule = new NineBallRule;
	result = m_rule->Init(NUM_BALL);
	if (!result) return false;

	return true;
}

SceneID SceneNineBall::Frame()
{
	SceneID result = SceneID::Keep;

	// �V�[����Ԃ��ς������t���O���I��
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

	// �J�����X�V
	UpdateCamera();

	// �G�X�P�[�v�������ꂽ��^�C�g���ɖ߂�
	if (m_inputManager->IsFrameKeyDown(UseKeys::Esc))
	{
		result = SceneID::Title;
	}

	return result;
}

void SceneNineBall::UpdateFoul()
{
	// ������
	if (m_isStateChangeFrame)
	{
		m_motion = CameraMotion::Birdeye;

		// ����������ł����畜��
		RestoreBall(0, XMFLOAT3(0, 0, 0));

		// �i�C���{�[��������ł����畜��
		RestoreBall(9, XMFLOAT3(TABLE_WIDTH * 0.25f, 0, 0));
	}

	m_player->InitShotState(m_balls[0]->GetPosition(), m_balls[m_rule->GetNextBallNum()]->GetPosition());

	// �t���[�h���b�v����󂯕t��
	m_player->UpdateFreeDrop(m_inputManager, m_balls[0],
		m_cameraMotion->IsFlipPos(), m_motion == CameraMotion::Firstperson);

	// �u���ꏊ�����܂�����R���g���[����
	if (m_player->IsDecideShot())
	{
		m_player->ResetDecideShot();
		m_playState = PlayState::Control;
	}

	// �{�[�������X�V
	for (int i = 1; i < NUM_BALL; i++)
	{
		m_physics->UpdateHitBallAndBall(m_balls[0], m_balls[i], false);
	}

	m_physics->UpdateHitBallAndTable(m_balls[0], m_table->GetTableWidth(), m_table->GetTableHeight());

	m_balls[0]->UpdateMove();

}

void SceneNineBall::RestoreBall(int ballNum, const XMFLOAT3& restorePos)
{
	// �|�P�b�g���ꂽ�{�[���𕜊�������
	if (!m_balls[ballNum]->IsPockets())
		return;

	m_balls[ballNum]->Restore(restorePos);
	m_rule->SetBallPocket(ballNum, false);

	// ���ɒu���Ă���{�[���Ɣ��������Ď��R�Ȉʒu�ɔz�u������
	// (�����n�_���ӂɃ{�[�����������ꍇ�̂߂荞�ݖh�~)
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

			// A,B�{�[���Ԃ̂��傤�ǊԂɑ��݂����ꍇ�A�����ŉ������Ĕ����o���Ȃ�
			// ���̎��̂�h�~���邽�߁A�����������W�����炷
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
	// ������
	if (m_isStateChangeFrame)
	{
		m_rule->Refresh();

		m_player->InitShotState(m_balls[0]->GetPosition(), m_balls[m_rule->GetNextBallNum()]->GetPosition());
		if (m_motion == CameraMotion::ShotMove)
			m_motion = CameraMotion::Birdeye;
		m_pocketAnim->Clear();
	}

	m_player->UpdateInput(m_inputManager);

	// �ł��������܂�����ł�
	if (m_player->IsDecideShot())
	{
		m_player->ShotBall(m_balls[0]);
		m_playState = PlayState::Shot;
	}

}

void SceneNineBall::UpdateShot()
{
	// ������
	if (m_isStateChangeFrame)
	{
		m_frameCount->Reset();
		m_motion = CameraMotion::Firstperson;
	}

	// �����o���Ă���V���b�g
	if (m_frameCount->GetCountFrame() < 30)
	{
		m_frameCount->CountingFrame();
		return;
	}
	m_motion = CameraMotion::ShotMove;

	// �R�}����f�o�b�O
	if (DEBUG_FRAME)
	{
		if (!m_inputManager->IsFrameKeyDown(DEBUG_FRAME_UPDATE_KEY) &&
			!m_inputManager->IsKeyDown(DEBUG_FRAME_PLAY_KEY))
			return;
	}

	// �{�[�������X�V
	for (int i = 0; i < NUM_BALL; i++)
	{
		if (m_balls[i]->IsPockets())
			continue;

		for (int j = i + 1; j < NUM_BALL; j++)
		{
			bool isHit;
			isHit = m_physics->UpdateHitBallAndBall(m_balls[i], m_balls[j], true);

			// �t�@�E������
			if (isHit && i == 0)
			{
				m_rule->CheckHitFoul(j);
			}
		}

		m_physics->UpdateHitBallAndTable(m_balls[i], m_table->GetTableWidth(), m_table->GetTableHeight());

		bool isPocket;
		isPocket = m_physics->UpdateHitBallAndPockets(m_balls[i], m_table);

		// �|�P�b�g�t���O�X�V&�t�@�E������
		if (isPocket)
		{
			m_rule->SetBallPocket(i, true);
			m_pocketAnim->StartPocketAnim(i);
		}
	}
	
	// �{�[���ړ�
	for (int i = 0; i < NUM_BALL; i++)
	{
		if (m_balls[i]->IsPockets())
			m_balls[i]->UpdatePocket();
		else
			m_balls[i]->UpdateMove();
	}

	// UI�X�V
	m_pocketAnim->UpdateAnim();
	if (m_rule->IsFoul())
	{
		m_pocketAnim->StartPocketAnim(-1); // �t�@�E������
	}

	// �S�Ẵ{�[�����~�܂�����
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
		// �t�@�E������
		m_rule->CheckTurnEndFoul();

		// �^�[���X�V
		m_rule->ChangeTurn();

		// �Q�[���I���Ȃ�finish�ցA
		// �t�@�E�����Ă�����foul�̃X�e�[�g�ցA�����łȂ��Ȃ�control��
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
	// ������
	if (m_isStateChangeFrame)
	{
		m_pocketAnim->Clear();
	}
}

void SceneNineBall::UpdateCamera()
{
	if (m_playState == PlayState::Foul || m_playState == PlayState::Control)
	{
		// �X�y�[�X�������ꂽ��؂�ւ�
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

	// �J����
	m_cameraMotion->Render();

	// �r���[�s��A�ˉe�s����擾
	m_cameraMotion->GetViewMatrix(&viewMatrix);
	if (m_cameraMotion->IsOrthoMode())
	{
		m_cameraMotion->GetOrthoMatrix(&projectionMatrix);
	}
	else
	{
		m_dx3D->GetProjectionMatrix(&projectionMatrix);
	}

	// �{�[���`��
	for (int i = 0; i < NUM_BALL; i++)
	{
		if (m_balls[i]->IsPockets() && m_balls[i]->IsStopBallMove())
			continue;

		result = m_balls[i]->Render(m_dx3D, m_shaderManager, viewMatrix, projectionMatrix, m_light);
		if (!result) return false;
	}

	// �e�[�u���`��
	m_table->Render(m_dx3D, m_shaderManager, viewMatrix, projectionMatrix, m_light);

	// �v���C���[(�ł���)�`��
	if (m_oldPlayState == PlayState::Control)
	{
		m_player->Render(m_dx3D, m_shaderManager, viewMatrix, projectionMatrix, m_light, m_balls[0]);
	}

	// UI�`��
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

	// UI�`��p�̐��ˉe�A�ėp���[���h�A�r���[�s����擾
	m_dx3D->GetOrthoMatrix(&orthoMatrix);
	m_dx3D->GetWorldMatrix(&screenWorldMatrix);
	m_dx3D->GetScreenViewMatrix(&screenViewMatrix);

	// �|�P�b�g���̃A�j���[�V�����`��
	if (m_playState == PlayState::Shot)
	{
		result = m_pocketAnim->Render(m_dx3D, m_shaderManager, screenWorldMatrix, screenViewMatrix, orthoMatrix);
		if (!result)return false;
	}

	// �c��{�[���̕`��
	result = m_billiardUI->RenderBallListUI(m_rule->GetBallPocketsFlag(), m_dx3D, m_shaderManager,
		screenWorldMatrix, screenViewMatrix, orthoMatrix);
	if (!result)return false;

	// ���ɑłׂ��{�[��UI�̕`��
	result = m_billiardUI->RenderNextBallUI(m_rule->GetNextBallNum(), m_rule->GetCurrentPlayer() ,m_dx3D, m_shaderManager,
		screenWorldMatrix, screenViewMatrix, orthoMatrix);
	if (!result)return false;

	// �����EXIT�\�L�̕`��
	result = m_billiardUI->RenderExitUI(m_dx3D, m_shaderManager,
		screenWorldMatrix, screenViewMatrix, orthoMatrix);
	if (!result)return false;

	// ������@�`��
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

	// �t���[�h���b�v�����`��
	if (m_playState == PlayState::Foul)
	{
		result = m_billiardUI->RenderFreeDropUI(m_dx3D, m_shaderManager,
			screenWorldMatrix, screenViewMatrix, orthoMatrix);
		if (!result)return false;
	}

	// ���ҕ`��
	if (m_playState == PlayState::Finish)
	{
		result = m_billiardUI->RenderWinnerUI(m_rule->GetCurrentPlayer(), m_dx3D, m_shaderManager,
			screenWorldMatrix, screenViewMatrix, orthoMatrix);
		if (!result)return false;
	}

	return true;
}