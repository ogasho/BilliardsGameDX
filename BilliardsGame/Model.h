/* モデルクラス */
// モデル描画の為のバッファを管理する

#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;

class Texture;
class ObjMesh;
struct ObjVertex;

class Model
{
private:
	struct VertexType
	{
		XMFLOAT3 pos;
		XMFLOAT2 tex;
		XMFLOAT3 nor;
	};

public:
	Model();
	~Model();

	bool Init(ID3D11Device* device, ID3D11DeviceContext* deviceContext,
		const char* modelFilename, const char* texFilename);
	bool Init(ID3D11Device* device, ID3D11DeviceContext* deviceContext,
		const ObjMesh* objMesh, const char* texFilename);

	void Render(ID3D11DeviceContext* deviceContext);
	void UpdateWorldMatrix(const XMFLOAT3& position, const XMFLOAT3& rotate, const XMFLOAT3& scale);

	int GetIndexCount()const{ return m_indexCount; }
	ID3D11ShaderResourceView* GetTexture();
	void GetWorldMatrix(XMFLOAT4X4* matrix)const{ *matrix = m_worldMatrix; }

private:
	bool InitBuffers(ID3D11Device* device, unsigned int vertexCount, unsigned int indexCount, ObjVertex* objVtx);
	bool LoadTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const char* filename);

	ID3D11Buffer *m_vertexBuffer;
	ID3D11Buffer *m_indexBuffer;
	XMFLOAT4X4 m_worldMatrix;
	Texture* m_texture;
	unsigned int m_indexCount;
};

