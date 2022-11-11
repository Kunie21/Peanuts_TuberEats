//=============================================================================
//
// ミサイル処理 [missile.h]
// Author : 國江 翔太
//
//=============================================================================
#pragma once

//*****************************************************************************
// 構造体定義
//*****************************************************************************
enum MISSILE_TYPE
{
	MISSILE_TYPE_ICE = 0,
	MISSILE_TYPE_RING,

	MISSILE_TYPE_MAX
};

struct MISSILE
{
	BOOL	use = FALSE;

	float	zRot = 0.0f;
	float	zRotOld = 0.0f;
	float	zRotSpd = 0.0f;

	float	zPos = 0.0f;
	float	zPosOld = 0.0f;
	float	zSpd = 0.0f;
	float	zAcl = 5.0f;

	float	colSizeHalf = XM_PIDIV4;
	MISSILE_TYPE	type = MISSILE_TYPE_ICE;
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitMissile(void);
void UninitMissile(void);
void UpdateMissile(void);
void DrawMissile(MISSILE_TYPE gimmick);
BOOL LaunchMissile(MISSILE_TYPE type, float zPos, float zSpd, float zRot, float zRotSpd);
