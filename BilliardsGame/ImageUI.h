/* イメージUIクラス */
// 画面上に表示する画像の描画管理クラス

#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;

class Texture;

class ImageUI
{
private:
	struct VertexType
	{
		XMFLOAT3 pos;
		XMFLOAT2 tex;
	};

public:
	ImageUI();
	~ImageUI();

	bool Init(ID3D11Device* device, ID3D11DeviceContext* deviceContext, XMINT2 screenSize,
		int imageWidth, int imageHeight, const char* texFilename);
	bool Render(ID3D11DeviceContext* deviceContext, int posX, int posY);

	int GetIndexCount()const{ return m_indexCount; }
	XMINT2 GetImageSize()const{ return m_imageSize; }
	ID3D11ShaderResourceView* GetTexture();

private:
	bool InitBuffers(ID3D11Device* device);
	bool UpdateBuffers(ID3D11DeviceContext* deviceContext, int posX, int posY);
	void RenderBuffers(ID3D11DeviceContext* deviceContext);

	bool LoadTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const char* texFilename);

	ID3D11Buffer *m_vertexBuffer;
	ID3D11Buffer *m_indexBuffer;
	unsigned int m_vertexCount;
	unsigned int m_indexCount;
	Texture* m_texture;

	XMINT2 m_screenSize;
	XMINT2 m_imageSize;
	XMINT2 m_previousPos;
};

