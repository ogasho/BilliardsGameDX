/* �r�����[�h�Q�[���p�����N���X */
// �{�[����e�[�u���̕����X�V���s��

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

	void UpdateHitBallAndBall(Ball* b1, Ball* b2);
	void UpdateHitBallAndTable(Ball* b, float tableWidth, float tableHeight);
	void UpdateHitBallAndPockets(Ball* b, const Table* table);

private:
	bool IsHit(const XMFLOAT3& pos1, const XMFLOAT3& pos2, float p1rad, float p2rad, float* getDistance);
};

