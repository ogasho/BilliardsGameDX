/* ���C���Q�[��(�i�C���{�[��)�N���X */
// �i�C���{�[�����[���̃r�����[�h�Q�[�����^�p����

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

	ObjMesh* m_ballMesh; // ���͈̂�f�[�^��ǂݍ���Ŏg���܂킷
};

