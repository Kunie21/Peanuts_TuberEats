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
//#define RIGHT_CENTER_LINE ()

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
	UI_SHOP,
	UI_BACK,

	UI_NUM
};
// 参照テクスチャ名（UI名順）
#define REF_TL {\
	TEXTURE_LABEL_BG,\
	TEXTURE_LABEL_ICON,\
	TEXTURE_LABEL_ROCKET_1,\
	TEXTURE_LABEL_CHARACTER_01,\
	TEXTURE_LABEL_START_BUTTON_1,\
	TEXTURE_LABEL_LEFT,\
	TEXTURE_LABEL_RIGHT,\
	TEXTURE_LABEL_ICON,\
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
#define BT_NUM_X 2
#define BT_NUM_Y 4
static int g_btTbl[BT_NUM_Y][BT_NUM_X] = {
	{BT_BACK, BT_BACK},
	{BT_LEFT, BT_RIGHT},
	{BT_SHOP, BT_SHOP},
	{BT_START, BT_START},
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
		SetFade(FADE_OUT, MODE_GAME);
		break;

	case BT_LEFT:
		break;

	case BT_RIGHT:
		break;

	case BT_SHOP:
		break;

	case BT_BACK:
		SetFade(FADE_OUT, MODE_TITLE_START);
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
	}
	g_td[UI_START].posType = POSITION_RIGHTBOTTOM;
	g_td[UI_START].ctrType = CENTER_BOTTOM;
	g_td[UI_START].pos = { -g_td[UI_START].size.x * 0.5f, -20.0f };
	g_td[UI_START].b_useOutline = TRUE;

	g_td[UI_ICON].posType = POSITION_LEFTTOP;
	g_td[UI_ICON].ctrType = CENTER_LEFTTOP;
	g_td[UI_ICON].b_useOutline = TRUE;

	g_td[UI_ROCKET_NAME].posType = POSITION_LEFTTOP;
	g_td[UI_ROCKET_NAME].ctrType = CENTER_LEFT;
	g_td[UI_ROCKET_NAME].pos = { g_td[UI_ICON].size.x, g_td[UI_ICON].size.y * 0.5f };

	g_td[UI_PLAYER].posType = POSITION_RIGHT;
	g_td[UI_PLAYER].ctrType = CENTER_CENTER;
	g_td[UI_PLAYER].pos = { g_td[UI_START].pos.x, 0.0f };

	g_td[UI_LEFT].posType = POSITION_RIGHT;
	g_td[UI_LEFT].ctrType = CENTER_RIGHT;
	g_td[UI_LEFT].pos = {
		g_td[UI_PLAYER].pos.x - g_td[UI_PLAYER].size.x * 0.5f,
		g_td[UI_PLAYER].pos.y
	};

	g_td[UI_RIGHT].posType = POSITION_RIGHT;
	g_td[UI_RIGHT].ctrType = CENTER_LEFT;
	g_td[UI_RIGHT].pos = {
		g_td[UI_PLAYER].pos.x + g_td[UI_PLAYER].size.x * 0.5f,
		g_td[UI_PLAYER].pos.y
	};

	g_td[UI_SHOP].posType = POSITION_LEFT;
	g_td[UI_SHOP].ctrType = CENTER_LEFT;
	g_td[UI_SHOP].pos = { g_td[UI_SHOP].size.x, 0.0f };

	g_td[UI_BACK].posType = POSITION_RIGHTTOP;
	g_td[UI_BACK].ctrType = CENTER_RIGHTTOP;

	// ボタン詳細設定
	UI_LABEL ul[BT_NUM] = REF_UL;
	for (int i = 0; i < BT_NUM; i++)
	{
		SetUIButton(&g_bd[i], &g_td[ul[i]]);
	}

	// ボタンテーブルへの登録
	g_bt = { &g_btTbl[0][0], BT_NUM_X, BT_NUM_Y, g_bd, BT_NUM, &g_cursor };

}
// 更新
static void UpdateUI(void)
{
	UpdateButton(&g_bt, ButtonPressed);
}
// 描画
static void DrawUI(void)
{
	for (int i = 0; i < UI_NUM; i++)
	{
		if (g_td[i].b_outline) DrawTexture2D(&g_td[i], TRUE, FALSE, TRUE);
		else DrawTexture2D(&g_td[i], TRUE);
	}
}
// 終了
static void UninitUI(void)
{
	delete[] g_td, g_bd;
}

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static BOOL				g_Load = FALSE;
static HOME_MODE		g_Home = HOME_MODE_HOME;

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitHome(void)
{
	InitUI();
	//InitRocketSelect();
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
	UpdateUI();

	//UpdateRocketSelect();
	//UpdateCharacterSelect();
	//UpdateWallet();

	if (GetKeyboardTrigger(DIK_1))
	{
		g_Home = HOME_MODE_HOME;
	}
	if (GetKeyboardTrigger(DIK_2))
	{
		g_Home = HOME_MODE_SHOP;
	}
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawHome(void)
{
	DrawUI();

	//DrawRocketSelect();
	//DrawCharacterSelect();
	//DrawWallet();
}

HOME_MODE GetHomeMode(void) {
	return g_Home;
}