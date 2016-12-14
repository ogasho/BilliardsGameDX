#include "BilliardPocketAnim.h"
#include "DXUtil.h"

#include <stdio.h>

#include "DX11Manager.h"
#include "ImageUI.h"
#include "Texture.h"
#include "ShaderManager.h"

#define INIT_ANIM_POS_X -200.0f
#define GOAL_ANIM_POS_X 10.0f

BilliardPocketAnim::BilliardPocketAnim()
{
	m_pocketImage = nullptr;
	m_ballImage = nullptr;
	m_ballTextures = nullptr;
	m_FoulImage = nullptr;
}

BilliardPocketAnim::~BilliardPocketAnim()
{
	SafeDelete(m_pocketImage);
	SafeDelete(m_ballImage);
	SafeDelete(m_FoulImage);

	if (m_ballTextures != nullptr)
	{
		for (int i = 0; i < m_maxBallNum; i++)
		{
			SafeDelete(m_ballTextures[i]);
		}
		SafeDeleteArr(m_ballTextures);
	}
}

bool BilliardPocketAnim::Init(const DX11Manager* dx3D, int maxBallNum)
{
	bool result;

	--maxBallNum;
	m_maxBallNum = maxBallNum;

	// 指定個のボールのテクスチャをロード
	m_ballTextures = new Texture*[maxBallNum];
	for (int i = 0; i < maxBallNum; i++)
	{
		char texName[20];
		sprintf_s(texName, 20, "data/UI/b%dUI.tga", i + 1);

		m_ballTextures[i] = new Texture;
		result = m_ballTextures[i]->Init(dx3D->GetDevice(), dx3D->GetDeviceContext(), texName, true);
		if (!result) return false;
	}

	// UIクラスロード
	XMINT2 screenSize = dx3D->GetScreenSize();

	m_pocketImage = new ImageUI;
	result = m_pocketImage->Init(dx3D->GetDevice(), dx3D->GetDeviceContext(), screenSize,
		200, 100, "data/UI/pocket.tga");
	if (!result) return false;

	m_ballImage = new ImageUI;
	result = m_ballImage->Init(dx3D->GetDevice(), dx3D->GetDeviceContext(), screenSize,
		50, 50, nullptr);
	if (!result) return false;

	m_FoulImage = new ImageUI;
	result = m_FoulImage->Init(dx3D->GetDevice(), dx3D->GetDeviceContext(), screenSize,
		150, 75, "data/UI/foul.tga");
	if (!result) return false;

	// 位置が固定のUIの座標を登録
	InitRenderPos(dx3D);

	return true;
}

void BilliardPocketAnim::InitRenderPos(const DX11Manager* dx3D)
{
	// "FOUL"
	m_FoulImage->SetRenderPosition(
		XMINT2(10,
		dx3D->GetScreenSize().y - m_FoulImage->GetImageSize().y - m_pocketImage->GetImageSize().y),
		XMFLOAT2(1, 1));
}

void BilliardPocketAnim::UpdateAnim()
{
	if (!m_isAnim)
		return;

	// ふわっと右に移動する
	m_imagePosX -= (m_imagePosX - GOAL_ANIM_POS_X) * 0.1f;
}

bool BilliardPocketAnim::Render(const DX11Manager* dx3D, const ShaderManager* shaderManager,
	const XMFLOAT4X4& worldMatrix, const XMFLOAT4X4& viewMatrix, const XMFLOAT4X4& orthoMatrix)
{
	bool result;

	if (m_isFoul)
	{
		// "FOUL"描画
		result = m_FoulImage->Render(dx3D->GetDeviceContext());
		if (!result) return false;

		shaderManager->RenderTextureShader(dx3D->GetDeviceContext(), m_pocketImage->GetIndexCount(),
			worldMatrix, viewMatrix, orthoMatrix, m_FoulImage->GetTexture());
		if (!result) return false;

	}

	if (!m_isAnim)
		return true;

	// "POCKET"描画
	m_pocketImage->SetRenderPosition(
		XMINT2((int)m_imagePosX, dx3D->GetScreenSize().y - m_pocketImage->GetImageSize().y), XMFLOAT2(1, 1));
	result = m_pocketImage->Render(dx3D->GetDeviceContext());
	if (!result) return false;

	shaderManager->RenderTextureShader(dx3D->GetDeviceContext(), m_pocketImage->GetIndexCount(),
		worldMatrix, viewMatrix, orthoMatrix, m_pocketImage->GetTexture());
	if (!result) return false;

	// ボールUI描画
	m_ballImage->SetRenderPosition(
		XMINT2((int)m_imagePosX + m_pocketImage->GetImageSize().x,
		dx3D->GetScreenSize().y - m_pocketImage->GetImageSize().y + (int)(m_ballImage->GetImageSize().y * 0.5f))
		,XMFLOAT2(1, 1));

	result = m_ballImage->Render(dx3D->GetDeviceContext());
	if (!result) return false;

	shaderManager->RenderTextureShader(dx3D->GetDeviceContext(), m_ballImage->GetIndexCount(),
		worldMatrix, viewMatrix, orthoMatrix, m_ballTextures[m_animBallNum - 1]->GetTexture());
	if (!result) return false;

	return true;
}

void BilliardPocketAnim::Clear()
{
	// アニメーション終了、描画解除
	m_isAnim = false;
	m_isFoul = false;
}

void BilliardPocketAnim::StartPocketAnim(int ballNum)
{
	if (ballNum <= 0)
	{
		m_isFoul = true;
		return;
	}

	// アニメーション開始
	m_imagePosX = INIT_ANIM_POS_X;

	m_animBallNum = ballNum;
	m_isAnim = true;
}