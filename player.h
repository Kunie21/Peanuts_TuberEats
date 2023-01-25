//=============================================================================
//
// �v���C���[���� [player.h]
// Author : ���] �đ�
//
//=============================================================================
#pragma once

#define ROCKET_Y		(-60.0f)	// ���P�b�g�̍���


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitPlayer(void);
void UninitPlayer(void);
void UpdatePlayer(void);
void DrawPlayer(void);
void DrawFire(void);

float GetPlayerInvTime(void);
int GetPlayerRocket(void);
float GetPlayerFireScale(void);
float GetPlayerSpeed(void);
float GetPlayerPosition(void);
float GetPlayerRotation(void);
int GetPlayerMissiles(void);
void SetPlayerRocket(int rocket);
void SetPlayerThroughRing(void);
void SetPlayerCollisionIce(void);
void SetPlayerCollisionRamen(void);
void SetPlayerCollisionLollipop(void);
void SetPlayerCollisionDonut(void);
void SetPlayerCollisionSushi(void);
void SetPlayerCollisionBlast(float rotAddSpd);
void DrawPlayerResult(void);
void DrawFireResult(void);

void SetRocketStart(void);

CURVE_BUFFER GetCurveTestStatus(void);
void SetStageCurvePlayer(void);

void ResetPlayer(void);

float GetFuel(void);
