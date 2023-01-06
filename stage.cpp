//=============================================================================
//
// ステージ処理 [stage.cpp]
// Author : 國江 翔太
//
//=============================================================================
#include "main.h"
#include "texture2d.h"
#include "debugproc.h"
#include "model.h"
#include "fade.h"
#include "input.h"
#include "tube.h"
#include "player.h"
#include "ui_game.h"
#include "stage.h"
#include "particle.h"
#include "gate.h"
#include "stage_select.h"


//*****************************************************************************
// マクロ定義
//*****************************************************************************
//#define MODEL_MAX		(1)
#define GIMMICK_OSAKA	(50)

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static BOOL		g_Load = FALSE;
static int		g_StageNo;
static STAGE	g_Stage[STAGE_MAX];
static STAGE2	g_Stage2[STAGE_MAX];

//static GIMMICK	g_Gmk_Osaka[GIMMICK_OSAKA];
//static CURVE	g_Crv_Osaka[6];


//struct STAGE_DESC {
//	STAGE_TABLE	tbl[TUBE_ZPS_NUM];	// ギミックの配列
//};

static COLOERPATTERN g_ColorPatternTbl[STAGE_MAX] = {
	{0,0,ORANGE,SKYBLUE},
	{2,3,RED,PINK},
	{1,1,YELLOW,BLUE},
	{4,5,GREEN,YELLOWGREEN},
};


