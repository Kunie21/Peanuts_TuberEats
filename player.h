//=============================================================================
//
// �v���C���[���� [player.h]
// Author : ���] �đ�
//
//=============================================================================
#pragma once

//*****************************************************************************
// �v���g�^�C�v�錾
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

