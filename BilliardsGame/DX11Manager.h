/* DirectX11 処理クラス */
// DirectX初期化、描画処理を司る

#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;


class DX11Manager
{
public:
	DX11Manager();
	~DX11Manager();

	bool Init(int screenWidth, int screenHeight, bool vsync, HWND hWnd,
		bool fullscreen, float screenDepth, float screenNear);

	void Begin(float r, float g, float b, float a);
	void End();

	void SpriteBegin() const;
	void SpriteEnd() const;

	ID3D11Device* GetDevice()const{ return m_device; }
	ID3D11DeviceContext* GetDeviceContext()const{ return m_deviceContext; }
	XMINT2 GetScreenSize()const{ return m_screenSize; }

	void GetProjectionMatrix(XMFLOAT4X4* projectionMatrix)const{ *projectionMatrix = m_projectionMatrix; }
	void GetWorldMatrix(XMFLOAT4X4* worldMatrix)const{ *worldMatrix = m_worldMatrix; }
	void GetOrthoMatrix(XMFLOAT4X4* orthoMatrix)const{ *orthoMatrix = m_orthoMatrix; }
	void GetScreenViewMatrix(XMFLOAT4X4* screenViewMatrix)const{ *screenViewMatrix = m_screenViewMatrix; }
	float GetScreenDepth()const{ return m_screenDepth; }
	float GetScreenNear()const{ return m_screenNear; }

	void GetVideoCardInfo(char* cardName, int* memory);

private:
	bool InitVideoCardInfo(int screenWidth, int screenHeight);
	bool InitDirectX(int screenWidth, int screenHeight, bool vsync, HWND hWnd,
		bool fullscreen, float screenDepth, float screenNear);

	void TurnOnZBuffer() const;
	void TurnOffZBuffer() const;
	void TurnOnAlphaBlend() const;
	void TurnOffAlphaBlend() const;
	void GetDepthStencilState(D3D11_DEPTH_STENCIL_DESC* depthStencilDesc, bool depthEnable, bool stencilEnable);
	void GetBlendState(D3D11_BLEND_DESC* blendStateDesc, bool blendEnable);

	bool m_vsync_enabled;
	int m_videoCardMemory;
	char m_videoCardDescription[128];
	unsigned int numerator, denominator;

	IDXGISwapChain* m_swapChain;
	ID3D11Device* m_device;
	ID3D11DeviceContext* m_deviceContext;
	D3D_DRIVER_TYPE	m_driverType;
	ID3D11RenderTargetView* m_renderTargetView;
	ID3D11Texture2D* m_depthStencilBuffer;
	ID3D11DepthStencilState* m_depthEnableStencilState;
	ID3D11DepthStencilState* m_depthDisableStencilState;
	ID3D11DepthStencilView* m_depthStencilView;
	ID3D11RasterizerState* m_rasterState;
	ID3D11BlendState* m_alphaEnableBlendingState;
	ID3D11BlendState* m_alphaDisableBlendingState;

	XMINT2 m_screenSize;
	float m_screenDepth, m_screenNear;
	XMFLOAT4X4 m_projectionMatrix;
	XMFLOAT4X4 m_worldMatrix;
	XMFLOAT4X4 m_orthoMatrix;
	XMFLOAT4X4 m_screenViewMatrix;
};