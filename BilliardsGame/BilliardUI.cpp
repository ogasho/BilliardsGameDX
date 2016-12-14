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
	m_winnerTextImage = nullptr;
	m_drawTextImage = nullptr;
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
	SafeDelete(m_winnerTextImage);
	SafeDelete(m_drawTextImage);
}


bool BilliardUI::Init(const DX11Manager* dx3D, int maxBallNum)
{
	bool result;

	XMINT2 screenSize = dx3D->GetScreenSize();

	// UI������
	--maxBallNum;
	m_maxBallNum = maxBallNum;

	// �w��̃{�[��UI�����[�h
	m_ballImages = new ImageUI*[maxBallNum];
	for (int i = 0; i < maxBallNum; i++)
	{
		char texName[20];
		sprintf_s(texName, 20, "data/UI/b%dUI.tga", i + 1);

		m_ballImages[i] = new ImageUI;
		result = m_ballImages[i]->Init(dx3D->GetDevice(), dx3D->GetDeviceContext(), screenSize,
			30, 30, texName);
		if (!result) return false;
	}

	// ���ɑłׂ��{�[���g
	m_nextBallImage = new ImageUI;
	m_nextBallImage->Init(dx3D->GetDevice(), dx3D->GetDeviceContext(), screenSize,
		60, 60, nullptr);
	if (!result) return false;

	// "NEXT"
	m_nextTextImage = new ImageUI;
	m_nextTextImage->Init(dx3D->GetDevice(), dx3D->GetDeviceContext(), screenSize,
		60, 30, "data/UI/next.tga");
	if (!result) return false;

	// "1P" "2P"
	m_playerTextImages = new ImageUI*[2];
	for (int i = 0; i < 2; i++)
	{
		char texName[20];
		sprintf_s(texName, 20, "data/UI/player%d.tga", i + 1);

		m_playerTextImages[i] = new ImageUI;
		result = m_playerTextImages[i]->Init(dx3D->GetDevice(), dx3D->GetDeviceContext(), screenSize,
			40, 40, texName);
		if (!result) return false;
	}

	// "Esc:Exit"
	m_exitTextImage = new ImageUI;
	m_exitTextImage->Init(dx3D->GetDevice(), dx3D->GetDeviceContext(), screenSize,
		60, 30, "data/UI/exit.tga");
	if (!result) return false;

	// ������@�̉摜
	m_helperImages = new ImageUI*[2];
	for (int i = 0; i < 2; i++)
	{
		char texName[30];
		sprintf_s(texName, 30, "data/UI/playhelper%d.tga", i + 1);

		m_helperImages[i] = new ImageUI;
		result = m_helperImages[i]->Init(dx3D->GetDevice(), dx3D->GetDeviceContext(), screenSize,
			280, 140, texName);
		if (!result) return false;
	}

	// "FreeDrop"
	m_freedropTextImage = new ImageUI;
	m_freedropTextImage->Init(dx3D->GetDevice(), dx3D->GetDeviceContext(), screenSize,
		250, 125, "data/UI/freedrop.tga");
	if (!result) return false;

	// "WINNER"
	m_winnerTextImage = new ImageUI;
	m_winnerTextImage->Init(dx3D->GetDevice(), dx3D->GetDeviceContext(), screenSize,
		250, 125, "data/UI/winner.tga");
	if (!result) return false;

	// "DRAW"
	m_drawTextImage = new ImageUI;
	m_drawTextImage->Init(dx3D->GetDevice(), dx3D->GetDeviceContext(), screenSize,
		250, 125, "data/UI/draw.tga");
	if (!result) return false;

	// �ʒu�Œ菉����
	InitRenderPos(dx3D);

	return true;
}

