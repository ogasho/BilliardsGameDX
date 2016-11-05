/* メインゲーム(ナインボール)クラス */
// ナインボールルールのビリヤードゲームを運用する

#pragma once
#include "SceneBase.h"

class Camera;
class Model;
class TextureShader;
class Light;

class SceneNineBall : public SceneBase
{
public:
	SceneNineBall();
	~SceneNineBall();

	bool Init(DX11Manager*, HWND);
	SceneID Frame();
	bool Render(DX11Manager*);

private:
	Camera* m_camera;
	Model* m_model;
	TextureShader* m_textureShader;
	Light* m_light;
};

