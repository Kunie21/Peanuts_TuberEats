//=============================================================================
//
// �L�����I����ʏ��� [wallet_select.h]
// Author : �t�u��
//
//=============================================================================
#pragma once


//*****************************************************************************
// �}�N����`
//*****************************************************************************




//*****************************************************************************
// �v���g�^�C�v�錾
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

