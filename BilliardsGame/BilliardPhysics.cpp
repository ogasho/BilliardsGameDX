#include "BilliardPhysics.h"

#include "Ball.h"
#include "Table.h"

// ボールがポケットに内包していた時、ボールがこの速度ベクトル以下だったらポケットされる
const float POCKETS_DICISION_VEC = 0.05f;
// ポケットゾーンにいる時の減退率
const float POCKETS_PULL_RATE = 0.8f;
// 接触した際の摩擦による減退率
const float FRICTION_RATE = 0.98f;

BilliardPhysics::BilliardPhysics()
{
}


BilliardPhysics::~BilliardPhysics()
{

}

bool BilliardPhysics::UpdateHitBallAndBall(Ball* b1, Ball* b2, bool transmission)
{
	if (b1->IsPockets() || b2->IsPockets())
		return false;

	float b1Rad = b1->GetRadius();
	float b2Rad = b2->GetRadius();
	XMFLOAT3 b1Pos, b2Pos;
	b1->GetPosition(&b1Pos);
	b2->GetPosition(&b2Pos);

	float distance;

	// 当たり判定
	XMFLOAT3 dirVec(b1Pos.x - b2Pos.x, 0.0f, b1Pos.z - b2Pos.z);
	if (!IsHit(dirVec, XMFLOAT3(0, 0, 0), b1Rad, b2Rad, &distance))
		return false;

	XMFLOAT3 b1Move, b2Move;
	b1->GetMoveVec(&b1Move);
	b2->GetMoveVec(&b2Move);

	// 完全に同じ座標にいた場合は、ほんの少しだけずらす
	if (distance == 0.0f)
	{
		distance = 0.01f;
		dirVec.x = 0.01f;
	}

	// 接触方向の単位ベクトル
	XMFLOAT3 dirNorVec(dirVec.x / distance, 0.0f, dirVec.z / distance);

	// めり込んでいる分だけ現在の移動ベクトルに反発が加わる
	float rebound = (b1Rad + b2Rad) - distance;
	b1Move.x += dirNorVec.x * rebound;
	b1Move.z += dirNorVec.z * rebound;
	b2Move.x -= dirNorVec.x * rebound;
	b2Move.z -= dirNorVec.z * rebound;

	// 接触による減退
	b1Move.x *= FRICTION_RATE;
	b1Move.z *= FRICTION_RATE;
	b2Move.x *= FRICTION_RATE;
	b2Move.z *= FRICTION_RATE;

	b1->SetMoveVec(b1Move);
	if (transmission)
	{
		b2->SetMoveVec(b2Move);
	}

	return true;
}

void BilliardPhysics::UpdateHitBallAndTable(Ball* b, float tableWidth, float tableHeight)
{
	// 現在のテーブルの当たり判定は矩形
	// 余裕があったら線の集合に↓
	// (接触条件①:現在座標から移動先座標までの線分と、壁の線分が交差しているか。)
	// (接触条件②:移動ベクトルと壁の法線のなす角が鈍角であるか。)

	bool isHit = false;
	float ballRadius = b->GetRadius();
	XMFLOAT3 ballPos;
	b->GetPosition(&ballPos);
	XMFLOAT3 ballMove;
	b->GetMoveVec(&ballMove);

	// 壁に接触したら反射
	// X軸
	if (-(tableWidth / 2) > ballPos.x - ballRadius)
	{
		ballMove = XMFLOAT3(-ballMove.x, ballMove.y, ballMove.z);
		b->SetPosition(XMFLOAT3(-tableWidth / 2 + ballRadius, ballPos.y, ballPos.z));
		isHit = true;
	}
	else if ((tableWidth / 2) < ballPos.x + ballRadius)
	{
		ballMove = XMFLOAT3(-ballMove.x, ballMove.y, ballMove.z);
		b->SetPosition(XMFLOAT3(tableWidth / 2 - ballRadius, ballPos.y, ballPos.z));
		isHit = true;
	}

	// Z軸
	if (-(tableHeight / 2) > ballPos.z - ballRadius)
	{
		ballMove = XMFLOAT3(ballMove.x, ballMove.y, -ballMove.z);
		b->SetPosition(XMFLOAT3(ballPos.x, ballPos.y, -tableHeight / 2 + ballRadius));
		isHit = true;
	}
	else if ((tableHeight / 2) < ballPos.z + ballRadius)
	{
		ballMove = XMFLOAT3(ballMove.x, ballMove.y, -ballMove.z);
		b->SetPosition(XMFLOAT3(ballPos.x, ballPos.y, tableHeight / 2 - ballRadius));
		isHit = true;
	}

	// 接触による減退
	if (isHit)
	{
		ballMove.x *= FRICTION_RATE;
		ballMove.z *= FRICTION_RATE;

		b->SetMoveVec(ballMove);
	}
}

