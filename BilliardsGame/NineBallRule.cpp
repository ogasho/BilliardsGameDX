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
	// �t���O��ԃ��Z�b�g
	m_isFoul = false;
	m_isHitChacked = false;
	m_isPockets = false;
}

void NineBallRule::ChangeTurn()
{
	// �t�@�E���łȂ����|�P�b�g���Ă���Ȃ�^�[�����̂܂�
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
	// �{�[���|�P�b�g�t���O�X�V
	m_isBallPockets[ballNum] = isPocket;

	if (isPocket == false)
		return;

	// �����������Ă��Ȃ�t�@�E��
	if (ballNum == 0)
	{
		m_isFoul = true;
	}
	else
	{
		m_isPockets = true;
	}

	// �l�N�X�g�{�[���X�V
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
	// ���ɂ��̃^�[���ŐڐG�����Ă���Ȃ�I���
	if (m_isHitChacked)
		return;

	m_isHitChacked = true;

	// �����l�N�X�g�{�[���ɍŏ��ɐG��ĂȂ�������t�@�E��
	if (hitBallNum != m_nextBallNum)
	{
		m_isFoul = true;
	}
}

void NineBallRule::CheckTurnEndFoul()
{
	// ���̃^�[���A��x���w��̃{�[���ɐڐG���Ă��Ȃ�������t�@�E��
	if (!m_isHitChacked)
	{
		m_isFoul = true;
	}
}

bool NineBallRule::IsGameEnd()const
{
	// 9�ԃ{�[�����|�P�b�g���ꂩ�t�@�E�����Ă��Ȃ��Ȃ�A�I��
	if (m_isBallPockets[m_maxBallNum - 1] && !m_isFoul)
	{
		return true;
	}

	return false;
}