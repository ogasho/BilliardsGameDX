/* メインカメラモーションクラス */
// ゲーム内で使われるカメラの動きを制御するクラス

#pragma once

#include <DirectXMath.h>
using namespace DirectX;

class Camera;
class DX11Manager;
class InputManager;

enum class CameraMotion
{
	Birdeye,
	Lookdown,
	Firstperson,
	ShotMove,
};

class MainCameraMotion
{
public:
	MainCameraMotion();
	~MainCameraMotion();

	void Init(DX11Manager* dx3D, float tableWidth, float tableHeight);
	void Render();

	void SetBirdeyeCamera();
	void SetLookdownCamera();
	void SetFirstpersonCamera(XMFLOAT3 targetPos, float angle);
	void SetShotMoveCamera(float angle);

	void GetViewMatrix(XMFLOAT4X4 *viewMatrix)const;
	void GetOrthoMatrix(XMFLOAT4X4 *orthoMatrix)const{ *orthoMatrix = m_orthoMatrix; }
	bool IsOrthoMode()const{ return m_isOrtho; }
	bool IsFlipPos()const{ return m_isFlipPos; }

private:
	void GetInitPos(XMFLOAT3* pos);
	void GetInitRot(XMFLOAT3* rot);

	Camera* m_camera;
	bool m_isOrtho;
	bool m_isFlipPos;
	XMFLOAT4X4 m_orthoMatrix;
	XMFLOAT3 m_initPosition;
	XMFLOAT3 m_initRotation;
	XMFLOAT2 m_tableSize;
};

