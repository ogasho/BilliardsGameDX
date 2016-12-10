/* メインゲーム(ローテーション)クラス */
// ローテーションルールのビリヤードゲームを運用する

#pragma once
#include "SceneBase.h"

class Camera;
class Light;
class Ball;
class BilliardPhysics;
class Player;
class Table;

enum class PlayState;

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

	PlayState m_playState;
	Camera* m_camera;
	Ball** m_balls;
	Player* m_player;
	Light* m_light;
	BilliardPhysics* m_physics;
	Table* m_table;
};

