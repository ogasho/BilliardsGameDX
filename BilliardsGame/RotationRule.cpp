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
	// �t���O��ԃ��Z�b�g
	m_isFoul = false;
	m_isHitChacked = false;
	m_isPockets = false;

	// �^�[�����̃|�P�b�g�t���O�����Z�b�g
	for (int i = 0; i < m_maxBallNum; i++)
		m_isBallPocketsTurn[i] = false;
}

void RotationRule::ChangeTurn()
{
	// �t�@�E���łȂ����|�P�b�g���Ă���Ȃ�^�[�����̂܂�
	if (!m_isFoul && m_isPockets)
	{
		// �b��|�C���g���m��
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
	// �{�[���|�P�b�g�t���O�X�V
	m_isBallPockets[ballNum] = isPocket;
	m_isBallPocketsTurn[ballNum] = isPocket;

	if (isPocket == false)
		return;

	// �����������Ă��Ȃ�t�@�E��
	if (ballNum == 0)
	{
		SetFoul();
	}
	else
	{
		m_isPockets = true;
	}

	// �l�N�X�g�{�[���X�V
	UpdateNextBall();

	// �t�@�E���łȂ��Ȃ�A�|�C���g���b��ǉ�
	if (!m_isFoul)
	{
		m_playerPoints[m_turnNum - 1] += ballNum;
	}
}

void RotationRule::UpdateNextBall()
{
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

void RotationRule::CheckHitFoul(int hitBallNum)
{
	// ���ɂ��̃^�[���ŐڐG�����Ă���Ȃ�I���
	if (m_isHitChacked)
		return;

	m_isHitChacked = true;

	// �����l�N�X�g�{�[���ɍŏ��ɐG��ĂȂ�������t�@�E��
	if (hitBallNum != m_nextBallNum)
	{
		SetFoul();
	}
}

void RotationRule::CheckTurnEndFoul()
{
	// ���̃^�[���A��x���w��̃{�[���ɐڐG���Ă��Ȃ�������t�@�E��
	if (!m_isHitChacked)
	{
		SetFoul();
	}
}

bool RotationRule::IsGameEnd()const
{
	// �S�Ẵ{�[�����|�P�b�g����A���t�@�E�����Ă��Ȃ��Ȃ�A�I��
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
	// �t�@�E�����̏���(���̃^�[���̊l���|�C���g��v��)
	m_playerPoints[m_turnNum - 1] = m_playerPointsPre[m_turnNum - 1];
	m_isFoul = true;
}

int RotationRule::GetWonPlayer()const
{
	// �|�C���g�������v���C���[�̏���
	if (m_playerPoints[0] > m_playerPoints[1])
		return 1;
	else if (m_playerPoints[1] > m_playerPoints[0])
		return 2;
	else
		return -1;
}