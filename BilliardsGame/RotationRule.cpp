#include "RotationRule.h"
#include "DXUtil.h"

RotationRule::RotationRule()
{
	m_isBallPockets = nullptr;
	m_isBallPocketsTurn = nullptr;
	m_playerPoints = nullptr;
	m_playerPointsPre = nullptr;
}


RotationRule::~RotationRule()
{
	SafeDeleteArr(m_isBallPockets);
	SafeDeleteArr(m_isBallPocketsTurn);
	SafeDeleteArr(m_playerPoints);
	SafeDeleteArr(m_playerPointsPre);
}

bool RotationRule::Init(int maxBallNum)
{
	m_maxBallNum = maxBallNum;

	m_turnNum = 1;
	m_nextBallNum = 1;
	m_isBallPockets = new bool[maxBallNum];
	m_isBallPocketsTurn = new bool[maxBallNum];
	for (int i = 0; i < maxBallNum; i++)
	{
		m_isBallPockets[i] = false;
		m_isBallPocketsTurn[i] = false;
	}

	m_playerPoints = new int[2];
	m_playerPointsPre = new int[2];
	for (int i = 0; i < 2; i++)
	{
		m_playerPoints[i] = 0;
		m_playerPointsPre[i] = 0;
	}

	Refresh();

	return true;
}

void RotationRule::Refresh()
{
	// フラグ状態リセット
	m_isFoul = false;
	m_isHitChacked = false;
	m_isPockets = false;

	// ターン内のポケットフラグをリセット
	for (int i = 0; i < m_maxBallNum; i++)
		m_isBallPocketsTurn[i] = false;
}

void RotationRule::ChangeTurn()
{
	// ファウルでないかつポケットしているならターンそのまま
	if (!m_isFoul && m_isPockets)
	{
		// 暫定ポイントを確定
		m_playerPointsPre[m_turnNum - 1] = m_playerPoints[m_turnNum - 1];
		return;
	}

	if (m_turnNum == 1)
		m_turnNum = 2;
	else
		m_turnNum = 1;
}

void RotationRule::SetBallPocket(int ballNum, bool isPocket)
{
	// ボールポケットフラグ更新
	m_isBallPockets[ballNum] = isPocket;
	m_isBallPocketsTurn[ballNum] = isPocket;

	if (isPocket == false)
		return;

	// 白球が落ちてたならファウル
	if (ballNum == 0)
	{
		SetFoul();
	}
	else
	{
		m_isPockets = true;
	}

	// ネクストボール更新
	UpdateNextBall();

	// ファウルでないなら、ポイントを暫定追加
	if (!m_isFoul)
	{
		m_playerPoints[m_turnNum - 1] += ballNum;
	}
}

void RotationRule::UpdateNextBall()
{
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

void RotationRule::CheckHitFoul(int hitBallNum)
{
	// 既にこのターンで接触をしているなら終わり
	if (m_isHitChacked)
		return;

	m_isHitChacked = true;

	// もしネクストボールに最初に触れてなかったらファウル
	if (hitBallNum != m_nextBallNum)
	{
		SetFoul();
	}
}

void RotationRule::CheckTurnEndFoul()
{
	// このターン、一度も指定のボールに接触していなかったらファウル
	if (!m_isHitChacked)
	{
		SetFoul();
	}
}

bool RotationRule::IsGameEnd()const
{
	// 全てのボールがポケットされ、かつファウルしていないなら、終了
	for (int i = 1; i < m_maxBallNum; i++)
	{
		if (!m_isBallPockets[i])
			return false;
	}

	if (m_isFoul)
	{
		return false;
	}

	return true;
}

void RotationRule::SetFoul()
{
	// ファウル時の処理(そのターンの獲得ポイントを没収)
	m_playerPoints[m_turnNum - 1] = m_playerPointsPre[m_turnNum - 1];
	m_isFoul = true;
}

int RotationRule::GetWonPlayer()const
{
	// ポイントが高いプレイヤーの勝利
	if (m_playerPoints[0] > m_playerPoints[1])
		return 1;
	else if (m_playerPoints[1] > m_playerPoints[0])
		return 2;
	else
		return -1;
}