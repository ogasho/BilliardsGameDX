#include "BilliardUI.h"
#include "DXUtil.h"

#include "DX11Manager.h"
#include "ShaderManager.h"
#include "ImageUI.h"

#include <stdio.h>

BilliardUI::BilliardUI()
{
	m_ballImages = nullptr;
	m_nextBallImage = nullptr;
	m_nextTextImage = nullptr;
	m_playerTextImages = nullptr;
	m_exitTextImage = nullptr;
	m_helperImages = nullptr;
	m_freedropTextImage = nullptr;
}


BilliardUI::~BilliardUI()
{
	for (int i = 0; i < m_maxBallNum; i++)
	{
		SafeDelete(m_ballImages[i]);
	}
	SafeDeleteArr(m_ballImages);
	SafeDelete(m_nextBallImage);
	SafeDelete(m_nextTextImage);

	for (int i = 0; i < 2; i++)
	{
		SafeDelete(m_playerTextImages[i]);
	}
	SafeDeleteArr(m_playerTextImages);

	SafeDelete(m_exitTextImage);

	for (int i = 0; i < 2; i++)
	{
		SafeDelete(m_helperImages[i]);
	}
	SafeDeleteArr(m_helperImages);
	SafeDelete(m_freedropTextImage);
}


bool BilliardUI::Init(DX11Manager* dx3D, int maxBallNum)
{
	bool result;

	// UI初期化
	--maxBallNum;
	m_maxBallNum = maxBallNum;

	// 指定個のボールUIをロード
	m_ballImages = new ImageUI*[maxBallNum];
	for (int i = 0; i < maxBallNum; i++)
	{
		char texName[20];
		sprintf_s(texName, 20, "data/UI/b%dUI.tga", i + 1);

		m_ballImages[i] = new ImageUI;
		result = m_ballImages[i]->Init(dx3D->GetDevice(), dx3D->GetDeviceContext(),
			dx3D->GetScreenSize(), 30, 30, texName);
		if (!result) return false;
	}

	// 次に打つべきボール枠
	m_nextBallImage = new ImageUI;
	m_nextBallImage->Init(dx3D->GetDevice(), dx3D->GetDeviceContext(), dx3D->GetScreenSize(),
		60, 60, nullptr);
	if (!result) return false;

	// "NEXT"
	m_nextTextImage = new ImageUI;
	m_nextTextImage->Init(dx3D->GetDevice(), dx3D->GetDeviceContext(), dx3D->GetScreenSize(),
		60, 30, "data/UI/next.tga");
	if (!result) return false;

	// "1P" "2P"
	m_playerTextImages = new ImageUI*[2];
	for (int i = 0; i < 2; i++)
	{
		char texName[20];
		sprintf_s(texName, 20, "data/UI/player%d.tga", i + 1);

		m_playerTextImages[i] = new ImageUI;
		result = m_playerTextImages[i]->Init(dx3D->GetDevice(), dx3D->GetDeviceContext(), dx3D->GetScreenSize(),
			40, 40, texName);
		if (!result) return false;
	}

	// "Esc:Exit"
	m_exitTextImage = new ImageUI;
	m_exitTextImage->Init(dx3D->GetDevice(), dx3D->GetDeviceContext(), dx3D->GetScreenSize(),
		60, 30, "data/UI/exit.tga");
	if (!result) return false;

	// 操作方法の画像
	m_helperImages = new ImageUI*[2];
	for (int i = 0; i < 2; i++)
	{
		char texName[30];
		sprintf_s(texName, 30, "data/UI/playhelper%d.tga", i + 1);

		m_helperImages[i] = new ImageUI;
		result = m_helperImages[i]->Init(dx3D->GetDevice(), dx3D->GetDeviceContext(), dx3D->GetScreenSize(),
			280, 140, texName);
		if (!result) return false;
	}

	// "FreeDrop"
	m_freedropTextImage = new ImageUI;
	m_freedropTextImage->Init(dx3D->GetDevice(), dx3D->GetDeviceContext(), dx3D->GetScreenSize(),
		250, 125, "data/UI/freedrop.tga");
	if (!result) return false;

	return true;
}

