#include "ObjMesh.h"
#include "DXUtil.h"

#include <fstream>
#include <vector>

ObjMesh::ObjMesh()
{
	m_vertices = nullptr;
}


ObjMesh::~ObjMesh()
{
	SafeDeleteArr(m_vertices);
}

bool ObjMesh::LoadMTLFile(const char*)
{
	return true;
}

bool ObjMesh::LoadOBJFile(const char *filename)
{
	std::ifstream file;

	file.open(filename, std::ios::in); // ファイルオープン
	if (file.fail()) return false;

	char buffer[1024] = {};
	std::vector<XMFLOAT3> positions;
	std::vector<XMFLOAT3> normals;
	std::vector<XMFLOAT2> texcoords;
	std::vector<ObjVertex> vertices;
	char input;

	// objファイル解析
	file.get(input);
	while (!file.eof())
	{
		if (input == 'v')
		{
			file.get(input);

			if (input == ' ') // v:頂点座標
			{
				float x, y, z;
				file >> x >> y >> z;
				z *= -1.0f; // 右手系→左手系
				positions.emplace_back(XMFLOAT3(x, y, z));
			}
			else if (input == 't') // vt:テクスチャ座標
			{
				float u, v;
				file >> u >> v;
				v = 1.0f - v; // 右手系→左手系
				texcoords.emplace_back(XMFLOAT2(u, v));
			}
			else if (input == 'n') // vn:法線ベクトル
			{
				float x, y, z;
				file >> x >> y >> z;
				z *= -1.0f; // 右手系→左手系
				normals.emplace_back(XMFLOAT3(x, y, z));
			}
		}
		else if (input == 'f') // f:面情報 (四角ポリゴンは考えない)
		{
			int fv, ft, fn;
			ObjVertex vertex;

			for (int i = 0; i < 3; i++)
			{
				ZeroMemory(&vertex, sizeof(ObjVertex));

				file >> fv;
				file.ignore();
				file >> ft;
				file.ignore();
				file >> fn;

				vertex.pos = positions[fv - 1]; // 頂点座標インデックス
				vertex.tex = texcoords[ft - 1]; // テクスチャ座標インデックス
				vertex.nor = normals[fn - 1]; // 法線ベクトルインデックス

				vertices.emplace_back(vertex);
			}
		}

		while (input != '\n')
		{
			file.get(input);
		}
		file.get(input); // 次の行へ
	}

	file.close(); // ファイルクローズ

	// 頂点情報を保持 (右手系→左手系:トライアングルの描画順を時計回りから反時計回りに)
	m_numVertices = vertices.size();
	m_vertices = new ObjVertex[m_numVertices];
	for (unsigned int i = 0; i < m_numVertices; i += 3)
	{
		m_vertices[i + 0] = vertices[i + 2];
		m_vertices[i + 1] = vertices[i + 1];
		m_vertices[i + 2] = vertices[i + 0];

	}

	// 破棄
	positions.clear();
	normals.clear();
	texcoords.clear();
	vertices.clear();

	return true;
}