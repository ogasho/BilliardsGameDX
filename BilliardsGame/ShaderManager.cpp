#include "ShaderManager.h"
#include "DXUtil.h"

#include "LightShader.h"
#include "TextureShader.h"

ShaderManager::ShaderManager()
{
	m_lightShader = nullptr;
	m_textureShader = nullptr;
}

ShaderManager::~ShaderManager()
{
	SafeDelete(m_lightShader);
	SafeDelete(m_textureShader);
}

bool ShaderManager::Init(ID3D11Device* device, HWND hWnd)
{
	bool result;

	// ライトシェーダ初期化
	m_lightShader = new LightShader;
	result = m_lightShader->Init(device, hWnd);
	if (!result) return false;

	// テクスチャシェーダ初期化
	m_textureShader = new TextureShader;
	result = m_textureShader->Init(device, hWnd);
	if (!result) return false;

	return true;
}

bool ShaderManager::RenderLightShader(ID3D11DeviceContext* deviceContext, int indexCount,
	const XMFLOAT4X4& worldMatrix, const XMFLOAT4X4& viewMatrix, const XMFLOAT4X4& projectionMatrix,
	ID3D11ShaderResourceView* tex,
	const XMFLOAT3& lightDirection, const XMFLOAT4& diffuseColor, const XMFLOAT4& ambientColor) const
{
	bool result;

	result = m_lightShader->Render(deviceContext, indexCount, worldMatrix, viewMatrix, projectionMatrix,
		tex, lightDirection, diffuseColor, ambientColor);

	return result;
}

bool ShaderManager::RenderTextureShader(ID3D11DeviceContext* deviceContext, int indexCount,
	const XMFLOAT4X4& worldMatrix, const XMFLOAT4X4& viewMatrix, const XMFLOAT4X4& projectionMatrix,
	ID3D11ShaderResourceView* tex, const XMFLOAT4& blendColor) const
{
	bool result = true;

	result = m_textureShader->Render(deviceContext, indexCount, worldMatrix, viewMatrix, projectionMatrix, tex, blendColor);

	return result;
}

bool ShaderManager::RenderTextureShader(ID3D11DeviceContext* deviceContext, int indexCount,
	const XMFLOAT4X4& worldMatrix, const XMFLOAT4X4& viewMatrix, const XMFLOAT4X4& projectionMatrix,
	ID3D11ShaderResourceView* tex) const
{
	bool result = true;

	XMFLOAT4 color = XMFLOAT4(1, 1, 1, 1);

	result = m_textureShader->Render(deviceContext, indexCount, worldMatrix, viewMatrix, projectionMatrix, tex, color);

	return result;
}