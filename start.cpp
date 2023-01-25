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
#include "load.h"

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
#define ANIM_SLIDE_X	(-1360.0f)	// スライド量

// メニュー部品の種類
enum {
	MENU_TEX_WHITE = 0,
	MENU_TEX_GREEN,
	MENU_TEX_TEXT,
	MENU_TEX_PANNEL,

	MENU_TEX_NUM,
};

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static float			g_AnimScl = 0.0f;				// アニメーション管理用
static float			g_AnimAlpha = 0.0f;				// アニメーション管理用
static float			g_AnimSlide = ANIM_SLIDE_X;	// アニメーション管理用
static BOOL				g_bStartOn = FALSE;
static BOOL				g_bStartFlg = FALSE;
static BOOL				g_bStartOffFlg = FALSE;
static BOOL				g_Load = FALSE;

//*****************************************************************************
// UI定義
//*****************************************************************************
// UI名（描画順）
enum UI_LABEL {
	UI_STAR = 0,

	UI_MENUBOARD,
	UI_MENU_LINE,

	UI_START,
	UI_START_01,
	UI_START_JP,
	UI_MENU_PANNEL_01,

	UI_OPTION,
	UI_OPTION_01,
	UI_OPTION_JP,
	UI_MENU_PANNEL_02,

	UI_GALLERY,
	UI_GALLERY_01,
	UI_GALLERY_JP,
	UI_MENU_PANNEL_03,

	UI_CREDIT,
	UI_CREDIT_01,
	UI_CREDIT_JP,
	UI_MENU_PANNEL_04,

	UI_QUIT,
	UI_QUIT_01,
	UI_QUIT_JP,
	UI_MENU_PANNEL_05,

	UI_NUM,
};
// 参照テクスチャ名（UI名順）
#define REF_TL {\
	TEXTURE_LABEL_STAR,\
\
	TEXTURE_LABEL_MENUBOARD,\
	TEXTURE_LABEL_MENU_LINE,\
\
	TEXTURE_LABEL_START,\
	TEXTURE_LABEL_START_01,\
	TEXTURE_LABEL_START_JP,\
	TEXTURE_LABEL_MENU_PANNEL_01,\
\
	TEXTURE_LABEL_OPTION,\
	TEXTURE_LABEL_OPTION_01,\
	TEXTURE_LABEL_OPTION_JP,\
	TEXTURE_LABEL_MENU_PANNEL_02,\
\
	TEXTURE_LABEL_GALLERY,\
	TEXTURE_LABEL_GALLERY_01,\
	TEXTURE_LABEL_GALLERY_JP,\
	TEXTURE_LABEL_MENU_PANNEL_03,\
\
	TEXTURE_LABEL_CREDIT,\
	TEXTURE_LABEL_CREDIT_01,\
	TEXTURE_LABEL_CREDIT_JP,\
	TEXTURE_LABEL_MENU_PANNEL_04,\
\
	TEXTURE_LABEL_QUIT,\
	TEXTURE_LABEL_QUIT_01,\
	TEXTURE_LABEL_QUIT_JP,\
	TEXTURE_LABEL_MENU_PANNEL_05,\
}
// UI詳細管理
static TEXTURE2D_DESC* g_td = NULL;

//*****************************************************************************
// ボタン定義
//*****************************************************************************
// ボタン名（優先順）
enum BT_LABEL {
	BT_START = 0,
	BT_OPTION,
	BT_GALLERY,
	BT_CREDIT,
	BT_QUIT,

