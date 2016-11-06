#include "Player.h"
#include "DXUtil.h"

#include "InputManager.h"
#include "Ball.h"

#include <string>
#include <DirectXMath.h>
using namespace DirectX;

#define MAX_SHOT_POWER 5.0f
#define SPEED_ROTATE_DIR 0.6f
#define SPEED_CHANGE_POW 0.1f


Player::Player()
{
	m_shotDirection = 0;
	m_shotPower = 2.0f;
	m_isDecideShot = false;
}


Player::~Player()
{
}


void Player::UpdateInput(InputManager* input)
{
	bool isUpdated = false;

	// 打つ方向
	if (input->IsKeyDown(UseKeys::Left))
	{
		isUpdated = true;
		m_shotDirection += SPEED_ROTATE_DIR;
	}
	else if (input->IsKeyDown(UseKeys::Right))
	{
		isUpdated = true;
		m_shotDirection -= SPEED_ROTATE_DIR;
	}

	// 打つ強さ
	if (input->IsKeyDown(UseKeys::Up))
	{
		isUpdated = true;
		m_shotPower += SPEED_CHANGE_POW;
	}
	else if (input->IsKeyDown(UseKeys::Down))
	{
		isUpdated = true;
		m_shotPower -= SPEED_CHANGE_POW;
	}

	// 決定
	if (input->IsKeyDown(UseKeys::Enter))
		m_isDecideShot = true;

	// 値補正
	if (m_shotPower < 0.0f)
		m_shotPower = 0.0f;
	else if (m_shotPower > MAX_SHOT_POWER)
		m_shotPower = MAX_SHOT_POWER;
	if (m_shotDirection > 180.0f)
		m_shotDirection -= 360.0f;
	else if (m_shotDirection < -180.0f)
		m_shotDirection += 360.0f;


	/* テスト出力 */
	if (isUpdated)
	{
		MyOutputDebugString(L"\n pow=%f, dir=%f\n", m_shotPower, m_shotDirection);
	}
}

void Player::ShotBall(Ball* b)
{
	// 指定の角度/強さで打つ
	float dirRadian = XMConvertToRadians(m_shotDirection);
	float xPower = cos(dirRadian) * m_shotPower;
	float zPower = sin(dirRadian) * m_shotPower;

	b->SetMoveVec(&XMFLOAT3(xPower, 0.0f, zPower));
}