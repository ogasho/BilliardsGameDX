/* ビリヤードボールクラス */
// ボールの物理処理、モデル管理を行う

#pragma once

class DX11Manager;
class Model;
class ObjMesh;

#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;

class Ball
{
public:
	Ball();
	~Ball();

	bool Init(DX11Manager* dx3D, ObjMesh* objMesh, XMFLOAT3 position, int ballNumber);
	bool Frame();
	void Render(ID3D11DeviceContext* deviceContext);

	void GetWorldMatrix(XMFLOAT4X4 *worldMatrix);
	Model* GetModelPtr();

private:
	Model* m_model;

	float count;
	float m_radius;
	XMFLOAT3 m_position;
	XMFLOAT3 m_scale;
};

