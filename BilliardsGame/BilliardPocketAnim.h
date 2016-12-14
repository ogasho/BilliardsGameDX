/* ポケットアニメーションクラス */
// ポケットした際に描画されるUIアニメを管理する

#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;

class DX11Manager;
class ImageUI;
class Texture;
class ShaderManager;

class BilliardPocketAnim
{
public:
	BilliardPocketAnim();
	~BilliardPocketAnim();

	bool Init(const DX11Manager* dx3D, int maxBallNum);
	void UpdateAnim();
	bool Render(const DX11Manager* dx3D, const ShaderManager* shaderManager,
		const XMFLOAT4X4& worldMatrix, const XMFLOAT4X4& viewMatrix, const XMFLOAT4X4& orthoMatrix);
	void Clear();

	void StartPocketAnim(int ballNum);

private:
	void InitRenderPos(const DX11Manager* dx3D);

	ImageUI* m_pocketImage;
	ImageUI* m_ballImage;
	ImageUI* m_FoulImage;
	Texture** m_ballTextures;

	bool m_isFoul;
	bool m_isAnim;
	int m_animBallNum;
	int m_maxBallNum;
	float m_imagePosX;
};

