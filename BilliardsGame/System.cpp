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
		// �E�B���h�E���j�����ꂽ
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;

		// �E�B���h�E������ꂽ
		case WM_CLOSE:
			PostQuitMessage(0);
			return 0;
			
		// ���̑����b�Z�[�W��System�N���X��WndProc�֐���
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
	// �}�E�X�J�[�\����\��
	ShowCursor(true);

	// �t���X�N���[��������
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

	// �E�B���h�E��������
	m_appTitle = L"BilliardsGame";

	// windowsAPI������
	InitWindows(&screenWidth, &screenHeight);

	// DX11������
	m_dx3D = new DX11Manager;
	result = m_dx3D->Init(screenWidth, screenHeight, VSYNC_ENABLED, m_hWnd,
		FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result) return false;

	// �V�F�[�_�[�}�l�[�W��������
	m_shaderManager = new ShaderManager;
	result = m_shaderManager->Init(m_dx3D->GetDevice(), m_hWnd);
	if (!result) return false;

	// ���̓N���X�쐬
	m_inputManager = new InputManager;
	m_inputManager->Init();
	
	// �V�[���}�l�[�W��������
	m_sceneManager = new SceneManager;
	result = m_sceneManager->Init(START_SCENE, m_dx3D, m_inputManager, m_shaderManager);
	if (!result) return false;

	// �����^�C�}�[������
	m_currentTime = timeGetTime();

	return true;
}

void System::Run()
{
	MSG msg;
	bool done, result;

	ZeroMemory(&msg, sizeof(MSG));

	/* ���C�����[�v */
	// �E�B���h�E����Frame���\�b�h����I�����߂��Ԃ�܂Ń��[�v
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

	// �G�X�P�[�v�������ꂽ��I��
	if (m_inputManager->IsKeyDown(VK_ESCAPE))
	{
		return false;
	}

	// �X�V
	result = m_sceneManager->Frame();
	if (!result) return false;

	// �`��
	result = m_sceneManager->Render(m_dx3D);
	if (!result) return false;

	// �V�[���̃t���O���ς���Ă�����V�[����؂�ւ���
	if (m_sceneManager->IsChangeScene())
	{
		m_sceneManager->UpdateChangeScene(m_dx3D, m_inputManager, m_shaderManager);
	}

	// ���͏��X�V
	m_inputManager->UpdateKeyState();

	return true;
}

LRESULT CALLBACK System::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		// �L�[�̉�����Ԃ��X�V
		case WM_KEYDOWN:
			m_inputManager->KeyDown((unsigned int)wParam);
			return 0;
		case WM_KEYUP:
			m_inputManager->KeyUp((unsigned int)wParam);
			return 0;

		// ���̑����b�Z�[�W�̓f�t�H���g��WndProc��
		default:
			return DefWindowProc(hWnd, msg, wParam, lParam);
	}
}

void System::InitWindows(int* screenWidth, int* screenHeight)
{
	WNDCLASSEX wcex;
	DEVMODE dmScreenSettings;
	int posX, posY;

	// ���̃I�u�W�F�N�g�̃|�C���^��ێ�
	ApplicationHandle = this;

	// ���̃A�v���P�[�V�����̃C���X�^���X���擾
	m_hInstance = GetModuleHandle(NULL);

	// �^�C�g�����ݒ肳��ĂȂ��Ȃ珉����
	if (!m_appTitle)
	{
		m_appTitle = L"Windows";
	}

	// Windows�ݒ�
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

	// �N���X�o�^
	RegisterClassEx(&wcex);

	// �𑜓x��ݒ�
	if (FULL_SCREEN)
	{
		// �t���X�N���[���𑜓x
		*screenWidth = GetSystemMetrics(SM_CXSCREEN);
		*screenHeight = GetSystemMetrics(SM_CYSCREEN);

		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long)*screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)*screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// �f�B�X�v���C�ݒ���t���X�N���[���ɕύX
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		// �E�B���h�E�ʒu����ʒ�����
		posX = 0;
		posY = 0;
	}
	else
	{
		// �E�B���h�E�𑜓x(�w��̉𑜓x)
		*screenWidth = WINDOW_WIDTH;
		*screenHeight = WINDOW_HEIGHT;

		// �E�B���h�E�ʒu����ʒ�����
		posX = (GetSystemMetrics(SM_CXSCREEN) - *screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - *screenHeight) / 2;
	}

	// �E�B���h�E�쐬�A�n���h�����擾
	m_hWnd = CreateWindowEx(WS_EX_APPWINDOW, m_appTitle, m_appTitle,
		WS_OVERLAPPEDWINDOW,
		posX, posY, *screenWidth, *screenHeight, NULL, NULL, m_hInstance, NULL);
	// �ʏ�E�B���h�E: WS_OVERLAPPEDWINDOW
	// �{�[�_�[���X:   WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP

	// �E�B���h�E���N�����t�H�[�J�X�����킹��
	ShowWindow(m_hWnd, SW_SHOW);
	SetForegroundWindow(m_hWnd);
	SetFocus(m_hWnd);

	// �}�E�X�J�[�\�����\���ɂ���
	//ShowCursor(false);
}

void System::SleepWindows()
{
	// �X�V�������ꍇ�A�w���fps�l�ɂȂ�܂ŋx��
	while (timeGetTime() - m_currentTime < 1000 / VSYNC_FPS)
	{
		Sleep(1);
	}

	m_currentTime = timeGetTime();
}