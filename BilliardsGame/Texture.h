/* テクスチャクラス */
// テクスチャデータ(.tga)の読み込み、管理を行う

#pragma once

#include <d3d11.h>

class Texture
{
private:
	// targaファイルのデータ構造体
	struct TargaHeader
	{
		unsigned char data1[12];
		unsigned short height;
		unsigned short width;
		unsigned char bpp;
		unsigned char data2;
	};

public:
	Texture();
	~Texture();

	bool Init(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const char* filename, bool flip);
	ID3D11ShaderResourceView* GetTexture()const{ return m_textureView; }

private:
	bool LoadTarga(const char* filename, int* width, int* height, bool flip);

	unsigned char* m_targaData;
	ID3D11Texture2D* m_texture;
	ID3D11ShaderResourceView* m_textureView;
};