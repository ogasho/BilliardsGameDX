/* �J�����N���X */
// ���[���h��ɔz�u����A�r���[�s����i��

#pragma once

#include <DirectXMath.h>
using namespace DirectX;

class Camera
{
public:
	Camera();
	~Camera();

	void SetPosition(float, float, float);
	void SetRotation(float, float, float);

	XMFLOAT3 GetPosition(){ return m_pos; }
	XMFLOAT3 GetRotation(){ return m_rot; }

	void Render();
	void GetViewMatrix(XMFLOAT4X4 *viewMatrix){ *viewMatrix = m_viewMatrix; }

private:
	XMFLOAT3 m_pos;
	XMFLOAT3 m_rot;
	XMFLOAT4X4 m_viewMatrix;
};