static STAGE_TABLE g_StageTbl[STAGE_MAX][TUBE_ZPS_NUM] = {
{// 大阪
	{{ 4, 4, 4, 4, 4, 4, 4, 4 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 4, 4, 4, 4, 4, 4, 4, 4 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 1, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 1, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 1, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 6, 0, 6, 0, 6, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 5, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 5, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.10f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.09f, 0.00f }},
	{{ 0, 2, 0, 2, 0, 2, 0, 2 }, { 0.08f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.07f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.06f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.05f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.04f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.03f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.02f, 0.00f }},
	{{ 0, 0, 0, 0, 5, 0, 0, 0 }, { 0.01f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 5, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 5, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 3, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 3, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 3, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.09f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.09f }},
	{{ 0, 0, 0, 1, 0, 0, 0, 0 }, { 0.00f, 0.09f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.09f }},
	{{ 0, 0, 0, 0, 1, 0, 0, 0 }, { 0.00f, 0.09f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.09f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.09f }},
	{{ 0, 0, 0, 1, 0, 0, 0, 0 }, { 0.00f, 0.09f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.09f }},
	{{ 0, 0, 0, 1, 0, 0, 0, 0 }, { 0.01f, 0.09f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.02f, 0.09f }},
	{{ 0, 0, 0, 0, 1, 0, 0, 0 }, { 0.03f, 0.09f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.04f, 0.09f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.03f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.02f, 0.00f }},
	{{ 0, 0, 0, 2, 0, 0, 0, 0 }, { 0.01f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 2, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 2, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }}
},
{// 上海
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 4, 4, 4, 4, 4, 4, 4, 4 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 2 }, { 0.00f, 0.00f }},
	{{ 0, 0, 2, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 2, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0,10, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 2, 0, 0, 2, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 1, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 2, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 1, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 1, 0, 0, 5, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 1, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 5, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 1, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 2, 0, 0, 0, 1 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 1, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 1, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 7, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 1, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 1, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 1, 0, 7, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 5, 0, 0, 0, 0, 0, 1 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 1, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 1, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 1, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 2, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 1, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 2, 0, 0, 5, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 1 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 2, 0, 0, 1, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 5, 0, 0, 0, 1, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 8, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 1, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 1, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 2, 0, 1, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 8, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 1, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 1 }, { 0.00f, 0.00f }},
	{{ 0, 0, 2, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 1, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 5, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }}
},
{// パリ
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 4, 4, 4, 4, 4, 4, 4, 4 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 2, 2, 0, 1, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 2, 2 }, { 0.00f, 0.00f }},
	{{ 0, 1, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 6, 1, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 7, 0, 0, 0, 6, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 2, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 1, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 2, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 1, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 6, 0, 0, 5, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 6, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 5, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 2, 0, 0, 1 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 6, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 2, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 1, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 1, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 7, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 1, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 2, 2, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 1, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 1, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 7, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 2, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 5, 1, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0,10, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 1, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 1, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 5, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 6, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 1, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 7, 0, 0, 1, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 2, 0, 6, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 2, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 1, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 5, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 9, 0, 2, 0, 2, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 1, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 5, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 1 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 2, 2, 2, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 5, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }}
},
{// ニューヨーク
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 4, 4, 4, 4, 4, 4, 4, 4 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 1, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 1, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 2, 0, 0, 0, 0, 2, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 1, 0, 0, 5, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 1, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 5, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 1, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 2, 0, 0, 0, 1 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { -0.4f, -0.1f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { -0.3f, 0.2f  }},
	{{ 0, 0, 0, 0, 0, 0, 1, 0 }, { -0.2f, 0.1f  }},
	{{ 0, 0, 0, 0, 1, 0, 0, 0 }, { -0.1f, 0.05f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 1, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 2, 2, 2, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 1, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 1, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 5, 0, 0, 2, 0, 0, 1 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 1, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 2, 0, 1, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 2, 0, 0, 0, 0, 0 }, { -0.3f, 0.3f  }},
	{{ 1, 0, 0, 0, 0, 0, 0, 0 }, { -0.1f, -0.1f }},
	{{ 0, 0, 0, 0, 2, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 1, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 2, 0, 0, 5, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 1 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 1, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 5, 0, 0, 0, 1, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 1, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 1, 0, 2, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 2, 0, 1, 0, 0, 0 }, { 0.3f , 0.3f  }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.2f , 0.2f  }},
	{{ 0, 2, 0, 0, 0, 0, 0, 0 }, { 0.1f , 0.1f  }},
	{{ 0, 0, 0, 0, 0, 1, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 1 }, { 0.00f, 0.00f }},
	{{ 0, 0, 2, 2, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 1, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 5, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }},
	{{ 0, 0, 0, 0, 0, 0, 0, 0 }, { 0.00f, 0.00f }}
}};

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitStage(void)
{
	//g_Stage[STAGE_OSAKA].arrGmk = g_Gmk_Osaka;
	//g_Stage[STAGE_OSAKA].arrCrv = g_Crv_Osaka;
	//g_Stage[STAGE_OSAKA].gmkNum = GIMMICK_OSAKA;
	//g_Stage[STAGE_OSAKA].crvNum = 6;
	//g_Stage[STAGE_OSAKA].length = 1000;

	//for (int i = 0; i < GIMMICK_OSAKA; i++)
	//{
	//	//g_Gmk_Osaka[i].rotPosNo = (i * 8) % MESH_NUM_X;	// 角度番号
	//	g_Gmk_Osaka[i].rotPosNo = i % MESH_NUM_X;	// 角度番号
	//	if (g_Gmk_Osaka[i].rotPosNo % MESH_NUM_X == 0) g_Gmk_Osaka[i].rotPosNo++;	// ライトとかぶるとき
	//	g_Gmk_Osaka[i].zPosNo = i * 10 + 20;			// 位置番号
	//	//g_Gmk_Osaka[i].type = (GIMMICK_TYPE)(i % 3);	// ギミック番号
	//	g_Gmk_Osaka[i].type = GIMMICK_RING;	// ギミック番号
	//	if (g_Gmk_Osaka[i].type == GIMMICK_MAX)
	//	{
	//		g_Gmk_Osaka[i].type = GIMMICK_CRACK;
	//		SetEmitter(g_Gmk_Osaka[i].zPosNo, g_Gmk_Osaka[i].rotPosNo);
	//	}
	//}

	//g_Crv_Osaka[0].angle = { 0.0f, 0.0f };
	//g_Crv_Osaka[0].zPosNo = 0;

	//g_Crv_Osaka[1].angle = { 0.0f, 0.0f };
	//g_Crv_Osaka[1].zPosNo = 100;

	//g_Crv_Osaka[2].angle = { 0.0f, 0.1f };
	//g_Crv_Osaka[2].zPosNo = 150;

	//g_Crv_Osaka[3].angle = { 0.0f, -0.1f };
	//g_Crv_Osaka[3].zPosNo = 200;

	//g_Crv_Osaka[4].angle = { -0.2f, 0.2f };
	//g_Crv_Osaka[4].zPosNo = 300;

	//g_Crv_Osaka[5].angle = { 0.2f, -0.2f };
	//g_Crv_Osaka[5].zPosNo = 1000;

	int gmkCount, zPosNo, rPosNo;
	for (int s = 0; s < STAGE_MAX; s++)
	{
		for (int z = 0; z < TUBE_ZPS_NUM; z++)
		{
			for (int r = 0; r < TUBE_ROT_NUM; r++)
			{
				if (g_StageTbl[s][z].gimmick[r] > 0
					&& g_StageTbl[s][z].gimmick[r] != 4)
				{
					g_Stage2[s].gmkNum++;
				}
				if (g_StageTbl[s][z].gimmick[r] == 6)
				{
					g_Stage2[s].gmkNum++;
				}
			}
		}
		g_Stage2[s].arrGmk = new GIMMICK[g_Stage2[s].gmkNum];

		gmkCount = 0;
		for (int z = 0; z < TUBE_ZPS_NUM; z++)
		{
			for (int r = 0; r < TUBE_ROT_NUM; r++)
			{
				zPosNo = (TUBE_ZPS_NUM - z - 1) * TUBE_ZPS_SCL;
				rPosNo = (r + 1) % TUBE_ROT_NUM; // (TUBE_ROT_NUM - r - 1);
				switch(g_StageTbl[s][z].gimmick[r])
				{
				case 1:
					g_Stage2[s].arrGmk[gmkCount].type = GIMMICK_RING;
					g_Stage2[s].arrGmk[gmkCount].zPosNo = zPosNo;
					g_Stage2[s].arrGmk[gmkCount].rotPosNo = rPosNo;
					gmkCount++;
					break;

				case 2:
					g_Stage2[s].arrGmk[gmkCount].type = GIMMICK_ICE;
					g_Stage2[s].arrGmk[gmkCount].zPosNo = zPosNo;
					g_Stage2[s].arrGmk[gmkCount].rotPosNo = rPosNo;
					gmkCount++;
					break;

				case 3:
					g_Stage2[s].arrGmk[gmkCount].type = GIMMICK_CRACK;
					g_Stage2[s].arrGmk[gmkCount].zPosNo = zPosNo;
					g_Stage2[s].arrGmk[gmkCount].rotPosNo = rPosNo;
					SetEmitter(zPosNo, rPosNo);
					gmkCount++;
					break;

				case 4:
					g_Stage2[s].goal = zPosNo * MESH_SIZE_Z;
					//SetGoalGate(g_Stage2[s].goal * MESH_SIZE_Z);
					break;

				case 5:
					g_Stage2[s].arrGmk[gmkCount].type = GIMMICK_SUSHI;
					g_Stage2[s].arrGmk[gmkCount].zPosNo = zPosNo;
					g_Stage2[s].arrGmk[gmkCount].rotPosNo = rPosNo;
					gmkCount++;
					break;

				case 6:	// 寿司アイス
					g_Stage2[s].arrGmk[gmkCount].type = GIMMICK_SUSHI_ICE;
					g_Stage2[s].arrGmk[gmkCount].zPosNo = zPosNo;
					g_Stage2[s].arrGmk[gmkCount].rotPosNo = rPosNo;
					gmkCount++;

					// 隠し寿司
					g_Stage2[s].arrGmk[gmkCount].type = GIMMICK_SUSHI;
					g_Stage2[s].arrGmk[gmkCount].zPosNo = zPosNo;
					g_Stage2[s].arrGmk[gmkCount].rotPosNo = rPosNo;
					g_Stage2[s].arrGmk[gmkCount].use = FALSE;
					g_Stage2[s].arrGmk[gmkCount].col.w = 0.0f;
					gmkCount++;

					break;

				case 7:
					g_Stage2[s].arrGmk[gmkCount].type = GIMMICK_IKURA;
					g_Stage2[s].arrGmk[gmkCount].zPosNo = zPosNo;
					g_Stage2[s].arrGmk[gmkCount].rotPosNo = rPosNo;
					gmkCount++;
					break;
				case 8:
					g_Stage2[s].arrGmk[gmkCount].type = GIMMICK_LOLLIPOP;
					g_Stage2[s].arrGmk[gmkCount].zPosNo = zPosNo;
					g_Stage2[s].arrGmk[gmkCount].rotPosNo = rPosNo;
					gmkCount++;
					break;
				case 9:
					g_Stage2[s].arrGmk[gmkCount].type = GIMMICK_DONUT;
					g_Stage2[s].arrGmk[gmkCount].zPosNo = zPosNo;
					g_Stage2[s].arrGmk[gmkCount].rotPosNo = rPosNo;
					gmkCount++;
					break;
				case 10:
					g_Stage2[s].arrGmk[gmkCount].type = GIMMICK_RAMEN;
					g_Stage2[s].arrGmk[gmkCount].zPosNo = zPosNo;
					g_Stage2[s].arrGmk[gmkCount].rotPosNo = rPosNo;
					gmkCount++;
					break;

				default:
					break;
				}
				if (gmkCount == g_Stage2[s].gmkNum) break;
			}
			if (gmkCount == g_Stage2[s].gmkNum) break;
		}
	}

	// ステージをセット
	SetStage(GetSelectedStage());

	// チューブの色をセット
	SetTubePattern(g_ColorPatternTbl[g_StageNo]);

	g_Load = TRUE;
	return S_OK;
}

