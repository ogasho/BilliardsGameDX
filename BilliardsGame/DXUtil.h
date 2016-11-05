/* �J���Z�b�g */

#pragma once

// ���C�u����
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")

// �I�v�V����
#define WIN32_LEAN_AND_MEAN

// �J��
template <class T> inline void SafeDelete(T& t)
{
	delete t;
	t = nullptr;
}

template <class T> inline void SafeDeleteArr(T& t)
{
	if (t)
	{
		delete[] t;
		t = nullptr;
	}
}

template <class T> inline void SafeRelease(T& t)
{
	if (t)
	{
		t->Release();
		t = nullptr;
	}
}