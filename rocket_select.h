//=============================================================================
//
// キャラ選択画面処理 [rocket_select.h]
// Author : 葉志成
//
//=============================================================================
#pragma once


//*****************************************************************************
// マクロ定義
//*****************************************************************************

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitRocketSelect(void);
void UninitRocketSelect(void);
void UpdateRocketSelect(void);
void DrawRocketSelect(void);

BOOL IsRocketSelectFinished(void);
int GetRocketSelected(void);
void ResetTransaction(void);

void DrawHomeRocket(void);

void SetRocketOutline(void);

void DrawTextureStatus(int rocket, float slide);