	BT_NUM
};
// 参照UI名（ボタン名順）
#define REF_UL {\
	UI_MENU_PANNEL_01,\
	UI_MENU_PANNEL_02,\
	UI_MENU_PANNEL_03,\
	UI_MENU_PANNEL_04,\
	UI_MENU_PANNEL_05,\
}
// ボタン詳細管理
static BUTTON_DESC* g_bd = NULL;
// ボタン表
#define BT_NUM_X 1
#define BT_NUM_Y 5
static int g_btTbl[BT_NUM_Y][BT_NUM_X] = {
	{BT_START},
	{BT_OPTION},
	{BT_GALLERY},
	{BT_CREDIT},
	{BT_QUIT},
};
static BUTTON_TABLE* g_bt = NULL;
// カーソル位置
static XMINT2 g_cursor = { 0, 0 };	// BT_STARTから
// ボタンごとの処理
static void ButtonPressed(int b)
{
	switch (b)
	{
	case BT_START:
		SetFade(FADE_OUT, MODE_HOME); // game
		PlaySound(SOUND_LABEL_SE_DECIDE);
		break;

	case BT_OPTION:
		PlaySound(SOUND_LABEL_SE_DECIDE);
		break;

	case BT_GALLERY:
		PlaySound(SOUND_LABEL_SE_DECIDE);
		break;

	case BT_CREDIT:
		PlaySound(SOUND_LABEL_SE_DECIDE);
		break;

	case BT_QUIT:
		SetFade(FADE_OUT, MODE_END);
		PlaySound(SOUND_LABEL_SE_DECIDE);
		break;

	default:
		g_bStartOffFlg = TRUE;
		break;
	}
}
// 初期化
static void InitUI(void)
{
	// メモリ確保
	g_td = new TEXTURE2D_DESC[UI_NUM];
	g_bd = new BUTTON_DESC[BT_NUM];

	// UI詳細設定
	TEXTURE_LABEL tl[UI_NUM] = REF_TL;
	for (int i = 0; i < UI_NUM; i++)
	{
		// テクスチャの対応付け
		g_td[i].tex = tl[i];

		// テクスチャサイズの取得
		g_td[i].size = GetTextureSize(g_td[i].tex);

		// 一括設定
		g_td[i].ctrType = CENTER_LEFTTOP;
		g_td[i].posType = POSITION_LEFTTOP;
		g_td[i].wight = 0.0f;
	}

	// メニューの土台
	g_td[UI_MENUBOARD].pos = { 0.0f, 0.0f };

	// メニュー飾り
	g_td[UI_MENU_LINE].pos = { 677.5f, 0.0f };

	// メニュー
	for (int i = 0; i < BT_NUM_Y; i++) {
		g_td[i * MENU_TEX_NUM + UI_START_01].pos = g_td[i * MENU_TEX_NUM + UI_START].pos = { 200.0f, MENU_BW * (float)(i + 1) };

		g_td[i * MENU_TEX_NUM + UI_MENU_PANNEL_01].pos = { 0.0f, MENU_BW * (float)(i + 1) - 8.0f };

		g_td[i * MENU_TEX_NUM + UI_START_JP].posType = POSITION_RELATIVE;
		g_td[i * MENU_TEX_NUM + UI_START_JP].pos = { -SCREEN_CENTER_X + 200.0f, SCREEN_CENTER_Y - TEXT_H * 1.75f };
	}

	// ボタン詳細設定
	UI_LABEL ul[BT_NUM] = REF_UL;
	for (int i = 0; i < BT_NUM; i++)
	{
		g_bd[i].col_off = g_bd[i].col_on;
		g_bd[i].scl_off = g_bd[i].scl_on;
		SetUIButton(&g_bd[i], &g_td[ul[i]]);
	}

	// ボタンテーブルへの登録
	g_bt = new BUTTON_TABLE;
	*g_bt = { &g_btTbl[0][0], BT_NUM_X, BT_NUM_Y, g_bd, BT_NUM, &g_cursor };

}
// 更新
static void UpdateUI(void)
{
	if (g_AnimScl < 1.0f) return;
	UpdateButton(g_bt, ButtonPressed);
}
// 描画
static void DrawUI(void)
{
	for (int i = 0; i < UI_NUM; i++)
	{
		DrawTexture2D(&g_td[i], TRUE);
	}
}
// 終了
static void UninitUI(void)
{
	delete[] g_td, g_bd;
	delete g_bt;
}

