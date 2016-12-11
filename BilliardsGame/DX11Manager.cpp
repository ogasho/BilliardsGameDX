#include "DX11Manager.h"
#include "DXUtil.h"

DX11Manager::DX11Manager()
{
	m_swapChain = nullptr;
	m_device = nullptr;
	m_deviceContext = nullptr;
	m_renderTargetView = nullptr;
	m_depthStencilBuffer = nullptr;
	m_depthStencilState = nullptr;
	m_disabledDepthStencilState = nullptr;
	m_depthStencilView = nullptr;
	m_rasterState = nullptr;

}

DX11Manager::~DX11Manager()
{
	// �t���X�N���[������
	if (m_swapChain)
	{
		m_swapChain->SetFullscreenState(false, NULL);
	}

	SafeRelease(m_swapChain);
	SafeRelease(m_device);
	SafeRelease(m_deviceContext);
	SafeRelease(m_renderTargetView);
	SafeRelease(m_depthStencilBuffer);
	SafeRelease(m_depthStencilState);
	SafeRelease(m_disabledDepthStencilState);
	SafeRelease(m_depthStencilView);
	SafeRelease(m_rasterState);
	
}

void DX11Manager::Begin(float r, float g, float b, float a)
{
	float color[4];
	color[0] = r;
	color[1] = g;
	color[2] = b;
	color[3] = a;

	// �o�b�N�o�b�t�@�N���A
	m_deviceContext->ClearRenderTargetView(m_renderTargetView, color);

	// �[�x�o�b�t�@�N���A
	m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void DX11Manager::End()
{
	// �����_�����O���������Ă���̂ŁA�o�b�N�o�b�t�@��\��
	if (m_vsync_enabled)
	{
		m_swapChain->Present(1, 0);
	}
	else
	{
		m_swapChain->Present(0, 0);
	}
}

void DX11Manager::TurnOnZBuffer()
{
	// �[�x�L����
	m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 1);
	return;
}

void DX11Manager::TurnOffZBuffer()
{
	// �[�x������
	m_deviceContext->OMSetDepthStencilState(m_disabledDepthStencilState, 1);
	return;
}

void DX11Manager::GetVideoCardInfo(char* cardName, int* memory)
{
	strcpy_s(cardName, 128, m_videoCardDescription);
	*memory = m_videoCardMemory;
	return;
}

bool DX11Manager::Init(int screenWidth, int screenHeight, bool vsync, HWND hWnd,
	bool fullscreen, float screenDepth, float screenNear)
{
	m_screenSize.x = screenWidth;
	m_screenSize.y = screenHeight;

	m_screenDepth = screenDepth;
	m_screenNear = screenNear;

	// �r�f�I�J�[�h���̏�����
	if (InitVideoCardInfo(screenWidth, screenHeight) == false)
	{
		return false;
	}

	// DirectX�̏�����
	if (InitDirectX(screenWidth, screenHeight, vsync, hWnd,
		fullscreen, screenDepth, screenNear) == false)
	{
		return false;
	}

	return true;
}

