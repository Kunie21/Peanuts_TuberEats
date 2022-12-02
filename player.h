//=============================================================================
//
// プレイヤー処理 [player.h]
// Author : 國江 翔太
//
//=============================================================================
#pragma once

#define ROCKET_Y		(-60.0f)	// ロケットの高さ
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
float GetPlayerRotation(void);
void SetPlayerThroughRing(void);
void SetPlayerCollisionIce(void);
void SetPlayerCollisionBlast(float rotAddSpd);
void DrawPlayerResult(void);
void DrawFireResult(void);

void SetRocketStart(void);

CURVE_BUFFER GetCurveTestStatus(void);
void SetStageCurvePlayer(void);

void ResetPlayer(void);