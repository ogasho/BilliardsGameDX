#include "SceneTitle.h"
#include "DXUtil.h"

#include "MainCameraMotion.h"
#include "LightShader.h"
#include "Light.h"
#include "Ball.h"
#include "BilliardPhysics.h"
#include "Player.h"
#include "ObjMesh.h"
#include "Table.h"
#include "SelectHelper.h"
#include "ImageUI.h"

#include <DirectXMath.h>
using namespace DirectX;

// �I�[�g�v���C�p�{�[���������W
// 1/ 7 8/ 11 12 13/ 9 14 15 10/ 2 4 5 6 3 �̎O�p�z�u�ɂȂ�悤�ɍ��W�𓙊Ԋu�ɂ��炷
const XMFLOAT2 BSET{ 5.0f, 3.0f };
const XMFLOAT3 BALLS_INIT_POS[] =
{ XMFLOAT3(-50, 0, 0),
XMFLOAT3(50 + BSET.x * 0, 0, BSET.y * 0), XMFLOAT3(50 + BSET.x * 4, 0, BSET.y *-4), XMFLOAT3(50.0f + BSET.x * 4, 0, BSET.y * 4),
XMFLOAT3(50 + BSET.x * 4, 0, BSET.y *-2), XMFLOAT3(50 + BSET.x * 4, 0, BSET.y * 0), XMFLOAT3(50.0f + BSET.x * 4, 0, BSET.y * 2),
XMFLOAT3(50 + BSET.x * 1, 0, BSET.y *-1), XMFLOAT3(50 + BSET.x * 1, 0, BSET.y * 1), XMFLOAT3(50.0f + BSET.x * 3, 0, BSET.y *-3),
XMFLOAT3(50 + BSET.x * 3, 0, BSET.y * 3), XMFLOAT3(50 + BSET.x * 2, 0, BSET.y *-2), XMFLOAT3(50.0f + BSET.x * 2, 0, BSET.y * 0),
XMFLOAT3(50 + BSET.x * 2, 0, BSET.y * 2), XMFLOAT3(50 + BSET.x * 3, 0, BSET.y *-1), XMFLOAT3(50.0f + BSET.x * 3, 0, BSET.y * 1) };

const int NUM_BALL = 16;
const float TABLE_WIDTH = 200.0f;
const float TABLE_HEIGHT = 100.0f;

enum class TitleState
{
	Aim,		// �_���Ă���
	Shot,		// �{�[����ł��Ă���
};

SceneTitle::SceneTitle(DX11Manager* dx3D, const InputManager* inputManager, const ShaderManager* shaderManager)
	:SceneBase(dx3D, inputManager, shaderManager)
{
	m_cameraMotion = nullptr;
	m_balls = nullptr;
	m_player = nullptr;
	m_light = nullptr;
	m_physics = nullptr;
	m_table = nullptr;
	m_select = nullptr;

	m_exitText = nullptr;
	m_nineballText = nullptr;
	m_rotationText = nullptr;
	m_titleText = nullptr;
	
	m_isStateChangeFrame = true;

	m_titleState = TitleState::Aim;
	m_oldTitleState = m_titleState;
}

SceneTitle::~SceneTitle()
{
	SafeDelete(m_cameraMotion);
	SafeDelete(m_player);
	SafeDelete(m_light);
	SafeDelete(m_physics);
	SafeDelete(m_table);
	SafeDelete(m_select);
	
	SafeDelete(m_exitText);
	SafeDelete(m_nineballText);
	SafeDelete(m_rotationText);
	SafeDelete(m_titleText);

	for (int i = 0; i < NUM_BALL; i++)
	{
		SafeDelete(m_balls[i]);
	}
	SafeDeleteArr(m_balls);
}


bool SceneTitle::Init()
{
	bool result;

	// �I�[�g�v���C�őł{�[���̔ԍ�
	m_aimBallNum = 0;

	// �J����������
	m_cameraMotion = new MainCameraMotion;
	m_cameraMotion->Init(m_dx3D, TABLE_WIDTH, TABLE_HEIGHT);
	m_cameraMotion->SetBirdeyeCamera();

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

	// �I����������
	m_select = new SelectHelper;
	result = m_select->Init(1, 2);
	if (!result) return false;

	// UI�Q������
	InitUI();

	return true;
}

void SceneTitle::InitUI()
{
	XMINT2 screenSize = m_dx3D->GetScreenSize();

	// ��ʍ����"exit"
	m_exitText = new ImageUI;
	m_exitText->Init(m_dx3D->GetDevice(), m_dx3D->GetDeviceContext(), screenSize,
		120, 60, "data/UI/exit.tga");
	m_exitText->SetRenderPosition(XMINT2(0, 0), XMFLOAT2(1, 1));

	// �I������"nineball"
	m_nineballText = new ImageUI;
	m_nineballText->Init(m_dx3D->GetDevice(), m_dx3D->GetDeviceContext(), screenSize,
		200, 100, "data/UI/nineball.tga");
	m_nineballText->SetRenderPosition(
		XMINT2((screenSize.x - m_nineballText->GetImageSize().x) / 2, 250), XMFLOAT2(1, 1));

	// �I������"rotation"
	m_rotationText = new ImageUI;
	m_rotationText->Init(m_dx3D->GetDevice(), m_dx3D->GetDeviceContext(), screenSize,
		200, 100, "data/UI/rotation.tga");
	m_rotationText->SetRenderPosition(
		XMINT2((screenSize.x - m_rotationText->GetImageSize().x) / 2, 350), XMFLOAT2(1, 1));

	// �^�C�g�����S
	m_titleText = new ImageUI;
	m_titleText->Init(m_dx3D->GetDevice(), m_dx3D->GetDeviceContext(), screenSize,
		600, 300, "data/UI/title.tga");
	m_titleText->SetRenderPosition(
		XMINT2((screenSize.x - m_titleText->GetImageSize().x) / 2, 0), XMFLOAT2(1, 1));
}

