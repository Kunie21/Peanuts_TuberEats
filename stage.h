//=============================================================================
//
// ステージ処理 [stage.h]
// Author : 國江 翔太
//
//=============================================================================
#pragma once

#include "gimmick.h"
#include "renderer.h"

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
	//int corner = 7;			// パイプ断面の頂点数
};

struct STAGE2
{
	GIMMICK* arrGmk = NULL;	// ギミックの配列
	int gmkNum;				// ギミック数
	float goal;				// ゴールの位置
};

enum STAGE_LABEL
{
	STAGE_OSAKA =0,
	STAGE_HOKKAIDO,
	STAGE_OKINAWA,

	STAGE_CHINA,
	STAGE_KOREA,
	STAGE_SINGAPORE,

	STAGE_FRANCE,
	STAGE_ENGLAND,
	STAGE_ITALY,

	STAGE_AMERICA,
	STAGE_CANADA,
	STAGE_MEXICO,

	STAGE_MAX
};

#define TUBE_ROT_NUM	(8)		// ギミックを配置する角度の数
#define TUBE_ZPS_SCL	(10)	// zPosNoの倍率（10おきに設置している）
#define TUBE_ZPS_NUM	(100)	// 10 * 100 で実際はzPosNo1000分ある
struct STAGE_TABLE {
	int			gimmick[TUBE_ROT_NUM];
	XMFLOAT2	curve;
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitStage(void);
void UninitStage(void);
void UpdateStage(void);
void DrawStage(void);

STAGE* GetStage(int stageNo);
STAGE2* GetStage2(int stageNo);
STAGE2* GetStage2(void);
STAGE_TABLE* GetStageTable(int stageNo);
void SetStageCurve(float zPos, float spd);
CURVE_BUFFER* GetCurveBuffer(void);

float GetZPos(int zPosNo);
float GetRotPos(int rotPosNo);

void SetStage(int stage);
int GetStageNo(void);
float GetBaseDeliveryTime(void);
BOOL CheckGoal(float oldZ, float newZ);