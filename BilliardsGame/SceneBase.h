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
	SceneBase(DX11Manager* dx3D, const InputManager* inputManager)
	{
		m_dx3D = dx3D; 
		m_inputManager = inputManager; 
		return;
	}
	virtual ~SceneBase(){}

	virtual bool Init(HWND hWnd) = 0;
	virtual SceneID Frame() = 0;
	virtual bool Render() = 0;

protected:
	const InputManager* m_inputManager;
	DX11Manager* m_dx3D;
};

