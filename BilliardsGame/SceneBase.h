/* シーンクラス */
// シーンを作る際はこのクラスを基底とする
// シーンはSceneManagerクラスにて管理される

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

