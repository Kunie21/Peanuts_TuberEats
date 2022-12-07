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
// UI定義
//*****************************************************************************
// UI名
enum UI_LABEL {
	UI_BG = 0,
	UI_ICON,
	UI_ROCKET_NAME,
	UI_PLAYER,
	UI_LEFT,
	UI_RIGHT,
	UI_START,
	UI_SHOP,
	UI_BACK,
	UI_NUM
};
// 参照テクスチャ名
TEXTURE_LABEL g_tl[UI_NUM] = {
	TEXTURE_LABEL_BG,
	TEXTURE_LABEL_ICON,
	TEXTURE_LABEL_ROCKET_1,
	TEXTURE_LABEL_CHARACTER_01,
	TEXTURE_LABEL_START_BUTTON_1,
	TEXTURE_LABEL_LEFT,
	TEXTURE_LABEL_RIGHT,
	TEXTURE_LABEL_SHOP,
	TEXTURE_LABEL_ICON,
};
// UI詳細管理
static TEXTURE2D_DESC	g_td[UI_NUM];

//*****************************************************************************
// ボタン定義
//*****************************************************************************
// ボタン名
enum BT_LABEL {
	BT_START = 0,
	BT_LEFT,
	BT_RIGHT,
	BT_SHOP,
	BT_BACK,
	BT_NUM
};
// ボタン表
#define BT_NUM_X 2
#define BT_NUM_Y 4
static BT_LABEL g_btTbl[BT_NUM_Y][BT_NUM_X] = {
	{BT_BACK, BT_BACK},
	{BT_LEFT, BT_RIGHT},
	{BT_SHOP, BT_SHOP},
	{BT_START, BT_START},
};
// カーソル位置
XMINT2 g_cursor = { 3, 0 };	// BT_STARTから
// ボタン詳細管理
static BUTTON_DESC g_bd[BT_NUM];
// 参照UI名
UI_LABEL g_ul[BT_NUM] = {
	UI_START,
	UI_LEFT,
	UI_RIGHT,
	UI_SHOP,
	UI_BACK
};
// ボタンを全てオフにする
void SetButtonOffAll(void)
{
	for (int i = 0; i < BT_NUM; i++) {
		if (g_bd[i].b_on) {
			g_bd[i].b_on = FALSE;
			g_bd[i].p_td->col = g_bd[i].col_off;
			g_bd[i].p_td->scl = g_bd[i].scl_off;
		}
	}
}
// ボタンをオンにする
void SetButtonOn(int bt)
{
	if (!g_bd[bt].b_on) {
		g_bd[bt].b_on = TRUE;
		g_bd[bt].p_td->col = g_bd[bt].col_on;
		g_bd[bt].p_td->scl = g_bd[bt].scl_on;
	}
}
// カーソル位置からボタン名を取得する
BT_LABEL GetButtonByCursor(void) {
	return g_btTbl[g_cursor.x][g_cursor.y];
}
// カーソルがあるボタンをオンにする
void SetButtonOnByCursor(void) {
	int bt = GetButtonByCursor();
	g_bd[bt].b_on = TRUE;
	SetButtonOn(bt);
}
// ボタン名からカーソル位置を変更する
void SetButtonPosition(int bt) {
	for (int x = 0; x < BT_NUM_X; x++) {
		for (int y = 0; y < BT_NUM_Y; y++) {
			if (g_btTbl[x][y] == (BT_LABEL)bt) {
				g_cursor = { x, y };
				return;
			}
		}
	}
}
// ボタンごとの処理
void ButtonPressed(int bt)
{
	switch (bt)
	{
	case BT_START:
		break;

	case BT_LEFT:
		break;

	case BT_RIGHT:
		break;

	case BT_SHOP:
		break;

	case BT_BACK:
		break;

	case BT_NUM:
		break;

	default:
		break;
	}
}
// マウス・カーソルのあるボタンをオンにする
void CheckButtonOn(void)
{
	// カーソルがあるボタンON
	if (GetMouseUse())	// マウスを使っているとき
	{
		for (int i = 0; i < BT_NUM; i++)
		{
			if (CollisionMouse(g_bd[i].pos, g_bd[i].size))
			{
				// マウスカーソルがのっているボタンをオンにする
				g_bd[i].b_on = TRUE;
				SetButtonOn(i);

				// キーボード用のボタン位置も変更
				SetButtonPosition(i);

				// ボタンが押された
				if (IsMouseLeftTriggered()) ButtonPressed(i);

				break;	// ONにするのは1つだけ
			}
		}
	}
	else	// キーボードを使っているとき
	{
		// カーソルを動かす
		MoveCursor(g_cursor, BT_NUM_X, BT_NUM_Y);

		// カーソルがのっているボタンをオンにする
		SetButtonOnByCursor();

		// ボタンが押された
		if (GetKeyboardTrigger(DIK_RETURN)) ButtonPressed(GetButtonByCursor());
	}
}

