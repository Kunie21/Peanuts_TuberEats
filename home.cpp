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
#define ARROW_Y (150.0f)
#define ANIM_ALPHA		(0.1f)		// メニューが色づくスピード


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static BOOL				g_Load = FALSE;
static HOME_MODE		g_Home = HOME_HOME;
static float			g_leftArrow = 0.0f;
static float			g_rightArrow = 0.0f;
static float			g_AnimAlpha = 0.0f;				// アニメーション管理用
static BOOL				g_bButton = FALSE;

//*****************************************************************************
// UI定義
//*****************************************************************************
// UI名（描画順）
enum UI_LABEL {
	UI_BG = 0,
	UI_ICON,
	UI_ROCKET_NAME,
	UI_PLAYER,
	UI_START,
	UI_LEFT,
	UI_RIGHT,
	UI_BACK,
	UI_SHOP,

	UI_NUM
};
// 参照テクスチャ名（UI名順）
#define REF_TL {\
	TEXTURE_LABEL_BG,\
	TEXTURE_LABEL_ICON,\
	TEXTURE_LABEL_ROCKET_1,\
	TEXTURE_LABEL_CHARACTER_01,\
	TEXTURE_LABEL_START_BUTTON_2,\
	TEXTURE_LABEL_LEFT,\
	TEXTURE_LABEL_RIGHT,\
	TEXTURE_LABEL_BACK,\
	TEXTURE_LABEL_ICON,\
}
// UI詳細管理
static TEXTURE2D_DESC* g_td;

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

	BT_NUM
};
// 参照UI名（ボタン名順）
#define REF_UL {\
	UI_START,\
	UI_LEFT,\
	UI_RIGHT,\
	UI_SHOP,\
	UI_BACK\
}
// ボタン詳細管理
static BUTTON_DESC* g_bd;
// ボタン表
#define BT_NUM_X 3
#define BT_NUM_Y 3
static int g_btTbl[BT_NUM_Y][BT_NUM_X] = {
	{BT_BACK, BT_BACK, BT_BACK},
	{BT_SHOP, BT_LEFT, BT_RIGHT},
	{BT_START, BT_START, BT_START},
};
static BUTTON_TABLE g_bt;
// カーソル位置
static XMINT2 g_cursor = { 0, 3 };	// BT_STARTから
// ボタンごとの処理
static void ButtonPressed(int b)
{
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
	}
	g_td[UI_START].posType = POSITION_RIGHTBOTTOM;
	g_td[UI_START].ctrType = CENTER_BOTTOM;
	g_td[UI_START].pos = { -g_td[UI_START].size.x * 0.5f, -30.0f };
	//g_td[UI_START].b_useOutline = TRUE;

	g_td[UI_ICON].posType = POSITION_LEFTTOP;
	g_td[UI_ICON].ctrType = CENTER_LEFTTOP;
	g_td[UI_ICON].scl = { 0.7f, 0.7f };

	g_td[UI_ROCKET_NAME].posType = POSITION_LEFTTOP;
	g_td[UI_ROCKET_NAME].ctrType = CENTER_LEFT;
	g_td[UI_ROCKET_NAME].col = { 0.0f, 0.3f, 0.0f, 1.0f };
	g_td[UI_ROCKET_NAME].pos = { g_td[UI_ICON].size.x, g_td[UI_ICON].size.y * 0.5f };
	g_td[UI_ROCKET_NAME].sd_pos = { 3.0f, 3.0f };

	g_td[UI_PLAYER].posType = POSITION_RIGHT;
	g_td[UI_PLAYER].ctrType = CENTER_CENTER;
	g_td[UI_PLAYER].pos = { g_td[UI_START].pos.x, 0.0f };

	g_td[UI_LEFT].posType = POSITION_RIGHT;
	g_td[UI_LEFT].ctrType = CENTER_RIGHT;
	g_leftArrow = g_td[UI_PLAYER].pos.x - g_td[UI_PLAYER].size.x * 0.55f;
	g_td[UI_LEFT].pos = { g_leftArrow, ARROW_Y };

	g_td[UI_RIGHT].posType = POSITION_RIGHT;
	g_td[UI_RIGHT].ctrType = CENTER_LEFT;
	g_rightArrow = g_td[UI_PLAYER].pos.x + g_td[UI_PLAYER].size.x * 0.55f;
	g_td[UI_RIGHT].pos = { g_rightArrow, ARROW_Y };


	g_td[UI_SHOP].posType = POSITION_LEFT;
	g_td[UI_SHOP].ctrType = CENTER_CENTER;
	g_td[UI_SHOP].pos = { SCREEN_CENTER_X * 0.5f, 0.0f };
	g_td[UI_SHOP].size.x *= 2.0f;
	g_td[UI_SHOP].size.y *= 2.0f;

	g_td[UI_BACK].posType = POSITION_RIGHTTOP;
	g_td[UI_BACK].ctrType = CENTER_RIGHTTOP;
	g_td[UI_BACK].pos = { -20.0f, 20.0f };
	g_td[UI_BACK].sd_pos = { 3.0f, 1.5f };
	g_bd[BT_BACK].col_on = { 0.2f, 0.5f, 0.2f, 1.0f };
	g_bd[BT_BACK].col_off = { 0.0f, 0.3f, 0.0f, 1.0f };

	// ボタン詳細設定
	UI_LABEL ul[BT_NUM] = REF_UL;
	for (int i = 0; i < BT_NUM; i++)
	{
		SetUIButton(&g_bd[i], &g_td[ul[i]]);
	}
	g_bd[BT_START].tex_on = TEXTURE_LABEL_START_BUTTON_3;

	// ボタンテーブルへの登録
	g_bt = { &g_btTbl[0][0], BT_NUM_X, BT_NUM_Y, g_bd, BT_NUM, &g_cursor };

}
// 更新
static void UpdateUI(void)
{
	static float time = 0.0f;
	time += 0.03f; if (time > XM_2PI) time -= XM_2PI;
	g_td[UI_PLAYER].pos.y = 3.0f * sinf(time);
	g_td[UI_LEFT].pos.x = g_leftArrow - 5.0f * cosf(time * 2.0f);
	g_td[UI_RIGHT].pos.x = g_rightArrow + 5.0f * cosf(time * 2.0f);

	if (g_bButton) UpdateButton(&g_bt, ButtonPressed);
	else g_bButton = TRUE;
}
// 描画
static void DrawUI(void)
{
	DrawTexture2D(&g_td[UI_BG]);
	DrawTexture2DAll();

	for (int i = 1; i < UI_BACK; i++)
	{
		if (g_td[i].b_outline) DrawTexture2D(&g_td[i], TRUE, FALSE, TRUE);
		else DrawTexture2D(&g_td[i], TRUE);
	}

	DrawTexture2D(&g_td[UI_BACK], TRUE);
}
// 終了
static void UninitUI(void)
{
	delete[] g_td, g_bd;
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
				g_td[i].col.w = g_AnimAlpha;
			}
			return;
		}
	}
	else
	{
		if (g_AnimAlpha > 0.0f)
		{
			g_AnimAlpha -= ANIM_ALPHA;
			for (int i = UI_ICON; i < UI_NUM; i++) {
				g_td[i].col.w = g_AnimAlpha;
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