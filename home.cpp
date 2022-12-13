//=============================================================================
//
// チームロゴ画面処理 [home.cpp]
// Author : 葉志成
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "fade.h"
#include "home.h"
#include "texture2d.h"
#include "character_select.h"
#include "rocket_select.h"
#include "wallet.h"
#include "input.h"
#include "collision.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define ARROW_Y			(-200.0f)
#define ANIM_ALPHA		(0.1f)		// メニューが色づくスピード

#define ANIM_SLIDE		(70.0f)		// メニューがスライドしてくるスピード
#define STATUS_SLIDE_Y	(-453.0f)
#define STATUS_SLIDE_Y_PLUS	(63.0f)

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static BOOL				g_Load = FALSE;
static HOME_MODE		g_Home = HOME_HOME;
static float			g_leftArrow = 0.0f;
static float			g_rightArrow = 0.0f;
static float			g_AnimAlpha = 0.0f;				// アニメーション管理用
static BOOL				g_bButton = FALSE;
static BOOL				g_bStatus = FALSE;
static float			g_AnimStatusSlide = STATUS_SLIDE_Y;	// アニメーション管理用

//*****************************************************************************
// UI定義
//*****************************************************************************
// UI名（描画順）
enum UI_LABEL {
	UI_BG = 0,
	UI_ICON,
	UI_NAME_PANNEL,
	UI_ROCKET_NAME,
	UI_PLAYER,
	UI_START,
	UI_LEFT,
	UI_RIGHT,
	UI_BACK,
	UI_SHOP,

	UI_STATUS_PANNEL,

	UI_NUM
};
// 参照テクスチャ名（UI名順）
#define REF_TL {\
	TEXTURE_LABEL_BG,\
	TEXTURE_LABEL_ICON,\
	TEXTURE_LABEL_NAME_PANNEL,\
	TEXTURE_LABEL_ROCKET_1,\
	TEXTURE_LABEL_CHARACTER_01,\
	TEXTURE_LABEL_START_BUTTON_2,\
	TEXTURE_LABEL_LEFT,\
	TEXTURE_LABEL_RIGHT,\
	TEXTURE_LABEL_BACK_BAR_LEFT,\
	TEXTURE_LABEL_ICON,\
\
	TEXTURE_LABEL_STATUS_PANNEL_UP,\
}
// UI詳細管理
static TEXTURE2D_DESC* g_td_home;

//*****************************************************************************
// ボタン定義
//*****************************************************************************
// ボタン名（優先順）
enum BT_LABEL {
	BT_START = 0,
	BT_LEFT,
	BT_RIGHT,
	BT_SHOP,
	BT_BACK,
	BT_STATUS_PANNEL,

