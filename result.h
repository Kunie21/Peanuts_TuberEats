//=============================================================================
//
// リサルト画面処理 [Result.h]
// Author : 譚 倩盈
//
//=============================================================================
#pragma once


//*****************************************************************************
// マクロ定義
//*****************************************************************************
struct RESULT
{
	int		length;
	int		time;
	int		fee;
	int		tip;
	int		damage;
};



//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitResult(void);
void UninitResult(void);
void UpdateResult(void);
void DrawResult(void);

void SetEventFinish(void);
int Len(int num);
void ResetResult(void);


void SetDeliveryTime(float time);
void SetResultDistance(int distance);
void SetDeliveryFee(int fee);
void SetDeliveryTip(int tip);
void SetDeliveryDamage(int damage);
void SetDeliveryResult(void);