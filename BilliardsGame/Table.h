/* ビリヤードテーブルクラス */
// テーブルの描画やコリジョンを管理する

#pragma once

#include <DirectXMath.h>
using namespace DirectX;

class DX11Manager;
class ShaderManager;
class Light;
class Model;

const int POCKETS_NUM = 6; // ポケット数

class Table
{
public:
	Table();
	~Table();

	bool Init(const DX11Manager* dx3D, float tableWidth, float tableHeight);
	bool Render(const DX11Manager* dx3D, const ShaderManager* shaderManager,
		const XMFLOAT4X4& view, const XMFLOAT4X4& projection, const Light* light);
	
	float GetTableWidth()const{ return m_tableWidth; }
	float GetTableHeight()const{ return m_tableHeight; }
	void GetTablePocketsPos(XMFLOAT3* position, int index)const{ *position = m_pocketPositions[index]; }
	inline float GetPocketRadius()const{ return m_pocketRadius; }

private:
	float m_tableWidth;
	float m_tableHeight;
	float m_pocketRadius;

	XMFLOAT3 m_pocketPositions[POCKETS_NUM];
	Model* m_tableModel;
	Model* m_pocketModel;
};

