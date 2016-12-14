#include "RotationPointsUI.h"
#include "DXUtil.h"

#include "DX11Manager.h"
#include "ImageUI.h"
#include "ShaderManager.h"
#include "Texture.h"

#include <stdio.h>

static const int UI_OFFSET_X = 60;
static const int UI_OFFSET_Y = 95;

RotationPointsUI::RotationPointsUI()
{
	m_pointNumberImage = nullptr;
	m_playerTextImages = nullptr;
}

RotationPointsUI::~RotationPointsUI()
{
	SafeDelete(m_pointNumberImage);

	if (m_pointNumberTextures != nullptr)
	{
		for (int i = 0; i < 10; i++)
		{
			SafeDelete(m_pointNumberTextures[i]);
		}
		SafeDeleteArr(m_pointNumberTextures);
	}

	if (m_playerTextImages != nullptr)
	{
		for (int i = 0; i < 2; i++)
		{
			SafeDelete(m_playerTextImages[i]);
		}
		SafeDeleteArr(m_playerTextImages);
	}
}

bool RotationPointsUI::Init(const DX11Manager* dx3D)
{
	bool result;

	XMINT2 screenSize = dx3D->GetScreenSize();

	// 数字画像ロード
	m_pointNumberTextures = new Texture*[10];
	for (int i = 0; i < 10; i++)
	{
		char texName[20];
		sprintf_s(texName, 20, "data/UI/point%d.tga", i);

		m_pointNumberTextures[i] = new Texture;
		result = m_pointNumberTextures[i]->Init(dx3D->GetDevice(), dx3D->GetDeviceContext(), texName, true);
		if (!result) return false;
	}

	m_pointNumberImage = new ImageUI;
	m_pointNumberImage->Init(dx3D->GetDevice(), dx3D->GetDeviceContext(), screenSize,
		35, 35, nullptr);

	// プレイヤーテキストロード
	m_playerTextImages = new ImageUI*[2];
	for (int i = 0; i < 2; i++)
	{
		char texName[20];
		sprintf_s(texName, 20, "data/UI/player%d.tga", i + 1);

		m_playerTextImages[i] = new ImageUI;
		result = m_playerTextImages[i]->Init(dx3D->GetDevice(), dx3D->GetDeviceContext(), screenSize,
			40, 40, texName);
		if (!result) return false;

		// 配置固定
		int textPosX = m_playerTextImages[i]->GetImageSize().x + UI_OFFSET_X;
		if (i == 0)
			textPosX *= -1;
		m_playerTextImages[i]->SetRenderPosition(
			XMINT2((
			screenSize.x - m_playerTextImages[i]->GetImageSize().x) / 2 + textPosX,
			screenSize.y - UI_OFFSET_Y - 35),
			XMFLOAT2(1,1));
	}

	return true;
}

bool RotationPointsUI::RenderPoints(const int* playerPoints,
	const DX11Manager* dx3D, const ShaderManager* shaderManager,
	const XMFLOAT4X4& worldMatrix, const XMFLOAT4X4& viewMatrix, const XMFLOAT4X4& orthoMatrix)
{
	bool result;

	for (int i = 0; i < 2; i++)
	{
		result = m_playerTextImages[i]->Render(dx3D->GetDeviceContext());
		if (!result) return false;

		result = shaderManager->RenderTextureShader(dx3D->GetDeviceContext(), m_playerTextImages[i]->GetIndexCount(),
			worldMatrix, viewMatrix, orthoMatrix, m_playerTextImages[i]->GetTexture());
		if (!result) return false;

		// ポイント描画(3桁分)
		for (int j = 1; j <= 3; j++)
		{
			int digit = GetDigit(playerPoints[i], j);

			int renderPosX = m_playerTextImages[i]->GetRenderPos().x + 20 * (2 - j);
			m_pointNumberImage->SetRenderPosition(
				XMINT2(renderPosX, dx3D->GetScreenSize().y - UI_OFFSET_Y), XMFLOAT2(1, 1));

			result = m_pointNumberImage->Render(dx3D->GetDeviceContext());
			if (!result) return false;

			result = shaderManager->RenderTextureShader(dx3D->GetDeviceContext(), m_pointNumberImage->GetIndexCount(),
				worldMatrix, viewMatrix, orthoMatrix, m_pointNumberTextures[digit]->GetTexture());
			if (!result) return false;
		}
	}

	return true;
}

int RotationPointsUI::GetDigit(int point, int digitNum)
{
	// 指定された桁の数字を抜き出す
	int digitPow = 1;
	for (int i = 0; i < digitNum; i++)
		digitPow *= 10;

	int digitLowPow = digitPow / 10;

	return point % digitPow / digitLowPow;
}