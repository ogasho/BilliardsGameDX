#include "System.h"
#include "DXUtil.h"

#include "SceneID.h"
#include "InputManager.h"
#include "SceneManager.h"
#include "DX11Manager.h"
#include "ShaderManager.h"

#pragma comment(lib, "winmm.lib")
#include <mmsystem.h>

const SceneID START_SCENE = SceneID::G_NineBall;

LRESULT CALLBACK MainWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		// ウィンドウが破棄された
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;

		// ウィンドウが閉じられた
		case WM_CLOSE:
			PostQuitMessage(0);
			return 0;
			
		// その他メッセージはSystemクラスのWndProc関数へ
		default:
			return ApplicationHandle->WndProc(hWnd, msg, wParam, lParam);
	}
}


System::System()
{
	m_inputManager = nullptr;
	m_dx3D = nullptr;
	m_sceneManager = nullptr;
	m_shaderManager = nullptr;
}

System::~System()
{
	SafeDelete(m_inputManager);
	SafeDelete(m_dx3D);
	SafeDelete(m_sceneManager);
	SafeDelete(m_shaderManager);

	ShutdownWindows();
}

void System::ShutdownWindows()
{
	// マウスカーソルを表示
	ShowCursor(true);

	// フルスクリーンを解除
	if (FULL_SCREEN)
	{
		ChangeDisplaySettings(NULL, 0);
	}

	DestroyWindow(m_hWnd);
	m_hWnd = nullptr;

	UnregisterClass(m_appTitle, m_hInstance);
	m_hInstance = nullptr;

	ApplicationHandle = nullptr;
}

bool System::Init()
{
	int screenWidth = 0;
	int screenHeight = 0;
	bool result;

	// ウィンドウ名初期化
	m_appTitle = L"BilliardsGame";

	// windowsAPI初期化
	InitWindows(&screenWidth, &screenHeight);

	// DX11初期化
	m_dx3D = new DX11Manager;
	result = m_dx3D->Init(screenWidth, screenHeight, VSYNC_ENABLED, m_hWnd,
		FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result) return false;

	// シェーダーマネージャ初期化
	m_shaderManager = new ShaderManager;
	result = m_shaderManager->Init(m_dx3D->GetDevice(), m_hWnd);
	if (!result) return false;

	// 入力クラス作成
	m_inputManager = new InputManager;
	m_inputManager->Init();
	
	// シーンマネージャ初期化
	m_sceneManager = new SceneManager;
	result = m_sceneManager->Init(START_SCENE, m_dx3D, m_inputManager, m_shaderManager);
	if (!result) return false;

	// 同期タイマー初期化
	m_currentTime = timeGetTime();

	return true;
}

void System::Run()
{
	MSG msg;
	bool done, result;

	ZeroMemory(&msg, sizeof(MSG));

	/* メインループ */
	// ウィンドウ又はFrameメソッドから終了命令が返るまでループ
	done = false;
	while (!done)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (msg.message == WM_QUIT)
		{
			done = true;
		}
		else
		{
			result = Frame();
			if (!result)
			{
				done = true;
			}
		}

		SleepWindows();
	}
}

bool System::Frame()
{
	bool result = true;

	// エスケープが押されたら終了
	if (m_inputManager->IsKeyDown(VK_ESCAPE))
	{
		return false;
	}

	// 更新
	result = m_sceneManager->Frame();
	if (!result) return false;

	// 描画
	result = m_sceneManager->Render(m_dx3D);
	if (!result) return false;

	// シーンのフラグが変わっていたらシーンを切り替える
	if (m_sceneManager->IsChangeScene())
	{
		m_sceneManager->UpdateChangeScene(m_dx3D, m_inputManager, m_shaderManager);
	}

	// 入力情報更新
	m_inputManager->UpdateKeyState();

	return true;
}

LRESULT CALLBACK System::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		// キーの押下状態を更新
		case WM_KEYDOWN:
			m_inputManager->KeyDown((unsigned int)wParam);
			return 0;
		case WM_KEYUP:
			m_inputManager->KeyUp((unsigned int)wParam);
			return 0;

		// その他メッセージはデフォルトのWndProcに
		default:
			return DefWindowProc(hWnd, msg, wParam, lParam);
	}
}

void System::InitWindows(int* screenWidth, int* screenHeight)
{
	WNDCLASSEX wcex;
	DEVMODE dmScreenSettings;
	int posX, posY;

	// このオブジェクトのポインタを保持
	ApplicationHandle = this;

	// このアプリケーションのインスタンスを取得
	m_hInstance = GetModuleHandle(NULL);

	// タイトルが設定されてないなら初期化
	if (!m_appTitle)
	{
		m_appTitle = L"Windows";
	}

	// Windows設定
	wcex.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wcex.lpfnWndProc = MainWndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = m_hInstance;
	wcex.hIcon = LoadIcon(NULL, IDI_WINLOGO); //IDI_APPLICATION, IDI_WINLOGO
	wcex.hIconSm = wcex.hIcon;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = m_appTitle;
	wcex.cbSize = sizeof(WNDCLASSEX);

	// クラス登録
	RegisterClassEx(&wcex);

	// 解像度を設定
	if (FULL_SCREEN)
	{
		// フルスクリーン解像度
		*screenWidth = GetSystemMetrics(SM_CXSCREEN);
		*screenHeight = GetSystemMetrics(SM_CYSCREEN);

		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long)*screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)*screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// ディスプレイ設定をフルスクリーンに変更
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		// ウィンドウ位置を画面中央に
		posX = 0;
		posY = 0;
	}
	else
	{
		// ウィンドウ解像度(指定の解像度)
		*screenWidth = WINDOW_WIDTH;
		*screenHeight = WINDOW_HEIGHT;

		// ウィンドウ位置を画面中央に
		posX = (GetSystemMetrics(SM_CXSCREEN) - *screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - *screenHeight) / 2;
	}

	// ウィンドウ作成、ハンドルを取得
	m_hWnd = CreateWindowEx(WS_EX_APPWINDOW, m_appTitle, m_appTitle,
		WS_OVERLAPPEDWINDOW,
		posX, posY, *screenWidth, *screenHeight, NULL, NULL, m_hInstance, NULL);
	// 通常ウィンドウ: WS_OVERLAPPEDWINDOW
	// ボーダーレス:   WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP

	// ウィンドウを起動しフォーカスを合わせる
	ShowWindow(m_hWnd, SW_SHOW);
	SetForegroundWindow(m_hWnd);
	SetFocus(m_hWnd);

	// マウスカーソルを非表示にする
	//ShowCursor(false);
}

void System::SleepWindows()
{
	// 更新が早い場合、指定のfps値になるまで休む
	while (timeGetTime() - m_currentTime < 1000 / VSYNC_FPS)
	{
		Sleep(1);
	}

	m_currentTime = timeGetTime();
}