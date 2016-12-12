#include "NineBallRule.h"
#include "DXUtil.h"

NineBallRule::NineBallRule()
{
	m_isBallPockets = nullptr;
}


NineBallRule::~NineBallRule()
{
	SafeDeleteArr(m_isBallPockets);
}

bool NineBallRule::Init(int maxBallNum)
{
	m_maxBallNum = maxBallNum;

	m_turnNum = 1;
	m_nextBallNum = 1;
	m_isBallPockets = new bool[maxBallNum];
	for (int i = 0; i < maxBallNum; i++)
	{
		m_isBallPockets[i] = false;
	}

	Refresh();

	return true;
}

void NineBallRule::Refresh()
{
	// フラグ状態リセット
	m_isFoul = false;
	m_isHitChacked = false;
	m_isPockets = false;
}

void NineBallRule::ChangeTurn()
{
	// ファウルでないかつポケットしているならターンそのまま
	if (!m_isFoul && m_isPockets)
	{
		return;
	}

	if (m_turnNum == 1)
		m_turnNum = 2;
	else
		m_turnNum = 1;
}

void NineBallRule::SetBallPocket(int ballNum, bool isPocket)
{
	// ボールポケットフラグ更新
	m_isBallPockets[ballNum] = isPocket;

	if (isPocket == false)
		return;

	// 白球が落ちてたならファウル
	if (ballNum == 0)
	{
		m_isFoul = true;
	}
	else
	{
		m_isPockets = true;
	}

	// ネクストボール更新
	for (int i = 1; i < m_maxBallNum; i++)
	{
		if (!m_isBallPockets[i])
		{
			m_nextBallNum = i;
			break;
		}
	}
}

void NineBallRule::CheckHitFoul(int hitBallNum)
{
	// 既にこのターンで接触をしているなら終わり
	if (m_isHitChacked)
		return;

	m_isHitChacked = true;

	// もしネクストボールに最初に触れてなかったらファウル
	if (hitBallNum != m_nextBallNum)
	{
		m_isFoul = true;
	}
}

void NineBallRule::CheckTurnEndFoul()
{
	// このターン、一度も指定のボールに接触していなかったらファウル
	if (!m_isHitChacked)
	{
		m_isFoul = true;
	}
}

bool NineBallRule::IsGameEnd()const
{
	// 9番ボールがポケットされかつファウルしていないなら、終了
	if (m_isBallPockets[m_maxBallNum - 1] && !m_isFoul)
	{
		return true;
	}

	return false;
}