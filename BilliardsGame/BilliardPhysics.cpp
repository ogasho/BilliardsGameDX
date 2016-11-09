#include "BilliardPhysics.h"

#include <DirectXMath.h>
using namespace DirectX;

#include "Ball.h"
#include "Table.h"

// �{�[�����|�P�b�g�ɓ���Ă������A�{�[�������̑��x�x�N�g���ȉ���������|�P�b�g�����
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

	// �܂���`�͈͂œ����蔻��
	if (abs(b1Pos.x - b2Pos.x) > b1Rad + b2Rad)
		return;
	if (abs(b1Pos.z - b2Pos.z) > b1Rad + b2Rad)
		return;

	// �����œ����蔻��
	XMFLOAT3 dirVec(b1Pos.x - b2Pos.x, 0.0f, b1Pos.z - b2Pos.z);
	float distance = sqrt(dirVec.x * dirVec.x + dirVec.z * dirVec.z);
	if (distance > b1Rad + b2Rad)
		return;

	XMFLOAT3 b1Move, b2Move;
	b1->GetMoveVec(&b1Move);
	b2->GetMoveVec(&b2Move);

	// �ڐG�����̒P�ʃx�N�g��
	XMFLOAT3 dirNorVec(dirVec.x / distance, 0.0f, dirVec.z / distance);

	// �߂荞��ł��镪�������݂̈ړ��x�N�g���ɔ����������
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
	// ���݂̃e�[�u���̓����蔻��͋�`
	// �]�T������������̏W���Ɂc

	float ballRadius = b->GetRadius();
	XMFLOAT3 ballPos;
	b->GetPosition(&ballPos);
	XMFLOAT3 ballMove;
	b->GetMoveVec(&ballMove);

	// �ǂɐڐG�����甽��
	// X��
	if (-(tableWidth / 2) > ballPos.x- ballRadius ||
		(tableWidth / 2) < ballPos.x + ballRadius)
	{
		ballMove = XMFLOAT3(-ballMove.x, ballMove.y, ballMove.z);
		b->SetMoveVec(ballMove);
		b->AddPosition(ballMove);
	}
	// Z��
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

		// ��`�͈͂œ����蔻��
		if (abs(ballPos.x - pocketPos.x) > pocketRad)
			continue;
		if (abs(ballPos.z - pocketPos.z) > pocketRad)
			continue;

		// �����œ����蔻��
		XMFLOAT3 dirVec(ballPos.x - pocketPos.x, 0.0f, ballPos.z - pocketPos.z);
		float distance = sqrt(dirVec.x * dirVec.x + dirVec.z * dirVec.z);
		if (distance > pocketRad)
			continue;

		// (�����܂ŃN���A������܂�A�{�[���̒��S�_���|�P�b�g�Ɋ܂܂�Ă���)
		// �ڐG�����̒P�ʃx�N�g��
		XMFLOAT3 dirNorVec(dirVec.x / distance, 0.0f, dirVec.z / distance);

		// �{�[�����|�P�b�g�̒��S�Ɉ���������
		float force = distance / pocketRad * 0.5f;
		ballMove.x -= dirNorVec.x * force;
		ballMove.z -= dirNorVec.z * force;

		// ����
		ballMove.x *= 0.9f;
		ballMove.z *= 0.9f;

		// ���x����薢���ɂȂ�����|�P�b�g���ꂽ�Ƃ���
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

	// �����|�P�b�g�ɂԂ����Ă����班������
	float ballRad = b->GetRadius();
	if (isHitPocket)
		ballPos.y = ballRad * 0.25f;
	else
		ballPos.y = ballRad;

	b->SetPosition(ballPos);
}