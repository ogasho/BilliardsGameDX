/* DirectX11 èàóùÉNÉâÉX */
// DirectXèâä˙âªÅAï`âÊèàóùÇéiÇÈ

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

	ID3D11Device* GetDevice()const{ return m_device; }
	ID3D11DeviceContext* GetDeviceContext()const{ return m_deviceContext; }

	void GetProjectionMatrix(XMFLOAT4X4* projectionMatrix)const{ *projectionMatrix = m_projectionMatrix; }
	void GetOrthoMatrix(XMFLOAT4X4* orthoMatrix)const{ *orthoMatrix = m_orthoMatrix; }

	void GetVideoCardInfo(char* cardName, int* memory);

private:
	bool InitVideoCardInfo(int screenWidth, int screenHeight);
	bool InitDirectX(int screenWidth, int screenHeight, bool vsync, HWND hWnd,
		bool fullscreen, float screenDepth, float screenNear);

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
	ID3D11DepthStencilState* m_depthStencilState;
	ID3D11DepthStencilView* m_depthStencilView;
	ID3D11RasterizerState* m_rasterState;

	XMFLOAT4X4 m_projectionMatrix;
	XMFLOAT4X4 m_orthoMatrix;
};