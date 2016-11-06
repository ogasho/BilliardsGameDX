#include "BilliardPhysics.h"

#include <DirectXMath.h>
using namespace DirectX;

#include "Ball.h"

BilliardPhysics::BilliardPhysics()
{
}


BilliardPhysics::~BilliardPhysics()
{
}

void BilliardPhysics::UpdateHitBallAndBall(Ball* b1, Ball* b2)
{
	XMFLOAT3 b1Pos, b2Pos;
	b1->GetPosition(&b1Pos);
	b2->GetPosition(&b2Pos);
	float b1Rad = b1->GetRadius();
	float b2Rad = b2->GetRadius();

	// まず矩形範囲で判定排除
	if (abs(b1Pos.x - b2Pos.x) > b1Rad + b2Rad)
		return;
	if (abs(b1Pos.z - b2Pos.z) > b1Rad + b2Rad)
		return;

	// 距離で当たり判定
	XMFLOAT3 dirVec(b1Pos.x - b2Pos.x, 0.0f, b1Pos.z - b2Pos.z);
	float distance = sqrt(dirVec.x * dirVec.x + dirVec.z * dirVec.z);
	if (distance > b1Rad + b2Rad)
		return;

	XMFLOAT3 b1Move, b2Move;
	b1->GetMoveVec(&b1Move);
	b2->GetMoveVec(&b2Move);

	// 接触方向の単位ベクトル
	XMFLOAT3 dirNorVec(dirVec.x / distance, 0.0f, dirVec.z / distance);

	// めり込んでいる分だけ現在の移動ベクトルに反発が加わる
	float rebound = distance - (b1Rad + b2Rad);
	b1Move.x -= dirNorVec.x * rebound;
	b2Move.x += dirNorVec.x * rebound;
	b1Move.z -= dirNorVec.z * rebound;
	b2Move.z += dirNorVec.z * rebound;

	b1->SetMoveVec(&b1Move);
	b2->SetMoveVec(&b2Move);
}

void BilliardPhysics::UpdateHitBallAndTable(Ball* b)
{
	/* 後のアップデートでテーブルの壁の当たり判定を線の集合にする */

	int tableWidth = 200;  // 仮置き テーブルが出来たら値を貰う
	int tableHeight = 100;

	float ballRadius = b->GetRadius();
	XMFLOAT3 ballPos;
	b->GetPosition(&ballPos);
	XMFLOAT3 ballMove;
	b->GetMoveVec(&ballMove);

	// 接触したら反射
	// X軸
	if (-(tableWidth / 2) > ballPos.x + ballRadius || (tableWidth / 2) < ballPos.x + ballRadius)
	{
		ballMove = XMFLOAT3(-ballMove.x, ballMove.y, ballMove.z);
		b->SetMoveVec(&ballMove);
		b->AddPosition(&ballMove);
	}
	// Z軸
	if (-(tableHeight / 2) > ballPos.z + ballRadius || (tableHeight / 2) < ballPos.z + ballRadius)
	{
		ballMove = XMFLOAT3(ballMove.x, ballMove.y, -ballMove.z);
		b->SetMoveVec(&ballMove);
		b->AddPosition(&ballMove);
	}
}