/* メインゲーム(ナインボール)クラス */
// ナインボールルールのビリヤードゲームを運用する

#pragma once
#include "SceneBase.h"

class Camera;
class TextureShader;
class Light;
class Ball;
class BilliardPhysics;
class Player;
class Table;

enum class PlayState;

class SceneNineBall : public SceneBase
{
public:
	SceneNineBall(DX11Manager* dx3D, const InputManager* inputManager);
	~SceneNineBall();

	bool Init(HWND hWnd);
	SceneID Frame();
	bool Render();

private:
	SceneID UpdateControl();
	SceneID UpdateShot();

	PlayState m_playState;
	Camera* m_camera;
	Ball** m_balls;
	Player* m_player;
	TextureShader* m_textureShader;
	Light* m_light;
	BilliardPhysics* m_physics;
	Table* m_table;
};

