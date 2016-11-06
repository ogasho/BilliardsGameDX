#include "Ball.h"
#include "DXUtil.h"

#include "DX11Manager.h"
#include "Model.h"
#include "ObjMesh.h"

#include <string>

Ball::Ball()
{
	m_model = nullptr;
	m_scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
	m_radius = 3.0f;
}


Ball::~Ball()
{
	SafeDelete(m_model);
}

bool Ball::Init(DX11Manager* dx3D, ObjMesh* objMesh, XMFLOAT3 position,int ballNumber)
{
	bool result;
	
	// ロードするテクスチャの名前を確定 "data/b(番号).tga" 0はキュー
	std::string headStr = "data/b";
	if (ballNumber == 0)
		headStr += "q";
	else
		headStr += std::to_string(ballNumber);
	headStr += ".tga";

	// モデル初期化
	m_model = new Model;
	result = m_model->Init(dx3D->GetDevice(), dx3D->GetDeviceContext(),
		objMesh, headStr.c_str());
	if (!result) return false;

	// 位置初期化
	m_position = position;

	// 数字が上を向くように
	m_model->AddRotation(XMFLOAT3(XMConvertToRadians(90), 0.0f, 0.0f));

	// 見た目の半径が3になるように調整    いらなそう
	//float resize = 1.0f;
	//m_scale = XMFLOAT3(resize, resize, resize);
	count = 0;
	return true;
}

bool Ball::Frame()
{
	return true;
}

void Ball::Render(ID3D11DeviceContext* deviceContext)
{
	m_model->Render(deviceContext);
}

void Ball::GetWorldMatrix(XMFLOAT4X4 *worldMatrix)
{

	m_model->GetWorldMatrix(worldMatrix, m_position, m_scale);
}

Model* Ball::GetModelPtr()
{
	return m_model;
}