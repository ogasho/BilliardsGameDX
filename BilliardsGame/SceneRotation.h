/* ���C���Q�[��(���[�e�[�V����)�N���X */
// ���[�e�[�V�������[���̃r�����[�h�Q�[�����^�p����

#pragma once
#include "SceneBase.h"

class MainCameraMotion;
class Light;
class Ball;
class BilliardPhysics;
class Player;
class Table;
class FrameCount;

enum class PlayState;
enum class CameraMotion;

class SceneRotation : public SceneBase
{
public:
	SceneRotation(DX11Manager* dx3D, const InputManager* inputManager, const ShaderManager* shaderManager);
	~SceneRotation();

	bool Init();
	SceneID Frame();
	bool Render();

private:
	SceneID UpdateControl();
	SceneID UpdateShot();
	void UpdateCamera();

	bool m_isStateChangeFrame;
	PlayState m_playState;
	PlayState m_oldPlayState;
	FrameCount* m_frameCount;

	MainCameraMotion* m_cameraMotion;
	CameraMotion m_motion;
	Ball** m_balls;
	Player* m_player;
	Light* m_light;
	BilliardPhysics* m_physics;
	Table* m_table;
};

