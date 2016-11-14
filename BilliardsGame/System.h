/* �V�X�e���Ǘ��N���X */
// �E�B���h�E�ADirectX�A�Q�[���V�[���}�l�[�W���Ȃ�
// �V�X�e���̍������̏������A�X�V�������s��

#pragma once

#include <Windows.h>

class SceneManager;
class InputManager;
class DX11Manager;

static const int WINDOW_WIDTH = 800;
static const int WINDOW_HEIGHT = 600;
static const bool FULL_SCREEN = false;
static const bool VSYNC_ENABLED = true;
static const int VSYNC_FPS = 60;
static const float SCREEN_DEPTH = 1000.0f;
static const float SCREEN_NEAR = 0.1f;

class System
{
public:
	System();
	~System();

	void Run();
	bool Init();

	LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

private:
	bool Frame();
	void InitWindows(int* screenWidth, int* screenHeight);
	void ShutdownWindows();
	void SleepWindows();

	LPCUWSTR m_appTitle;
	HINSTANCE m_hInstance;
	HWND m_hWnd;
	DWORD m_currentTime;

	InputManager* m_inputManager;
	DX11Manager* m_dx3D;
	SceneManager* m_sceneManager;
};

// �E�B���h�E�v���V�[�W��
static LRESULT CALLBACK MainWndProc(HWND, UINT, WPARAM, LPARAM);
// �N���X�n���h��(�ÓI�֐���WndProc����N���X����WndProc���Ă�)
static System* ApplicationHandle;