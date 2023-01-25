//=============================================================================
//
// キャラ選択画面処理 [wallet_select.h]
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
HRESULT InitWallet(void);
void UninitWallet(void);
void UpdateWallet(void);
void DrawWallet(void);

void SetPriceRocket(const ULONG64& price);
void StartTransaction(void);
BOOL InTransaction(void);
BOOL SuccessfulTransaction(void);

ULONG64& GetMoney(void);
void GainMoney(int gain);

