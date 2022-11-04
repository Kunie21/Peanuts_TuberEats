//=============================================================================
//
// プレイヤー処理 [player.h]
// Author : 國江 翔太
//
//=============================================================================
#pragma once

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitPlayer(void);
void UninitPlayer(void);
void UpdatePlayer(void);
void DrawPlayer(void);
void DrawFire(void);

float GetPlayerSpeed(void);
float GetPlayerPosition(void);
void SetPlayerThroughRing(void);
void SetPlayerCollisionIce(void);

