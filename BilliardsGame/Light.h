/* ライトクラス */
// 光の情報を管理する

#include <DirectXMath.h>
using namespace DirectX;

#pragma once
class Light
{
public:
	Light();
	~Light();

	void SetDiffuseColor(float r, float g, float b, float a);
	void SetDirection(float x, float y, float z);

	XMFLOAT4 GetDiffuseColor()const{ return m_diffuseColor; }
	XMFLOAT3 GetDirection()const{ return m_direction; }

private:
	XMFLOAT4 m_diffuseColor;
	XMFLOAT3 m_direction;
};

