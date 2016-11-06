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

	void SetMoveVec(const XMFLOAT3* moveVec){ m_move = *moveVec; }
	void AddPosition(const XMFLOAT3* addPos);
	void AddPosition(float x, float y, float z);

	void GetWorldMatrix(XMFLOAT4X4 *worldMatrix);
	Model* GetModelPtr();
	float GetRadius(){ return m_radius; }
	void GetPosition(XMFLOAT3* pos){ *pos = m_position; }
	void GetMoveVec(XMFLOAT3* move){ *move = m_move; }
	bool IsStopBallMove(){ return m_isStop; }

private:
	void MulMoveVec(float t);

	bool m_isStop;
	Model* m_model;
	float m_radius;
	XMFLOAT3 m_move;
	XMFLOAT3 m_position;
	XMFLOAT3 m_scale;
};

