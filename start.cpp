//=============================================================================
//
// スタート画面 [start.cpp]
// Author : サン
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "texture2d.h"
#include "start.h"
#include "fade.h"
#include "input.h"
#include "sound.h"
#include "title.h"

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

// メニューの種類
enum {
	MENU_START = 0,
	MENU_OPTION,
	MENU_GALLERY,
	MENU_CREDIT,
	MENU_QUIT,

	MENU_NUM,
};

// メニュー部品の種類
enum {
	MENU_TEX_WHITE = 0,
	MENU_TEX_GREEN,
	MENU_TEX_TEXT,
	MENU_TEX_PANNEL,

	MENU_TEX_NUM,
};

// メニューテクスチャ名
enum {
	TEXTURE_STAR = 0,
	TEXTURE_MENUBOARD,
	TEXTURE_MENU_LINE,

	TEXTURE_START,
	TEXTURE_START_01,
	TEXTURE_START_JP,
	TEXTURE_MENU_PANNEL_01,

	TEXTURE_OPTION,
	TEXTURE_OPTION_01,
	TEXTURE_OPTION_JP,
	TEXTURE_MENU_PANNEL_02,

	TEXTURE_GALLERY,
	TEXTURE_GALLERY_01,
	TEXTURE_GALLERY_JP,
	TEXTURE_MENU_PANNEL_03,

	TEXTURE_CREDIT,
	TEXTURE_CREDIT_01,
	TEXTURE_CREDIT_JP,
	TEXTURE_MENU_PANNEL_04,

	TEXTURE_QUIT,
	TEXTURE_QUIT_01,
	TEXTURE_QUIT_JP,
	TEXTURE_MENU_PANNEL_05,

	TEXTURE_BG,
	TEXTURE_MAX,
};

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static int				g_Menu = MENU_START;			// 選択されているメニュー管理用
static TEXTURE2D_DESC	g_td[TEXTURE_MAX];				// UI管理用
static float			g_AnimScl = 0.0f;				// アニメーション管理用
static float			g_AnimAlpha = 0.0f;				// アニメーション管理用
static float			g_AnimSlide = -SCREEN_WIDTH;	// アニメーション管理用
static BOOL				g_Load = FALSE;
static BOOL				g_bStartOn = FALSE;
static BOOL				g_bStartFlg = FALSE;
static BOOL				g_bStartOffFlg = FALSE;

