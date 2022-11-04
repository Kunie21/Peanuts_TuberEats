//=============================================================================
//
// ゲームUI処理 [ui_game.h]
// Author : 國江 翔太
//
//=============================================================================
#pragma once

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitGameUI(void);
void UninitGameUI(void);
void UpdateGameUI(void);
void DrawGameUI(void);

void SetMapPosition(float rate);
void SetFuelMeter(float rate);
void SetSpeedMeter(float rate);
void SetDamageEffect(void);
void SetBoostEffect(void);
void SetTimer(float time);
void OnTimer(void);
float GetTimer(void);
