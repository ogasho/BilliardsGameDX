#include "MainCameraMotion.h"
#include "DXUtil.h"

#include "DX11Manager.h"
#include "Camera.h"
#include "InputManager.h"

#define FIRSTPERSON_RADIUS 50.0f
#define FIRSTPERSON_HEIGHT 30.0f
#define BIRD_MOVE_SPEED 0.05f
#define BIRD_ROT_SPEED 0.02f

MainCameraMotion::MainCameraMotion()
{
	m_camera = nullptr;
}


MainCameraMotion::~MainCameraMotion()
{
	SafeDelete(m_camera);
}

void MainCameraMotion::Init(const DX11Manager* dx3D, float tableWidth, float tableHeight)
{
	// カメラ設定初期化
	m_camera = new Camera;

	m_tableSize.x = tableWidth;
	m_tableSize.y = tableHeight;
	m_isFlipPos = false;

	// テーブル正射影用行列初期化
	float orthoWidth = m_tableSize.x + 50.0f;
	float orthoHeight = orthoWidth * dx3D->GetScreenSize().y / dx3D->GetScreenSize().x;
	XMMATRIX orthoMat = XMMatrixOrthographicLH(orthoWidth, orthoHeight, dx3D->GetScreenNear(), dx3D->GetScreenDepth());

	XMStoreFloat4x4(&m_orthoMatrix, XMMatrixTranspose(orthoMat));

	// 初期位置
	m_initPosition = XMFLOAT3(0.0f, m_tableSize.x, -m_tableSize.y * 0.75f);
	m_initRotation = XMFLOAT3(70.0f, 0.0f, 0.0f);
}
void MainCameraMotion::Render()
{
	m_camera->Render();
}

void MainCameraMotion::SetBirdeyeCamera()
{
	// 俯瞰視点
	XMFLOAT3 initPos;
	GetInitPos(&initPos);
	XMFLOAT3 initRot;
	GetInitRot(&initRot);

	m_camera->SetPosition(initPos);
	m_camera->SetRotation(initRot);
	m_isOrtho = false;
}

void MainCameraMotion::SetLookdownCamera()
{
	// 見下ろし正射影視点
	m_camera->SetPosition(0.0f, 100.0f, 0.0f);

	if (m_isFlipPos)
	{
		m_camera->SetRotation(90.0f, 180.0f, 0.0f);
	}
	else
	{
		m_camera->SetRotation(90.0f, 0.0f, 0.0f);
	}
	m_isOrtho = true;
}

void MainCameraMotion::SetFirstpersonCamera(XMFLOAT3 targetPos, float angle)
{
	// 一人称視点
	float angleRad = XMConvertToRadians(angle);
	float radius = FIRSTPERSON_RADIUS;
	float height = FIRSTPERSON_HEIGHT;

	m_camera->SetPosition(
		targetPos.x - radius * cos(angleRad),
		targetPos.y + height,
		targetPos.z - radius * sin(angleRad));

	m_camera->SetRotation(20.0f, 90.0f - angle, 0.0f);
	m_isOrtho = false;
}

void MainCameraMotion::SetShotMoveCamera(float angle)
{
	if (angle >= 0.0f)
	{
		m_isFlipPos = false;
	}
	else
	{
		m_isFlipPos = true;
	}

	// 指定位置へゆっくり動く
	XMFLOAT3 currentPos = m_camera->GetPosition();
	XMFLOAT3 initPos;
	GetInitPos(&initPos);

	m_camera->SetPosition(
		currentPos.x - (currentPos.x - initPos.x) * BIRD_MOVE_SPEED * 0.5f,
		currentPos.y - (currentPos.y - initPos.y) * BIRD_MOVE_SPEED * 0.5f,
		currentPos.z - (currentPos.z - initPos.z) * BIRD_MOVE_SPEED);

	// 原点へゆっくり向く
	XMFLOAT3 currentRot = m_camera->GetRotation();
	XMFLOAT3 initRot;
	GetInitRot(&initRot);
	m_camera->SetRotation(
		currentRot.x - (currentRot.x - initRot.x) * BIRD_ROT_SPEED,
		currentRot.y - (currentRot.y - initRot.y) * BIRD_ROT_SPEED,
		currentRot.z - (currentRot.z - initRot.z) * BIRD_ROT_SPEED);

}

void MainCameraMotion::GetViewMatrix(XMFLOAT4X4 *viewMatrix) const
{
	m_camera->GetViewMatrix(viewMatrix);
}

void MainCameraMotion::GetInitPos(XMFLOAT3* pos)
{
	if (!m_isFlipPos)
	{
		*pos = m_initPosition;
		return;
	}

	pos->x = -m_initPosition.x;
	pos->y = m_initPosition.y;
	pos->z = -m_initPosition.z;
	return;
}

void MainCameraMotion::GetInitRot(XMFLOAT3* rot)
{
	if (!m_isFlipPos)
	{
		*rot = m_initRotation;
		return;
	}

	rot->x = m_initRotation.x;
	rot->y = m_initRotation.y + 180.0f;
	rot->z = m_initRotation.z;
	return;
}