STAGE* GetStage(int stageNo) { return &g_Stage[stageNo]; }
STAGE2* GetStage2(int stageNo) { return &g_Stage2[stageNo]; }
STAGE2* GetStage2(void) { return &g_Stage2[g_StageNo]; }
STAGE_TABLE* GetStageTable(int stageNo) { return g_StageTbl[stageNo]; }

int GetStageZindex(float zPos) {
	int zIndex = TUBE_ZPS_NUM - (int)(zPos / MESH_SIZE_Z * 0.1f) - 1;

	// エラー対策
	if (zIndex < 0) zIndex = 0;
	if (zIndex >= TUBE_ZPS_NUM) zIndex = TUBE_ZPS_NUM;

	return zIndex;
}

static CURVE_BUFFER g_CrvBuf;
void SetStageCurve(float zPos, float spd)
{
	// テクスチャの移動スピードを設定
	g_CrvBuf.TexPos = zPos / (TUBE_SIZE / TUBE_TEX_Z);
	g_CrvBuf.Spd = spd;

	// チューブを曲げる
	XMFLOAT2 rot = g_StageTbl[g_StageNo][GetStageZindex(zPos)].curve;
	g_CrvBuf.Angle.x += (rot.x - g_CrvBuf.Angle.x) * 0.01f;
	g_CrvBuf.Angle.y += (rot.y - g_CrvBuf.Angle.y) * 0.01f;

	// シェーダーに送る
	SetCurveBuffer(&g_CrvBuf);
}
CURVE_BUFFER* GetCurveBuffer(void) {
	return &g_CrvBuf;
}
//void SetStageCurve(int stageNo, float zPos) {
//	CURVE_BUFFER curve;
//	curve.TexPos = zPos / MESH_SIZE_Z;
//	float rate, rateA, rateB, start, end;
//	for (int i = 0; i < g_Stage[stageNo].crvNum - 1; i++) {
//		start = g_Stage[stageNo].arrCrv[i].zPosNo * MESH_SIZE_Z;
//		end = g_Stage[stageNo].arrCrv[i + 1].zPosNo * MESH_SIZE_Z;
//
//		if (start < zPos && zPos < end) {
//			rate = (zPos - start) / (end - start);
//			rateA = (1.0f - rate) * (1.0f - rate);
//			rateB = rate * rate;
//			//curve.Angle.x = g_Stage[stageNo].arrCrv[i].angle.x * (1.0f - rate) + g_Stage[stageNo].arrCrv[i + 1].angle.x * rate;
//			//curve.Angle.y = g_Stage[stageNo].arrCrv[i].angle.y * (1.0f - rate) + g_Stage[stageNo].arrCrv[i + 1].angle.y * rate;
//			curve.Angle.x = g_Stage[stageNo].arrCrv[i].angle.x * rateA + g_Stage[stageNo].arrCrv[i + 1].angle.x * rateB;
//			curve.Angle.y = g_Stage[stageNo].arrCrv[i].angle.y * rateA + g_Stage[stageNo].arrCrv[i + 1].angle.y * rateB;
//		}
//	}
//	SetCurveBuffer(&curve);
//}


//=============================================================================
// 終了処理
//=============================================================================
void UninitStage(void)
{
	if (g_Load == FALSE) return;

	for (int s = 0; s < STAGE_MAX; s++)
	{
		if(g_Stage2[s].arrGmk) delete[] g_Stage2[s].arrGmk;
	}

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

float GetZPos(int zPosNo)
{
	return MESH_SIZE_Z * zPosNo - GetPlayerPosition();
}
float GetRotPos(int rotPosNo)
{
	return XM_2PI * (float)rotPosNo / (float)MESH_NUM_X + GetTubeRotation() + XM_PIDIV2;
}

void SetStage(int stage) {
	g_StageNo = stage;
	SetGoalGate(g_Stage2[g_StageNo].goal);
}
int GetStageNo(void) {
	return g_StageNo;
}

BOOL CheckGoal(float oldZ, float newZ) {
	if (oldZ < g_Stage2[g_StageNo].goal &&
		newZ > g_Stage2[g_StageNo].goal)
		return TRUE;
	return FALSE;
}