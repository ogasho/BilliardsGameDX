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

	bool Init(const DX11Manager* dx3D, int maxBallNum);

	bool RenderBallListUI(const bool* isBallPockets, 
		const DX11Manager* dx3D, const ShaderManager* shaderManager,
		const XMFLOAT4X4& worldMatrix, const XMFLOAT4X4& viewMatrix, const XMFLOAT4X4& orthoMatrix);
	bool RenderNextBallUI(int nextBallNum, int currentTurn,
		const DX11Manager* dx3D, const ShaderManager* shaderManager,
		const XMFLOAT4X4& worldMatrix, const XMFLOAT4X4& viewMatrix, const XMFLOAT4X4& orthoMatrix);
	bool RenderExitUI(
		const DX11Manager* dx3D, const ShaderManager* shaderManager,
		const XMFLOAT4X4& worldMatrix, const XMFLOAT4X4& viewMatrix, const XMFLOAT4X4& orthoMatrix);
	bool RenderHelperUI(int helperKind,
		const DX11Manager* dx3D, const ShaderManager* shaderManager,
		const XMFLOAT4X4& worldMatrix, const XMFLOAT4X4& viewMatrix, const XMFLOAT4X4& orthoMatrix);
	bool RenderFreeDropUI(
		const DX11Manager* dx3D, const ShaderManager* shaderManager,
		const XMFLOAT4X4& worldMatrix, const XMFLOAT4X4& viewMatrix, const XMFLOAT4X4& orthoMatrix);
	bool RenderWinnerUI(int currentTurn,
		const DX11Manager* dx3D, const ShaderManager* shaderManager,
		const XMFLOAT4X4& worldMatrix, const XMFLOAT4X4& viewMatrix, const XMFLOAT4X4& orthoMatrix);

private:
	bool RenderDrawUI(
		const DX11Manager* dx3D, const ShaderManager* shaderManager,
		const XMFLOAT4X4& worldMatrix, const XMFLOAT4X4& viewMatrix, const XMFLOAT4X4& orthoMatrix);
	void InitRenderPos(const DX11Manager* dx3D);

	ImageUI** m_ballImages;
	ImageUI* m_nextBallImage;
	ImageUI* m_nextTextImage;
	ImageUI** m_playerTextImages;
	ImageUI* m_exitTextImage;
	ImageUI** m_helperImages;
	ImageUI* m_freedropTextImage;
	ImageUI* m_winnerTextImage;
	ImageUI* m_drawTextImage;

	int m_maxBallNum;
};

