/* �V�[���N���X */
// �V�[�������ۂ͂��̃N���X�����Ƃ���
// �V�[����SceneManager�N���X�ɂĊǗ������

#pragma once

#include "SceneID.h"
#include "DX11Manager.h"
#include "InputManager.h"

class SceneBase
{
public:
	SceneBase(){}
	virtual ~SceneBase(){}

	virtual bool Init(DX11Manager* dx3D, HWND hWnd, InputManager* inputManager) = 0;
	virtual SceneID Frame() = 0;
	virtual bool Render() = 0;
};

