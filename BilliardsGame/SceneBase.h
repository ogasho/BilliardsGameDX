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
	~SceneBase(){}

	virtual bool Init(DX11Manager*, HWND) = 0;
	virtual SceneID Frame() = 0;
	virtual bool Render(DX11Manager*) = 0;
};

