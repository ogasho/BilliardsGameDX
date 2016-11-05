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

	file.open(filename, std::ios::in); // �t�@�C���I�[�v��
	if (file.fail()) return false;

	char buffer[1024] = {};
	std::vector<XMFLOAT3> positions;
	std::vector<XMFLOAT3> normals;
	std::vector<XMFLOAT2> texcoords;
	std::vector<ObjVertex> vertices;
	char input;

	// obj�t�@�C�����
	file.get(input);
	while (!file.eof())
	{
		if (input == 'v')
		{
			file.get(input);

			if (input == ' ') // v:���_���W
			{
				float x, y, z;
				file >> x >> y >> z;
				z *= -1.0f; // �E��n������n
				positions.emplace_back(XMFLOAT3(x, y, z));
			}
			else if (input == 't') // vt:�e�N�X�`�����W
			{
				float u, v;
				file >> u >> v;
				v = 1.0f - v; // �E��n������n
				texcoords.emplace_back(XMFLOAT2(u, v));
			}
			else if (input == 'n') // vn:�@���x�N�g��
			{
				float x, y, z;
				file >> x >> y >> z;
				z *= -1.0f; // �E��n������n
				normals.emplace_back(XMFLOAT3(x, y, z));
			}
		}
		else if (input == 'f') // f:�ʏ�� (�l�p�|���S���͍l���Ȃ�)
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

				vertex.pos = positions[fv - 1]; // ���_���W�C���f�b�N�X
				vertex.tex = texcoords[ft - 1]; // �e�N�X�`�����W�C���f�b�N�X
				vertex.nor = normals[fn - 1]; // �@���x�N�g���C���f�b�N�X

				vertices.emplace_back(vertex);
			}
		}

		while (input != '\n')
		{
			file.get(input);
		}
		file.get(input); // ���̍s��
	}

	file.close(); // �t�@�C���N���[�Y

	// ���_����ێ� (�E��n������n:�g���C�A���O���̕`�揇�����v��肩�甽���v����)
	m_numVertices = vertices.size();
	m_vertices = new ObjVertex[m_numVertices];
	for (unsigned int i = 0; i < m_numVertices; i += 3)
	{
		m_vertices[i + 0] = vertices[i + 2];
		m_vertices[i + 1] = vertices[i + 1];
		m_vertices[i + 2] = vertices[i + 0];

	}

	// �j��
	positions.clear();
	normals.clear();
	texcoords.clear();
	vertices.clear();

	return true;
}