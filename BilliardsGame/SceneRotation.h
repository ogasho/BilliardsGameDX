/* メインゲーム(ローテーション)クラス */
// ローテーションルールのビリヤードゲームを運用する

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
class RotationRule;
class BilliardUI;
class RotationPointsUI;

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
	void UpdateFoul();
	void UpdateControl();
	void UpdateShot();
	void UpdateFinish();
	void UpdateCamera();
	void RestoreBall(int ballNum, const XMFLOAT3& restorePos);
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
	RotationRule* m_rule;
	BilliardUI* m_billiardUI;
	RotationPointsUI* m_pointsUI;

	BilliardPocketAnim* m_pocketAnim;
};

