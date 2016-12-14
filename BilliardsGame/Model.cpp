#include "Model.h"
#include "DXUtil.h"

#include "Texture.h"
#include "ObjMesh.h"

Model::Model()
{
	m_vertexBuffer = nullptr;
	m_indexBuffer = nullptr;
	m_texture = nullptr;
}

Model::~Model()
{
	// 開放
	SafeRelease(m_vertexBuffer);
	SafeRelease(m_indexBuffer);
	SafeDelete(m_texture);
}

bool Model::Init(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const char* modelFilename, const char* texFilename)
{
	bool result;

	// モデル情報のロード
	ObjMesh* objMesh = new ObjMesh();
	result = objMesh->LoadOBJFile(modelFilename);
	if (!result) return false;

	unsigned int vertexCount = objMesh->GetNumVertices();
	m_indexCount = objMesh->GetNumVertices();
	ObjVertex* objVtx = objMesh->GetVertices();

	// 頂点バッファとインデックスバッファの初期化
	result = InitBuffers(device, vertexCount, m_indexCount, objVtx);
	if (!result) return false;

	// モデルのテクスチャをロード (nullならロードしない)
	if (texFilename)
	{
		result = LoadTexture(device, deviceContext, texFilename);
		if (!result) return false;
	}

	// ロードしたモデル情報を破棄
	SafeDelete(objMesh);

	return true;
}

bool Model::Init(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const ObjMesh* objMesh, const char* texFilename)
{
	bool result;

	unsigned int vertexCount = objMesh->GetNumVertices();
	m_indexCount = objMesh->GetNumVertices();
	ObjVertex* objVtx = objMesh->GetVertices();

	// 頂点バッファとインデックスバッファの初期化
	result = InitBuffers(device, vertexCount, m_indexCount, objVtx);
	if (!result) return false;

	// モデルのテクスチャをロード (nullならロードしない)
	if (texFilename)
	{
		result = LoadTexture(device, deviceContext, texFilename);
		if (!result) return false;
	}

	return true;
}

void Model::Render(ID3D11DeviceContext* deviceContext)
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

bool Model::InitBuffers(ID3D11Device* device, 
	unsigned int vertexCount, unsigned int indexCount, ObjVertex* objVtx)
{
	HRESULT result;
	
	// 頂点配列を作成
	VertexType* vertices;
	vertices = new VertexType[vertexCount];

	// インデックス配列を作成
	unsigned long* indices;
	indices = new unsigned long[indexCount];
	
	// 頂点情報を入れ込む
	for (unsigned int i = 0; i < vertexCount; i++)
	{
		vertices[i].pos = objVtx[i].pos;
		vertices[i].nor = objVtx[i].nor;
		vertices[i].tex = objVtx[i].tex;

		indices[i] = i;
	}

	// 頂点バッファの設定
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// 頂点バッファを作成
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if FAILED(result) return false;

	// インデックスバッファの設定
	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * indexCount;
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

bool Model::LoadTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const char* filename)
{
	bool result;

	m_texture = new Texture;

	result = m_texture->Init(device, deviceContext, filename, false);
	if (!result) return false;
	
	return true;
}

ID3D11ShaderResourceView* Model::GetTexture()
{
	if (!m_texture)
		return nullptr;

	return m_texture->GetTexture(); 
}

void Model::UpdateWorldMatrix(const XMFLOAT3& position, const XMFLOAT3& rotate, const XMFLOAT3& scale)
{
	XMMATRIX matrix;
	matrix = XMMatrixIdentity();

	// スケール
	matrix *= XMMatrixScaling(scale.x, scale.y, scale.z);

	// 回転
	matrix *= XMMatrixRotationRollPitchYaw(rotate.x, rotate.y, rotate.z);

	// 移動
	matrix *= XMMatrixTranslation(position.x, position.y, position.z);

	XMStoreFloat4x4(&m_worldMatrix, XMMatrixTranspose(matrix));
}

