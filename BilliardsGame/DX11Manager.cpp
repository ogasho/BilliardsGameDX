#include "DX11Manager.h"
#include "DXUtil.h"

DX11Manager::DX11Manager()
{
	m_swapChain = nullptr;
	m_device = nullptr;
	m_deviceContext = nullptr;
	m_renderTargetView = nullptr;
	m_depthStencilBuffer = nullptr;
	m_depthEnableStencilState = nullptr;
	m_depthDisableStencilState = nullptr;
	m_depthStencilView = nullptr;
	m_rasterState = nullptr;
	m_alphaEnableBlendingState = nullptr;
	m_alphaDisableBlendingState = nullptr;
}

DX11Manager::~DX11Manager()
{
	// フルスクリーン解除
	if (m_swapChain)
	{
		m_swapChain->SetFullscreenState(false, NULL);
	}

	SafeRelease(m_swapChain);
	SafeRelease(m_device);
	SafeRelease(m_deviceContext);
	SafeRelease(m_renderTargetView);
	SafeRelease(m_depthStencilBuffer);
	SafeRelease(m_depthEnableStencilState);
	SafeRelease(m_depthDisableStencilState);
	SafeRelease(m_depthStencilView);
	SafeRelease(m_rasterState);
	SafeRelease(m_alphaEnableBlendingState);
	SafeRelease(m_alphaDisableBlendingState);
}

void DX11Manager::Begin(float r, float g, float b, float a)
{
	float color[4];
	color[0] = r;
	color[1] = g;
	color[2] = b;
	color[3] = a;

	// バックバッファクリア
	m_deviceContext->ClearRenderTargetView(m_renderTargetView, color);

	// 深度バッファクリア
	m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void DX11Manager::End()
{
	// レンダリングが完了しているので、バックバッファを表示
	if (m_vsync_enabled)
	{
		m_swapChain->Present(1, 0);
	}
	else
	{
		m_swapChain->Present(0, 0);
	}
}

void DX11Manager::SpriteBegin() const
{
	TurnOffZBuffer();
	TurnOnAlphaBlend();
}

void DX11Manager::SpriteEnd() const
{
	TurnOnZBuffer();
	TurnOffAlphaBlend();
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

	// ビデオカード情報の初期化
	if (InitVideoCardInfo(screenWidth, screenHeight) == false)
	{
		return false;
	}

	// DirectXの初期化
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

	// ファクトリ作成
	result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	if FAILED(result) goto End;

	// アダプタ作成
	result = factory->EnumAdapters(0, &adapter);
	if FAILED(result) goto End;

	// 出力(モニター)を列挙
	result = adapter->EnumOutputs(0, &adapterOutput);
	if FAILED(result) goto End;

	// 形式(R8G8B8A8)に対応しているモニターの表示モードを取得
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM,
		DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
	if FAILED(result) goto End;

	// モニター/ビデオカードの組み合わせで使用出来る表示モードのリストを作成
	displayModeList = new DXGI_MODE_DESC[numModes];
	if FAILED(result) goto End;

	// 表示モードを取得
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM,
		DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
	if FAILED(result) goto End;

	// 指定の画面の幅と高さに一致するモードのリフレッシュレートを取得
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

	// ビデオカードの説明を取得
	result = adapter->GetDesc(&adapterDesc);
	if FAILED(result) goto End;

	// ビデオカードのメモリ量を記録(MB単位)
	m_videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

	// ビデオカードの名前を記録
	error = wcstombs_s(&stringLength, m_videoCardDescription, 128, adapterDesc.Description, 128);
	if (error != 0) goto End;

	completed = true;

End: // 初期化処理に失敗した場合は終了

	/* 開放 */
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

	// スワップチェイン設定
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
	swapChainDesc.BufferCount = 1; // ダブルバッファ
	swapChainDesc.BufferDesc.Width = screenWidth; // バックバッファの幅
	swapChainDesc.BufferDesc.Height = screenHeight;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // 32bit
	if (m_vsync_enabled) // 垂直同期
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
	}
	else
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // バックバッファ使用量
	swapChainDesc.OutputWindow = hWnd; // 描画を出力するウィンドウ
	swapChainDesc.SampleDesc.Count = 1; // マルチサンプル無し
	swapChainDesc.SampleDesc.Quality = 0;
	if (fullscreen) // フルスクリーン設定
	{
		swapChainDesc.Windowed = false;
	}
	else
	{
		swapChainDesc.Windowed = true;
	}
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED; // 走査の順序
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED; // 走査のスケール
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD; // 使用後バックバッファ破棄

	swapChainDesc.Flags = 0;

	// 対応する機能レベルを検索して設定
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

	// バックバッファへのポインタを取得
	ID3D11Texture2D* backBufferPtr;
	result = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr);
	if FAILED(result) return false;

	// レンダーターゲットビュー作成
	result = m_device->CreateRenderTargetView(backBufferPtr, NULL, &m_renderTargetView);
	if FAILED(result) return false;

	// 使用したバックバッファを開放
	SafeRelease(backBufferPtr);

	// 深度バッファ設定
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

	// 深度バッファ用テクスチャ作成
	result = m_device->CreateTexture2D(&depthBufferDesc, NULL, &m_depthStencilBuffer);
	if FAILED(result) return false;

	// 深度ステンシル設定
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	// 深度ステンシル(有効)設定作成
	GetDepthStencilState(&depthStencilDesc, true, false);
	result = m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthEnableStencilState);
	if FAILED(result) return false;

	// 深度ステンシル(深度無効)設定作成
	GetDepthStencilState(&depthStencilDesc, false, false);
	result = m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthDisableStencilState);
	if FAILED(result) return false;

	// 深度ステンシル設定を適用
	m_deviceContext->OMSetDepthStencilState(m_depthEnableStencilState, 1);

	// 深度ステンシルビュー設定
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	// 深度ステンシルビュー作成
	result = m_device->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView);
	if FAILED(result) return false;

	// 二つのビューをレンダリング出力にバインド
	m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);

	// ブレンドステート設定
	D3D11_BLEND_DESC blendStateDesc;
	ZeroMemory(&blendStateDesc, sizeof(blendStateDesc));

	// ブレンドステート(有効)作成
	GetBlendState(&blendStateDesc, true);
	result = m_device->CreateBlendState(&blendStateDesc, &m_alphaEnableBlendingState);
	if FAILED(result) return false;

	// ブレンドステート(無効)作成
	GetBlendState(&blendStateDesc, false);
	result = m_device->CreateBlendState(&blendStateDesc, &m_alphaDisableBlendingState);
	if FAILED(result) return false;

	// ラスタライザ設定
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

	// ラスタライザ設定作成
	result = m_device->CreateRasterizerState(&rasterDesc, &m_rasterState);
	if FAILED(result) return false;

	// ラスタライザ設定適用
	m_deviceContext->RSSetState(m_rasterState);

	// ビューポート設定
	D3D11_VIEWPORT viewport;
	viewport.Width = (float)screenWidth;
	viewport.Height = (float)screenHeight;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	// ビューポート設定適用
	m_deviceContext->RSSetViewports(1, &viewport);

	// 射影設定
	float fieldOfView, screenAspect;
	fieldOfView = 3.141592654f / 4.0f; // 45'
	screenAspect = (float)screenWidth / (float)screenHeight;

	// 射影変換行列作成
	XMMATRIX projMat = XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, screenNear, screenDepth);
	XMStoreFloat4x4(&m_projectionMatrix, XMMatrixTranspose(projMat));

	// 2D描画用の汎用ワールド行列設定
	XMStoreFloat4x4(&m_worldMatrix, XMMatrixTranspose(XMMatrixIdentity()));

	// 2D描画用の汎用ビュー行列設定 (座標0, 0, -1, 角度0, 0, 0のビュー)
	m_screenViewMatrix._11 = 1.0f;
	m_screenViewMatrix._22 = 1.0f;
	m_screenViewMatrix._33 = 1.0f;
	m_screenViewMatrix._34 = 1.0f;
	m_screenViewMatrix._44 = 1.0f;

	// 2D描画用の平行射影設定
	XMMATRIX orthoMat = XMMatrixOrthographicLH((float)screenWidth, (float)screenHeight, screenNear, screenDepth);

	XMStoreFloat4x4(&m_orthoMatrix, XMMatrixTranspose(orthoMat));

	return true;
}

