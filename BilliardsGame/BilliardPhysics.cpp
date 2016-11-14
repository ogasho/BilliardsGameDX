#include "BilliardPhysics.h"

#include <DirectXMath.h>
using namespace DirectX;

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

void BilliardPhysics::UpdateHitBallAndBall(Ball* b1, Ball* b2)
{
	if (b1->IsPockets() || b2->IsPockets())
		return;

	float b1Rad = b1->GetRadius();
	float b2Rad = b2->GetRadius();
	XMFLOAT3 b1Pos, b2Pos;
	b1->GetPosition(&b1Pos);
	b2->GetPosition(&b2Pos);

	// まず矩形範囲で当たり判定
	if (abs(b1Pos.x - b2Pos.x) > b1Rad + b2Rad)
		return;
	if (abs(b1Pos.z - b2Pos.z) > b1Rad + b2Rad)
		return;

	// 距離で当たり判定 二乗比較
	XMFLOAT3 dirVec(b1Pos.x - b2Pos.x, 0.0f, b1Pos.z - b2Pos.z);
	float distance = dirVec.x * dirVec.x + dirVec.z * dirVec.z;
	if (distance > (b1Rad + b2Rad) * (b1Rad + b2Rad))
		return;

	// ルート化
	distance = sqrt(distance);

	XMFLOAT3 b1Move, b2Move;
	b1->GetMoveVec(&b1Move);
	b2->GetMoveVec(&b2Move);

	// 接触方向の単位ベクトル
	XMFLOAT3 dirNorVec(dirVec.x / distance, 0.0f, dirVec.z / distance);

	// めり込んでいる分だけ現在の移動ベクトルに反発が加わる
	float rebound = (b1Rad + b2Rad) - distance;
	b1Move.x += dirNorVec.x * rebound;
	b2Move.x -= dirNorVec.x * rebound;
	b1Move.z += dirNorVec.z * rebound;
	b2Move.z -= dirNorVec.z * rebound;

	// 接触による減退
	b1Move.x *= FRICTION_RATE;
	b1Move.z *= FRICTION_RATE;
	b2Move.x *= FRICTION_RATE;
	b2Move.z *= FRICTION_RATE;

	b1->SetMoveVec(b1Move);
	b2->SetMoveVec(b2Move);
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

void BilliardPhysics::UpdateHitBallAndPockets(Ball* b, const Table* table)
{
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

		// 矩形範囲で当たり判定
		if (abs(ballPos.x - pocketPos.x) > pocketRad)
			continue;
		if (abs(ballPos.z - pocketPos.z) > pocketRad)
			continue;

		// 距離で当たり判定
		XMFLOAT3 dirVec(ballPos.x - pocketPos.x, 0.0f, ballPos.z - pocketPos.z);
		float distance = sqrt(dirVec.x * dirVec.x + dirVec.z * dirVec.z);
		if (distance > pocketRad)
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
}