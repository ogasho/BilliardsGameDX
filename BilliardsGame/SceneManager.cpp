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

bool SceneManager::Init(SceneID firstScene, DX11Manager* dx3D, HWND hWnd, InputManager* inputManager)
{
	m_currentSceneID = firstScene;

	bool result = UpdateChangeScene(dx3D, hWnd, inputManager);
	if (!result) return false;

	return true;
}

bool SceneManager::UpdateChangeScene(DX11Manager* dx3D, HWND hWnd, InputManager* inputManager)
{
	bool result;

	m_isChangeScene = false;

	// 今までのシーンを破棄
	SafeDelete(m_currentScene);

	// 新しいシーン生成
	switch (m_currentSceneID)
	{
	case SceneID::Title: // タイトル
		//m_currentScene = new SceneBase; // 未実装
		break;

	case SceneID::G_NineBall: // メインゲーム(ナインボール)
		m_currentScene = new SceneNineBall;
		break;
	}
	if (!m_currentScene) return false;

	result = m_currentScene->Init(dx3D, hWnd, inputManager);
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
	else
	{
		m_currentSceneID = sceneID;
		m_isChangeScene = true;
	}

	return true;
}

bool SceneManager::Render(DX11Manager* dx3D)
{
	bool result;

	// 画面クリア
	dx3D->Begin(0.5f, 0.5f, 0.5f, 1.0f);

	// シーン描画
	result = m_currentScene->Render();
	if (!result) return false;

	// 描画
	dx3D->End();

	return true;
}