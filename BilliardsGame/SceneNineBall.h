/* メインゲーム(ナインボール)クラス */
// ナインボールルールのビリヤードゲームを運用する

#pragma once
#include "SceneBase.h"

class Camera;
class TextureShader;
class Light;
class ObjMesh;
class Ball;
class BilliardPhysics;
class Player;

enum class PlayState;

class SceneNineBall : public SceneBase
{
public:
	SceneNineBall();
	~SceneNineBall();

	bool Init(DX11Manager* dx3D, HWND hWnd, InputManager* inputManager);
	SceneID Frame();
	bool Render();

private:
	SceneID UpdateControl();
	SceneID UpdateShot();

	InputManager* m_inputManager;
	DX11Manager* m_dx3D;

	PlayState m_playState;
	Camera* m_camera;
	Ball** m_balls;
	Player* m_player;
	TextureShader* m_textureShader;
	Light* m_light;
	BilliardPhysics* m_physics;
};