	BT_NUM
};
// 参照UI名（ボタン名順）
#define REF_UL {\
	UI_START,\
	UI_LEFT,\
	UI_RIGHT,\
	UI_SHOP,\
	UI_BACK,\
	UI_STATUS_PANNEL,\
}
// ボタン詳細管理
static BUTTON_DESC* g_bd;
// ボタン表
#define BT_NUM_X 2
#define BT_NUM_Y 4
static int g_btTbl[BT_NUM_Y][BT_NUM_X] = {
	{BT_BACK, BT_BACK},
	{BT_STATUS_PANNEL, BT_STATUS_PANNEL},
	{BT_LEFT, BT_RIGHT},
	{BT_SHOP, BT_START},
};
static BUTTON_TABLE g_bt;
// カーソル位置
static XMINT2 g_cursor = { BT_NUM_X - 1, BT_NUM_Y - 1 };	// BT_STARTから
// ボタンごとの処理
static void ButtonPressed(int b)
{
	if (g_bStatus) {
		g_bStatus = FALSE;
		g_bButton = FALSE;
		return;
	}

	switch (b)
	{
	case BT_START:
		SetFade(FADE_OUT, MODE_STAGESELECT);
		break;

	case BT_LEFT:
		break;

	case BT_RIGHT:
		break;

	case BT_SHOP:
		SetHomeMode(HOME_SHOP);
		g_bt.bd[BT_SHOP].b_on = FALSE;
		break;

	case BT_STATUS_PANNEL:
		g_bStatus = TRUE;
		break;

	case BT_BACK:
	case BT_BACKSPACE:
		SetFade(FADE_OUT, MODE_TITLE_START);
		break;
	}
	g_bButton = FALSE;	// ボタンを一瞬オフにする
}
// 初期化
static void InitUI(void)
{
	// メモリ確保
	g_td_home = new TEXTURE2D_DESC[UI_NUM];
	g_bd = new BUTTON_DESC[BT_NUM];

	// UI詳細設定
	TEXTURE_LABEL tl[UI_NUM] = REF_TL;
	for (int i = 0; i < UI_NUM; i++)
	{
		// テクスチャの対応付け
		g_td_home[i].tex = tl[i];

		// テクスチャサイズの取得
		g_td_home[i].size = GetTextureSize(g_td_home[i].tex);
	}
	g_td_home[UI_START].posType = POSITION_RIGHTBOTTOM;
	g_td_home[UI_START].ctrType = CENTER_BOTTOM;
	g_td_home[UI_START].pos = { -g_td_home[UI_START].size.x * 0.5f, -30.0f };
	//g_td_home[UI_START].b_useOutline = TRUE;
	g_bd[BT_START].col_on = { 1.0f, 1.0f, 0.0f, 1.0f };
	g_bd[BT_START].col_off = { 1.0f, 1.0f, 1.0f, 0.7f };

	g_td_home[UI_ICON].posType = POSITION_LEFTTOP;
	g_td_home[UI_ICON].ctrType = CENTER_LEFTTOP;
	g_td_home[UI_ICON].scl = { 1.0f, 1.0f };

	g_td_home[UI_NAME_PANNEL].posType = POSITION_LEFTTOP;
	g_td_home[UI_NAME_PANNEL].ctrType = CENTER_LEFTTOP;
	g_td_home[UI_NAME_PANNEL].pos = { 20.0f, 55.0f + STATUS_SLIDE_Y_PLUS };

	g_td_home[UI_ROCKET_NAME].posType = POSITION_LEFTTOP;
	g_td_home[UI_ROCKET_NAME].ctrType = CENTER_LEFT;
	g_td_home[UI_ROCKET_NAME].col = { 0.0f, 0.3f, 0.0f, 1.0f };
	g_td_home[UI_ROCKET_NAME].pos = {
		g_td_home[UI_NAME_PANNEL].pos.x + g_td_home[UI_ICON].size.x * 1.1f,
		g_td_home[UI_NAME_PANNEL].pos.y + g_td_home[UI_NAME_PANNEL].size.y * 0.575f
	};
	g_td_home[UI_ROCKET_NAME].sd_pos = { 3.0f, 3.0f };

	g_td_home[UI_PLAYER].posType = POSITION_RIGHT;
	g_td_home[UI_PLAYER].ctrType = CENTER_CENTER;
	g_td_home[UI_PLAYER].pos = { g_td_home[UI_START].pos.x, 0.0f };
	g_td_home[UI_PLAYER].wight = 10.0f;

	g_td_home[UI_LEFT].posType = POSITION_RIGHT;
	g_td_home[UI_LEFT].ctrType = CENTER_RIGHT;
	g_leftArrow = g_td_home[UI_PLAYER].pos.x - g_td_home[UI_PLAYER].size.x * 0.5f;
	g_td_home[UI_LEFT].pos = { g_leftArrow, ARROW_Y };
	g_bd[BT_LEFT].col_on = { 1.0f, 1.0f, 0.0f, 1.0f };
	g_bd[BT_LEFT].col_off = { 1.0f, 1.0f, 1.0f, 1.0f };

	g_td_home[UI_RIGHT].posType = POSITION_RIGHT;
	g_td_home[UI_RIGHT].ctrType = CENTER_LEFT;
	g_rightArrow = g_td_home[UI_PLAYER].pos.x + g_td_home[UI_PLAYER].size.x * 0.5f;
	g_td_home[UI_RIGHT].pos = { g_rightArrow, ARROW_Y };
	g_bd[BT_RIGHT].col_on = { 1.0f, 1.0f, 0.0f, 1.0f };
	g_bd[BT_RIGHT].col_off = { 1.0f, 1.0f, 1.0f, 1.0f };


	g_td_home[UI_SHOP].posType = POSITION_LEFT;
	g_td_home[UI_SHOP].ctrType = CENTER_CENTER;
	g_td_home[UI_SHOP].pos = { SCREEN_CENTER_X * 0.5f, 0.0f };
	g_td_home[UI_SHOP].size.x *= 2.0f;
	g_td_home[UI_SHOP].size.y *= 2.0f;

	g_td_home[UI_BACK].posType = POSITION_LEFTTOP;
	g_td_home[UI_BACK].ctrType = CENTER_LEFTTOP;
	g_bd[BT_BACK].col_on = { 1.0f, 1.0f, 0.0f, 1.0f };
	g_bd[BT_BACK].col_off = { 1.0f, 1.0f, 1.0f, 1.0f };
	g_bd[BT_BACK].scl_off = g_bd[BT_BACK].scl_on;
	//g_td_home[UI_BACK].pos = { -30.0f, 15.0f };
	//g_td_home[UI_BACK].sd_pos = { 2.0f, 2.0f };
	//g_bd[BT_BACK].col_on = { 100.0f, 100.0f, 0.0f, 1.0f };
	//g_bd[BT_BACK].col_off = { 1.0f, 1.0f, 1.0f, 1.0f };

	g_td_home[UI_STATUS_PANNEL].posType = POSITION_LEFTTOP;
	g_td_home[UI_STATUS_PANNEL].ctrType = CENTER_LEFTTOP;
	g_td_home[UI_STATUS_PANNEL].pos = { -5.0f, -5.0f };
	g_td_home[UI_STATUS_PANNEL].pos.y = STATUS_SLIDE_Y_PLUS + g_AnimStatusSlide;
	g_bd[BT_STATUS_PANNEL].col_on = { 1.0f, 1.0f, 0.0f, 1.0f };
	g_bd[BT_STATUS_PANNEL].col_off = { 1.0f, 1.0f, 1.0f, 1.0f };
	g_bd[BT_STATUS_PANNEL].scl_off = g_bd[BT_STATUS_PANNEL].scl_on;

	// ボタン詳細設定
	UI_LABEL ul[BT_NUM] = REF_UL;
	for (int i = 0; i < BT_NUM; i++)
	{
		SetUIButton(&g_bd[i], &g_td_home[ul[i]]);
	}
	g_bd[BT_BACK].size.y = g_bd[BT_BACK].size.y * 0.5f;
	g_bd[BT_BACK].pos.y -= g_bd[BT_BACK].size.y * 0.5f;
	//g_bd[BT_START].tex_on = TEXTURE_LABEL_START_BUTTON_3;

	// ボタンテーブルへの登録
	g_bt = { &g_btTbl[0][0], BT_NUM_X, BT_NUM_Y, g_bd, BT_NUM, &g_cursor };

}
// 更新
static void UpdateUI(void)
{
	static float time = 0.0f;
	time += 0.03f; if (time > XM_2PI) time -= XM_2PI;
	g_td_home[UI_PLAYER].pos.y = 3.0f * sinf(time);
	g_td_home[UI_LEFT].pos.x = g_leftArrow - 5.0f * cosf(time * 2.0f);
	g_td_home[UI_RIGHT].pos.x = g_rightArrow + 5.0f * cosf(time * 2.0f);

	if (g_bButton) UpdateButton(&g_bt, ButtonPressed);
	else g_bButton = TRUE;
}
// 描画
static void DrawUI(void)
{
	DrawTexture2D(&g_td_home[UI_BG]);
	DrawTexture2DAll();

	DrawTexture2D(&g_td_home[UI_NAME_PANNEL]);
	DrawTexture2D(&g_td_home[UI_ROCKET_NAME], TRUE);
	if (g_AnimStatusSlide == STATUS_SLIDE_Y) {
		g_td_home[UI_STATUS_PANNEL].tex = TEXTURE_LABEL_STATUS_PANNEL_DOWN;
	}
	else {
		g_td_home[UI_STATUS_PANNEL].tex = TEXTURE_LABEL_STATUS_PANNEL_UP;
		g_td_home[UI_STATUS_PANNEL].col = g_bd[BT_STATUS_PANNEL].col_off;
	}
	DrawTexture2D(&g_td_home[UI_STATUS_PANNEL]);

	DrawTextureStatus(-1, STATUS_SLIDE_Y_PLUS + 50.0f + g_AnimStatusSlide);

	DrawTexture2D(&g_td_home[UI_BACK]);

	if (g_bd[BT_LEFT].b_on || g_bd[BT_RIGHT].b_on) DrawTexture2D(&g_td_home[UI_PLAYER], TRUE, FALSE, TRUE);
	else DrawTexture2D(&g_td_home[UI_PLAYER], TRUE);
	DrawTexture2D(&g_td_home[UI_START]);
	DrawTexture2D(&g_td_home[UI_LEFT]);
	DrawTexture2D(&g_td_home[UI_RIGHT]);

}
// 終了
static void UninitUI(void)
{
	delete[] g_td_home, g_bd;
}

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitHome(void)
{
	InitUI();

	InitRocketSelect();
	//InitCharacterSelect();
	//InitWallet();

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitHome(void)
{
	if (g_Load == FALSE) return;

	UninitUI();

	UninitRocketSelect();
	UninitCharacterSelect();
	UninitWallet();

	g_Load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateHome(void)
{
	UpdateRocketSelect();
	//UpdateCharacterSelect();
	//UpdateWallet();


	if (GetHomeMode() == HOME_HOME)
	{
		if (g_AnimAlpha < 1.0f)
		{
			g_AnimAlpha += ANIM_ALPHA;
			for (int i = UI_ICON; i < UI_NUM; i++) {
				g_td_home[i].col.w = g_AnimAlpha;
			}
			return;
		}

		if (g_bStatus && g_AnimStatusSlide < 0.0f) {
			g_AnimStatusSlide = min(g_AnimStatusSlide + ANIM_SLIDE, 0.0f);
			g_td_home[UI_STATUS_PANNEL].pos.y = STATUS_SLIDE_Y_PLUS + g_AnimStatusSlide;
		}
		else if (!g_bStatus && g_AnimStatusSlide > STATUS_SLIDE_Y) {
			g_AnimStatusSlide = max(g_AnimStatusSlide - ANIM_SLIDE, STATUS_SLIDE_Y);
			g_td_home[UI_STATUS_PANNEL].pos.y = STATUS_SLIDE_Y_PLUS + g_AnimStatusSlide;
		}
	}
	else
	{

		if (g_AnimAlpha > 0.0f)
		{
			g_AnimAlpha -= ANIM_ALPHA;
			for (int i = UI_ICON; i < UI_NUM; i++) {
				g_td_home[i].col.w = g_AnimAlpha;
			}
			return;
		}
	}


	if (GetHomeMode() == HOME_HOME)
	{
		UpdateUI();

		if (g_bt.bd[BT_SHOP].b_on) SetRocketOutline();
	}
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawHome(void)
{
	DrawUI();
	
	DrawHomeRocket();

	DrawRocketSelect();
	//DrawCharacterSelect();
	//DrawWallet();
}

HOME_MODE GetHomeMode(void) {
	return g_Home;
}

void SetHomeMode(HOME_MODE mode) {
	g_Home = mode;
}