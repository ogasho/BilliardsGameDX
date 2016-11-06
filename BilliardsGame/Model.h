/* ���f���N���X */
// ���f���`��ׂ̈̃o�b�t�@���Ǘ�����

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
		const char* modelFilename, const char* texFilename); // ���f���f�[�^��ǂݍ���
	bool Init(ID3D11Device* device, ID3D11DeviceContext* deviceContext,
		ObjMesh* loadedObj, const char* texFilename); // ���ɂ��郂�f���f�[�^�𗬗p����

	void Render(ID3D11DeviceContext* deviceContext);

	int GetIndexCount(){ return m_indexCount; }
	ID3D11ShaderResourceView* GetTexture();
	void GetWorldMatrix(XMFLOAT4X4 *worldMatrix, XMFLOAT3 position, XMFLOAT3 rotate, XMFLOAT3 scale);
	void GetWorldMatrix(XMFLOAT4X4 *worldMatrix, XMFLOAT3 position, XMFLOAT3 scale); // �p�x�ۑ��p

	void AddRotation(const XMFLOAT3* rotate); // �ۊǂ��Ă����]�s��ɉ�]��ǉ�����

private:
	bool InitBuffers(ID3D11Device* device, unsigned int vertexCount, unsigned int indexCount, ObjVertex* objVtx);
	bool LoadTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const char* filename);

	XMFLOAT4X4 m_rotateMatrix;
	ID3D11Buffer *m_vertexBuffer;
	ID3D11Buffer *m_indexBuffer;
	Texture* m_texture;
	unsigned int m_indexCount;
};

