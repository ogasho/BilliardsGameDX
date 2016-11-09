#include "Texture.h"
#include "DXUtil.h"

#include <stdio.h>

Texture::Texture()
{
	m_targaData = nullptr;
	m_texture = nullptr;
	m_textureView = nullptr;
}


Texture::~Texture()
{
	// �J��
	SafeDeleteArr(m_targaData);
	SafeRelease(m_texture);
	SafeRelease(m_textureView);
}


bool Texture::Init(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const char* filename)
{
	bool result;
	HRESULT hResult;
	int width, height;

	// targa�t�@�C�������[�h
	result = LoadTarga(filename, &width, &height);
	if (!result) return false;

	// �e�N�X�`���ݒ�(32bit/RGBA)
	D3D11_TEXTURE2D_DESC texDesc;
	texDesc.Width = width;
	texDesc.Height = height;
	texDesc.MipLevels = 0;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

	// �e�N�X�`���쐬
	hResult = device->CreateTexture2D(&texDesc, nullptr, &m_texture);
	if FAILED(hResult) return false;

	// �s�s�b�`
	unsigned int rowPitch = (width * 4)*sizeof(unsigned char);

	// �e�N�X�`��(�\����)�ɉ摜�f�[�^����
	deviceContext->UpdateSubresource(m_texture, 0, nullptr, m_targaData, rowPitch, 0);

	// �V�F�[�_���\�[�X�r���[�ݒ�
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = texDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1;

	// �V�F�[�_���\�[�X�r���[�쐬
	hResult = device->CreateShaderResourceView(m_texture, &srvDesc, &m_textureView);
	if FAILED(hResult) return false;

	// �~�b�v�}�b�v�쐬
	deviceContext->GenerateMips(m_textureView);

	// ���[�h�����摜�f�[�^���J��
	SafeDeleteArr(m_targaData);

	return true;
}

bool Texture::LoadTarga(const char* filename, int* width, int* height)
{
	FILE* filePtr;
	unsigned int count;

	// �t�@�C���I�[�v��
	if (fopen_s(&filePtr, filename, "rb") != 0) return false;

	// �w�b�_�[�ǂݍ���
	TargaHeader targaHeader;
	count = (unsigned int)fread(&targaHeader, sizeof(TargaHeader), 1, filePtr);
	if (count != 1) return false;

	// �摜�����L�^
	*width = (int)targaHeader.width;
	*height = (int)targaHeader.height;
	int bpp = (int)targaHeader.bpp;

	// �摜�f�[�^��32bit��32bit�ł��邩
	if (bpp != 32 && bpp != 24) return false;

	int bppByte = bpp / 8;

	// �T�C�Y���v�Z
	int imageSize = (*width) * (*height) * bppByte;

	// �摜�f�[�^�p�Ƀ����������蓖�Ă�
	unsigned char* targaImage = new unsigned char[imageSize];
	if (!targaImage) return false;

	// �摜�f�[�^�ǂݍ���
	count = (unsigned int)fread(targaImage, 1, imageSize, filePtr);
	if (count != imageSize) return false;

	// �t�@�C���N���[�Y
	if (fclose(filePtr) != 0) return false;

	// �f�[�^�ۑ���̃����������蓖�Ă�
	m_targaData = new unsigned char[imageSize];
	if (!m_targaData) return false;

	// �摜�f�[�^���R�s�[
	int index = 0;
	int k = 0; // �ォ�牺�֐F�f�[�^��ǂݍ���ł���
	// �������֓ǂݍ��ނȂ炱��: k = imageSize - ((*width) * bppByte);
	for (int i = 0; i < (*height); i++)
	{
		for (int j = 0; j < (*width); j++)
		{
			m_targaData[index + 0] = targaImage[k + 2]; // R
			m_targaData[index + 1] = targaImage[k + 1]; // G
			m_targaData[index + 2] = targaImage[k + 0]; // B
			if (bpp == 32) // 32bit
				m_targaData[index + 3] = targaImage[k + 3]; // A
			else // 24bit
				m_targaData[index + 3] = (unsigned char)255; // A

			k += bppByte;
			index += 4;
		}

		//k -= (*width) * (bppByte * 2); // �������֓ǂݍ��ނȂ炱�̃R�����g�O��
	}

	// �J��
	SafeDeleteArr(targaImage);

	return true;
}