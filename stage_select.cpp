//=============================================================================
//
// スタート画面 [stageselect.cpp]
// Author : サン
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "texture2d.h"
#include "stage_select.h"
#include "fade.h"
#include "input.h"
#include "sound.h"
#include "model.h"
#include "debugproc.h"
//#include "title.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MENU_H			(111)		// メニュー文字高さ
#define TEXT_H			( 61)		// メニュー詳細文高さ
#define MENU_BW			(150)		// メニュー行間
//#define ANIM_SMALL		(0.05f)		// メニューが大きくなる倍率
#define ANIM_SCALING	(0.1f)		// メニューが大きくなる倍率
#define ANIM_ALPHA		(0.1f)		// メニューが色づくスピード
#define ANIM_SLIDE		(80.0f)		// メニューがスライドしてくるスピード
#define OBJ_DIST		(200)
#define COL_BLACK		{0.0f,0.0f,0.0f,1.0f}	// 黒い色
#define COL_ORIGINAL	{1.0f,1.0f,1.0f,1.0f}	// 元の色

// メニューの種類
enum {

	STAGE_JAPAN = 0,
	STAGE_ASIA,
	STAGE_EUROPE,
	STAGE_NORTH_AMERICA,

	STAGE_NUM,
};

 //メニュー部品の種類
enum {

	MENU_STAGE_1 = 0,
	MENU_STAGE_2,
	MENU_STAGE_3,
	MENU_STAGE_4,
	MENU_STAGE_5,
	MENU_STAGE_6,
	MENU_STAGE_7,

	MENU_PANNEL_NUM,
};

// メニューテクスチャ名
enum {
	TEXTURE_STAGE_MENU,
	TEXTURE_STAGE_MENU_1,
	TEXTURE_STAGE_CIRCLE,
	TEXTURE_STAGE_JAPAN,
	TEXTURE_STAGE_ASIA,
	TEXTURE_STAGE_EUROPE,
	TEXTURE_STAGE_NORTH_AMERICA,
	TEXTURE_STAGE_SELECT_1,
	TEXTURE_STAGE_SELECT_2,
	TEXTURE_STAGE_SELECT_3,
	TEXTURE_STAGE_SELECT_4,
	TEXTURE_STAGE_SELECT_5,
	TEXTURE_STAGE_SELECT_6,
	TEXTURE_STAGE_SELECT_7,
	TEXTURE_STAGE_JAPAN_MENU,
	TEXTURE_JAPAN_STAGE_1,
	TEXTURE_JAPAN_STAGE_2,
	TEXTURE_JAPAN_STAGE_3,
	TEXTURE_STAGE_ASIA_MENU,
	TEXTURE_ASIA_STAGE_1,
	TEXTURE_ASIA_STAGE_2,
	TEXTURE_ASIA_STAGE_3,
	TEXTURE_STAGE_EUROPE_MENU,
	TEXTURE_EUROPE_STAGE_1,
	TEXTURE_EUROPE_STAGE_2,
	TEXTURE_EUROPE_STAGE_3,
	TEXTURE_STAGE_NORTH_AMERICA_MENU,
	TEXTURE_NORTH_AMERICA_STAGE_1,
	TEXTURE_NORTH_AMERICA_STAGE_2,
	TEXTURE_NORTH_AMERICA_STAGE_3,

	TEXTURE_MAX,
};

