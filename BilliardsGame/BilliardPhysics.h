/* ビリヤードゲーム用物理クラス */
// ボールやテーブルの物理更新を行う

#pragma once

#include <DirectXMath.h>
using namespace DirectX;

class Ball;
class Table;

class BilliardPhysics
{
public:
	BilliardPhysics();
	~BilliardPhysics();

	bool UpdateHitBallAndBall(Ball* b1, Ball* b2, bool transmission);
	void UpdateHitBallAndTable(Ball* b, float tableWidth, float tableHeight);
	bool UpdateHitBallAndPockets(Ball* b, const Table* table);

private:
	bool IsHit(const XMFLOAT3& pos1, const XMFLOAT3& pos2, float p1rad, float p2rad, float* getDistance);
};

