/* �V�[���J�ڊǗ��N���X */
// ���ݓ��삵�Ă���V�[���̊Ď��A�؂�ւ����s��

#pragma once

#include <d3d11.h>

enum class SceneID;
class DX11Manager;
class SceneBase;
class SceneNineBall;

class SceneManager
{
public:
	SceneManager();
	~SceneManager();

	bool Init(SceneID, DX11Manager*, HWND);
	bool UpdateChangeScene(DX11Manager*, HWND);

	bool Frame();
	bool Render(DX11Manager*);

	bool IsChangeScene(){ return m_isChangeScene; }

private:
	bool m_isChangeScene;
	SceneID m_currentSceneID;
	SceneBase* m_currentScene;

};

