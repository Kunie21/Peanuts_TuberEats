//=============================================================================
//
// ���T���g��ʏ��� [Result.h]
// Author : � ��m
//
//=============================================================================
#pragma once


//*****************************************************************************
// �}�N����`
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
// �v���g�^�C�v�錾
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