#include "SceneManager.h"
#include "DXUtil.h"

#include "SceneID.h"
#include "DX11Manager.h"
#include "SceneBase.h"
#include "SceneNineBall.h"
#include "SceneRotation.h"

SceneManager::SceneManager()
{
	m_currentScene = nullptr;
	m_isChangeScene = false;
}


SceneManager::~SceneManager()
{
	SafeDelete(m_currentScene);
}

bool SceneManager::Init(SceneID firstScene, DX11Manager* dx3D, HWND hWnd, const InputManager* inputManager)
{
	m_currentSceneID = firstScene;

	bool result = UpdateChangeScene(dx3D, hWnd, inputManager);
	if (!result) return false;

	return true;
}

bool SceneManager::UpdateChangeScene(DX11Manager* dx3D, HWND hWnd, const InputManager* inputManager)
{
	bool result;

	m_isChangeScene = false;

	// ���܂ł̃V�[����j��
	SafeDelete(m_currentScene);

	// �V�����V�[������
	switch (m_currentSceneID)
	{
	case SceneID::Title: // �^�C�g��
		//m_currentScene = new SceneBase; // ������
		break;

	case SceneID::G_NineBall: // ���C���Q�[��(�i�C���{�[��)
		m_currentScene = new SceneNineBall(dx3D, inputManager);
		break;

	case SceneID::G_Rotation: // ���C���Q�[��(���[�e�[�V����)
		m_currentScene = new SceneRotation(dx3D, inputManager);
		break;
	}
	if (!m_currentScene) return false;

	result = m_currentScene->Init(hWnd);
	if (!result) return false;

	return true;
}

bool SceneManager::Frame()
{
	SceneID sceneID = SceneID::Keep;
	
	// ���݂̃V�[�����X�V�AID���擾
	sceneID = m_currentScene->Frame();

	// ID���ɂ���ăV�[���̏�Ԃ�ς���
	if (sceneID == SceneID::Exit)
	{
		return false;
	}
	else if (sceneID == SceneID::Keep)
	{
		return true;
	}
	else if (sceneID == SceneID::Reset)
	{
		m_isChangeScene = true;
		return true;
	}
	else if (sceneID != m_currentSceneID)
	{
		m_currentSceneID = sceneID;
		m_isChangeScene = true;
		return true;
	}

	return true;
}

bool SceneManager::Render(DX11Manager* dx3D)
{
	bool result;

	// ��ʃN���A
	dx3D->Begin(0.5f, 0.5f, 0.5f, 1.0f);

	// �V�[���`��
	result = m_currentScene->Render();
	if (!result) return false;

	// �`��
	dx3D->End();

	return true;
}