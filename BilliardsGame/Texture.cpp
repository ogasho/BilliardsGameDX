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
	// 開放
	SafeDeleteArr(m_targaData);
	SafeRelease(m_texture);
	SafeRelease(m_textureView);
}


bool Texture::Init(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const char* filename)
{
	bool result;
	HRESULT hResult;
	int width, height;

	// targaファイルをロード
	result = LoadTarga(filename, &width, &height);
	if (!result) return false;

	// テクスチャ設定(32bit/RGBA)
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

	// テクスチャ作成
	hResult = device->CreateTexture2D(&texDesc, nullptr, &m_texture);
	if FAILED(hResult) return false;

	// 行ピッチ
	unsigned int rowPitch = (width * 4)*sizeof(unsigned char);

	// テクスチャ(構造体)に画像データを代入
	deviceContext->UpdateSubresource(m_texture, 0, nullptr, m_targaData, rowPitch, 0);

	// シェーダリソースビュー設定
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = texDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1;

	// シェーダリソースビュー作成
	hResult = device->CreateShaderResourceView(m_texture, &srvDesc, &m_textureView);
	if FAILED(hResult) return false;

	// ミップマップ作成
	deviceContext->GenerateMips(m_textureView);

	// ロードした画像データを開放
	SafeDeleteArr(m_targaData);

	return true;
}

bool Texture::LoadTarga(const char* filename, int* width, int* height)
{
	FILE* filePtr;
	unsigned int count;

	// ファイルオープン
	if (fopen_s(&filePtr, filename, "rb") != 0) return false;

	// ヘッダー読み込み
	TargaHeader targaHeader;
	count = (unsigned int)fread(&targaHeader, sizeof(TargaHeader), 1, filePtr);
	if (count != 1) return false;

	// 画像情報を記録
	*width = (int)targaHeader.width;
	*height = (int)targaHeader.height;
	int bpp = (int)targaHeader.bpp;

	// 画像データは32bitか32bitであるか
	if (bpp != 32 && bpp != 24) return false;

	int bppByte = bpp / 8;

	// サイズを計算
	int imageSize = (*width) * (*height) * bppByte;

	// 画像データ用にメモリを割り当てる
	unsigned char* targaImage = new unsigned char[imageSize];
	if (!targaImage) return false;

	// 画像データ読み込み
	count = (unsigned int)fread(targaImage, 1, imageSize, filePtr);
	if (count != imageSize) return false;

	// ファイルクローズ
	if (fclose(filePtr) != 0) return false;

	// データ保存先のメモリを割り当てる
	m_targaData = new unsigned char[imageSize];
	if (!m_targaData) return false;

	// 画像データをコピー
	int index = 0;
	int k = 0; // 上から下へ色データを読み込んでいく
	// 下から上へ読み込むならこれ: k = imageSize - ((*width) * bppByte);
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

		//k -= (*width) * (bppByte * 2); // 下から上へ読み込むならこのコメント外す
	}

	// 開放
	SafeDeleteArr(targaImage);

	return true;
}