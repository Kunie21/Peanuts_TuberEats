//=============================================================================
//
// ギミック処理 [gimmick.h]
// Author : 國江 翔太
//
//=============================================================================
#pragma once

//*****************************************************************************
// 構造体定義
//*****************************************************************************
enum GIMMICK_TYPE
{
	GIMMICK_ICE = 0,
	GIMMICK_RING,

	GIMMICK_MAX
};

struct GIMMICK
{
	BOOL use = TRUE;
	int rotPosNo = 0;
	int zPosNo = 0;
	float rotSizeHalf = XM_PIDIV4;
	GIMMICK_TYPE type = GIMMICK_ICE;
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitGimmick(void);
void UninitGimmick(void);
void UpdateGimmick(void);
void DrawGimmick(GIMMICK_TYPE gimmick);
void DrawGimmickInstancing(GIMMICK_TYPE gimmick);

bool CollisionGimmick(int stageNo, float oldZ, float newZ, float oldRot, float newRot);
bool CollisionMissile(int stageNo, float oldZ, float newZ, float oldRot, float newRot);
