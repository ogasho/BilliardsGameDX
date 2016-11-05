/* �G���g�� */

#include "System.h"
#include "DXUtil.h"
#include "ObjMesh.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	System* system;
	bool result;

	system = new System;
	if (!system) return 0;
	
	// ������&���s
	result = system->Init();
	if (result)
	{
		system->Run();
	}

	// �I��
	SafeDelete(system);
	
	return 0;
}