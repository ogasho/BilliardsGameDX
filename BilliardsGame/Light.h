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

	void SetDiffuseColor(float, float, float, float);
	void SetDirection(float, float, float);

	XMFLOAT4 GetDiffuseColor(){ return m_diffuseColor; }
	XMFLOAT3 GetDirection(){ return m_direction; }

private:
	XMFLOAT4 m_diffuseColor;
	XMFLOAT3 m_direction;
};

