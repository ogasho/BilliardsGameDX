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

	// �X�N���[���T�C�Y
	m_screenSize.x = screenWidth;
	m_screenSize.y = screenHeight;

	// �摜�T�C�Y
	m_imageSize.x = imageWidth;
	m_imageSize.y = imageHeight;

	// �O�t���[���ʒu���
	m_previousPos.x = -1;
	m_previousPos.y = -1;

	// �o�b�t�@������
	result = InitBuffers(device);
	if (!result) return false;

	// �摜�����[�h
	result = LoadTexture(device, deviceContext, texFilename);
	if (!result) return false;

	return true;
}

bool ImageUI::Render(ID3D11DeviceContext* deviceContext, int posX, int posY)
{
	bool result;

	// ���I���_�o�b�t�@���X�V
	result = UpdateBuffers(deviceContext, posX, posY);
	if (!result) return false;

	// ���_�o�b�t�@�ƃC���f�b�N�X�o�b�t�@���p�C�v���C���ɗ�������
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

	// ���_�z����쐬
	VertexType* vertices;
	vertices = new VertexType[m_vertexCount];
	memset(vertices, 0, (sizeof(VertexType) * m_vertexCount));

	// �C���f�b�N�X�z����쐬
	unsigned long* indices;
	indices = new unsigned long[m_indexCount];
	for (unsigned int i = 0; i < m_indexCount; i++)
	{
		indices[i] = i;
	}

	// ���_�o�b�t�@�̐ݒ�
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC; // ���I���_�o�b�t�@
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; // CPU�A�N�Z�X����
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// ���_�o�b�t�@�쐬
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if FAILED(result) return false;

	// �C���f�b�N�X�o�b�t�@�̐ݒ�
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

	// �C���f�b�N�X�o�b�t�@���쐬
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if FAILED(result) return false;

	// �J��
	SafeDeleteArr(vertices);
	SafeDeleteArr(indices);

	return true;
}

bool ImageUI::UpdateBuffers(ID3D11DeviceContext* deviceContext, int posX, int posY)
{
	HRESULT result;

	// �`��ʒu���ύX����Ă��Ȃ��Ȃ�A�o�b�t�@�̍X�V�͍s��Ȃ�
	if (posX == m_previousPos.x && posY == m_previousPos.y) return true;

	// ���W�X�V
	m_previousPos.x = posX;
	m_previousPos.y = posY;

	// �摜�̈ʒu�����v�Z (���W��͍��� + �摜�T�C�Y)
	float top = (float)(m_screenSize.y / 2 - posY);
	float bottom = (float)(top - m_imageSize.y);
	float left = (float)((m_screenSize.x / 2)*-1 + posX);
	float right = (float)(left + m_imageSize.x);

	// �X�V�p�̒��_�z����쐬
	VertexType* vertices;
	vertices = new VertexType[m_vertexCount];

	// �|���쐬
	vertices[0].pos = XMFLOAT3(left, top, 0.0f); // ����
	vertices[0].tex = XMFLOAT2(0.0f, 0.0f);
	vertices[1].pos = XMFLOAT3(right, bottom, 0.0f); // �E��
	vertices[1].tex = XMFLOAT2(1.0f, 1.0f);
	vertices[2].pos = XMFLOAT3(left, bottom, 0.0f); // ����
	vertices[2].tex = XMFLOAT2(0.0f, 1.0f);

	vertices[3].pos = XMFLOAT3(left, top, 0.0f); // ����
	vertices[3].tex = XMFLOAT2(0.0f, 0.0f);
	vertices[4].pos = XMFLOAT3(right, top, 0.0f); // �E��
	vertices[4].tex = XMFLOAT2(1.0f, 0.0f);
	vertices[5].pos = XMFLOAT3(right, bottom, 0.0f); // �E��
	vertices[5].tex = XMFLOAT2(1.0f, 1.0f);

	// ���o�b�t�@��������
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	result = deviceContext->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if FAILED(result) return false;

	// ���_�����R�s�[
	VertexType* verticesPtr;
	verticesPtr = (VertexType*)mappedResource.pData;
	memcpy(verticesPtr, (void*)vertices, (sizeof(VertexType) * m_vertexCount));

	// ���o�b�t�@�������݉���
	deviceContext->Unmap(m_vertexBuffer, 0);

	// �J��
	SafeDeleteArr(vertices);

	return true;
}

void ImageUI::RenderBuffers(ID3D11DeviceContext* deviceContext)
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

bool ImageUI::LoadTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const char* texFilename)
{
	bool result;

	m_texture = new Texture;
	result = m_texture->Init(device, deviceContext, texFilename);
	if (!result) return false;

	return true;
}