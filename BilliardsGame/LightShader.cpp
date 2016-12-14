#include "LightShader.h"
#include "DXUtil.h"

LightShader::LightShader()
{
	m_vertexShader = nullptr;
	m_pixelShader = nullptr;
	m_layout = nullptr;
	m_matrixBuffer = nullptr;
	m_lightBuffer = nullptr;
	m_sampleState = nullptr;
}


LightShader::~LightShader()
{
	// �V�F�[�_�֘A�̊J��
	SafeRelease(m_vertexShader);
	SafeRelease(m_pixelShader);
	SafeRelease(m_layout);
	SafeRelease(m_matrixBuffer);
	SafeRelease(m_lightBuffer);
	SafeRelease(m_sampleState);
}

bool LightShader::Init(ID3D11Device* device, HWND hWnd)
{
	bool result;

	// �V�F�[�_�t�@�C���̃p�X���w��A������
	result = InitShader(device, hWnd, "data/LightShader_v.cso", "data/LightShader_p.cso");
	if (!result) return false;
	
	return true;
}

bool LightShader::Render(ID3D11DeviceContext* deviceContext, int indexCount,
	const XMFLOAT4X4& worldMatrix, const XMFLOAT4X4& viewMatrix, const XMFLOAT4X4& projectionMatrix,
	ID3D11ShaderResourceView* tex,
	const XMFLOAT3& lightDirection, const XMFLOAT4& diffuseColor, const XMFLOAT4& ambientColor) const
{
	bool result;
	
	// �V�F�[�_�̃p�����[�^��ݒ�
	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, 
		tex, lightDirection, diffuseColor, ambientColor);
	if (!result) return false;

	// �`��
	RenderShader(deviceContext, indexCount);

	return true;
}

bool LightShader::InitShader(ID3D11Device* device, HWND hWnd, char* vsFileName, char* psFileName)
{
	HRESULT result;
	
	// �o�C�i��(�v���R���p�C���V�F�[�_)��ǂݍ���Ŋe�V�F�[�_���쐬����
	// ���_�V�F�[�_�쐬
	std::vector<char> vertexBinaryData;
	result = LoadShaderBinary(vsFileName, &vertexBinaryData);
	if FAILED(result) return false;

	result = device->CreateVertexShader(&vertexBinaryData[0],
		vertexBinaryData.size(), nullptr, &m_vertexShader);
	if FAILED(result) return false;

	// �s�N�Z���V�F�[�_�쐬
	std::vector<char> pixelBinaryData;
	result = LoadShaderBinary(psFileName, &pixelBinaryData);
	if FAILED(result) return false;

	result = device->CreatePixelShader(&pixelBinaryData[0],
		pixelBinaryData.size(), nullptr, &m_pixelShader);
	if FAILED(result) return false;
	
	// ���_�f�[�^�̃��C�A�E�g���쐬
	D3D11_INPUT_ELEMENT_DESC polygonLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA , 0},
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	unsigned int numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	result = device->CreateInputLayout(polygonLayout, numElements,
		&vertexBinaryData[0], vertexBinaryData.size(), &m_layout);
	if FAILED(result) return false;

	// �o�C�i���m�ۂɎg����vector�𖾎��I�ɊJ��(�ꉞ)
	std::vector<char>().swap(vertexBinaryData);
	std::vector<char>().swap(pixelBinaryData);

	// �e�N�X�`���T���v���[�ݒ�
	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;  // �ٕ����t�B���^
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

	// (�萔)�o�b�t�@�ݒ�
	D3D11_BUFFER_DESC matrixBufferDesc;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	result = device->CreateBuffer(&matrixBufferDesc, nullptr, &m_matrixBuffer);
	if FAILED(result) return false;

	// (�萔)���C�g�o�b�t�@�ݒ�
	D3D11_BUFFER_DESC lightBufferDesc;
	lightBufferDesc.ByteWidth = sizeof(LightBufferType);
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;

	result = device->CreateBuffer(&lightBufferDesc, nullptr, &m_lightBuffer);
	if FAILED(result) return false;
	
	return true;
}

HRESULT LightShader::LoadShaderBinary(char* filename, std::vector<char>* binaryData)
{
	FILE* filePtr;
	long size;

	// �t�@�C���I�[�v��
	if (fopen_s(&filePtr, filename, "rb") != 0) return -1;
	
	fseek(filePtr, 0, SEEK_END);
	size = ftell(filePtr);
	fseek(filePtr, 0, SEEK_SET);

	binaryData->clear();
	binaryData->resize(size);

	fread(&(*binaryData)[0], sizeof(char), size, filePtr);
	// �t�@�C���N���[�Y
	fclose(filePtr);
	
	return S_OK;
}

bool LightShader::SetShaderParameters(ID3D11DeviceContext* deviceContext,
	const XMFLOAT4X4& worldMatrix, const XMFLOAT4X4& viewMatrix, const XMFLOAT4X4& projectionMatrix,
	ID3D11ShaderResourceView* tex,
	const XMFLOAT3& lightDirection, const XMFLOAT4& diffuseColor, const XMFLOAT4& ambientColor) const
{
	HRESULT result;

	// ���萔�o�b�t�@��������
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if FAILED(result) return false;

	// ���_�����R�s�[
	MatrixBufferType* matrixPtr;
	matrixPtr = (MatrixBufferType*)mappedResource.pData;
	
	matrixPtr->world = worldMatrix;
	matrixPtr->view = viewMatrix;
	matrixPtr->projection = projectionMatrix;

	// ���萔�o�b�t�@�������݉���
	deviceContext->Unmap(m_matrixBuffer, 0);

	// �V�F�[�_�֒l���󂯓n��
	deviceContext->VSSetConstantBuffers(0, 1, &m_matrixBuffer);
	deviceContext->PSSetShaderResources(0, 1, &tex);

	// �����C�g�o�b�t�@��������
	result = deviceContext->Map(m_lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if FAILED(result) return false;

	// ���_�����R�s�[
	LightBufferType* lightPtr;
	lightPtr = (LightBufferType*)mappedResource.pData;
	lightPtr->ambientColor = ambientColor;
	lightPtr->diffuseColor = diffuseColor;
	lightPtr->lightDirection = lightDirection;
	lightPtr->padding = 0.0f;

	// �����C�g�o�b�t�@�������݉���
	deviceContext->Unmap(m_lightBuffer, 0);

	// �V�F�[�_�֒l���󂯓n��
	deviceContext->PSSetConstantBuffers(0, 1, &m_lightBuffer);
	
	return true;
}

void LightShader::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount) const
{
	// ���C�A�E�g�ݒ�
	deviceContext->IASetInputLayout(m_layout);
	
	// �V�F�[�_�ݒ�
	deviceContext->VSSetShader(m_vertexShader, nullptr, 0);
	deviceContext->PSSetShader(m_pixelShader, nullptr, 0);
	deviceContext->PSSetSamplers(0, 1, &m_sampleState);

	// �`��
	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}