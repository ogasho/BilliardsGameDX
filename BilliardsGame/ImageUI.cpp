#include "ImageUI.h"
#include "DXUtil.h"

#include "Texture.h"

ImageUI::ImageUI()
{
	m_vertexBuffer = nullptr;
	m_indexBuffer = nullptr;
	m_texture = nullptr;
}


ImageUI::~ImageUI()
{
	SafeRelease(m_vertexBuffer);
	SafeRelease(m_indexBuffer);
	SafeDelete(m_texture);

}

bool ImageUI::Init(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int screenWidth, int screenHeight,
	int imageWidth, int imageHeight, const char* texFilename)
{
	bool result;

	// スクリーンサイズ
	m_screenSize.x = screenWidth;
	m_screenSize.y = screenHeight;

	// 画像サイズ
	m_imageSize.x = imageWidth;
	m_imageSize.y = imageHeight;

	// 前フレーム位置情報
	m_previousPos.x = -1;
	m_previousPos.y = -1;

	// バッファ初期化
	result = InitBuffers(device);
	if (!result) return false;

	// 画像をロード
	result = LoadTexture(device, deviceContext, texFilename);
	if (!result) return false;

	return true;
}

bool ImageUI::Render(ID3D11DeviceContext* deviceContext, int posX, int posY)
{
	bool result;

	// 動的頂点バッファを更新
	result = UpdateBuffers(deviceContext, posX, posY);
	if (!result) return false;

	// 頂点バッファとインデックスバッファをパイプラインに流し込む
	RenderBuffers(deviceContext);

	return true;
}

ID3D11ShaderResourceView* ImageUI::GetTexture()
{
	return m_texture->GetTexture();
}

bool ImageUI::InitBuffers(ID3D11Device* device)
{
	HRESULT result;

	m_vertexCount = 6;
	m_indexCount = m_vertexCount;

	// 頂点配列を作成
	VertexType* vertices;
	vertices = new VertexType[m_vertexCount];
	memset(vertices, 0, (sizeof(VertexType) * m_vertexCount));

	// インデックス配列を作成
	unsigned long* indices;
	indices = new unsigned long[m_indexCount];
	for (unsigned int i = 0; i < m_indexCount; i++)
	{
		indices[i] = i;
	}

	// 頂点バッファの設定
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC; // 動的頂点バッファ
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; // CPUアクセス許可
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// 頂点バッファ作成
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if FAILED(result) return false;

	// インデックスバッファの設定
	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// インデックスバッファを作成
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if FAILED(result) return false;

	// 開放
	SafeDeleteArr(vertices);
	SafeDeleteArr(indices);

	return true;
}

bool ImageUI::UpdateBuffers(ID3D11DeviceContext* deviceContext, int posX, int posY)
{
	HRESULT result;

	// 描画位置が変更されていないなら、バッファの更新は行わない
	if (posX == m_previousPos.x && posY == m_previousPos.y) return true;

	// 座標更新
	m_previousPos.x = posX;
	m_previousPos.y = posY;

	// 画像の位置情報を計算 (座標基準は左上 + 画像サイズ)
	float top = (float)(m_screenSize.y / 2 - posY);
	float bottom = (float)(top - m_imageSize.y);
	float left = (float)((m_screenSize.x / 2)*-1 + posX);
	float right = (float)(left + m_imageSize.x);

	// 更新用の頂点配列を作成
	VertexType* vertices;
	vertices = new VertexType[m_vertexCount];

	// 板ポリ作成
	vertices[0].pos = XMFLOAT3(left, top, 0.0f); // 左上
	vertices[0].tex = XMFLOAT2(0.0f, 0.0f);
	vertices[1].pos = XMFLOAT3(right, bottom, 0.0f); // 右下
	vertices[1].tex = XMFLOAT2(1.0f, 1.0f);
	vertices[2].pos = XMFLOAT3(left, bottom, 0.0f); // 左下
	vertices[2].tex = XMFLOAT2(0.0f, 1.0f);

	vertices[3].pos = XMFLOAT3(left, top, 0.0f); // 左上
	vertices[3].tex = XMFLOAT2(0.0f, 0.0f);
	vertices[4].pos = XMFLOAT3(right, top, 0.0f); // 右上
	vertices[4].tex = XMFLOAT2(1.0f, 0.0f);
	vertices[5].pos = XMFLOAT3(right, bottom, 0.0f); // 右下
	vertices[5].tex = XMFLOAT2(1.0f, 1.0f);

	// ↓バッファ書き込み
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	result = deviceContext->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if FAILED(result) return false;

	// 頂点情報をコピー
	VertexType* verticesPtr;
	verticesPtr = (VertexType*)mappedResource.pData;
	memcpy(verticesPtr, (void*)vertices, (sizeof(VertexType) * m_vertexCount));

	// ↑バッファ書き込み解除
	deviceContext->Unmap(m_vertexBuffer, 0);

	// 開放
	SafeDeleteArr(vertices);

	return true;
}

void ImageUI::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride = sizeof(VertexType);
	unsigned int offset = 0;

	// 頂点バッファを適用
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
	// インデックスバッファを適用
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	// 描画するプリミティブタイプの設定(トライアングルリスト)
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

bool ImageUI::LoadTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const char* texFilename)
{
	bool result;

	m_texture = new Texture;
	result = m_texture->Init(device, deviceContext, texFilename);
	if (!result) return false;

	return true;
}