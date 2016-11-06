/* メインゲーム(ナインボール)クラス */
// ナインボールルールのビリヤードゲームを運用する

#pragma once
#include "SceneBase.h"

class Camera;
class TextureShader;
class Light;
class ObjMesh;
class Ball;

class SceneNineBall : public SceneBase
{
public:
	SceneNineBall();
	~SceneNineBall();

	bool Init(DX11Manager* dx3D, HWND hWnd);
	SceneID Frame();
	bool Render(DX11Manager* dx3D);

private:
	Camera* m_camera;
	Ball** m_balls;
	TextureShader* m_textureShader;
	Light* m_light;

	ObjMesh* m_ballMesh; // 球体は一つデータを読み込んで使いまわす
};