bool BilliardUI::RenderBallListUI(const bool* isBallPockets, DX11Manager* dx3D, const ShaderManager* shaderManager,
	const XMFLOAT4X4& worldMatrix, const XMFLOAT4X4& viewMatrix, const XMFLOAT4X4& orthoMatrix)
{
	bool result;

	int evenPosY = 10;
	int oddPosY = m_ballImages[0]->GetImageSize().y + evenPosY;
	int imageWidth = m_ballImages[0]->GetImageSize().x + 5;
	int startPosX = 150;

	// ボールリストを描画
	for (int i = 0; i < m_maxBallNum; i++)
	{
		float alpha = 1.0f;
		if (isBallPockets[i + 1])
		{
			alpha = 0.3f;
		}

		int posY = evenPosY;
		if (i % 2 != 0)
		{
			posY = oddPosY;
		}

		result = m_ballImages[i]->Render(dx3D->GetDeviceContext(),
			(int)(i * imageWidth * 0.5f + startPosX), posY);
		if (!result)return false;
		result = shaderManager->RenderTextureShader(dx3D->GetDeviceContext(), m_ballImages[i]->GetIndexCount(),
			worldMatrix, viewMatrix, orthoMatrix, m_ballImages[i]->GetTexture(), XMFLOAT4(1, 1, 1, alpha));
		if (!result)return false;
	}
	return true;
}

bool BilliardUI::RenderNextBallUI(int nextBallNum, int currentTurn, DX11Manager* dx3D, const ShaderManager* shaderManager,
	const XMFLOAT4X4& worldMatrix, const XMFLOAT4X4& viewMatrix, const XMFLOAT4X4& orthoMatrix)
{
	bool result;

	int offsetPosX = 80;

	// 次に打つべきボール
	result = m_nextBallImage->Render(dx3D->GetDeviceContext(), offsetPosX, 20);
	if (!result)return false;
	result = shaderManager->RenderTextureShader(dx3D->GetDeviceContext(), m_exitTextImage->GetIndexCount(),
		worldMatrix, viewMatrix, orthoMatrix, m_ballImages[nextBallNum-1]->GetTexture());
	if (!result)return false;

	// NEXT文字
	result = m_nextTextImage->Render(dx3D->GetDeviceContext(), offsetPosX, 60);
	if (!result)return false;
	result = shaderManager->RenderTextureShader(dx3D->GetDeviceContext(), m_exitTextImage->GetIndexCount(),
		worldMatrix, viewMatrix, orthoMatrix, m_nextTextImage->GetTexture());
	if (!result)return false;

	// ターン文字
	result = m_playerTextImages[currentTurn - 1]->Render(dx3D->GetDeviceContext(), offsetPosX + 12, 0);
	if (!result)return false;
	result = shaderManager->RenderTextureShader(dx3D->GetDeviceContext(), m_playerTextImages[currentTurn - 1]->GetIndexCount(),
		worldMatrix, viewMatrix, orthoMatrix, m_playerTextImages[currentTurn - 1]->GetTexture());
	if (!result)return false;

	return true;
}

bool BilliardUI::RenderExitUI(DX11Manager* dx3D, const ShaderManager* shaderManager,
	const XMFLOAT4X4& worldMatrix, const XMFLOAT4X4& viewMatrix, const XMFLOAT4X4& orthoMatrix)
{
	bool result;

	// 左上のExit表記を描画
	result = m_exitTextImage->Render(dx3D->GetDeviceContext(), 0, 0);
	if (!result)return false;

	result = shaderManager->RenderTextureShader(dx3D->GetDeviceContext(), m_exitTextImage->GetIndexCount(),
		worldMatrix, viewMatrix, orthoMatrix, m_exitTextImage->GetTexture());
	if (!result)return false;

	return true;
}

bool BilliardUI::RenderHelperUI(int helperKind, DX11Manager* dx3D, const ShaderManager* shaderManager,
	const XMFLOAT4X4& worldMatrix, const XMFLOAT4X4& viewMatrix, const XMFLOAT4X4& orthoMatrix)
{
	bool result;

	// 操作方法表示
	result = m_helperImages[helperKind]->Render(dx3D->GetDeviceContext(),
		dx3D->GetScreenSize().x - m_helperImages[helperKind]->GetImageSize().x, 0);
	if (!result)return false;
	result = shaderManager->RenderTextureShader(dx3D->GetDeviceContext(), m_helperImages[helperKind]->GetIndexCount(),
		worldMatrix, viewMatrix, orthoMatrix, m_helperImages[helperKind]->GetTexture());
	if (!result)return false;

	return true;
}

bool BilliardUI::RenderFreeDropUI(DX11Manager* dx3D, const ShaderManager* shaderManager,
	const XMFLOAT4X4& worldMatrix, const XMFLOAT4X4& viewMatrix, const XMFLOAT4X4& orthoMatrix)
{
	bool result;

	// FreeDrop表示
	result = m_freedropTextImage->Render(dx3D->GetDeviceContext(),
		dx3D->GetScreenSize().x / 2 - m_freedropTextImage->GetImageSize().x / 2, 50);
	if (!result)return false;
	result = shaderManager->RenderTextureShader(dx3D->GetDeviceContext(), m_freedropTextImage->GetIndexCount(),
		worldMatrix, viewMatrix, orthoMatrix, m_freedropTextImage->GetTexture());
	if (!result)return false;

	return true;
}