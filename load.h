//=============================================================================
//
// ロード処理 [load.h]
// Author : 國江 翔太
//
//=============================================================================
#pragma once

enum LOAD_LABEL
{
	LOAD_GAMETEXTURE = 0,
	LOAD_SOUND,
	LOAD_OBJECT,
	LOAD_SKYDOME,
	LOAD_WALL,
	LOAD_HOLD,
	LOAD_EXHOLD,
	LOAD_PARTICLE,
	LOAD_STARPARTICLE,
	LOAD_EFFECT,
	LOAD_ARROW,
	LOAD_PLAYER,
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