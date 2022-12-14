//=============================================================================
//
// Q[UI [ui_game.h]
// Author :  ] ãÄ¾
//
//=============================================================================
#pragma once

//*****************************************************************************
// vg^Cvé¾
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
void SetSushiEffect(void);
void SetAlertYellowEffect(void);
void SetAlertRedEffect(void);
void SetNoAlertEffect(void);
void SetTimer(float time);
void ResetTimer(void);
void OnTimer(void);
void OffTimer(void);
float GetTime(void);
