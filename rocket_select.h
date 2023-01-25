//=============================================================================
//
// �L�����I����ʏ��� [rocket_select.h]
// Author : �t�u��
//
//=============================================================================
#pragma once


//*****************************************************************************
// �}�N����`
//*****************************************************************************
enum ROCKET_LABEL
{
	ROCKET01 = 0,
	ROCKET02,
	ROCKET03,
	ROCKET04,

	ROCKET_NUM
};
enum LOCK_STATUS
{
	STATUS_LOCK = 0,
	STATUS_NEW,
	STATUS_NORMAL,
	STATUS_EQUIP
};

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitRocketSelect(void);
void UninitRocketSelect(void);
void UpdateRocketSelect(void);
void DrawRocketSelect(void);

BOOL IsRocketSelectFinished(void);
int GetRocketSelected(void);
void ResetTransaction(void);

void DrawHomeRocket(void);

ULONG64 GetRocketPrice(void);
void BuyRocket(void);

void SetRocketOutline(void);

void DrawTextureStatus(int rocket, float slide);