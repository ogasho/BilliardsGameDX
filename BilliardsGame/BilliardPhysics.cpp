#include "BilliardPhysics.h"

#include <DirectXMath.h>
using namespace DirectX;

#include "Ball.h"
#include "Table.h"

// ボールがポケットに内包していた時、ボールがこの速度ベクトル以下だったらポケットされる
const float POCKETS_DICISION_VEC = 0.05f;

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
	float rebound = (b1Rad + b2Rad) - distance;
	b1Move.x += dirNorVec.x * rebound;
	b2Move.x -= dirNorVec.x * rebound;
	b1Move.z += dirNorVec.z * rebound;
	b2Move.z -= dirNorVec.z * rebound;

	b1->SetMoveVec(b1Move);
	b2->SetMoveVec(b2Move);
}

void BilliardPhysics::UpdateHitBallAndTable(Ball* b, float tableWidth, float tableHeight)
{
	// 現在のテーブルの当たり判定は矩形
	// 余裕があったら線の集合に…

	float ballRadius = b->GetRadius();
	XMFLOAT3 ballPos;
	b->GetPosition(&ballPos);
	XMFLOAT3 ballMove;
	b->GetMoveVec(&ballMove);

	// 壁に接触したら反射
	// X軸
	if (-(tableWidth / 2) > ballPos.x- ballRadius ||
		(tableWidth / 2) < ballPos.x + ballRadius)
	{
		ballMove = XMFLOAT3(-ballMove.x, ballMove.y, ballMove.z);
		b->SetMoveVec(ballMove);
		b->AddPosition(ballMove);
	}
	// Z軸
	if (-(tableHeight / 2) > ballPos.z - ballRadius ||
		(tableHeight / 2) < ballPos.z + ballRadius)
	{
		ballMove = XMFLOAT3(ballMove.x, ballMove.y, -ballMove.z);
		b->SetMoveVec(ballMove);
		b->AddPosition(ballMove);
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

		// 減退
		ballMove.x *= 0.9f;
		ballMove.z *= 0.9f;

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