//=============================================================================
//
// �Q�[��UI���� [ui_game.h]
// Author : ���] �đ�
//
//=============================================================================
#pragma once

//*****************************************************************************
// �v���g�^�C�v�錾
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
void ResetTimer(void);
void OnTimer(void);
void OffTimer(void);
float GetTime(void);