//*****************************************************************************
// ローカル関数
//*****************************************************************************
// メニューの種類と部品名からテクスチャ名を取得
static int GetTexNo(int menu_tex) {
	return UI_START + MENU_TEX_NUM * g_cursor.y + menu_tex;
}
// パネルのアニメーション
static void PannelAnim(void)
{
	int tex = GetTexNo(MENU_TEX_PANNEL);
	g_td[tex].scl.y = g_AnimScl;
	g_td[tex].uv_pos.v = 0.5f - g_AnimScl * 0.5f;
	g_td[tex].uv_pos.vh = g_AnimScl;
	
	tex = GetTexNo(MENU_TEX_GREEN);
	g_td[tex].scl.y = g_AnimScl;
	g_td[tex].uv_pos.v = 0.5f - g_AnimScl * 0.5f;
	g_td[tex].uv_pos.vh = g_AnimScl;
}

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitStart(void)
{
	if (g_Load) return S_OK;
	
	InitUI();

	// アニメーション初期設定
	g_AnimSlide = ANIM_SLIDE_X;
	for (int i = 0; i < UI_NUM; i++) {
		g_td[i].posAdd.x = g_AnimSlide;
	}

	g_AnimAlpha = 0.0f;
	for (int i = UI_MENU_LINE; i < UI_NUM; i++) {
		g_td[i].col.w = g_AnimAlpha;
	}

#ifdef ANIM_SMALL
	g_td[UI_MENUBOARD].scl = {
		1.0f + ANIM_SMALL * (g_AnimAlpha - 1.0f),
		1.0f + ANIM_SMALL * (g_AnimAlpha - 1.0f)
	};
#endif

	g_AnimScl = 0.0f;
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

	UninitUI();
	
	g_Load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateStart(void)
{
	if (!CheckFadeIn()) return;

	if (!g_bStartOn)
	{
		if (g_bStartFlg)
		{
			g_bStartFlg = FALSE;
			g_bStartOn = TRUE;
		}
		return;
	}

	if (g_AnimAlpha > 0.0f) UpdateUI();

	static int old_cur_y = g_cursor.y;
	if (old_cur_y != g_cursor.y) {
		g_AnimScl = 0.0f;
		PannelAnim();
		old_cur_y = g_cursor.y;

		PlaySound(SOUND_LABEL_SE_CURSOR);
	}

	// ひっこみアニメーション
	if (g_bStartOffFlg)
	{
		if (g_AnimAlpha > 0.0f)
		{
			g_AnimAlpha -= ANIM_SCALING;
			for (int i = UI_MENU_LINE; i < UI_NUM; i++) g_td[i].col.w = g_AnimAlpha;

#ifdef ANIM_SMALL
			g_td[UI_MENUBOARD].scl = {
				1.0f + ANIM_SMALL * (g_AnimAlpha - 1.0f),
				1.0f + ANIM_SMALL * (g_AnimAlpha - 1.0f)
			};
#endif
		}
		else if (g_AnimSlide > ANIM_SLIDE_X)
		{
			g_AnimSlide -= ANIM_SLIDE;
			for (int i = 0; i < UI_NUM; i++) {
				g_td[i].posAdd.x = g_AnimSlide;
			}
			PannelAnim();
			SetTitleAlpha(g_AnimSlide / ANIM_SLIDE_X);
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
		for (int i = 0; i < UI_NUM; i++) {
			g_td[i].posAdd.x = g_AnimSlide;
		}
		PannelAnim();
		SetTitleAlpha(g_AnimSlide / ANIM_SLIDE_X);
		//return;
	}
	else if (g_AnimAlpha < 1.0f)
	{
		g_AnimAlpha += ANIM_ALPHA;
		for (int i = UI_MENU_LINE; i < UI_NUM; i++) g_td[i].col.w = g_AnimAlpha;

#ifdef ANIM_SMALL
		g_td[UI_MENUBOARD].scl = {
			1.0f + ANIM_SMALL * (g_AnimAlpha - 1.0f),
			1.0f + ANIM_SMALL * (g_AnimAlpha - 1.0f)
		};
#endif
		//// バラバラに色が付く
		//g_td[UI_MENU_LINE].col.w = g_AnimAlpha;
		//for (int i = 0; i < BT_NUM; i++) {
		//	g_td[i * MENU_TEX_NUM + UI_START].col.w = g_AnimAlpha * 2.0f - (float)i * 0.25f;
		//}
		//return;
	}
	else if (g_AnimScl < 1.0f)
	{
		g_AnimScl += ANIM_SCALING;
		PannelAnim();
	}

	if (GetKeyboardTrigger(DIK_BACK)) {
		g_bStartOffFlg = TRUE;
	}
}

//=============================================================================
// 
//=============================================================================
void DrawStart(void)
{
	if (!g_bStartOn) return;
	//DrawUI();

	DrawTexture2D(&g_td[UI_MENUBOARD]);	// メニュー背景
	DrawTexture2D(&g_td[UI_MENU_LINE]);	// メニュー装飾線

	DrawTexture2D(&g_td[UI_START]);		// スタート
	DrawTexture2D(&g_td[UI_OPTION]);	// オプション
	DrawTexture2D(&g_td[UI_GALLERY]);	// ギャラリー
	DrawTexture2D(&g_td[UI_CREDIT]);	// クレジット
	DrawTexture2D(&g_td[UI_QUIT]);		// 退出

	DrawTexture2D(&g_td[GetTexNo(MENU_TEX_TEXT)], TRUE);			// メニュー詳細

	if (g_AnimScl == 0.0f) return;

	static float time = 0.0f;
	time += 0.05f; if (time > XM_2PI) time -= XM_2PI;
	if (g_AnimScl >= 1.0f) {
		g_td[GetTexNo(MENU_TEX_GREEN)].scl.x = 1.0f + 0.025f * sinf(time);
		g_td[GetTexNo(MENU_TEX_GREEN)].scl.y = 1.0f + 0.025f * sinf(time);
	}

	DrawTexture2D(&g_td[GetTexNo(MENU_TEX_PANNEL)], FALSE, TRUE);	// メニューパネル
	DrawTexture2D(&g_td[GetTexNo(MENU_TEX_GREEN)], TRUE, TRUE);		// メニュー名
}

void PressedAnyButton(void)
{
	if (!GetLoadAfter()) return;
	if (g_bStartOn) return;

	g_bStartFlg = TRUE;
	g_AnimScl = 0.0f;
	g_AnimAlpha = 0.0f;
	g_AnimSlide = ANIM_SLIDE_X;

	for (int i = UI_MENU_LINE; i < UI_NUM; i++) {
		g_td[i].col.w = g_AnimAlpha;
	}

	PlaySound(SOUND_LABEL_SE_DECIDE);
	//StopSound();
	//PlaySound(SOUND_LABEL_BGM_START);
}