SceneID SceneTitle::Frame()
{
	SceneID result = SceneID::Keep;

	// �V�[����Ԃ��ς������t���O���I��
	if (m_titleState != m_oldTitleState)
		m_isStateChangeFrame = true;
	else
		m_isStateChangeFrame = false;
	m_oldTitleState = m_titleState;

	switch (m_titleState)
	{
	case TitleState::Aim:
		UpdateAim();
		break;
	case TitleState::Shot:
		UpdateShot();
		break;
	default:
		break;
	}

	// ���͍X�V
	result = UpdateInput();

	// �G�X�P�[�v�������ꂽ��Q�[�����I������
	if (m_inputManager->IsFrameKeyDown(UseKeys::Esc))
	{
		result = SceneID::Exit;
	}

	return result;
}

SceneID SceneTitle::UpdateInput()
{
	m_select->UpdateInput(m_inputManager);

	// ���肳�ꂽ��A�I�����ɉ����Ď��̃V�[���ɍs��
	if (m_inputManager->IsFrameKeyDown(UseKeys::Enter))
	{
		int currentSelect = m_select->GetSelectedHeight();

		if (currentSelect == 1)
		{
			return SceneID::G_NineBall;
		}
		else
		{
			return SceneID::G_Rotation;
		}
	}

	return SceneID::Keep;
}

void SceneTitle::UpdateAim()
{
	// �K���ȃ{�[���Ɍ������đł�

	++m_aimBallNum;
	if (m_aimBallNum >= NUM_BALL)
		m_aimBallNum = 1;

	m_player->InitShotState(m_balls[0]->GetPosition(), m_balls[m_aimBallNum]->GetPosition());
	m_player->ShotBallMaxPower(m_balls[0]);
	m_titleState = TitleState::Shot;
}

void SceneTitle::UpdateShot()
{
	// �S�Ẵ{�[�����~�܂�����Aim��
	// �{�[�������X�V
	for (int i = 0; i < NUM_BALL; i++)
	{
		if (m_balls[i]->IsPockets())
			continue;

		for (int j = i + 1; j < NUM_BALL; j++)
		{
			m_physics->UpdateHitBallAndBall(m_balls[i], m_balls[j], true);
		}

		m_physics->UpdateHitBallAndTable(m_balls[i], m_table->GetTableWidth(), m_table->GetTableHeight());
	}

	// �{�[���ړ�
	for (int i = 0; i < NUM_BALL; i++)
	{
		m_balls[i]->UpdateMove();
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
		m_titleState = TitleState::Aim;
	}
}

bool SceneTitle::Render()
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

	// UI�`��
	m_dx3D->SpriteBegin();
	result = RenderUI();
	m_dx3D->SpriteEnd();

	if (!result) return false;

	return true;
}

bool SceneTitle::RenderUI()
{
	bool result;
	XMFLOAT4X4 screenViewMatrix, orthoMatrix, screenWorldMatrix;

	// UI�`��p�̐��ˉe�A�ėp���[���h�A�r���[�s����擾
	m_dx3D->GetOrthoMatrix(&orthoMatrix);
	m_dx3D->GetWorldMatrix(&screenWorldMatrix);
	m_dx3D->GetScreenViewMatrix(&screenViewMatrix);

	// exit�`��
	result = m_exitText->Render(m_dx3D->GetDeviceContext());
	if (!result) return false;
	result = m_shaderManager->RenderTextureShader(m_dx3D->GetDeviceContext(), m_exitText->GetIndexCount(),
		screenWorldMatrix, screenViewMatrix, orthoMatrix, m_exitText->GetTexture());
	if (!result) return false;

	// �I�����`��
	int selectNum = m_select->GetSelectedHeight();
	float nineballAlpha = 1.0f;
	float rotationAlpha = 1.0f;
	if (selectNum == 1)
	{
		rotationAlpha = 0.5f;
	}
	else
	{
		nineballAlpha = 0.5f;
	}
	
	result = m_nineballText->Render(m_dx3D->GetDeviceContext());
	if (!result) return false;
	result = m_shaderManager->RenderTextureShader(m_dx3D->GetDeviceContext(), m_nineballText->GetIndexCount(),
		screenWorldMatrix, screenViewMatrix, orthoMatrix, m_nineballText->GetTexture(),
		XMFLOAT4(1, 1, 1, nineballAlpha));
	if (!result) return false;

	result = m_rotationText->Render(m_dx3D->GetDeviceContext());
	if (!result) return false;
	result = m_shaderManager->RenderTextureShader(m_dx3D->GetDeviceContext(), m_rotationText->GetIndexCount(),
		screenWorldMatrix, screenViewMatrix, orthoMatrix, m_rotationText->GetTexture(),
		XMFLOAT4(1, 1, 1, rotationAlpha));
	if (!result) return false;

	// �^�C�g�����S�`��
	result = m_titleText->Render(m_dx3D->GetDeviceContext());
	if (!result) return false;
	result = m_shaderManager->RenderTextureShader(m_dx3D->GetDeviceContext(), m_titleText->GetIndexCount(),
		screenWorldMatrix, screenViewMatrix, orthoMatrix, m_titleText->GetTexture());
	if (!result) return false;

	return true;
}