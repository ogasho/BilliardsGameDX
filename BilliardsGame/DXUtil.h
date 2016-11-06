/* 開放セット */

#pragma once

// ライブラリ
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")

// インクルード
#include <tchar.h>

// オプション
#define WIN32_LEAN_AND_MEAN

// 開放
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

// デバッグテキスト
#ifdef _DEBUG
#   define MyOutputDebugString( str, ... ) \
      { \
        TCHAR c[256]; \
        _stprintf_s( c, str, __VA_ARGS__ ); \
        OutputDebugString( c ); \
      }
#else
#    define MyOutputDebugString( str, ... )
#endif