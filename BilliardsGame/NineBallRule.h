/* ナインボールルールクラス */
// ナインボールのルールに基づき、様々なフラグを管理するクラス

#pragma once
class NineBallRule
{
public:
	NineBallRule();
	~NineBallRule();

	bool Init(int maxBallNum);

	void Refresh();
	void ChangeTurn();
	void SetBallPocket(int ballNum, bool isPocket);
	void CheckHitFoul(int hitBallNum);
	void CheckTurnEndFoul();

	int GetNextBallNum()const{ return m_nextBallNum; }
	int GetCurrentPlayer()const{ return m_turnNum; }
	bool* GetBallPocketsFlag()const{ return m_isBallPockets; }
	bool IsFoul()const{ return m_isFoul; }
	bool IsGameEnd()const;

private:
	bool* m_isBallPockets;
	int m_nextBallNum;
	int m_maxBallNum;
	int m_turnNum;
	bool m_isFoul;
	bool m_isHitChacked;
	bool m_isPockets;
};

