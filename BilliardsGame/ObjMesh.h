/* Obj���b�V���N���X */
// .obj�t�@�C���̓ǂݍ��݁A�Ǘ����s��

#pragma region Disable Waring C4996
//
// Disable Warning C4996
//
#ifndef _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES
#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES 1
#endif
#ifndef _CRT_SECURE_CPP_OVERLOAD_SECURE_NAMES
#define _CRT_SECURE_CPP_OVERLOAD_SECURE_NAMES 1
#endif
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS 1
#endif
#pragma endregion

#include <Windows.h>
#include <DirectXMath.h>
using namespace DirectX;

#pragma once
class ObjMesh
{
public:
	// Obj�f�[�^�\����
	struct ObjVertex
	{
		XMFLOAT3 pos;
		XMFLOAT2 tex;
		XMFLOAT3 nor;
	};

	ObjMesh();
	~ObjMesh();

	bool LoadMTLFile(const char*);
	bool LoadOBJFile(const char*);

	ObjVertex* GetVertices(){ return m_vertices; }
	unsigned int GetNumVertices(){ return m_numVertices; }

private:
	bool LoadFile(const char*);

	ObjVertex* m_vertices;
	unsigned int m_numVertices;
};

