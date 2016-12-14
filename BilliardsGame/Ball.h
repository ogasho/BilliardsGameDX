/* ビリヤードボールクラス */
// ボールの物理処理、モデル管理を行う

#pragma once

class DX11Manager;
class Model;
class ObjMesh;
class ShaderManager;
class Light;

#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;

class Ball
{
public:
	Ball();
	~Ball();

	bool Init(const DX11Manager* dx3D, XMFLOAT3 position, int ballNumber, const ObjMesh* ballMesh);
	bool Render(const DX11Manager* dx3D, const ShaderManager* shaderManager,
		const XMFLOAT4X4& view, const XMFLOAT4X4& projection, const Light* light);
	bool UpdateMove();
	bool UpdatePocket();
	
	void Restore(const XMFLOAT3& pos);
	void SetMoveVec(const XMFLOAT3& moveVec){ m_move = moveVec; }
	void SetPosition(const XMFLOAT3& pos){ m_position = pos; }
	void AddPosition(const XMFLOAT3& addPos);
	void AddPosition(float x, float y, float z);
	void Pockets(){ m_isPocket = true; }

	void GetCurrentWorldMatrix(XMFLOAT4X4 *worldMatrix);
	ID3D11ShaderResourceView* GetTexture();
	int GetIndexCount()const;

	float GetRadius()const{ return m_radius; }
	void GetPosition(XMFLOAT3* pos)const{ *pos = m_position; }
	XMFLOAT3 GetPosition()const{ return m_position; }
	void GetMoveVec(XMFLOAT3* move)const{ *move = m_move; }
	XMFLOAT3 GetMoveVec()const{ return m_move; }
	bool IsStopBallMove()const{ return m_isStop; }
	bool IsPockets()const{ return m_isPocket; }

private:
	void MulMoveVec(float t);
	void AddRotation(const XMFLOAT3& rotate);

	bool m_isPocket;
	bool m_isStop;
	float m_radius;
	XMFLOAT3 m_move;
	XMFLOAT3 m_position;
	XMFLOAT3 m_scale;

	// 回転行列を保存した状態でワールド行列を作るので、modelの持つワールドは使用しない
	XMFLOAT4X4 m_rotateMatrix;
	Model* m_model;
};