bool DX11Manager::InitVideoCardInfo(int screenWidth, int screenHeight)
{
	HRESULT result;
	IDXGIFactory* factory;
	IDXGIAdapter* adapter;
	IDXGIOutput* adapterOutput;
	unsigned int numModes;
	DXGI_MODE_DESC* displayModeList;
	DXGI_ADAPTER_DESC adapterDesc;
	int error;
	size_t stringLength;

	bool completed = false;

	// �t�@�N�g���쐬
	result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	if FAILED(result) goto End;

	// �A�_�v�^�쐬
	result = factory->EnumAdapters(0, &adapter);
	if FAILED(result) goto End;

	// �o��(���j�^�[)���
	result = adapter->EnumOutputs(0, &adapterOutput);
	if FAILED(result) goto End;

	// �`��(R8G8B8A8)�ɑΉ����Ă��郂�j�^�[�̕\�����[�h���擾
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM,
		DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
	if FAILED(result) goto End;

	// ���j�^�[/�r�f�I�J�[�h�̑g�ݍ��킹�Ŏg�p�o����\�����[�h�̃��X�g���쐬
	displayModeList = new DXGI_MODE_DESC[numModes];
	if FAILED(result) goto End;

	// �\�����[�h���擾
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM,
		DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
	if FAILED(result) goto End;

	// �w��̉�ʂ̕��ƍ����Ɉ�v���郂�[�h�̃��t���b�V�����[�g���擾
	for (unsigned int i = 0; i < numModes; i++)
	{
		if (displayModeList[i].Width == (unsigned int)screenWidth)
		{
			if (displayModeList[i].Height == (unsigned int)screenHeight)
			{
				numerator = displayModeList[i].RefreshRate.Numerator;
				denominator = displayModeList[i].RefreshRate.Denominator;
			}
		}
	}

	// �r�f�I�J�[�h�̐������擾
	result = adapter->GetDesc(&adapterDesc);
	if FAILED(result) goto End;

	// �r�f�I�J�[�h�̃������ʂ��L�^(MB�P��)
	m_videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

	// �r�f�I�J�[�h�̖��O���L�^
	error = wcstombs_s(&stringLength, m_videoCardDescription, 128, adapterDesc.Description, 128);
	if (error != 0) goto End;

	completed = true;

End: // �����������Ɏ��s�����ꍇ�͏I��

	/* �J�� */
	SafeRelease(factory);
	SafeRelease(adapter);
	SafeRelease(adapterOutput);
	SafeDeleteArr(displayModeList);

	return completed;
}

