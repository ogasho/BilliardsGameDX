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

	// �J����������
	m_camera = new Camera;
	m_camera->SetPosition(0.0f, 2.0f, -10.0f);
	m_camera->SetRotation(10, 0, 0);

	// ���f��������
	m_model = new Model;
	result = m_model->Init(dx3D->GetDevice(), dx3D->GetDeviceContext(),
		"data/pool_balls.obj", "data/b1.tga");
	if (!result) return false;

	// �e�N�X�`���V�F�[�_������
	m_textureShader = new TextureShader;
	result = m_textureShader->Init(dx3D->GetDevice(), hWnd);
	if (!result) return false;

	// ���C�g������
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

	// ��ʃN���A
	dx3D->Begin(0.0f, 0.0f, 0.0f, 1.0f);

	// �J����
	m_camera->Render();

	// �s��擾
	m_model->GetCurrentMatrix(&worldMatrix);
	m_camera->GetViewMatrix(&viewMatrix);
	dx3D->GetProjectionMatrix(&projectionMatrix);

	// ���f���`��
	m_model->Render(dx3D->GetDeviceContext());
	result = m_textureShader->Render(dx3D->GetDeviceContext(), m_model->GetIndexCount(),
		worldMatrix, viewMatrix, projectionMatrix, m_model->GetTexture(),
		m_light->GetDirection(), m_light->GetDiffuseColor());
	result = true;
	if (!result) return false;

	// �`��
	dx3D->End();

	return true;
}
