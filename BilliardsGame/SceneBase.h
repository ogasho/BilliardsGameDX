/* シーンクラス */
// シーンを作る際はこのクラスを基底とする
// シーンはSceneManagerクラスにて管理される

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

