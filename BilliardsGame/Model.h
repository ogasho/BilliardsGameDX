/* モデルクラス */
// モデル描画の為のバッファを管理する
// Transform情報を持ち、ワールド行列に変換する

#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;

class Texture;
class ObjMesh;

class Model
{
private:
	struct VertexType
	{
		XMFLOAT3 pos;
		XMFLOAT2 tex;
		XMFLOAT3 nor;
	};

	struct Transform
	{
		XMFLOAT3 position;
		XMFLOAT3 rotate;
		XMFLOAT3 scale;
	};

public:
	Model();
	~Model();

	bool Init(ID3D11Device*, ID3D11DeviceContext*, char*, char*);
	void Render(ID3D11DeviceContext*);

	int GetIndexCount(){ return m_indexCount; }
	ID3D11ShaderResourceView* GetTexture();
	void GetCurrentMatrix(XMFLOAT4X4 *worldMatrix);

	void SetPosition(float, float, float);
	void SetRotation(float, float, float);
	void SetScale(float, float, float);
	void AddPosition(float, float, float);
	void AddRotation(float, float, float);
	void AddScale(float, float, float);

private:
	bool InitBuffers(ID3D11Device*);
	bool LoadModel(char*);
	bool LoadTexture(ID3D11Device*, ID3D11DeviceContext*, char*);

	bool m_isUpdatedTransform;
	XMFLOAT4X4 m_worldMatrix;
	Transform m_transform;
	ID3D11Buffer *m_vertexBuffer;
	ID3D11Buffer *m_indexBuffer;
	int m_vertexCount;
	int m_indexCount;
	Texture* m_texture;
	ObjMesh* m_mesh;
};