void DX11Manager::GetDepthStencilState(D3D11_DEPTH_STENCIL_DESC* depthStencilDesc, bool depthEnable, bool stencilEnable)
{
	depthStencilDesc->DepthEnable = depthEnable;
	depthStencilDesc->DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc->DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilDesc->StencilEnable = stencilEnable;
	depthStencilDesc->StencilReadMask = 0xFF;
	depthStencilDesc->StencilWriteMask = 0xFF;

	depthStencilDesc->FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc->FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc->FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc->FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	depthStencilDesc->BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc->BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc->BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc->BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
}

void DX11Manager::GetBlendState(D3D11_BLEND_DESC* blendStateDesc, bool blendEnable)
{
	blendStateDesc->RenderTarget[0].BlendEnable = blendEnable;
	blendStateDesc->RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendStateDesc->RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendStateDesc->RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendStateDesc->RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendStateDesc->RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendStateDesc->RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendStateDesc->RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
}

void DX11Manager::TurnOnZBuffer() const
{
	// 深度有効化
	m_deviceContext->OMSetDepthStencilState(m_depthEnableStencilState, 1);
	return;
}

void DX11Manager::TurnOffZBuffer() const
{
	// 深度無効化
	m_deviceContext->OMSetDepthStencilState(m_depthDisableStencilState, 1);
	return;
}

void DX11Manager::TurnOnAlphaBlend() const
{
	// アルファブレンド有効化
	float blendFactor[4]{0.0f, 0.0f, 0.0f, 0.0f};

	m_deviceContext->OMSetBlendState(m_alphaEnableBlendingState, blendFactor, 0xffffffff);
}

void DX11Manager::TurnOffAlphaBlend() const
{
	// アルファブレンド無効化
	float blendFactor[4]{0.0f, 0.0f, 0.0f, 0.0f};

	m_deviceContext->OMSetBlendState(m_alphaDisableBlendingState, blendFactor, 0xffffffff);
}