void BilliardUI::InitRenderPos(const DX11Manager* dx3D)
{
	// ���ꂼ���UI�̕`��ʒu���Œ�
	// �{�[�����X�g
	{
		int evenPosY = 10;
		int oddPosY = m_ballImages[0]->GetImageSize().y + evenPosY;
		int imageWidth = m_ballImages[0]->GetImageSize().x + 5;
		int startPosX = 150;

		for (int i = 0; i < m_maxBallNum; i++)
		{
			int posY = evenPosY;
			if (i % 2 != 0)
				posY = oddPosY;

			m_ballImages[i]->SetRenderPosition(XMINT2((int)(i * imageWidth * 0.5f + startPosX), posY), XMFLOAT2(1, 1));
		}
	}

	// NEXT�����n
	{
		const int offsetPosX = 80;

		// ���ɑłׂ��{�[��
		m_nextBallImage->SetRenderPosition(XMINT2(offsetPosX, 20), XMFLOAT2(1, 1));

		// NEXT����
		m_nextTextImage->SetRenderPosition(XMINT2(offsetPosX, 60), XMFLOAT2(1, 1));
	}

	// Exit����
	m_exitTextImage->SetRenderPosition(XMINT2(0, 0), XMFLOAT2(1, 1));

	// ������@
	for (int i = 0; i < 2; i++)
	{
		m_helperImages[i]->SetRenderPosition(
			XMINT2(dx3D->GetScreenSize().x - m_helperImages[i]->GetImageSize().x, 0), XMFLOAT2(1, 1));
	}

	// FreeDrop����
	m_freedropTextImage->SetRenderPosition(
		XMINT2((dx3D->GetScreenSize().x - m_freedropTextImage->GetImageSize().x) / 2, 50), XMFLOAT2(1, 1));

	// WINNER����
	m_winnerTextImage->SetRenderPosition(
		XMINT2(dx3D->GetScreenSize().x / 2 - m_winnerTextImage->GetImageSize().x, 100), XMFLOAT2(1, 1));

	// DRAW����
	m_drawTextImage->SetRenderPosition(
		XMINT2((dx3D->GetScreenSize().x - m_drawTextImage->GetImageSize().x) / 2, 100), XMFLOAT2(1, 1));
}

bool BilliardUI::RenderBallListUI(const bool* isBallPockets,
	const DX11Manager* dx3D, const ShaderManager* shaderManager,
	const XMFLOAT4X4& worldMatrix, const XMFLOAT4X4& viewMatrix, const XMFLOAT4X4& orthoMatrix)
{
	bool result;

	// �{�[�����X�g��`��
	for (int i = 0; i < m_maxBallNum; i++)
	{
		float alpha = 1.0f;
		if (isBallPockets[i + 1])
		{
			alpha = 0.3f;
		}

		result = m_ballImages[i]->Render(dx3D->GetDeviceContext());
		if (!result)return false;
		result = shaderManager->RenderTextureShader(dx3D->GetDeviceContext(), m_ballImages[i]->GetIndexCount(),
			worldMatrix, viewMatrix, orthoMatrix, m_ballImages[i]->GetTexture(), XMFLOAT4(1, 1, 1, alpha));
		if (!result)return false;
	}
	return true;
}

bool BilliardUI::RenderNextBallUI(int nextBallNum, int currentTurn, 
	const DX11Manager* dx3D, const ShaderManager* shaderManager,
	const XMFLOAT4X4& worldMatrix, const XMFLOAT4X4& viewMatrix, const XMFLOAT4X4& orthoMatrix)
{
	bool result;

	// ���ɑłׂ��{�[��
	result = m_nextBallImage->Render(dx3D->GetDeviceContext());
	if (!result)return false;
	result = shaderManager->RenderTextureShader(dx3D->GetDeviceContext(), m_exitTextImage->GetIndexCount(),
		worldMatrix, viewMatrix, orthoMatrix, m_ballImages[nextBallNum-1]->GetTexture());
	if (!result)return false;

	// NEXT����
	result = m_nextTextImage->Render(dx3D->GetDeviceContext());
	if (!result)return false;
	result = shaderManager->RenderTextureShader(dx3D->GetDeviceContext(), m_exitTextImage->GetIndexCount(),
		worldMatrix, viewMatrix, orthoMatrix, m_nextTextImage->GetTexture());
	if (!result)return false;

	// �^�[������
	m_playerTextImages[currentTurn - 1]->SetRenderPosition(
		XMINT2(m_nextTextImage->GetRenderPos().x + 12, 0), XMFLOAT2(1, 1));

	result = m_playerTextImages[currentTurn - 1]->Render(dx3D->GetDeviceContext());
	if (!result)return false;
	result = shaderManager->RenderTextureShader(dx3D->GetDeviceContext(), m_playerTextImages[currentTurn - 1]->GetIndexCount(),
		worldMatrix, viewMatrix, orthoMatrix, m_playerTextImages[currentTurn - 1]->GetTexture());
	if (!result)return false;

	return true;
}