//*****************************************************************************
// HOME_UI初期設定
//*****************************************************************************
void InitHomeUI(void)
{
	// UI詳細設定
	for (int i = 0; i < UI_NUM; i++)
	{
		// テクスチャの対応付け
		g_td[i].tex = g_tl[i];

		// テクスチャサイズの取得
		g_td[i].size = GetTextureSize(g_tl[i]);

		// その他設定
		switch (i)
		{
		case UI_BG:
			break;

		case UI_ICON:
			g_td[i].posType = POSITION_LEFTTOP;
			g_td[i].ctrType = CENTER_LEFTTOP;
			g_td[i].pos = { 0.0f, 0.0f };
			break;

		case UI_ROCKET_NAME:
			g_td[i].posType = POSITION_LEFTTOP;
			g_td[i].ctrType = CENTER_LEFT;
			g_td[i].pos = { g_td[UI_ICON].size.x, g_td[UI_ICON].size.y * 0.5f };
			break;

		case UI_PLAYER:
			g_td[i].posType = POSITION_RIGHT;
			g_td[i].ctrType = CENTER_RIGHT;
			g_td[i].pos = { -50.0f, 0.0f };
			break;

		case UI_LEFT:
			g_td[i].posType = POSITION_RIGHT;
			g_td[i].ctrType = CENTER_CENTER;
			g_td[i].pos = {
				g_td[UI_PLAYER].pos.x - g_td[UI_PLAYER].size.x,
				g_td[UI_PLAYER].pos.y
			};
			break;

		case UI_RIGHT:
			g_td[i].posType = POSITION_RIGHT;
			g_td[i].ctrType = CENTER_CENTER;
			g_td[i].pos = {
				g_td[UI_PLAYER].pos.x,
				g_td[UI_PLAYER].pos.y
			};
			break;

		case UI_START:
			g_td[i].posType = POSITION_RIGHTBOTTOM;
			g_td[i].ctrType = CENTER_RIGHTBOTTOM;
			g_td[i].pos = { -5.0f, -5.0f };
			break;

		case UI_SHOP:
			g_td[i].posType = POSITION_RIGHTBOTTOM;
			g_td[i].ctrType = CENTER_RIGHTBOTTOM;
			g_td[i].pos = {
				g_td[UI_START].pos.x,
				g_td[UI_START].pos.y - g_td[UI_START].size.y
			};
			break;

		case UI_BACK:
			g_td[i].posType = POSITION_RIGHTTOP;
			g_td[i].ctrType = CENTER_RIGHTTOP;
			g_td[i].pos = { 0.0f, 0.0f };
			break;

		default:
			break;
		}
	}

	// ボタン詳細設定
	for (int i = 0; i < BT_NUM; i++)
	{
		SetUIButton(&g_bd[i], &g_td[g_ul[i]]);
	}
}
void UpdateHomeUI(void)
{
	// 全ボタンOFF
	SetButtonOffAll();

	// ボタンONチェック
	CheckButtonOn();

}
void DrawHomeUI(void)
{
	for (int i = 0; i < UI_NUM; i++)
	{
		DrawTexture2D(&g_td[i]);
	}
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
	InitHomeUI();
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
	UpdateHomeUI();

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
	DrawHomeUI();

	//DrawRocketSelect();
	//DrawCharacterSelect();
	//DrawWallet();
}

HOME_MODE GetHomeMode(void) {
	return g_Home;
}