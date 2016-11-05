#include "Camera.h"


Camera::Camera()
{
	m_pos = { 0, 0, 0 };
	m_rot = { 0, 0, 0 };
}


Camera::~Camera()
{

}

void Camera::SetPosition(float x, float y, float z)
{
	m_pos.x = x;
	m_pos.y = y;
	m_pos.z = z;
}

void Camera::SetRotation(float x, float y, float z)
{
	m_rot.x = x;
	m_rot.y = y;
	m_rot.z = z;
}

void Camera::Render()
{
	// ������x�N�g��
	XMFLOAT3 up = { 0, 1, 0 };
	XMVECTOR upVec = XMLoadFloat3(&up);

	// ���W�x�N�g��
	XMVECTOR posVec = XMLoadFloat3(&m_pos);

	// �œ_�x�N�g��
	XMFLOAT3 lookAt = { 0, 0, 1 };
	XMVECTOR lookAtVec = XMLoadFloat3(&lookAt);

	// �x�����ʓx�ɕύX�A��]�ݒ�
	float pitch = m_rot.x * 0.0174532925f;
	float yaw = m_rot.y * 0.0174532925f;
	float roll = m_rot.z * 0.0174532925f;
	XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	// ���_�����]�����悤�ɕ␳
	lookAtVec = XMVector3TransformCoord(lookAtVec, rotationMatrix);
	upVec = XMVector3TransformCoord(upVec, rotationMatrix);

	// ���W�ɉ�]��������
	lookAtVec = XMVectorAdd(posVec, lookAtVec);

	// �ʒu�A������A�œ_����r���[�s����쐬
	XMMATRIX viewMat = XMMatrixLookAtLH(posVec, lookAtVec, upVec);
	XMStoreFloat4x4(&m_viewMatrix, XMMatrixTranspose(viewMat));
}