/* �V�[���N���X */
// �V�[�������ۂ͂��̃N���X�����Ƃ���
// �V�[����SceneManager�N���X�ɂĊǗ������

#pragma once

#include "SceneID.h"
#include "DX11Manager.h"

class SceneBase
{
public:
	SceneBase(){}
	virtual ~SceneBase(){}

	virtual bool Init(DX11Manager* dx3D, HWND hWnd) = 0;
	virtual SceneID Frame() = 0;
	virtual bool Render(DX11Manager* dx3D) = 0;
};

