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

ULONG64 GetRocketPrice(void);
void BuyRocket(void);

void SetRocketOutline(void);

void DrawTextureStatus(int rocket, float slide);