/* テクスチャシェーダクラス */
// シェーダの読み込み、パラメータ設定、描画を行う

#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>
using namespace DirectX;

class TextureShader
{
private:
	struct MatrixBufferType
	{
		XMFLOAT4X4 world;
		XMFLOAT4X4 view;
		XMFLOAT4X4 projection;
	};

	struct BlendBufferType
	{
		XMFLOAT4 color;
	};

public:
	TextureShader();
	~TextureShader();

	bool Init(ID3D11Device* device, HWND hWnd);
	bool Render(ID3D11DeviceContext* deviceContext, int indexCount,
		const XMFLOAT4X4& worldMatrix, const XMFLOAT4X4& viewMatrix, const XMFLOAT4X4& projectionMatrix,
		ID3D11ShaderResourceView* tex, const XMFLOAT4& blendColor) const;

private:
	bool InitShader(ID3D11Device* device, HWND hWnd, char* vsFileName, char* psFileName);
	HRESULT LoadShaderBinary(char* filename, std::vector<char>* binaryData);
	bool SetShaderParameters(ID3D11DeviceContext* deviceContext,
		const XMFLOAT4X4& worldMatrix, const XMFLOAT4X4& viewMatrix, const XMFLOAT4X4& projectionMatrix,
		ID3D11ShaderResourceView* tex, const XMFLOAT4& blendColor) const;
	void RenderShader(ID3D11DeviceContext* deviceContext, int indexCount) const;

	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11SamplerState* m_sampleState;
	ID3D11Buffer* m_matrixBuffer;
	ID3D11Buffer* m_blendBuffer;
};

