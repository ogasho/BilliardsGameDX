#include "Ball.h"
#include "DXUtil.h"

#include "DX11Manager.h"
#include "Model.h"
#include "ObjMesh.h"

#include <string>
#include <random>

static const float DECLINE_VEC_RATE = 0.993f; // �ړ��x�N�g���̌���
static const float STOP_VEC_VALUE = 0.008f; // �ړ��ʂ����̒l�ȉ��ɂȂ������~����
static const float INIT_POS_DEV = 0.005f; // �������W�ɂ��̒l*(+-DEV_NUM)�̌덷�����܂��
static const int DEV_NUM = 20;

Ball::Ball()
{
	m_model = nullptr;
	m_scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
	m_radius = 2.8f;
	m_isStop = true;
}

Ball::~Ball()
{
	SafeDelete(m_model);
}

bool Ball::Init(DX11Manager* dx3D, ObjMesh* objMesh, XMFLOAT3 position, int ballNumber)
{
	bool result;

	// ���[�h����e�N�X�`���̖��O���m�� "data/b(�ԍ�).tga" 0�̓L���[
	std::string headStr = "data/b";
	if (ballNumber == 0)
		headStr += "q";
	else
		headStr += std::to_string(ballNumber);
	headStr += ".tga";

	// ���f��������
	m_model = new Model;
	result = m_model->Init(dx3D->GetDevice(), dx3D->GetDeviceContext(),
		objMesh, headStr.c_str());
	if (!result) return false;

	// �ʒu������ (�ق�̏��������u����)
	std::random_device rand;
	m_position = position;
	m_position.x += (rand() % DEV_NUM * 2 + 1) * INIT_POS_DEV - INIT_POS_DEV * DEV_NUM;
	m_position.z += (rand() % DEV_NUM * 2 + 1) * INIT_POS_DEV - INIT_POS_DEV * DEV_NUM;

	// �����̊G����������悤��
	m_model->AddRotation(&XMFLOAT3(XMConvertToRadians(90), 0.0f, 0.0f));
	m_model->AddRotation(&XMFLOAT3(0.0f, XMConvertToRadians(90), 0.0f));

	return true;
}

bool Ball::Frame()
{
	// �ړ��x�N�g�����ړ�
	AddPosition(&m_move);

	// �ړ��x�N�g������]
	m_model->AddRotation(&XMFLOAT3(m_move.z / m_radius, 0.0f, -m_move.x / m_radius));

	// �ړ��x�N�g��������
	MulMoveVec(DECLINE_VEC_RATE);
	// �ړ��x�N�g�������ȉ��ɂȂ������~ 
	// (���t���[���傫�������߂�̂��d�����̂Ő����ʂ̂�)
	if (abs(m_move.x) < STOP_VEC_VALUE && abs(m_move.z) < STOP_VEC_VALUE)
	{
		m_move = XMFLOAT3(0, 0, 0);
		m_isStop = true;
	}
	else
	{
		m_isStop = false;
	}
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

void Ball::AddPosition(const XMFLOAT3* addPos)
{
	m_position.x += addPos->x;
	m_position.y += addPos->y;
	m_position.z += addPos->z;
}

void Ball::AddPosition(float x, float y, float z)
{
	m_position.x += x;
	m_position.y += y;
	m_position.z += z;
}

void Ball::MulMoveVec(float t)
{
	m_move.x *= t;
	m_move.y *= t;
	m_move.z *= t;
}