bool DX11Manager::InitDirectX(int screenWidth, int screenHeight, bool vsync, HWND hWnd,
	bool fullscreen, float screenDepth, float screenNear)
{
	HRESULT result;

	D3D_DRIVER_TYPE driverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE
	};
	UINT numDriverTypes = ARRAYSIZE(driverTypes);

	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0
	};
	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

	m_vsync_enabled = vsync;

	// �X���b�v�`�F�C���ݒ�
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
	swapChainDesc.BufferCount = 1; // �_�u���o�b�t�@
	swapChainDesc.BufferDesc.Width = screenWidth; // �o�b�N�o�b�t�@�̕�
	swapChainDesc.BufferDesc.Height = screenHeight;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // 32bit
	if (m_vsync_enabled) // ��������
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
	}
	else
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // �o�b�N�o�b�t�@�g�p��
	swapChainDesc.OutputWindow = hWnd; // �`����o�͂���E�B���h�E
	swapChainDesc.SampleDesc.Count = 1; // �}���`�T���v������
	swapChainDesc.SampleDesc.Quality = 0;
	if (fullscreen) // �t���X�N���[���ݒ�
	{
		swapChainDesc.Windowed = false;
	}
	else
	{
		swapChainDesc.Windowed = true;
	}
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED; // �����̏���
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED; // �����̃X�P�[��
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD; // �g�p��o�b�N�o�b�t�@�j��

	swapChainDesc.Flags = 0;

	// �Ή�����@�\���x�����������Đݒ�
	for (unsigned int i = 0; i < numDriverTypes; i++)
	{
		result = D3D11CreateDeviceAndSwapChain(
			NULL, driverTypes[i], NULL, 0, featureLevels,
			numFeatureLevels, D3D11_SDK_VERSION, &swapChainDesc, &m_swapChain,
			&m_device, NULL, &m_deviceContext);

		if SUCCEEDED(result)
		{
			m_driverType = driverTypes[i];
			break;
		}
	}

	if FAILED(result)
	{
		OutputDebugString(L"Failed to create device and SwapChain");
		return false;
	}

	// �o�b�N�o�b�t�@�ւ̃|�C���^���擾
	ID3D11Texture2D* backBufferPtr;
	result = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr);
	if FAILED(result) return false;

	// �����_�[�^�[�Q�b�g�r���[�쐬
	result = m_device->CreateRenderTargetView(backBufferPtr, NULL, &m_renderTargetView);
	if FAILED(result) return false;

	// �g�p�����o�b�N�o�b�t�@���J��
	SafeRelease(backBufferPtr);

	// �[�x�o�b�t�@�ݒ�
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	depthBufferDesc.Width = screenWidth;
	depthBufferDesc.Height = screenHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	// �[�x�o�b�t�@�p�e�N�X�`���쐬
	result = m_device->CreateTexture2D(&depthBufferDesc, NULL, &m_depthStencilBuffer);
	if FAILED(result) return false;

	// �[�x�X�e���V���ݒ�
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilDesc.StencilEnable = false; // �X�e���V������
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// �[�x�X�e���V���ݒ�쐬
	result = m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState);
	if FAILED(result) return false;

	// �[�x�X�e���V���ݒ��K�p
	m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 1);

	// �[�x�X�e���V���r���[�ݒ�
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	// �[�x�X�e���V���r���[�쐬
	result = m_device->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView);
	if FAILED(result) return false;

	// ��̃r���[�������_�����O�o�͂Ƀo�C���h
	m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);

	// 2D�`��p�̐[�x�X�e���V�������̐ݒ�
	D3D11_DEPTH_STENCIL_DESC disabledDepthStencilDesc;
	ZeroMemory(&disabledDepthStencilDesc, sizeof(disabledDepthStencilDesc));

	disabledDepthStencilDesc.DepthEnable = false; // �[�x����
	disabledDepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	disabledDepthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	disabledDepthStencilDesc.StencilEnable = false; // �X�e���V������
	disabledDepthStencilDesc.StencilReadMask = 0xFF;
	disabledDepthStencilDesc.StencilWriteMask = 0xFF;

	disabledDepthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	disabledDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	disabledDepthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	disabledDepthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	disabledDepthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	disabledDepthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	disabledDepthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	disabledDepthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// �[�x�X�e���V�������ݒ�쐬
	result = m_device->CreateDepthStencilState(&disabledDepthStencilDesc, &m_disabledDepthStencilState);
	if FAILED(result) return false;

	// ���X�^���C�U�ݒ�
	D3D11_RASTERIZER_DESC rasterDesc;
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	// ���X�^���C�U�ݒ�쐬
	result = m_device->CreateRasterizerState(&rasterDesc, &m_rasterState);
	if FAILED(result) return false;

	// ���X�^���C�U�ݒ�K�p
	m_deviceContext->RSSetState(m_rasterState);

	// �r���[�|�[�g�ݒ�
	D3D11_VIEWPORT viewport;
	viewport.Width = (float)screenWidth;
	viewport.Height = (float)screenHeight;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	// �r���[�|�[�g�ݒ�K�p
	m_deviceContext->RSSetViewports(1, &viewport);

	// �ˉe�ݒ�
	float fieldOfView, screenAspect;
	fieldOfView = 3.141592654f / 4.0f; // 45'
	screenAspect = (float)screenWidth / (float)screenHeight;

	// �ˉe�ϊ��s��쐬
	XMMATRIX projMat = XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, screenNear, screenDepth);
	XMStoreFloat4x4(&m_projectionMatrix, XMMatrixTranspose(projMat));

	// 2D�`��p�̔ėp���[���h�s��ݒ�
	XMStoreFloat4x4(&m_worldMatrix, XMMatrixTranspose(XMMatrixIdentity()));

	// 2D�`��p�̔ėp�r���[�s��ݒ� (���W0, 0, -1, �p�x0, 0, 0�̃r���[)
	m_screenViewMatrix._11 = 1.0f;
	m_screenViewMatrix._22 = 1.0f;
	m_screenViewMatrix._33 = 1.0f;
	m_screenViewMatrix._34 = 1.0f;
	m_screenViewMatrix._44 = 1.0f;

	// 2D�`��p�̕��s�ˉe�ݒ�
	XMMATRIX orthoMat = XMMatrixOrthographicLH((float)screenWidth, (float)screenHeight, screenNear, screenDepth);

	XMStoreFloat4x4(&m_orthoMatrix, XMMatrixTranspose(orthoMat));

	return true;
}