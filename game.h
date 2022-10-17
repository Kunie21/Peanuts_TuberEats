//=============================================================================
//
// ゲーム画面処理 [game.h]
// Author : 國江 翔太
//
//=============================================================================
#pragma once

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitGame(void);
void UninitGame(void);
void UpdateGame(void);
void DrawGame(void);

BOOL LoadGameTextureKernel(float* loadPalam, int* loadSum);
BOOL LoadGameTexture(void);