//=============================================================================
//
// メイン処理 [main.h]
// Author : 
//
//=============================================================================
#pragma once


#pragma warning(push)
#pragma warning(disable:4005)

#define _CRT_SECURE_NO_WARNINGS			// scanf のwarning防止
#include <stdio.h>

#include <d3d11.h>
#include <d3dx11.h>
#include <d3dcompiler.h>

#include <DirectXMath.h>

//#include <pthread.h>

//#include <windows.h>


// 本来はヘッダに書かない方が良い
using namespace DirectX;


#define DIRECTINPUT_VERSION 0x0800		// 警告対処
#include "dinput.h"
#include "mmsystem.h"

#include "Xinput.h"        // XInput用

#pragma warning(pop)

#ifdef _DEBUG
// メモリリークチェック用
#include < cstdio >
#include < cstdlib >
#include < crtdbg.h >
#define _CRTDBG_MAP_ALLOC
#define new ::new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define malloc(X) _malloc_dbg(X,_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

//*****************************************************************************
// ライブラリのリンク
//*****************************************************************************
#pragma comment (lib, "d3d11.lib")		
#pragma comment (lib, "d3dcompiler.lib")
#pragma comment (lib, "d3dx11.lib")	
#pragma comment (lib, "d3dx9.lib")	
#pragma comment (lib, "winmm.lib")
#pragma comment (lib, "dxerr.lib")
#pragma comment (lib, "dxguid.lib")
#pragma comment (lib, "dinput8.lib")

#pragma comment (lib, "xinput.lib")        // XInput用


//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define SCREEN_WIDTH	(1920)			// ウインドウの幅
#define SCREEN_HEIGHT	(1080)			// ウインドウの高さ
//#define SCREEN_WIDTH	(1536)		// ウインドウの幅
//#define SCREEN_HEIGHT	(864)		// ウインドウの高さ
//#define SCREEN_WIDTH	(960)			// ウインドウの幅
//#define SCREEN_HEIGHT	(540)			// ウインドウの高さ
#define SCREEN_CENTER_X	(SCREEN_WIDTH / 2)	// ウインドウの中心Ｘ座標
#define SCREEN_CENTER_Y	(SCREEN_HEIGHT / 2)	// ウインドウの中心Ｙ座標

#define	MAP_W			(1280.0f)
#define	MAP_H			(1280.0f)
#define	MAP_TOP			(MAP_H/2)
#define	MAP_DOWN		(-MAP_H/2)
#define	MAP_LEFT		(-MAP_W/2)
#define	MAP_RIGHT		(MAP_W/2)

// 補間用のデータ構造体を定義
struct INTERPOLATION_DATA
{
	XMFLOAT3	pos;		// 頂点座標
	XMFLOAT3	rot;		// 回転
	XMFLOAT3	scl;		// 拡大縮小
	float		frame;		// 実行フレーム数 ( dt = 1.0f/frame )
};

struct INTERPOLATION_COLOR_DATA
{
	XMFLOAT4	color;		// 
	float		frame;		// 実行フレーム数 ( dt = 1.0f/frame )
};

enum MODE_LABEL
{
	MODE_OPENING = 0,

	MODE_LOADING,
	MODE_LOADING2,

	MODE_TITLE_START,
	MODE_HOME,
	MODE_STAGESELECT,
	MODE_GAME,
	MODE_RESULT,

	MODE_END,
};

#define	START_MODE		(MODE_OPENING)
#define	POST_LOAD_MODE	(MODE_TITLE_START)

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
long GetMousePosX(void);
long GetMousePosY(void);
char* GetDebugStr(void);

void SetMode(MODE_LABEL mode);
int GetMode(void);

long GetWinWidth(void);
long GetWinHeight(void);

HWND GetWindowHandle(void);

void SetButtonPressed(void);
BOOL GetCanUseButton(void);