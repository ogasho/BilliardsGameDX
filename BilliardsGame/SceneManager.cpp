#include "SceneManager.h"
#include "DXUtil.h"

#include "SceneID.h"
#include "DX11Manager.h"
#include "SceneBase.h"
#include "SceneNineBall.h"

SceneManager::SceneManager()
{
	m_currentScene = nullptr;
	m_isChangeScene = false;
}


SceneManager::~SceneManager()
{
	SafeDelete(m_currentScene);
}

bool SceneManager::Init(SceneID firstScene, DX11Manager* dx3D, HWND hWnd)
{
	m_currentSceneID = firstScene;

	bool result = UpdateChangeScene(dx3D, hWnd);
	if (!result) return false;

	return true;
}

bool SceneManager::UpdateChangeScene(DX11Manager* dx3D, HWND hWnd)
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
		m_currentScene = new SceneNineBall;
		break;
	}
	if (!m_currentScene) return false;

	result = m_currentScene->Init(dx3D, hWnd);
	if (!result) return false;

	return true;
}

bool SceneManager::Frame()
{
	SceneID sceneID = SceneID::Keep;
	
	sceneID = m_currentScene->Frame();

	if (sceneID == SceneID::Keep)
	{
		return true;
	}
	else if (sceneID == SceneID::Exit)
	{
		return false;
	}
	else if (sceneID != m_currentSceneID)
	{
		m_currentSceneID = sceneID;
		m_isChangeScene = true;
	}

	return true;
}

bool SceneManager::Render(DX11Manager* dx3D)
{
	bool result;

	// �V�[���`��
	result = m_currentScene->Render(dx3D);
	if (!result) return false;

	return true;
}