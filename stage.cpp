//=============================================================================
//
// ステージ処理 [stage.cpp]
// Author : 國江 翔太
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "texture2d.h"
#include "debugproc.h"
#include "model.h"
#include "fade.h"
#include "input.h"
#include "tube.h"
#include "player.h"
#include "ui_game.h"
#include "gimmick.h"
#include "stage.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
//#define MODEL_MAX		(1)
#define GIMMICK_OSAKA		(50)

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static BOOL		g_Load = FALSE;
static STAGE	g_Stage[STAGE_MAX];

static GIMMICK	g_Gmk_Osaka[GIMMICK_OSAKA];
static CURVE	g_Crv_Osaka[6];

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitStage(void)
{
	g_Stage[STAGE_OSAKA].arrGmk = g_Gmk_Osaka;
	g_Stage[STAGE_OSAKA].arrCrv = g_Crv_Osaka;
	g_Stage[STAGE_OSAKA].gmkNum = GIMMICK_OSAKA;
	g_Stage[STAGE_OSAKA].crvNum = 6;
	g_Stage[STAGE_OSAKA].length = 1000;

	for (int i = 0; i < GIMMICK_OSAKA; i++)
	{
		g_Gmk_Osaka[i].rotPosNo = (i * 5) % 8;
		g_Gmk_Osaka[i].zPosNo = i * 20;
		g_Gmk_Osaka[i].type = (GIMMICK_TYPE)(i % 2);
	}

	g_Crv_Osaka[0].angle = { 0.0f, 0.0f };
	g_Crv_Osaka[0].zPosNo = 0;

	g_Crv_Osaka[1].angle = { 0.0f, 0.0f };
	g_Crv_Osaka[1].zPosNo = 100;

	g_Crv_Osaka[2].angle = { 0.05f, 0.1f };
	g_Crv_Osaka[2].zPosNo = 150;

	g_Crv_Osaka[3].angle = { 0.05f, -0.1f };
	g_Crv_Osaka[3].zPosNo = 200;

	g_Crv_Osaka[4].angle = { -0.05f, 0.0f };
	g_Crv_Osaka[4].zPosNo = 300;

	g_Crv_Osaka[5].angle = { 0.0f, 0.0f };
	g_Crv_Osaka[5].zPosNo = 1000;

	g_Load = TRUE;
	return S_OK;
}

STAGE* GetStage(int stageNo) { return &g_Stage[stageNo]; }

void SetStageCurve(int stageNo, float zPos) {
	CURVE_BUFFER curve;
	curve.TexPos = zPos / MESH_SIZE;
	float rate, rateA, rateB, start, end;
	for (int i = 0; i < g_Stage[stageNo].crvNum - 1; i++) {
		start = g_Stage[stageNo].arrCrv[i].zPosNo * MESH_SIZE;
		end = g_Stage[stageNo].arrCrv[i + 1].zPosNo * MESH_SIZE;

		if (start < zPos && zPos < end) {
			rate = (zPos - start) / (end - start);
			rateA = (1.0f - rate) * (1.0f - rate);
			rateB = rate * rate;
			//curve.Angle.x = g_Stage[stageNo].arrCrv[i].angle.x * (1.0f - rate) + g_Stage[stageNo].arrCrv[i + 1].angle.x * rate;
			//curve.Angle.y = g_Stage[stageNo].arrCrv[i].angle.y * (1.0f - rate) + g_Stage[stageNo].arrCrv[i + 1].angle.y * rate;
			curve.Angle.x = g_Stage[stageNo].arrCrv[i].angle.x * rateA + g_Stage[stageNo].arrCrv[i + 1].angle.x * rateB;
			curve.Angle.y = g_Stage[stageNo].arrCrv[i].angle.y * rateA + g_Stage[stageNo].arrCrv[i + 1].angle.y * rateB;
		}
	}
	SetCurveBuffer(&curve);
}


//=============================================================================
// 終了処理
//=============================================================================
void UninitStage(void)
{
	if (g_Load == FALSE) return;

	g_Load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateStage(void)
{

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawStage(void)
{
	
}
