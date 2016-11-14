/* システム管理クラス */
// ウィンドウ、DirectX、ゲームシーンマネージャなど
// システムの根っこの初期化、更新処理を行う

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

// ウィンドウプロシージャ
static LRESULT CALLBACK MainWndProc(HWND, UINT, WPARAM, LPARAM);
// クラスハンドル(静的関数のWndProcからクラス内のWndProcを呼ぶ)
static System* ApplicationHandle;