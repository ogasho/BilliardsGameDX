/* カメラクラス */
// ワールド上に配置され、ビュー行列を司る

#pragma once

#include <DirectXMath.h>
using namespace DirectX;

class Camera
{
public:
	Camera();
	~Camera();

	void Render();

	void SetPosition(float x, float y, float z);
	void SetRotation(float x, float y, float z);

	XMFLOAT3 GetPosition()const{ return m_pos; }
	XMFLOAT3 GetRotation()const{ return m_rot; }

	void GetViewMatrix(XMFLOAT4X4 *viewMatrix)const{ *viewMatrix = m_viewMatrix; }

private:
	XMFLOAT3 m_pos;
	XMFLOAT3 m_rot;
	XMFLOAT4X4 m_viewMatrix;
};

