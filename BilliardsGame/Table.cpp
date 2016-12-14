#include "Table.h"
#include "DXUtil.h"

#include "DX11Manager.h"
#include "ShaderManager.h"
#include "Model.h"
#include "Light.h"
#include "ObjMesh.h"

const float POCKETS_RADIUS = 3.5f;
const XMFLOAT3 POCKETS_SCALE = { POCKETS_RADIUS, 1, POCKETS_RADIUS }; // ポケットスケール
// 全てのポケットの位置をどれだけ外側に移動させるか
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

	// 幅設定
	m_tableWidth = tableWidth;
	m_tableHeight = tableHeight;

	// ポリゴン情報読み込み
	ObjMesh* objMesh = new ObjMesh;
	result = objMesh->LoadOBJFile("data/square.obj");
	if (!result) return false;

	// 床モデル初期化
	m_tableModel = new Model;
	result = m_tableModel->Init(dx3D->GetDevice(), dx3D->GetDeviceContext(), objMesh, "data/table.tga");
	if (!result) return false;

	m_tableModel->UpdateWorldMatrix(XMFLOAT3(0, 0, 0), XMFLOAT3(0, 0, 0), XMFLOAT3(tableWidth / 2, 1, tableHeight / 2));

	// ポケットモデル初期化
	m_pocketModel = new Model;
	result = m_pocketModel->Init(dx3D->GetDevice(), dx3D->GetDeviceContext(), objMesh, "data/pocket.tga");
	if (!result) return false;

	// ポリゴン情報破棄
	SafeDelete(objMesh);

	// ポケット位置設定
	m_pocketRadius = POCKETS_RADIUS;
	float offsetX = m_tableWidth / 2 + POCKETS_OFFSET;
	float offsetY = 0.1f;
	float offsetZ = m_tableHeight / 2 + POCKETS_OFFSET;
	m_pocketPositions[0] = XMFLOAT3(-offsetX, offsetY, offsetZ); // 左上
	m_pocketPositions[1] = XMFLOAT3(0, offsetY, offsetZ); // 上
	m_pocketPositions[2] = XMFLOAT3(offsetX, offsetY, offsetZ); // 右上
	m_pocketPositions[3] = XMFLOAT3(-offsetX, offsetY, -offsetZ); // 左下
	m_pocketPositions[4] = XMFLOAT3(0, offsetY, -offsetZ); // 下
	m_pocketPositions[5] = XMFLOAT3(offsetX, offsetY, -offsetZ); // 右下

	return true;
}

bool Table::Render(const DX11Manager* dx3D, const ShaderManager* shaderManager,
	const XMFLOAT4X4& view, const XMFLOAT4X4& projection, const Light* light)
{
	bool result;

	XMFLOAT4X4 worldMatrix;

	// 床描画
	m_tableModel->Render(dx3D->GetDeviceContext());
	m_tableModel->GetWorldMatrix(&worldMatrix);

	result = shaderManager->RenderLightShader(dx3D->GetDeviceContext(), m_tableModel->GetIndexCount(),
		worldMatrix, view, projection, m_tableModel->GetTexture(),
		light->GetDirection(), light->GetDiffuseColor());
	if (!result) return false;

	// ポケット描画
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