//=============================================================================
//
// ロード処理 [load.h]
// Author : 國江 翔太
//
//=============================================================================
#pragma once

enum LOAD_LABEL
{
	LOAD_TEXTURE = 0,
	LOAD_MODEL,
	LOAD_SOUND,
	LOAD_MAX,
};

/*******************************************************************************
* プロトタイプ宣言
*******************************************************************************/
void InitLoad(void);
void UpdateLoad(void);
BOOL LoadingNow(void);
void DrawLoad(void);
DWORD WINAPI ThreadFuncLoad(LPVOID pParam);
void AddLoadSum(void);