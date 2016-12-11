#include "TextureShader.h"
#include "DXUtil.h"

TextureShader::TextureShader()
{
	m_vertexShader = nullptr;
	m_pixelShader = nullptr;
	m_layout = nullptr;
	m_matrixBuffer = nullptr;
	m_blendBuffer = nullptr;
	m_sampleState = nullptr;
}


TextureShader::~TextureShader()
{
	// シェーダ関連の開放
	SafeRelease(m_vertexShader);
	SafeRelease(m_pixelShader);
	SafeRelease(m_layout);
	SafeRelease(m_matrixBuffer);
	SafeRelease(m_blendBuffer);
	SafeRelease(m_sampleState);
}

bool TextureShader::Init(ID3D11Device* device, HWND hWnd)
{
	bool result;

	// シェーダファイルのパスを指定、初期化
	result = InitShader(device, hWnd, "data/TextureShader_v.cso", "data/TextureShader_p.cso");
	if (!result) return false;
	
	return true;
}

bool TextureShader::Render(ID3D11DeviceContext* deviceContext, int indexCount,
	const XMFLOAT4X4& worldMatrix, const XMFLOAT4X4& viewMatrix, const XMFLOAT4X4& projectionMatrix,
	ID3D11ShaderResourceView* tex, const XMFLOAT4& blendColor) const
{
	bool result;
	
	// シェーダのパラメータを設定
	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, tex, blendColor);
	if (!result) return false;

	// 描画
	RenderShader(deviceContext, indexCount);

	return true;
}

bool TextureShader::InitShader(ID3D11Device* device, HWND hWnd, char* vsFileName, char* psFileName)
{
	HRESULT result;
	
	// バイナリ(プリコンパイルシェーダ)を読み込んで各シェーダを作成する
	// 頂点シェーダ作成
	std::vector<char> vertexBinaryData;
	result = LoadShaderBinary(vsFileName, &vertexBinaryData);
	if FAILED(result) return false;

	result = device->CreateVertexShader(&vertexBinaryData[0],
		vertexBinaryData.size(), nullptr, &m_vertexShader);
	if FAILED(result) return false;

	// ピクセルシェーダ作成
	std::vector<char> pixelBinaryData;
	result = LoadShaderBinary(psFileName, &pixelBinaryData);
	if FAILED(result) return false;

	result = device->CreatePixelShader(&pixelBinaryData[0],
		pixelBinaryData.size(), nullptr, &m_pixelShader);
	if FAILED(result) return false;
	
	// 頂点データのレイアウトを作成
	D3D11_INPUT_ELEMENT_DESC polygonLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA , 0},
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	unsigned int numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	result = device->CreateInputLayout(polygonLayout, numElements,
		&vertexBinaryData[0], vertexBinaryData.size(), &m_layout);
	if FAILED(result) return false;

	// バイナリ確保に使ったvectorを明示的に開放(一応)
	std::vector<char>().swap(vertexBinaryData);
	std::vector<char>().swap(pixelBinaryData);

	// テクスチャサンプラー設定
	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;  // 異方性フィルタ
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 16;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	result = device->CreateSamplerState(&samplerDesc, &m_sampleState);
	if FAILED(result) return false;

	// (定数)バッファ設定
	D3D11_BUFFER_DESC matrixBufferDesc;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	result = device->CreateBuffer(&matrixBufferDesc, nullptr, &m_matrixBuffer);
	if FAILED(result) return false;

	// (定数)ブレンドカラーバッファ設定
	D3D11_BUFFER_DESC lightBufferDesc;
	lightBufferDesc.ByteWidth = sizeof(BlendBufferType);
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;

	result = device->CreateBuffer(&lightBufferDesc, nullptr, &m_blendBuffer);
	if FAILED(result) return false;

	return true;
}

HRESULT TextureShader::LoadShaderBinary(char* filename, std::vector<char>* binaryData)
{
	FILE* filePtr;
	long size;

	// ファイルオープン
	if (fopen_s(&filePtr, filename, "rb") != 0) return -1;
	
	fseek(filePtr, 0, SEEK_END);
	size = ftell(filePtr);
	fseek(filePtr, 0, SEEK_SET);

	binaryData->clear();
	binaryData->resize(size);

	fread(&(*binaryData)[0], sizeof(char), size, filePtr);
	// ファイルクローズ
	fclose(filePtr);
	
	return S_OK;
}

bool TextureShader::SetShaderParameters(ID3D11DeviceContext* deviceContext,
	const XMFLOAT4X4& worldMatrix, const XMFLOAT4X4& viewMatrix, const XMFLOAT4X4& projectionMatrix,
	ID3D11ShaderResourceView* tex, const XMFLOAT4& blendColor) const
{
	HRESULT result;

	// ↓定数バッファ書き込み
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if FAILED(result) return false;

	// 頂点情報をコピー
	MatrixBufferType* matrixPtr;
	matrixPtr = (MatrixBufferType*)mappedResource.pData;
	
	matrixPtr->world = worldMatrix;
	matrixPtr->view = viewMatrix;
	matrixPtr->projection = projectionMatrix;

	// ↑定数バッファ書き込み解除
	deviceContext->Unmap(m_matrixBuffer, 0);

	// シェーダへ値を受け渡す
	deviceContext->VSSetConstantBuffers(0, 1, &m_matrixBuffer);
	deviceContext->PSSetShaderResources(0, 1, &tex);

	// ↓ブレンドカラーバッファ書き込み
	result = deviceContext->Map(m_blendBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if FAILED(result) return false;

	// 頂点情報をコピー
	BlendBufferType* blendPtr;
	blendPtr = (BlendBufferType*)mappedResource.pData;
	blendPtr->color = blendColor;

	// ↑ライトバッファ書き込み解除
	deviceContext->Unmap(m_blendBuffer, 0);

	// シェーダへ値を受け渡す
	deviceContext->PSSetConstantBuffers(0, 1, &m_blendBuffer);

	return true;
}

void TextureShader::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount) const
{
	// レイアウト設定
	deviceContext->IASetInputLayout(m_layout);

	// シェーダ設定
	deviceContext->VSSetShader(m_vertexShader, nullptr, 0);
	deviceContext->PSSetShader(m_pixelShader, nullptr, 0);
	deviceContext->PSSetSamplers(0, 1, &m_sampleState);

	// 描画
	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}