/* Obj���b�V���N���X */
// .obj�t�@�C���̓ǂݍ��݁A�Ǘ����s��

#pragma once

#include <Windows.h>
#include <DirectXMath.h>
using namespace DirectX;

// Obj�f�[�^�\����
struct ObjVertex
{
	XMFLOAT3 pos;
	XMFLOAT2 tex;
	XMFLOAT3 nor;
};

class ObjMesh
{
public:
	ObjMesh();
	~ObjMesh();

	//bool LoadMTLFile(const char*);
	bool LoadOBJFile(const char *filename);

	ObjVertex* GetVertices()const{ return m_vertices; }
	unsigned int GetNumVertices()const{ return m_numVertices; }

private:
	ObjVertex* m_vertices;
	unsigned int m_numVertices;
};