// モデル名
enum {
	MODEL_TITLE_STAR = 0,
	MODEL_TITLE_EARTH,
	MODEL_TITLE_MAX,
};


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static int				g_Menu = STAGE_JAPAN;			// 選択されているメニュー管理用
static int				g_pannel = MENU_STAGE_1;
static int				g_pannelNow;
static int				g_pressSelect;
static TEXTURE2D_DESC	g_td[TEXTURE_MAX];				// UI管理用
static float			g_AnimScl = 0.0f;				// アニメーション管理用
static float			g_AnimAlpha = 0.0f;				// アニメーション管理用
static float			g_AnimSlide = -SCREEN_WIDTH;	// アニメーション管理用
static BOOL				g_Load = FALSE;
static BOOL				g_bStartOn = FALSE;
static BOOL				g_bStartFlg = FALSE;
static BOOL				g_bStartOffFlg = FALSE;
static MODEL_DATA		g_Model[MODEL_TITLE_MAX];
static BOOL				isSelect = FALSE;
static int				test;
static int				test2;
static int				texnum = TEXTURE_ASIA_STAGE_1;
//static int				menunum = TEXTURE_STAGE_SELECT_1;
//*****************************************************************************
// ローカル関数
//*****************************************************************************
// メニューの種類と部品名からテクスチャ名を取得
//int GetPannelNo(int menu_tex) {
//	return TEXTURE_STAGE_SELECT_1 + MENU_PANNEL_NUM * g_pannel + menu_tex;
//}
// パネルのアニメーション
//void StagePannelAnim(void) {
//
//	g_td[g_pannel].scl.y = g_AnimScl;
//	g_td[g_pannel].uv_pos.v = 0.5f - g_AnimScl * 0.5f;
//	g_td[g_pannel].uv_pos.vh = g_AnimScl;
//
//	if (g_pannel > TEXTURE_STAGE_SELECT_7)
//	{
//		g_pannel = TEXTURE_STAGE_SELECT_1;
//	}
//	//g_td[GetStageNo(MENU_TEX_GREEN)].scl.y = g_AnimScl;
//	//g_td[GetStageNo(MENU_TEX_GREEN)].uv_pos.v = 0.5f - g_AnimScl * 0.5f;
//	//g_td[GetStageNo(MENU_TEX_GREEN)].uv_pos.vh = g_AnimScl;
//}

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitStageSelect(void)
{
	// UI設定 ///////////////////////

	// 一括設定
	for (int i = 0; i < TEXTURE_MAX; i++) {

		g_td[i].tex = (TEXTURE_LABEL)(TEXTURE_LABEL_STAGE_SELECTION_MAP + i);
		g_td[i].ctrType = CENTER_LEFTTOP;
		g_td[i].posType = POSITION_ABSOLUTE;
	}
	//g_td[TEXTURE_BG].tex = TEXTURE_LABEL_WHITE;

	// メニューの土台
	g_td[TEXTURE_STAGE_MENU].size = { 986.0f, SCREEN_HEIGHT };
	g_td[TEXTURE_STAGE_MENU].pos = { 0.0f, 0.0f };

	// メニュー飾り
	g_td[TEXTURE_STAGE_MENU_1].size = { 969.0f, SCREEN_HEIGHT };
	g_td[TEXTURE_STAGE_MENU_1].pos = { -2.0f, 0.0f };

	g_td[TEXTURE_STAGE_CIRCLE].size = { 1692.0f, 1692.0f };
	g_td[TEXTURE_STAGE_CIRCLE].pos = { 770.0f, -370.0f };

	g_td[TEXTURE_STAGE_JAPAN].size = { 244.0f, 101.0f };
	g_td[TEXTURE_STAGE_JAPAN].pos = { 220.0f, 172.0f };
	g_td[TEXTURE_STAGE_JAPAN].col = COL_BLACK;

	g_td[TEXTURE_STAGE_ASIA].size = { 168.0f, 101.0f };
	g_td[TEXTURE_STAGE_ASIA].pos = { 250.0f, 290.0f };

	g_td[TEXTURE_STAGE_EUROPE].size = { 288.0f, 101.0f };
	g_td[TEXTURE_STAGE_EUROPE].pos = { 192.0f, 408.0f };

	g_td[TEXTURE_STAGE_NORTH_AMERICA].size = { 580.0f, 100.0f };
	g_td[TEXTURE_STAGE_NORTH_AMERICA].pos = { 40.0f, 526.0f };

	g_td[TEXTURE_STAGE_SELECT_1].size = { 704.0f, 120.0f };
	g_td[TEXTURE_STAGE_SELECT_1].pos = { 0.0f, 156.0f };

	g_td[TEXTURE_STAGE_SELECT_2].size = { 672.0f, 120.0f };
	g_td[TEXTURE_STAGE_SELECT_2].pos = { 0.0f, 274.0f };

	g_td[TEXTURE_STAGE_SELECT_3].size = { 661.0f, 120.0f };
	g_td[TEXTURE_STAGE_SELECT_3].pos = { 0.0f, 393.0f };

	g_td[TEXTURE_STAGE_SELECT_4].size = { 682.0f, 120.0f };
	g_td[TEXTURE_STAGE_SELECT_4].pos = { 0.0f, 512.0f };

	g_td[TEXTURE_STAGE_SELECT_5].size = { 720.0f, 120.0f };
	g_td[TEXTURE_STAGE_SELECT_5].pos = { 0.0f, 631.0f };

	g_td[TEXTURE_STAGE_SELECT_6].size = { 780.0f, 120.0f };
	g_td[TEXTURE_STAGE_SELECT_6].pos = { 2.0f, 749.0f };

	g_td[TEXTURE_STAGE_SELECT_7].size = { 849.0f, 120.0f };
	g_td[TEXTURE_STAGE_SELECT_7].pos = { 18.0f, 870.0f };


	g_td[TEXTURE_STAGE_JAPAN_MENU].size = { 682.0f, 358.0f };
	g_td[TEXTURE_STAGE_JAPAN_MENU].pos = { 0.0f, 275.0f };

	g_td[TEXTURE_JAPAN_STAGE_1].size = { 388.0f, 100.0f };
	g_td[TEXTURE_JAPAN_STAGE_1].pos = { 54.0f, 290.0f };

	g_td[TEXTURE_JAPAN_STAGE_2].size = { 515.0f, 100.0f };
	g_td[TEXTURE_JAPAN_STAGE_2].pos = { 54.0f, 410.0f };

	g_td[TEXTURE_JAPAN_STAGE_3].size = { 490.0f, 100.0f };
	g_td[TEXTURE_JAPAN_STAGE_3].pos = { 54.0f, 530.0f };


	g_td[TEXTURE_STAGE_ASIA_MENU].size = { 723.0f, 358.0f };
	g_td[TEXTURE_STAGE_ASIA_MENU].pos = { 0.0f, 394.0f };

	g_td[TEXTURE_ASIA_STAGE_1].size = { 374.0f, 100.0f };
	g_td[TEXTURE_ASIA_STAGE_1].pos = { 54.0f, 419.0f };

	g_td[TEXTURE_ASIA_STAGE_2].size = { 377.0f, 100.0f };
	g_td[TEXTURE_ASIA_STAGE_2].pos = { 54.0f, 529.0f };

	g_td[TEXTURE_ASIA_STAGE_3].size = { 566.0f, 100.0f };
	g_td[TEXTURE_ASIA_STAGE_3].pos = { 54.0f, 639.0f };


	g_td[TEXTURE_STAGE_EUROPE_MENU].size = { 787.0f, 358.0f };
	g_td[TEXTURE_STAGE_EUROPE_MENU].pos = { 0.0f, 513.0f };

	g_td[TEXTURE_EUROPE_STAGE_1].size = { 414.0f, 100.0f };
	g_td[TEXTURE_EUROPE_STAGE_1].pos = { 54.0f, 530.0f };

	g_td[TEXTURE_EUROPE_STAGE_2].size = { 473.0f, 100.0f };
	g_td[TEXTURE_EUROPE_STAGE_2].pos = { 54.0f, 650.0f };

	g_td[TEXTURE_EUROPE_STAGE_3].size = { 316.0f, 100.0f };
	g_td[TEXTURE_EUROPE_STAGE_3].pos = { 54.0f, 770.0f };


	g_td[TEXTURE_STAGE_NORTH_AMERICA_MENU].size = { 875.0f, 358.0f };
	g_td[TEXTURE_STAGE_NORTH_AMERICA_MENU].pos = { 0.0f, 632.0f };

	g_td[TEXTURE_NORTH_AMERICA_STAGE_1].size = { 485.0f, 100.0f };
	g_td[TEXTURE_NORTH_AMERICA_STAGE_1].pos = { 130.0f, 645.0f };

	g_td[TEXTURE_NORTH_AMERICA_STAGE_2].size = { 466.0f, 100.0f };
	g_td[TEXTURE_NORTH_AMERICA_STAGE_2].pos = { 130.0f, 755.0f };

	g_td[TEXTURE_NORTH_AMERICA_STAGE_3].size = { 449.0f, 100.0f };
	g_td[TEXTURE_NORTH_AMERICA_STAGE_3].pos = { 130.0f, 865.0f };


	// メニュー
	//for (int i = 0; i < MENU_NUM; i++) {
	//	g_td[i * MENU_TEX_NUM + TEXTURE_START_01].pos = g_td[i * MENU_TEX_NUM + TEXTURE_START].pos = { 200.0f, MENU_BW * (float)(i + 1) };

	//	g_td[i * MENU_TEX_NUM + TEXTURE_MENU_PANNEL_01].pos = { 0.0f, MENU_BW * (float)(i + 1) - 8.0f };
	//	g_td[i * MENU_TEX_NUM + TEXTURE_MENU_PANNEL_01].size = { 1273.0f, 125.0f };

	//	g_td[i * MENU_TEX_NUM + TEXTURE_START_JP].posType = POSITION_RELATIVE;
	//	g_td[i * MENU_TEX_NUM + TEXTURE_START_JP].pos = { -SCREEN_CENTER_X + 200.0f, SCREEN_CENTER_Y - TEXT_H * 1.75f };
	//}
	//g_td[TEXTURE_START_01].size = g_td[TEXTURE_START].size = { 282.0f, MENU_H };
	//g_td[TEXTURE_OPTION_01].size = g_td[TEXTURE_OPTION].size = { 368.0f, MENU_H };
	//g_td[TEXTURE_GALLERY_01].size = g_td[TEXTURE_GALLERY].size = { 416.0f, MENU_H };
	//g_td[TEXTURE_CREDIT_01].size = g_td[TEXTURE_CREDIT].size = { 335.0f, MENU_H };
	//g_td[TEXTURE_QUIT_01].size = g_td[TEXTURE_QUIT].size = { 227.0f, MENU_H };

	//g_td[TEXTURE_START_JP].size = { 217.0f, TEXT_H };
	//g_td[TEXTURE_OPTION_JP].size = { 293.0f, TEXT_H };
	//g_td[TEXTURE_GALLERY_JP].size = { 399.0f, TEXT_H };
	//g_td[TEXTURE_CREDIT_JP].size = { 600.0f, TEXT_H };
	//g_td[TEXTURE_QUIT_JP].size = { 121.0f, TEXT_H };


	// アニメーション初期設定
	//for (int i = 0; i < TEXTURE_MAX; i++) {
	//	g_td[i].pos.x -= SCREEN_WIDTH;
	//}
	//for (int i = TEXTURE_MENU_LINE; i < TEXTURE_MAX; i++) {
	//	g_td[i].col.w = g_AnimAlpha;
	//}
//#ifdef ANIM_SMALL
//	g_td[TEXTURE_MENUBOARD].scl = {
//		1.0f + ANIM_SMALL * (g_AnimAlpha - 1.0f),
//		1.0f + ANIM_SMALL * (g_AnimAlpha - 1.0f)
//	};
//#endif
	//PannelAnim();
	g_Model[MODEL_TITLE_STAR].model = MODEL_EARTH;
	g_Model[MODEL_TITLE_STAR].srt.pos = { 0.0f, 0.0f, 0.0f };
	g_Model[MODEL_TITLE_STAR].srt.scl = { 80.0f, 80.0f, 80.0f };

	g_Model[MODEL_TITLE_EARTH].model = MODEL_EARTH;
	g_Model[MODEL_TITLE_EARTH].srt.pos = { 120.0f, 0.0f, OBJ_DIST };
	g_Model[MODEL_TITLE_EARTH].srt.scl = { 20.0f, 20.0f, 20.0f };

	g_Load = TRUE;
	test = (TEXTURE_STAGE_JAPAN_MENU + g_Menu * 4);

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitStageSelect(void)
{
	if (g_Load == FALSE) return;

	g_Load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateStageSelect(void)
{

	if (GetKeyboardTrigger(DIK_W))
	{
		g_td[texnum].pos.y += 5.0f;
	}

	if (GetKeyboardTrigger(DIK_S))
	{
		g_td[texnum].pos.y -= 5.0f;
	}

	if (GetKeyboardTrigger(DIK_A))
	{
		g_td[texnum].pos.x -= 5.0f;
	}

	if (GetKeyboardTrigger(DIK_D))
	{
		g_td[texnum].pos.x += 5.0f;
	}

	if (GetKeyboardTrigger(DIK_SPACE))
	{
		texnum++;
		if (texnum > TEXTURE_ASIA_STAGE_3)
		{
			texnum = TEXTURE_ASIA_STAGE_1;
		}

	}
	else if (GetKeyboardTrigger(DIK_M))
	{
		texnum--;
		if (texnum < TEXTURE_ASIA_STAGE_1)
		{
			texnum = TEXTURE_ASIA_STAGE_3;
		}
	}

	if (GetKeyboardTrigger(DIK_UPARROW)) {


		if (isSelect == TRUE)
		{
			g_pressSelect = (g_pressSelect + 3 - 1) % 3;
			g_pannel = g_pannelNow + g_pressSelect;
			g_td[test2 + g_pressSelect ].col = COL_BLACK;
			g_td[test2 + g_pressSelect + 1].col = COL_ORIGINAL;
			if (g_pressSelect == 2)
			{
				g_td[test2].col = COL_ORIGINAL;
			}

		}
		else
		{
			g_Menu = (g_Menu + STAGE_NUM - 1) % STAGE_NUM;
			g_pannel = g_Menu;
			UpColChange(TEXTURE_STAGE_JAPAN, STAGE_NORTH_AMERICA);

		}


	}
	if (GetKeyboardTrigger(DIK_DOWNARROW)) {

		if (isSelect == TRUE)
		{
			g_pressSelect = (g_pressSelect + 1) % 3;
			g_pannel = g_pannelNow + g_pressSelect;
			g_td[test2 + g_pressSelect].col = COL_BLACK;
			g_td[test2 + g_pressSelect - 1].col = COL_ORIGINAL;
			if (g_pressSelect == 0)
			{
				g_td[test2 + 2].col = COL_ORIGINAL;
			}

		}
		else
		{

			g_Menu = (g_Menu + 1) % STAGE_NUM;
			g_pannel = g_Menu;
			DownColChange(TEXTURE_STAGE_JAPAN, STAGE_JAPAN, TEXTURE_STAGE_NORTH_AMERICA);

		}
	}


	//if (GetKeyboardTrigger(DIK_UPARROW))
	//{
	//	g_td[texnum--].tex;
	//}
	//else if (GetKeyboardTrigger(DIK_DOWNARROW))
	//{
	//	g_td[texnum++].tex;
	//}

	if (GetKeyboardTrigger(DIK_RETURN))
	{
		if (isSelect == TRUE)
		{
			SetFade(FADE_OUT, MODE_GAME);
		}
		else
		{
			switch (g_Menu)
			{
			case STAGE_JAPAN:
				isSelect = TRUE;
				test2 = TEXTURE_JAPAN_STAGE_1;
				for (int i = TEXTURE_STAGE_ASIA; i <= TEXTURE_STAGE_NORTH_AMERICA; i++)
				{
					g_td[i].pos.y += 360.0f;

				}
				g_td[TEXTURE_STAGE_NORTH_AMERICA].pos.x = 100.0f;
				PannelPlus();
				g_td[TEXTURE_JAPAN_STAGE_1].col = COL_BLACK;

				break;

			case STAGE_ASIA:
				isSelect = TRUE;
				test2 = TEXTURE_ASIA_STAGE_1;
				for (int i = TEXTURE_STAGE_EUROPE; i <= TEXTURE_STAGE_NORTH_AMERICA; i++)
				{
					g_td[i].pos.y += 360.0f;
				}
				g_td[TEXTURE_STAGE_NORTH_AMERICA].pos.x = 100.0f;
				g_td[TEXTURE_ASIA_STAGE_1].col = COL_BLACK;

				PannelPlus();
				break;

			case STAGE_EUROPE:
				isSelect = TRUE;
				test2 = TEXTURE_EUROPE_STAGE_1;
				g_td[TEXTURE_STAGE_NORTH_AMERICA].pos.y += 360.0f;
				g_td[TEXTURE_STAGE_NORTH_AMERICA].pos.x = 100.0f;
				g_td[TEXTURE_EUROPE_STAGE_1].col = COL_BLACK;

				PannelPlus();
				break;

			case STAGE_NORTH_AMERICA:
				isSelect = TRUE;
				test2 = TEXTURE_NORTH_AMERICA_STAGE_1;
				g_td[TEXTURE_NORTH_AMERICA_STAGE_1].col = COL_BLACK;
				PannelPlus();
				break;

			}

		}

	}

	if (GetKeyboardTrigger(DIK_L))
	{	
		isSelect = FALSE;
		g_td[test2 + g_pressSelect].col = COL_ORIGINAL;

		switch (g_Menu)
		{
		case STAGE_JAPAN:
			g_pannel = MENU_STAGE_1;
			g_td[TEXTURE_STAGE_ASIA].pos = { 250.0f, 290.0f };
			g_td[TEXTURE_STAGE_EUROPE].pos = { 192.0f, 408.0f };
			g_td[TEXTURE_STAGE_NORTH_AMERICA].pos = { 40.0f, 526.0f };
			break;
		case STAGE_ASIA:
			g_pannel = MENU_STAGE_2;
			g_td[TEXTURE_STAGE_EUROPE].pos = { 192.0f, 408.0f };
			g_td[TEXTURE_STAGE_NORTH_AMERICA].pos = { 40.0f, 526.0f };
			break;
		case STAGE_EUROPE:
			g_pannel = MENU_STAGE_3;
			g_td[TEXTURE_STAGE_NORTH_AMERICA].pos = { 40.0f, 526.0f };

			break;
		case STAGE_NORTH_AMERICA:
			g_pannel = MENU_STAGE_4;
			break;

		default:
			break;
		}
	}


	//PrintDebugProc("%f, %f", g_td[texnum].pos.x, g_td[texnum].pos.y);
	PrintDebugProc("%d", GetStageNumber());

	//if (!g_bStartOn)
	//{
	//	if (g_bStartFlg)
	//	{
	//		g_bStartFlg = FALSE;
	//		g_bStartOn = TRUE;
	//	}
	//	return;
	//}

	// ひっこみアニメーション
//	if (g_bStartOffFlg)
//	{
//		if (g_AnimAlpha > 0.0f)
//		{
//			g_AnimAlpha -= ANIM_SCALING;
//			for (int i = TEXTURE_MENU_LINE; i < TEXTURE_MAX; i++) g_td[i].col.w = g_AnimAlpha;
//
//#ifdef ANIM_SMALL
//			g_td[TEXTURE_MENUBOARD].scl = {
//				1.0f + ANIM_SMALL * (g_AnimAlpha - 1.0f),
//				1.0f + ANIM_SMALL * (g_AnimAlpha - 1.0f)
//			};
//#endif
//		}
//		else if (g_AnimSlide > -SCREEN_WIDTH)
//		{
//			g_AnimSlide -= ANIM_SLIDE;
//			for (int i = 0; i < TEXTURE_MAX; i++) g_td[i].pos.x -= ANIM_SLIDE;
//			PannelAnim();
//			SetTitleAlpha(-g_AnimSlide / SCREEN_WIDTH);
//		}
//		else
//		{
//			g_bStartOn = FALSE;
//			g_bStartOffFlg = FALSE;
//		}
//		return;
//	}
//
//	 //とびだしアニメーション
//	if (g_AnimSlide < 0.0f)
//	{
//		g_AnimSlide += ANIM_SLIDE;
//		for (int i = 0; i < TEXTURE_MAX; i++) {
//			g_td[i].pos.x += ANIM_SLIDE;
//		}
//		PannelAnim();
//		SetTitleAlpha(-g_AnimSlide / SCREEN_WIDTH);
//	}
//	else if (g_AnimAlpha < 1.0f)
//	{
//		g_AnimAlpha += ANIM_ALPHA;
//		for (int i = TEXTURE_MENU_LINE; i < TEXTURE_MAX; i++) g_td[i].col.w = g_AnimAlpha;
//
//#ifdef ANIM_SMALL
//		g_td[TEXTURE_MENUBOARD].scl = {
//			1.0f + ANIM_SMALL * (g_AnimAlpha - 1.0f),
//			1.0f + ANIM_SMALL * (g_AnimAlpha - 1.0f)
//		};
//#endif
//		// バラバラに色が付く
//		//g_td[TEXTURE_MENU_LINE].col.w = g_AnimAlpha;
//		//for (int i = 0; i < MENU_NUM; i++) {
//		//	g_td[i * MENU_TEX_NUM + TEXTURE_START].col.w = g_AnimAlpha - (float)i * 0.25f;
//		//}
//	}
//	else if (g_AnimScl < 1.0f)
//	{
//		g_AnimScl += ANIM_SCALING;
//		PannelAnim();
//	}
//
//	if (GetKeyboardTrigger(DIK_BACK)) {
//		g_bStartOffFlg = TRUE;
//	}
//
//	if (GetKeyboardTrigger(DIK_UPARROW)) {
//		g_Menu = (g_Menu + MENU_NUM - 1) % MENU_NUM;
//		g_AnimScl = 0.0f;
//		PannelAnim();
//	}
//	if (GetKeyboardTrigger(DIK_DOWNARROW)) {
//		g_Menu = (g_Menu + 1) % MENU_NUM;
//		g_AnimScl = 0.0f;
//		PannelAnim();
//	}
//

}

//=============================================================================
// 
//=============================================================================
void DrawStageSelect(void)
{
	//if (!g_bStartOn) return;
	//XMMATRIX mtxWorld;
	SRT srt;

	SetDrawNoLighting();

	// 地球
	DrawModel(&g_Model[MODEL_TITLE_EARTH].model, &g_Model[MODEL_TITLE_EARTH].srt);

	// 星
	SetCullingMode(CULL_MODE_NONE);
	DrawModel(&g_Model[MODEL_TITLE_STAR].model, &g_Model[MODEL_TITLE_STAR].srt, TEXTURE_LABEL_STAR);
	SetCullingMode(CULL_MODE_BACK);

	DrawTexture2D(&g_td[TEXTURE_STAGE_MENU], FALSE);	// メニュー背景
	DrawTexture2D(&g_td[TEXTURE_STAGE_CIRCLE], FALSE);	// メニュー詳細背景
	if (isSelect == TRUE)
	{
		DrawTexture2D(&g_td[TEXTURE_STAGE_JAPAN_MENU + g_Menu * 4], FALSE);	// メニュー詳細背景
		DrawTexture2D(&g_td[g_pannel + TEXTURE_STAGE_SELECT_1], FALSE);	// メニュー詳細背景
		DrawTexture2D(&g_td[(TEXTURE_STAGE_JAPAN_MENU + g_Menu * 4) + 1], FALSE);	// メニュー詳細背景
		DrawTexture2D(&g_td[(TEXTURE_STAGE_JAPAN_MENU + g_Menu * 4) + 2], FALSE);	// メニュー詳細背景
		DrawTexture2D(&g_td[(TEXTURE_STAGE_JAPAN_MENU + g_Menu * 4) + 3], FALSE);	// メニュー詳細背景

	}
	else
	{
		DrawTexture2D(&g_td[g_pannel + TEXTURE_STAGE_SELECT_1], FALSE);	// メニュー詳細背景
	}

	DrawTexture2D(&g_td[TEXTURE_STAGE_JAPAN], FALSE);	// メニュー詳細背景
	DrawTexture2D(&g_td[TEXTURE_STAGE_ASIA], FALSE);	// メニュー詳細背景
	DrawTexture2D(&g_td[TEXTURE_STAGE_EUROPE], FALSE);	// メニュー詳細背景
	DrawTexture2D(&g_td[TEXTURE_STAGE_NORTH_AMERICA], FALSE);	// メニュー詳細背景

	DrawTexture2D(&g_td[TEXTURE_STAGE_MENU_1], FALSE);	// メニュー詳細背景


}

//void PressedAnyButton(void) {
//	if (g_bStartOn) return;
//
//	g_bStartFlg = TRUE;
//	g_AnimScl = 0.0f;
//	g_AnimAlpha = 0.0f;
//	g_AnimSlide = -SCREEN_WIDTH;
//
//	for (int i = TEXTURE_MENU_LINE; i < TEXTURE_MAX; i++) g_td[i].col.w = g_AnimAlpha;
//	StopSound();
//	PlaySound(SOUND_LABEL_BGM_START);
//}

void DownColChange(int a, int b,int c)
{
	g_td[g_Menu + a].col = COL_BLACK;
	g_td[g_Menu + a - 1].col = COL_ORIGINAL;
	if (g_Menu == b)
	{
		g_td[c].col = COL_ORIGINAL;
	}

}


void UpColChange(int a, int b)
{
	g_td[g_Menu + a].col = COL_BLACK;
	g_td[g_Menu + a + 1].col = COL_ORIGINAL;
	if (g_Menu == b)
	{
		g_td[a].col = COL_ORIGINAL;
	}

}

void PannelPlus(void)
{
	g_pannel++;
	g_pannelNow = g_pannel;
	g_pressSelect = 0;
}

int GetStageNumber(void)
{
	switch (g_Menu)
	{
	case STAGE_JAPAN:

		return test2 + g_pressSelect - TEXTURE_JAPAN_STAGE_1;

		break;
	case STAGE_ASIA:

		return test2 + g_pressSelect - TEXTURE_JAPAN_STAGE_1 - 1;

		break;
	case STAGE_EUROPE:

		return test2 + g_pressSelect - TEXTURE_JAPAN_STAGE_1 - 2;

		break;
	case STAGE_NORTH_AMERICA:

		return test2 + g_pressSelect - TEXTURE_JAPAN_STAGE_1 - 3;

		break;

	default:
		break;
	}
}
