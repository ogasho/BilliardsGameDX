/* ビリヤードUI */
// ビリヤードで使う汎用的なUI群のクラス

#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;

class DX11Manager;
class Texture;
class ImageUI;
class ShaderManager;

class BilliardUI
{
public:
	BilliardUI();
	~BilliardUI();

	bool Init(DX11Manager* dx3D, int maxBallNum);

	bool RenderBallListUI(const bool* isBallPockets, DX11Manager* dx3D, const ShaderManager* shaderManager,
		const XMFLOAT4X4& worldMatrix, const XMFLOAT4X4& viewMatrix, const XMFLOAT4X4& orthoMatrix);
	bool RenderNextBallUI(int nextBallNum, int currentTurn, DX11Manager* dx3D, const ShaderManager* shaderManager,
		const XMFLOAT4X4& worldMatrix, const XMFLOAT4X4& viewMatrix, const XMFLOAT4X4& orthoMatrix);
	bool RenderExitUI(DX11Manager* dx3D, const ShaderManager* shaderManager,
		const XMFLOAT4X4& worldMatrix, const XMFLOAT4X4& viewMatrix, const XMFLOAT4X4& orthoMatrix);
	bool RenderHelperUI(int helperKind, DX11Manager* dx3D, const ShaderManager* shaderManager,
		const XMFLOAT4X4& worldMatrix, const XMFLOAT4X4& viewMatrix, const XMFLOAT4X4& orthoMatrix);
	bool RenderFreeDropUI(DX11Manager* dx3D, const ShaderManager* shaderManager,
		const XMFLOAT4X4& worldMatrix, const XMFLOAT4X4& viewMatrix, const XMFLOAT4X4& orthoMatrix);

private:
	ImageUI** m_ballImages;
	ImageUI* m_nextBallImage;
	ImageUI* m_nextTextImage;
	ImageUI** m_playerTextImages;
	ImageUI* m_exitTextImage;
	ImageUI** m_helperImages;
	ImageUI* m_freedropTextImage;

	int m_maxBallNum;
};

