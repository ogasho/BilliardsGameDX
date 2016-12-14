/* タイトルシーン */
// タイトル画面を運用する

#pragma once

#include "SceneBase.h"

class MainCameraMotion;
class Light;
class Ball;
class BilliardPhysics;
class Player;
class Table;
class SelectHelper;
class ImageUI;

enum class TitleState;
enum class CameraMotion;

class SceneTitle : public SceneBase
{
public:
	SceneTitle(DX11Manager* dx3D, const InputManager* inputManager, const ShaderManager* shaderManager);
	~SceneTitle();

	bool Init();
	SceneID Frame();
	bool Render();

private:
	void InitUI();
	SceneID UpdateInput();
	void UpdateAim();
	void UpdateShot();
	bool RenderUI();

	bool m_isStateChangeFrame;

	TitleState m_titleState;
	TitleState m_oldTitleState;

	MainCameraMotion* m_cameraMotion;
	Ball** m_balls;
	Player* m_player;
	Light* m_light;
	BilliardPhysics* m_physics;
	Table* m_table;
	SelectHelper* m_select;

	ImageUI* m_exitText;
	ImageUI* m_nineballText;
	ImageUI* m_rotationText;
	ImageUI* m_titleText;

	int m_aimBallNum;
};
