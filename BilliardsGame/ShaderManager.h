/* シェーダマネージャクラス */
// 利用するシェーダ全てを管理、カプセル化するクラス

#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;

class LightShader;
class TextureShader;

class ShaderManager
{
public:
	ShaderManager();
	~ShaderManager();

	bool Init(ID3D11Device* device, HWND hWnd);

	bool RenderLightShader(ID3D11DeviceContext* deviceContext, int indexCount,
		const XMFLOAT4X4& worldMatrix, const XMFLOAT4X4& viewMatrix, const XMFLOAT4X4& projectionMatrix,
		ID3D11ShaderResourceView* tex, XMFLOAT3 lightDirection, XMFLOAT4 diffuseColor) const;

	bool RenderTextureShader(ID3D11DeviceContext* deviceContext, int indexCount,
		const XMFLOAT4X4& worldMatrix, const XMFLOAT4X4& viewMatrix, const XMFLOAT4X4& projectionMatrix,
		ID3D11ShaderResourceView* tex) const;

private:
	LightShader* m_lightShader;
	TextureShader* m_textureShader;
};

