#include "Model.h"
#include "DXUtil.h"

#include "Texture.h"
#include "ObjMesh.h"

Model::Model()
{
	m_vertexBuffer = nullptr;
	m_indexBuffer = nullptr;
	m_texture = nullptr;
	m_mesh = nullptr;

	m_isUpdatedTransform = true;
	
	// 向き初期化
	m_transform.scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
}

Model::~Model()
{
	// 開放
	SafeRelease(m_vertexBuffer);
	SafeRelease(m_indexBuffer);
	SafeDelete(m_texture);
	SafeDelete(m_mesh);
}

bool Model::Init(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* modelFilename, char* texFilename)
{
	bool result;

	// モデルのロード
	result = LoadModel(modelFilename);
	if (!result) return false;

	// 頂点バッファとインデックスバッファの初期化
	result = InitBuffers(device);
	if (!result) return false;

	// モデルのテクスチャをロード
	result = LoadTexture(device, deviceContext, texFilename);
	if (!result) return false;

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

bool Model::LoadModel(char* modelFilename)
{
	bool result;

	m_mesh = new ObjMesh();
	result = m_mesh->LoadOBJFile(modelFilename);
	if (!result) return false;

	m_vertexCount = m_mesh->GetNumVertices();
	m_indexCount = m_mesh->GetNumVertices();

	return true;
}

bool Model::InitBuffers(ID3D11Device* device)
{
	HRESULT result;
	
	// 頂点配列を作成
	VertexType* vertices;
	vertices = new VertexType[m_vertexCount];

	// インデックス配列を作成
	unsigned long* indices;
	indices = new unsigned long[m_indexCount];
	
	ObjMesh::ObjVertex* objVer = m_mesh->GetVertices();
	for (int i = 0; i < m_vertexCount; i++)
	{
		vertices[i].pos = objVer[i].pos;
		vertices[i].nor = objVer[i].nor;
		vertices[i].tex = objVer[i].tex;

		indices[i] = i;
	}


	// 頂点バッファの設定
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
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

bool Model::LoadTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* filename)
{
	bool result;

	m_texture = new Texture;

	result = m_texture->Init(device, deviceContext, filename);
	if (!result) return false;
	
	return true;
}

ID3D11ShaderResourceView* Model::GetTexture()
{
	return m_texture->GetTexture(); 
}

void Model::GetCurrentMatrix(XMFLOAT4X4 *worldMatrix)
{
	//XMFLOAT4X4 matrix;

	if (m_isUpdatedTransform)
	{
		XMMATRIX matrix;
		matrix = XMMatrixIdentity();
		matrix *= XMMatrixScaling(m_transform.scale.x, m_transform.scale.y, m_transform.scale.z);
		matrix *= XMMatrixRotationRollPitchYaw(XMConvertToRadians(m_transform.rotate.x), XMConvertToRadians(m_transform.rotate.y), XMConvertToRadians(m_transform.rotate.z));
		matrix *= XMMatrixTranslation(m_transform.position.x, m_transform.position.y, m_transform.position.z);

		m_isUpdatedTransform = false;
		XMStoreFloat4x4(&m_worldMatrix, XMMatrixTranspose(matrix));
	}

	*worldMatrix = m_worldMatrix;
	//*worldMatrix = matrix;
}


void Model::SetPosition(float x, float y, float z)
{
	m_transform.position.x = x;
	m_transform.position.y = y;
	m_transform.position.z = z;

	m_isUpdatedTransform = true;
}

void Model::SetRotation(float x, float y, float z)
{
	m_transform.rotate.x = x;
	m_transform.rotate.y = y;
	m_transform.rotate.z = z;

	m_isUpdatedTransform = true;
}

void Model::SetScale(float x, float y, float z)
{
	m_transform.scale.x = x;
	m_transform.scale.y = y;
	m_transform.scale.z = z;

	m_isUpdatedTransform = true;
}


void Model::AddPosition(float x, float y, float z)
{
	m_transform.position.x += x;
	m_transform.position.y += y;
	m_transform.position.z += z;

	m_isUpdatedTransform = true;
}

void Model::AddRotation(float x, float y, float z)
{
	m_transform.rotate.x += x;
	m_transform.rotate.y += y;
	m_transform.rotate.z += z;

	m_isUpdatedTransform = true;
}

void Model::AddScale(float x, float y, float z)
{
	m_transform.scale.x += x;
	m_transform.scale.y += y;
	m_transform.scale.z += z;

	m_isUpdatedTransform = true;
}