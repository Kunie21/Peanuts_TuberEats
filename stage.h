//=============================================================================
//
// ステージ処理 [stage.h]
// Author : 國江 翔太
//
//=============================================================================
#pragma once

//*****************************************************************************
// 構造体定義
//*****************************************************************************
struct CURVE
{
	int zPosNo;		// 位置
	XMFLOAT2 angle;	// 角度
};

struct STAGE
{
	GIMMICK* arrGmk = NULL;	// ギミックの配列
	CURVE* arrCrv = NULL;	// カーブの配列
	int gmkNum;				// ギミック数
	int crvNum;				// カーブ数
	int length;				// パイプの長さ
	int corner = 7;			// パイプ断面の頂点数
};

enum STAGE_LABEL
{
	STAGE_OSAKA =0,
	STAGE_SHANGHAI,
	STAGE_PARIS,
	STAGE_NEWYORK,
	STAGE_MAX
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitStage(void);
void UninitStage(void);
void UpdateStage(void);
void DrawStage(void);

STAGE* GetStage(int stageNo);
void SetStageCurve(int stageNo, float zPos);