//*****************************************************************************
// ローカル関数
//*****************************************************************************
// メニューの種類と部品名からテクスチャ名を取得
int GetTexNo(int menu_tex) {
	return TEXTURE_START + MENU_TEX_NUM * g_Menu + menu_tex;
}
// パネルのアニメーション
void PannelAnim(void) {
	g_td[GetTexNo(MENU_TEX_PANNEL)].scl.y = g_AnimScl;
	g_td[GetTexNo(MENU_TEX_PANNEL)].uv_pos.v = 0.5f - g_AnimScl * 0.5f;
	g_td[GetTexNo(MENU_TEX_PANNEL)].uv_pos.vh = g_AnimScl;

	g_td[GetTexNo(MENU_TEX_GREEN)].scl.y = g_AnimScl;
	g_td[GetTexNo(MENU_TEX_GREEN)].uv_pos.v = 0.5f - g_AnimScl * 0.5f;
	g_td[GetTexNo(MENU_TEX_GREEN)].uv_pos.vh = g_AnimScl;
}

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitStart(void)
{
	// UI設定 ///////////////////////

	// 一括設定
	for (int i = 0; i < TEXTURE_MAX; i++) {
		g_td[i].tex = (TEXTURE_LABEL)(TEXTURE_LABEL_STAR + i);
		g_td[i].ctrType = CENTER_LEFTTOP;
		g_td[i].posType = POSITION_ABSOLUTE;
	}
	g_td[TEXTURE_BG].tex = TEXTURE_LABEL_WHITE;

	// メニューの土台
	g_td[TEXTURE_MENUBOARD].size = { 1340.0f, SCREEN_HEIGHT };
	g_td[TEXTURE_MENUBOARD].pos = { 0.0f, 0.0f };

	// メニュー飾り
	g_td[TEXTURE_MENU_LINE].size = { 601.0f, SCREEN_HEIGHT };
	g_td[TEXTURE_MENU_LINE].pos = { 677.5f, 0.0f };

	// メニュー
	for (int i = 0; i < MENU_NUM; i++) {
		g_td[i * MENU_TEX_NUM + TEXTURE_START_01].pos = g_td[i * MENU_TEX_NUM + TEXTURE_START].pos = { 200.0f, MENU_BW * (float)(i + 1) };

		g_td[i * MENU_TEX_NUM + TEXTURE_MENU_PANNEL_01].pos = { 0.0f, MENU_BW * (float)(i + 1) - 8.0f };
		g_td[i * MENU_TEX_NUM + TEXTURE_MENU_PANNEL_01].size = { 1273.0f, 125.0f };

		g_td[i * MENU_TEX_NUM + TEXTURE_START_JP].posType = POSITION_RELATIVE;
		g_td[i * MENU_TEX_NUM + TEXTURE_START_JP].pos = { -SCREEN_CENTER_X + 200.0f, SCREEN_CENTER_Y - TEXT_H * 1.75f };
	}
	g_td[TEXTURE_START_01].size = g_td[TEXTURE_START].size = { 282.0f, MENU_H };
	g_td[TEXTURE_OPTION_01].size = g_td[TEXTURE_OPTION].size = { 368.0f, MENU_H };
	g_td[TEXTURE_GALLERY_01].size = g_td[TEXTURE_GALLERY].size = { 416.0f, MENU_H };
	g_td[TEXTURE_CREDIT_01].size = g_td[TEXTURE_CREDIT].size = { 335.0f, MENU_H };
	g_td[TEXTURE_QUIT_01].size = g_td[TEXTURE_QUIT].size = { 227.0f, MENU_H };

	g_td[TEXTURE_START_JP].size = { 217.0f, TEXT_H };
	g_td[TEXTURE_OPTION_JP].size = { 293.0f, TEXT_H };
	g_td[TEXTURE_GALLERY_JP].size = { 399.0f, TEXT_H };
	g_td[TEXTURE_CREDIT_JP].size = { 600.0f, TEXT_H };
	g_td[TEXTURE_QUIT_JP].size = { 121.0f, TEXT_H };


	// アニメーション初期設定
	for (int i = 0; i < TEXTURE_MAX; i++) {
		g_td[i].pos.x -= SCREEN_WIDTH;
	}
	for (int i = TEXTURE_MENU_LINE; i < TEXTURE_MAX; i++) {
		g_td[i].col.w = g_AnimAlpha;
	}
#ifdef ANIM_SMALL
	g_td[TEXTURE_MENUBOARD].scl = {
		1.0f + ANIM_SMALL * (g_AnimAlpha - 1.0f),
		1.0f + ANIM_SMALL * (g_AnimAlpha - 1.0f)
	};
#endif
	PannelAnim();

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitStart(void)
{
	if (g_Load == FALSE) return;

	g_Load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateStart(void)
{
	if (!g_bStartOn)
	{
		if (g_bStartFlg)
		{
			g_bStartFlg = FALSE;
			g_bStartOn = TRUE;
		}
		return;
	}

	// ひっこみアニメーション
	if (g_bStartOffFlg)
	{
		if (g_AnimAlpha > 0.0f)
		{
			g_AnimAlpha -= ANIM_SCALING;
			for (int i = TEXTURE_MENU_LINE; i < TEXTURE_MAX; i++) g_td[i].col.w = g_AnimAlpha;

#ifdef ANIM_SMALL
			g_td[TEXTURE_MENUBOARD].scl = {
				1.0f + ANIM_SMALL * (g_AnimAlpha - 1.0f),
				1.0f + ANIM_SMALL * (g_AnimAlpha - 1.0f)
			};
#endif
		}
		else if (g_AnimSlide > -SCREEN_WIDTH)
		{
			g_AnimSlide -= ANIM_SLIDE;
			for (int i = 0; i < TEXTURE_MAX; i++) g_td[i].pos.x -= ANIM_SLIDE;
			PannelAnim();
			SetTitleAlpha(-g_AnimSlide / SCREEN_WIDTH);
		}
		else
		{
			g_bStartOn = FALSE;
			g_bStartOffFlg = FALSE;
		}
		return;
	}

	// とびだしアニメーション
	if (g_AnimSlide < 0.0f)
	{
		g_AnimSlide += ANIM_SLIDE;
		for (int i = 0; i < TEXTURE_MAX; i++) {
			g_td[i].pos.x += ANIM_SLIDE;
		}
		PannelAnim();
		SetTitleAlpha(-g_AnimSlide / SCREEN_WIDTH);
	}
	else if (g_AnimAlpha < 1.0f)
	{
		g_AnimAlpha += ANIM_ALPHA;
		for (int i = TEXTURE_MENU_LINE; i < TEXTURE_MAX; i++) g_td[i].col.w = g_AnimAlpha;

#ifdef ANIM_SMALL
		g_td[TEXTURE_MENUBOARD].scl = {
			1.0f + ANIM_SMALL * (g_AnimAlpha - 1.0f),
			1.0f + ANIM_SMALL * (g_AnimAlpha - 1.0f)
		};
#endif
		// バラバラに色が付く
		//g_td[TEXTURE_MENU_LINE].col.w = g_AnimAlpha;
		//for (int i = 0; i < MENU_NUM; i++) {
		//	g_td[i * MENU_TEX_NUM + TEXTURE_START].col.w = g_AnimAlpha - (float)i * 0.25f;
		//}
	}
	else if (g_AnimScl < 1.0f)
	{
		g_AnimScl += ANIM_SCALING;
		PannelAnim();
	}

	if (GetKeyboardTrigger(DIK_BACK)) {
		g_bStartOffFlg = TRUE;
	}

	if (GetKeyboardTrigger(DIK_UPARROW)) {
		g_Menu = (g_Menu + MENU_NUM - 1) % MENU_NUM;
		g_AnimScl = 0.0f;
		PannelAnim();
	}
	if (GetKeyboardTrigger(DIK_DOWNARROW)) {
		g_Menu = (g_Menu + 1) % MENU_NUM;
		g_AnimScl = 0.0f;
		PannelAnim();
	}


	if (GetKeyboardTrigger(DIK_RETURN))
	{
		switch (g_Menu)
		{
		case MENU_START:
			SetMode(MODE_HOME); // game
			break;

		case MENU_OPTION:
			//SetMode(); //option
			break;

		case MENU_GALLERY:
			//SetMode(); //gallary
			break;

		case MENU_CREDIT:
			//SetMode(); //credit
			break;

		case MENU_QUIT:
			exit(-1); //quit
			break;
		}
	}
}

//=============================================================================
// 
//=============================================================================
void DrawStart(void)
{
	if (!g_bStartOn) return;

	DrawTexture2D(&g_td[TEXTURE_MENUBOARD], FALSE);	// メニュー背景
	DrawTexture2D(&g_td[TEXTURE_MENU_LINE], FALSE);	// メニュー詳細背景

	DrawTexture2D(&g_td[TEXTURE_START], FALSE);		// スタート
	DrawTexture2D(&g_td[TEXTURE_OPTION], FALSE);	// オプション
	DrawTexture2D(&g_td[TEXTURE_GALLERY], FALSE);	// ギャラリー
	DrawTexture2D(&g_td[TEXTURE_CREDIT], FALSE);	// クレジット
	DrawTexture2D(&g_td[TEXTURE_QUIT], FALSE);		// 退出

	DrawTexture2D(&g_td[GetTexNo(MENU_TEX_PANNEL)], FALSE, TRUE);	// メニューパネル
	DrawTexture2D(&g_td[GetTexNo(MENU_TEX_GREEN)], TRUE, TRUE);		// メニュー名
	DrawTexture2D(&g_td[GetTexNo(MENU_TEX_TEXT)], TRUE);			// メニュー詳細
}

void PressedAnyButton(void) {
	if (g_bStartOn) return;

	g_bStartFlg = TRUE;
	g_AnimScl = 0.0f;
	g_AnimAlpha = 0.0f;
	g_AnimSlide = -SCREEN_WIDTH;

	for (int i = TEXTURE_MENU_LINE; i < TEXTURE_MAX; i++) g_td[i].col.w = g_AnimAlpha;
	//StopSound();
	//PlaySound(SOUND_LABEL_BGM_START);
}