bool BilliardPhysics::UpdateHitBallAndPockets(Ball* b, const Table* table)
{
	if (b->IsPockets())
		return false;

	float pocketRad = table->GetPocketRadius();
	XMFLOAT3 ballPos;
	b->GetPosition(&ballPos);
	XMFLOAT3 ballMove;
	b->GetMoveVec(&ballMove);
	XMFLOAT3 pocketPos;

	bool isHitPocket = false;
	for (int i = 0; i<POCKETS_NUM; i++)
	{
		table->GetTablePocketsPos(&pocketPos, i);

		// 当たり判定
		XMFLOAT3 dirVec(ballPos.x - pocketPos.x, 0.0f, ballPos.z - pocketPos.z);
		float distance;
		if (!IsHit(dirVec, XMFLOAT3(0,0,0), pocketRad, 0, &distance))
			continue;

		// (ここまでクリアしたらつまり、ボールの中心点がポケットに含まれている)
		// 接触方向の単位ベクトル
		XMFLOAT3 dirNorVec(dirVec.x / distance, 0.0f, dirVec.z / distance);

		// ボールがポケットの中心に引っ張られる
		float force = distance / pocketRad * 0.5f;
		ballMove.x -= dirNorVec.x * force;
		ballMove.z -= dirNorVec.z * force;

		// 吸い込み減退
		ballMove.x *= POCKETS_PULL_RATE;
		ballMove.z *= POCKETS_PULL_RATE;

		// 速度が一定未満になったらポケットされたとする
		if (abs(ballMove.x) < POCKETS_DICISION_VEC && abs(ballMove.z) < POCKETS_DICISION_VEC)
		{
			b->Pockets();
			ballPos.x = pocketPos.x;
			ballPos.z = pocketPos.z;
		}

		b->SetMoveVec(ballMove);

		isHitPocket = true;
		break;
	}

	// もしポケットにぶつかっていたら少し沈む
	float ballRad = b->GetRadius();
	if (isHitPocket)
		ballPos.y = ballRad * 0.25f;
	else
		ballPos.y = ballRad;
	
	b->SetPosition(ballPos);

	return b->IsPockets();
}

bool BilliardPhysics::IsHit(const XMFLOAT3& pos1, const XMFLOAT3& pos2, float p1rad, float p2rad, float* getDistance)
{
	// 矩形判定
	if (abs(pos1.x - pos2.x) > p1rad + p2rad)
		return false;
	if (abs(pos1.z - pos2.z) > p1rad + p2rad)
		return false;

	// 円判定
	XMFLOAT3 dirVec(pos1.x - pos2.x, 0.0f, pos1.z - pos2.z);
	float distance = dirVec.x * dirVec.x + dirVec.z * dirVec.z;
	if (distance > (p1rad + p2rad) * (p1rad + p2rad))
		return false;

	// 距離を渡しておく
	*getDistance = sqrt(distance);

	return true;
}