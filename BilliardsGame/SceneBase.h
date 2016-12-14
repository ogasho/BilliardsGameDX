/* シーンクラス */
// シーンを作る際はこのクラスを基底とする
// シーンはSceneManagerクラスにて管理される

#pragma once

#include "SceneID.h"
#include "DX11Manager.h"
#include "InputManager.h"
#include "ShaderManager.h"

class SceneBase
{
public:
	SceneBase(const DX11Manager* dx3D, const InputManager* inputManager, const ShaderManager* shaderManager)
	{
		m_dx3D = dx3D; 
		m_inputManager = inputManager; 
		m_shaderManager = shaderManager;
	}
	virtual ~SceneBase(){}

	virtual bool Init() = 0;
	virtual SceneID Frame() = 0;
	virtual bool Render() = 0;

protected:
	const InputManager* m_inputManager;
	const ShaderManager* m_shaderManager;
	const DX11Manager* m_dx3D;
};