bool BilliardUI::RenderExitUI(
	const DX11Manager* dx3D, const ShaderManager* shaderManager,
	const XMFLOAT4X4& worldMatrix, const XMFLOAT4X4& viewMatrix, const XMFLOAT4X4& orthoMatrix)
{
	bool result;

	// �����Exit�\�L��`��
	result = m_exitTextImage->Render(dx3D->GetDeviceContext());
	if (!result)return false;

	result = shaderManager->RenderTextureShader(dx3D->GetDeviceContext(), m_exitTextImage->GetIndexCount(),
		worldMatrix, viewMatrix, orthoMatrix, m_exitTextImage->GetTexture());
	if (!result)return false;

	return true;
}

bool BilliardUI::RenderHelperUI(int helperKind,
	const DX11Manager* dx3D, const ShaderManager* shaderManager,
	const XMFLOAT4X4& worldMatrix, const XMFLOAT4X4& viewMatrix, const XMFLOAT4X4& orthoMatrix)
{
	bool result;

	// ������@�\��
	result = m_helperImages[helperKind]->Render(dx3D->GetDeviceContext());
	if (!result)return false;
	result = shaderManager->RenderTextureShader(dx3D->GetDeviceContext(), m_helperImages[helperKind]->GetIndexCount(),
		worldMatrix, viewMatrix, orthoMatrix, m_helperImages[helperKind]->GetTexture());
	if (!result)return false;

	return true;
}

bool BilliardUI::RenderFreeDropUI(
	const DX11Manager* dx3D, const ShaderManager* shaderManager,
	const XMFLOAT4X4& worldMatrix, const XMFLOAT4X4& viewMatrix, const XMFLOAT4X4& orthoMatrix)
{
	bool result;

	// FreeDrop�\��
	result = m_freedropTextImage->Render(dx3D->GetDeviceContext());
	if (!result)return false;
	result = shaderManager->RenderTextureShader(dx3D->GetDeviceContext(), m_freedropTextImage->GetIndexCount(),
		worldMatrix, viewMatrix, orthoMatrix, m_freedropTextImage->GetTexture());
	if (!result)return false;

	return true;
}

bool BilliardUI::RenderWinnerUI(int wonPlayer, 
	const DX11Manager* dx3D, const ShaderManager* shaderManager,
	const XMFLOAT4X4& worldMatrix, const XMFLOAT4X4& viewMatrix, const XMFLOAT4X4& orthoMatrix)
{
	bool result;

	if (wonPlayer != 1 && wonPlayer != 2)
	{
		result = RenderDrawUI(dx3D, shaderManager, worldMatrix, viewMatrix, orthoMatrix);
		return result;
	}

	// "WINNER"�\��
	result = m_winnerTextImage->Render(dx3D->GetDeviceContext());
	if (!result)return false;
	result = shaderManager->RenderTextureShader(dx3D->GetDeviceContext(), m_winnerTextImage->GetIndexCount(),
		worldMatrix, viewMatrix, orthoMatrix, m_winnerTextImage->GetTexture());
	if (!result)return false;

	// �v���C���[�e�L�X�g�\��
	m_playerTextImages[wonPlayer - 1]->SetRenderPosition
		(XMINT2(dx3D->GetScreenSize().x / 2, m_winnerTextImage->GetRenderPos().y), XMFLOAT2(3.0f, 3.0f));

	result = m_playerTextImages[wonPlayer - 1]->Render(dx3D->GetDeviceContext());
	if (!result)return false;
	result = shaderManager->RenderTextureShader(dx3D->GetDeviceContext(), m_playerTextImages[wonPlayer - 1]->GetIndexCount(),
		worldMatrix, viewMatrix, orthoMatrix, m_playerTextImages[wonPlayer - 1]->GetTexture());
	if (!result)return false;

	return true;
}

bool BilliardUI::RenderDrawUI(
	const DX11Manager* dx3D, const ShaderManager* shaderManager,
	const XMFLOAT4X4& worldMatrix, const XMFLOAT4X4& viewMatrix, const XMFLOAT4X4& orthoMatrix)
{
	bool result;

	// "DRAW"�\��
	result = m_drawTextImage->Render(dx3D->GetDeviceContext());
	if (!result)return false;
	result = shaderManager->RenderTextureShader(dx3D->GetDeviceContext(), m_drawTextImage->GetIndexCount(),
		worldMatrix, viewMatrix, orthoMatrix, m_drawTextImage->GetTexture());
	if (!result)return false;

	return true;
}