/* ローテーションルールクラス */
// ローテーションのルールに基づき、様々なフラグを管理するクラス

#pragma once
class RotationRule
{
public:
	RotationRule();
	~RotationRule();

	bool Init(int maxBallNum);

	void Refresh();
	void ChangeTurn();
	void SetBallPocket(int ballNum, bool isPocket);
	void CheckHitFoul(int hitBallNum);
	void CheckTurnEndFoul();
	void UpdateNextBall();

	int GetWonPlayer()const;
	int GetNextBallNum()const{ return m_nextBallNum; }
	int GetCurrentPlayer()const{ return m_turnNum; }
	bool* GetBallPocketsFlag()const{ return m_isBallPockets; }
	bool* GetPreTurnBallPocketsFlag()const{ return m_isBallPocketsTurn; }
	int* GetPlayerPoints()const{ return m_playerPoints; }
	bool IsFoul()const{ return m_isFoul; }
	bool IsGameEnd()const;

private:
	void SetFoul();

	bool* m_isBallPockets;
	bool* m_isBallPocketsTurn;
	int* m_playerPoints;
	int* m_playerPointsPre;

	int m_nextBallNum;
	int m_maxBallNum;
	int m_turnNum;
	bool m_isFoul;
	bool m_isHitChacked;
	bool m_isPockets;
};

