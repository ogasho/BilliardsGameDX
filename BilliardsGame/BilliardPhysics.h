/* ビリヤードゲーム用物理クラス */
// ボールやテーブルの物理更新を行う

#pragma once

class Ball;
class Table;

class BilliardPhysics
{
public:
	BilliardPhysics();
	~BilliardPhysics();

	void UpdateHitBallAndBall(Ball* b1, Ball* b2);
	void UpdateHitBallAndTable(Ball* b, float tableWidth, float tableHeight);
	void UpdateHitBallAndPockets(Ball* b, const Table* table);
};

