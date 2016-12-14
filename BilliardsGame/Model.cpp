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
	// �J��
	SafeRelease(m_vertexBuffer);
	SafeRelease(m_indexBuffer);
	SafeDelete(m_texture);
}

bool Model::Init(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const char* modelFilename, const char* texFilename)
{
	bool result;

	// ���f�����̃��[�h
	ObjMesh* objMesh = new ObjMesh();
	result = objMesh->LoadOBJFile(modelFilename);
	if (!result) return false;

	unsigned int vertexCount = objMesh->GetNumVertices();
	m_indexCount = objMesh->GetNumVertices();
	ObjVertex* objVtx = objMesh->GetVertices();

	// ���_�o�b�t�@�ƃC���f�b�N�X�o�b�t�@�̏�����
	result = InitBuffers(device, vertexCount, m_indexCount, objVtx);
	if (!result) return false;

	// ���f���̃e�N�X�`�������[�h (null�Ȃ烍�[�h���Ȃ�)
	if (texFilename)
	{
		result = LoadTexture(device, deviceContext, texFilename);
		if (!result) return false;
	}

	// ���[�h�������f������j��
	SafeDelete(objMesh);

	return true;
}

bool Model::Init(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const ObjMesh* objMesh, const char* texFilename)
{
	bool result;

	unsigned int vertexCount = objMesh->GetNumVertices();
	m_indexCount = objMesh->GetNumVertices();
	ObjVertex* objVtx = objMesh->GetVertices();

	// ���_�o�b�t�@�ƃC���f�b�N�X�o�b�t�@�̏�����
	result = InitBuffers(device, vertexCount, m_indexCount, objVtx);
	if (!result) return false;

	// ���f���̃e�N�X�`�������[�h (null�Ȃ烍�[�h���Ȃ�)
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
	
	// ���_�o�b�t�@��K�p
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
	// �C���f�b�N�X�o�b�t�@��K�p
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	// �`�悷��v���~�e�B�u�^�C�v�̐ݒ�(�g���C�A���O�����X�g)
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

bool Model::InitBuffers(ID3D11Device* device, 
	unsigned int vertexCount, unsigned int indexCount, ObjVertex* objVtx)
{
	HRESULT result;
	
	// ���_�z����쐬
	VertexType* vertices;
	vertices = new VertexType[vertexCount];

	// �C���f�b�N�X�z����쐬
	unsigned long* indices;
	indices = new unsigned long[indexCount];
	
	// ���_������ꍞ��
	for (unsigned int i = 0; i < vertexCount; i++)
	{
		vertices[i].pos = objVtx[i].pos;
		vertices[i].nor = objVtx[i].nor;
		vertices[i].tex = objVtx[i].tex;

		indices[i] = i;
	}

	// ���_�o�b�t�@�̐ݒ�
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

	// ���_�o�b�t�@���쐬
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if FAILED(result) return false;

	// �C���f�b�N�X�o�b�t�@�̐ݒ�
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

	// �C���f�b�N�X�o�b�t�@���쐬
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if FAILED(result) return false;

	// �J��
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

	// �X�P�[��
	matrix *= XMMatrixScaling(scale.x, scale.y, scale.z);

	// ��]
	matrix *= XMMatrixRotationRollPitchYaw(rotate.x, rotate.y, rotate.z);

	// �ړ�
	matrix *= XMMatrixTranslation(position.x, position.y, position.z);

	XMStoreFloat4x4(&m_worldMatrix, XMMatrixTranspose(matrix));
}

