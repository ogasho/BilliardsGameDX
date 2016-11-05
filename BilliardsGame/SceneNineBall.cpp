#include "SceneNineBall.h"
#include "DXUtil.h"

#include "Camera.h"
#include "Model.h"
#include "TextureShader.h"
#include "Light.h"

SceneNineBall::SceneNineBall()
{
	m_camera = nullptr;
	m_model = nullptr;
	m_textureShader = nullptr;
	m_light = nullptr;
}


SceneNineBall::~SceneNineBall()
{
	SafeDelete(m_camera);
	SafeDelete(m_model);
	SafeDelete(m_textureShader);
	SafeDelete(m_light);
}

bool SceneNineBall::Init(DX11Manager* dx3D, HWND hWnd)
{
	bool result;

	// カメラ初期化
	m_camera = new Camera;
	m_camera->SetPosition(0.0f, 2.0f, -10.0f);
	m_camera->SetRotation(10, 0, 0);

	// モデル初期化
	m_model = new Model;
	result = m_model->Init(dx3D->GetDevice(), dx3D->GetDeviceContext(),
		"data/pool_balls.obj", "data/b1.tga");
	if (!result) return false;

	// テクスチャシェーダ初期化
	m_textureShader = new TextureShader;
	result = m_textureShader->Init(dx3D->GetDevice(), hWnd);
	if (!result) return false;

	// ライト初期化
	m_light = new Light;
	m_light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_light->SetDirection(0.0f, 0.0f, 1.0f);

	return true;
}

SceneID SceneNineBall::Frame()
{
	m_model->AddRotation(0.0f, 0.2f, 0.0f);

	return SceneID::Keep;
}

bool SceneNineBall::Render(DX11Manager* dx3D)
{
	XMFLOAT4X4 worldMatrix, viewMatrix, projectionMatrix;
	bool result;

	// 画面クリア
	dx3D->Begin(0.0f, 0.0f, 0.0f, 1.0f);

	// カメラ
	m_camera->Render();

	// 行列取得
	m_model->GetCurrentMatrix(&worldMatrix);
	m_camera->GetViewMatrix(&viewMatrix);
	dx3D->GetProjectionMatrix(&projectionMatrix);

	// モデル描画
	m_model->Render(dx3D->GetDeviceContext());
	result = m_textureShader->Render(dx3D->GetDeviceContext(), m_model->GetIndexCount(),
		worldMatrix, viewMatrix, projectionMatrix, m_model->GetTexture(),
		m_light->GetDirection(), m_light->GetDiffuseColor());
	result = true;
	if (!result) return false;

	// 描画
	dx3D->End();

	return true;
}
