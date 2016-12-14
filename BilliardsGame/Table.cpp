#include "Table.h"
#include "DXUtil.h"

#include "DX11Manager.h"
#include "ShaderManager.h"
#include "Model.h"
#include "Light.h"
#include "ObjMesh.h"

const float POCKETS_RADIUS = 3.5f;
const XMFLOAT3 POCKETS_SCALE = { POCKETS_RADIUS, 1, POCKETS_RADIUS }; // �|�P�b�g�X�P�[��
// �S�Ẵ|�P�b�g�̈ʒu���ǂꂾ���O���Ɉړ������邩
const float POCKETS_OFFSET = -POCKETS_RADIUS;

Table::Table()
{
	m_tableModel = nullptr;
	m_pocketModel = nullptr;
}


Table::~Table()
{
	SafeDelete(m_tableModel);
	SafeDelete(m_pocketModel);
}

bool Table::Init(const DX11Manager* dx3D, float tableWidth, float tableHeight)
{
	bool result;

	// ���ݒ�
	m_tableWidth = tableWidth;
	m_tableHeight = tableHeight;

	// �|���S�����ǂݍ���
	ObjMesh* objMesh = new ObjMesh;
	result = objMesh->LoadOBJFile("data/square.obj");
	if (!result) return false;

	// �����f��������
	m_tableModel = new Model;
	result = m_tableModel->Init(dx3D->GetDevice(), dx3D->GetDeviceContext(), objMesh, "data/table.tga");
	if (!result) return false;

	m_tableModel->UpdateWorldMatrix(XMFLOAT3(0, 0, 0), XMFLOAT3(0, 0, 0), XMFLOAT3(tableWidth / 2, 1, tableHeight / 2));

	// �|�P�b�g���f��������
	m_pocketModel = new Model;
	result = m_pocketModel->Init(dx3D->GetDevice(), dx3D->GetDeviceContext(), objMesh, "data/pocket.tga");
	if (!result) return false;

	// �|���S�����j��
	SafeDelete(objMesh);

	// �|�P�b�g�ʒu�ݒ�
	m_pocketRadius = POCKETS_RADIUS;
	float offsetX = m_tableWidth / 2 + POCKETS_OFFSET;
	float offsetY = 0.1f;
	float offsetZ = m_tableHeight / 2 + POCKETS_OFFSET;
	m_pocketPositions[0] = XMFLOAT3(-offsetX, offsetY, offsetZ); // ����
	m_pocketPositions[1] = XMFLOAT3(0, offsetY, offsetZ); // ��
	m_pocketPositions[2] = XMFLOAT3(offsetX, offsetY, offsetZ); // �E��
	m_pocketPositions[3] = XMFLOAT3(-offsetX, offsetY, -offsetZ); // ����
	m_pocketPositions[4] = XMFLOAT3(0, offsetY, -offsetZ); // ��
	m_pocketPositions[5] = XMFLOAT3(offsetX, offsetY, -offsetZ); // �E��

	return true;
}

bool Table::Render(const DX11Manager* dx3D, const ShaderManager* shaderManager,
	const XMFLOAT4X4& view, const XMFLOAT4X4& projection, const Light* light)
{
	bool result;

	XMFLOAT4X4 worldMatrix;

	// ���`��
	m_tableModel->Render(dx3D->GetDeviceContext());
	m_tableModel->GetWorldMatrix(&worldMatrix);

	result = shaderManager->RenderLightShader(dx3D->GetDeviceContext(), m_tableModel->GetIndexCount(),
		worldMatrix, view, projection, m_tableModel->GetTexture(),
		light->GetDirection(), light->GetDiffuseColor());
	if (!result) return false;

	// �|�P�b�g�`��
	m_pocketModel->Render(dx3D->GetDeviceContext());
	for (int i = 0; i < 6; i++)
	{
		m_pocketModel->UpdateWorldMatrix(m_pocketPositions[i], XMFLOAT3(0,0,0), POCKETS_SCALE);
		m_pocketModel->GetWorldMatrix(&worldMatrix);

		result = shaderManager->RenderLightShader(dx3D->GetDeviceContext(), m_pocketModel->GetIndexCount(),
			worldMatrix, view, projection, m_pocketModel->GetTexture(),
			light->GetDirection(), light->GetDiffuseColor());
		if (!result) return false;
	}

	return true;
}