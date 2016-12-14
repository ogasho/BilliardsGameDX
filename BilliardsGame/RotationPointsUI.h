/* ローテーションポイントUIクラス */
// ローテーション向けのポイントUI描画を管理するクラス

#pragma once

#include <DirectXMath.h>
using namespace DirectX;

class ImageUI;
class DX11Manager;
class ShaderManager;
class Texture;

class RotationPointsUI
{
public:
	RotationPointsUI();
	~RotationPointsUI();

	bool Init(const DX11Manager* dx3D);
	bool RenderPoints(const int* playerPoints,
		const DX11Manager* dx3D, const ShaderManager* shaderManager,
		const XMFLOAT4X4& worldMatrix, const XMFLOAT4X4& viewMatrix, const XMFLOAT4X4& orthoMatrix);

private:
	int GetDigit(int point, int digitNum);

	ImageUI* m_pointNumberImage;
	Texture** m_pointNumberTextures;
	ImageUI** m_playerTextImages;
};

