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

	struct LightBufferType
	{
		XMFLOAT4 diffuseColor;
		XMFLOAT3 lightDirection;
		float padding;
	};

public:
	TextureShader();
	~TextureShader();

	bool Init(ID3D11Device*, HWND);
	bool Render(ID3D11DeviceContext*, int, const XMFLOAT4X4&, const XMFLOAT4X4&, const XMFLOAT4X4&,
		ID3D11ShaderResourceView*, XMFLOAT3, XMFLOAT4);

private:
	bool InitShader(ID3D11Device*, HWND, char*, char*);
	HRESULT LoadShaderBinary(char*, std::vector<char>*);
	bool SetShaderParameters(ID3D11DeviceContext*, const XMFLOAT4X4&, const XMFLOAT4X4&, const XMFLOAT4X4&,
		ID3D11ShaderResourceView*, XMFLOAT3, XMFLOAT4);
	void RenderShader(ID3D11DeviceContext*, int);

	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11SamplerState* m_sampleState;
	ID3D11Buffer* m_matrixBuffer;
	ID3D11Buffer* m_lightBuffer;
};

