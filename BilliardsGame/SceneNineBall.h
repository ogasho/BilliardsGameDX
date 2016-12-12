/* メインゲーム(ナインボール)クラス */
// ナインボールルールのビリヤードゲームを運用する

#pragma once
#include "SceneBase.h"

class MainCameraMotion;
class Light;
class Ball;
class BilliardPhysics;
class Player;
class Table;
class FrameCount;
class BilliardPocketAnim;
class NineBallRule;
class BilliardUI;

enum class PlayState;
enum class CameraMotion;

class SceneNineBall : public SceneBase
{
public:
	SceneNineBall(DX11Manager* dx3D, const InputManager* inputManager, const ShaderManager* shaderManager);
	~SceneNineBall();

	bool Init();
	SceneID Frame();
	bool Render();

private:
	void UpdateFoul();
	void UpdateControl();
	void UpdateShot();
	void UpdateFinish();
	void UpdateCamera();
	bool RenderUI();

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
	NineBallRule* m_rule;
	BilliardUI* m_billiardUI;
	
	BilliardPocketAnim* m_pocketAnim;
};

