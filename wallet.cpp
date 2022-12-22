//=============================================================================
//
// チームロゴ画面処理 [character.cpp]
// Author : 葉志成
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "fade.h"
#include "input.h"
#include "texture2d.h"
#include "wallet.h"
#include "rocket_select.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH)	// 背景サイズ横
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)	// 背景サイズ縦

#define MONEY_MAX			(999999999999)			// 所持金の上限

#define SIZE_WALLET			{536.0f,183.0f}			// 財布サイズ
#define POS_WALLET			{1300.0f,880.0f}		// 財布座標

#define SIZE_X_TOTAL_NUM	(347.0f/10.0f)	// ナンバーサイズ横
#define SIZE_Y_TOTAL_NUM	(45.0f)			// ナンバーサイズ縦
#define POS_X_TOTAL_NUM		(1790.0f)		// ナンバー座標横
#define POS_Y_TOTAL_NUM		(945.0f)		// ナンバー座標縦

#define SIZE_X_MINUS_NUM	(262.0f/10.0f)	// キャラクターサイズ横
#define SIZE_Y_MINUS_NUM	(33.0f)			// キャラクターサイズ横
#define POS_X_MINUS_NUM		(1793.0f)		// キャラクターサイズ縦
#define POS_Y_MINUS_NUM		(1014.0f)		// キャラクターサイズ縦

#define POS_X_SELECTION_NO		(1175.0f)		// キャラクターサイズ縦
#define POS_X_SELECTION_BUY		(1506.0f)		// キャラクターサイズ縦

#define COL_BLACK			{0.0f,0.0f,0.0f,1.0f}	// 黒い色
#define COL_ORIGINAL		{1.0f,1.0f,1.0f,1.0f}	// 白色


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static BOOL				g_Load = FALSE;
static BOOL				g_InTransaction = FALSE;
static BOOL				g_SuccessfulTransaction = FALSE;
static ULONG64			g_Money;
static ULONG64			g_Price;
static float			g_num_x;

// テクスチャ管理
//enum
//{
//	TEXTURE_WALLET = 0,
//	TEXTURE_WALLET_TOTAL,
//	TEXTURE_WALLET_DOT,
//	TEXTURE_WALLET_COMMA,
//	TEXTURE_YEN,
//	TEXTURE_MINUS_TOTAL,
//	TEXTURE_MINUS_DOT,
//	TEXTURE_MINUS_COMMA,
//	TEXTURE_MINUS,
//	TEXTURE_SHOP_MENU_OPACITY,
//	TEXTURE_BUY_MENU,
//	TEXTURE_BUY_MENU_SELECTION,
//	TEXTURE_NO,
//	TEXTURE_BUY,
//	TEXTURE_MAX,
//};
//static TEXTURE2D_DESC*	g_td;

// UI名（描画順）
enum UI_LABEL {
	UI_WALLET_BG = 0,
	UI_WALLET,
	UI_WALLET_NUMBER,
	UI_WALLET_DOT,
	UI_WALLET_COMMA,
	UI_YEN,
	UI_MINUS_NUMBER,
	UI_MINUS_DOT,
	UI_MINUS_COMMA,
	UI_MINUS,
	UI_SHOP_MENU_OPACITY,
	UI_BUY_MENU,
	UI_BUY_MENU_SELECTION,
	UI_NO,
	UI_BUY,

	UI_NUM,
};
//#define ALPHA_START (UI_ROCKET_ICON_LOCK)
//#define ALPHA_END (UI_SHOP_DETAIL)
// 参照テクスチャ名（UI名順）
#define REF_TL {\
	TEXTURE_LABEL_WALLET,\
	TEXTURE_LABEL_WALLET_TEXT,\
	TEXTURE_LABEL_WALLET_TOTAL,\
	TEXTURE_LABEL_WALLET_DOT,\
	TEXTURE_LABEL_WALLET_COMMA,\
	TEXTURE_LABEL_YEN,\
	TEXTURE_LABEL_MINUS_TOTAL,\
	TEXTURE_LABEL_MINUS_DOT,\
	TEXTURE_LABEL_MINUS_COMMA,\
	TEXTURE_LABEL_MINUS,\
	TEXTURE_LABEL_SHOP_MENU_OPACITY,\
	TEXTURE_LABEL_BUY_MENU,\
	TEXTURE_LABEL_BUY_MENU_SELECTION,\
	TEXTURE_LABEL_NO,\
	TEXTURE_LABEL_BUY,\
}
// UI詳細管理
static TEXTURE2D_DESC* g_td = NULL;

//*****************************************************************************
// ボタン定義
//*****************************************************************************
// ボタン名（優先順）
enum BT_LABEL {
	BT_CANCEL = 0,
	BT_BUY,

	BT_NUM
};
// 参照UI名（ボタン名順）
#define REF_UL {\
	UI_NO,\
	UI_BUY,\
}
// ボタン詳細管理
static BUTTON_DESC* g_bd = NULL;
// ボタン表
#define BT_NUM_X 2
#define BT_NUM_Y 1
static int g_btTbl[BT_NUM_Y][BT_NUM_X] = {
	{BT_CANCEL, BT_BUY}
};
static BUTTON_TABLE* g_bt = NULL;
// カーソル位置
static XMINT2 g_cursor = { 0, 0 };	// BT_ROCKET1から
// ボタンごとの処理
static void ButtonPressed(int b)
{
	switch (b)
	{
	case BT_CANCEL:
		break;

	case BT_BUY:
		break;

	default:
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

	g_td[UI_WALLET_BG].posType = POSITION_LEFTBOTTOM;
	g_td[UI_WALLET_BG].ctrType = CENTER_LEFTBOTTOM;
	g_td[UI_WALLET_BG].pos = { 20.0f, -20.0f };
	g_td[UI_WALLET_BG].col.w = 0.7f;
	g_td[UI_WALLET].posType = g_td[UI_WALLET_BG].posType;
	g_td[UI_WALLET].ctrType = g_td[UI_WALLET_BG].ctrType;
	g_td[UI_WALLET].pos = g_td[UI_WALLET_BG].pos;

	g_td[UI_SHOP_MENU_OPACITY].posType = POSITION_RIGHTBOTTOM;
	g_td[UI_SHOP_MENU_OPACITY].ctrType = CENTER_RIGHTBOTTOM;

	g_td[TEXTURE_LABEL_BUY_MENU].posType = POSITION_RIGHTBOTTOM;
	g_td[TEXTURE_LABEL_BUY_MENU].ctrType = CENTER_RIGHTBOTTOM;
	g_td[TEXTURE_LABEL_BUY_MENU].pos = { -50.0f, -50.0f };

	g_td[UI_WALLET_NUMBER].posType = POSITION_LEFTBOTTOM;
	g_td[UI_WALLET_NUMBER].ctrType = CENTER_RIGHTBOTTOM;
	g_td[UI_WALLET_NUMBER].size.x *= 0.1f;
	g_td[UI_WALLET_NUMBER].uv_pos.uw = 0.1f;
	g_td[UI_WALLET_NUMBER].pos = {
		g_td[UI_WALLET_BG].pos.x + g_td[UI_WALLET_BG].size.x - 10.0f,
		g_td[UI_WALLET_BG].pos.y - g_td[UI_WALLET_BG].size.y * 0.4f,
	};
	g_td[UI_WALLET_DOT].posType = POSITION_LEFTBOTTOM;
	g_td[UI_WALLET_DOT].ctrType = CENTER_RIGHTBOTTOM;
	g_td[UI_WALLET_DOT].pos.y = g_td[UI_WALLET_NUMBER].pos.y;
	g_td[UI_WALLET_COMMA].posType = POSITION_LEFTBOTTOM;
	g_td[UI_WALLET_COMMA].ctrType = CENTER_RIGHTBOTTOM;
	g_td[UI_WALLET_COMMA].pos.y = g_td[UI_WALLET_NUMBER].pos.y;
	g_td[UI_YEN].posType = POSITION_LEFTBOTTOM;
	g_td[UI_YEN].ctrType = CENTER_RIGHTBOTTOM;
	g_td[UI_YEN].pos.y = g_td[UI_WALLET_NUMBER].pos.y;

	g_td[UI_MINUS_NUMBER].posType = POSITION_LEFTBOTTOM;
	g_td[UI_MINUS_NUMBER].ctrType = CENTER_RIGHTBOTTOM;
	g_td[UI_MINUS_NUMBER].size.x *= 0.1f;
	g_td[UI_MINUS_NUMBER].uv_pos.uw = 0.1f;
	g_td[UI_MINUS_NUMBER].pos = {
		g_td[UI_WALLET_BG].pos.x + g_td[UI_WALLET_BG].size.x - 15.0f,
		g_td[UI_WALLET_BG].pos.y - 10.0f,
	};
	g_td[UI_MINUS_DOT].posType = POSITION_LEFTBOTTOM;
	g_td[UI_MINUS_DOT].ctrType = CENTER_RIGHTBOTTOM;
	g_td[UI_MINUS_DOT].pos.y = g_td[UI_MINUS_NUMBER].pos.y;
	g_td[UI_MINUS_COMMA].posType = POSITION_LEFTBOTTOM;
	g_td[UI_MINUS_COMMA].ctrType = CENTER_RIGHTBOTTOM;
	g_td[UI_MINUS_COMMA].pos.y = g_td[UI_MINUS_NUMBER].pos.y;
	g_td[UI_MINUS].posType = POSITION_LEFTBOTTOM;
	g_td[UI_MINUS].ctrType = CENTER_RIGHTBOTTOM;
	g_td[UI_MINUS].pos.y = g_td[UI_MINUS_NUMBER].pos.y;

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
	*g_bt = { &g_btTbl[0][0], BT_NUM_X, BT_NUM_Y, &g_bd[0], BT_NUM, &g_cursor };
}
// 更新
static void UpdateUI(void)
{
	UpdateButton(g_bt, ButtonPressed);
}
// 描画
static void DrawUI(void)
{

}
// 終了
static void UninitUI(void)
{
	delete[] g_td, g_bd;
	delete g_bt;
}

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitWallet(void)
{
	InitUI();
	//g_td = new TEXTURE2D_DESC[TEXTURE_MAX];

	//// テクスチャ生成
	//for (int i = 0; i < TEXTURE_MAX; i++)
	//{
	//	g_td[i].tex = (TEXTURE_LABEL)(TEXTURE_LABEL_WALLET + i);
	//	g_td[i].ctrType = CENTER_LEFTTOP;
	//	g_td[i].posType = POSITION_ABSOLUTE;
	//}

	//// 詳細設定
	//g_td[TEXTURE_WALLET].size = SIZE_WALLET;
	//g_td[TEXTURE_WALLET].pos = POS_WALLET;

	//g_td[TEXTURE_WALLET_TOTAL].size = { SIZE_X_TOTAL_NUM ,SIZE_Y_TOTAL_NUM };
	//g_td[TEXTURE_WALLET_TOTAL].pos = { POS_X_TOTAL_NUM ,POS_Y_TOTAL_NUM };
	//g_td[TEXTURE_WALLET_TOTAL].uv_pos = { 0.1f, 0.0f, 0.1f, 1.0f };
	//g_td[TEXTURE_WALLET_DOT].size = {17.0f,52.0f};
	//g_td[TEXTURE_WALLET_DOT].pos = { POS_X_TOTAL_NUM ,POS_Y_TOTAL_NUM };
	//g_td[TEXTURE_WALLET_COMMA].size = { 17.0f,52.0f };
	//g_td[TEXTURE_WALLET_COMMA].pos = { POS_X_TOTAL_NUM ,POS_Y_TOTAL_NUM };
	//g_td[TEXTURE_YEN].size = { 30.0f,37.0f };
	//g_td[TEXTURE_YEN].pos = { POS_X_TOTAL_NUM ,POS_Y_TOTAL_NUM };
	//g_td[TEXTURE_YEN].pos.y += 10.0f;

	//g_td[TEXTURE_MINUS_TOTAL].size = { SIZE_X_MINUS_NUM ,SIZE_Y_MINUS_NUM };
	//g_td[TEXTURE_MINUS_TOTAL].pos = { POS_X_MINUS_NUM ,POS_Y_MINUS_NUM };
	//g_td[TEXTURE_MINUS_TOTAL].uv_pos = { 0.1f, 0.0f, 0.1f, 1.0f };
	//g_td[TEXTURE_MINUS_DOT].size = { 13.0f,37.0f };
	//g_td[TEXTURE_MINUS_DOT].pos = { POS_X_MINUS_NUM ,POS_Y_MINUS_NUM };
	//g_td[TEXTURE_MINUS_COMMA].size = { 13.0f,37.0f };
	//g_td[TEXTURE_MINUS_COMMA].pos = { POS_X_MINUS_NUM ,POS_Y_MINUS_NUM };
	//g_td[TEXTURE_MINUS].size = { 19.0f,37.0f };
	//g_td[TEXTURE_MINUS].pos = { POS_X_MINUS_NUM ,POS_Y_MINUS_NUM };

	//g_td[TEXTURE_SHOP_MENU_OPACITY].size = { 1001.0f, 1080.0f };
	//g_td[TEXTURE_SHOP_MENU_OPACITY].pos = { 919.0f,0.0f };
	//g_td[TEXTURE_BUY_MENU].size = { 669.0f, 225.0f };
	//g_td[TEXTURE_BUY_MENU].pos = { 1130.0f,600.0f };
	//g_td[TEXTURE_BUY_MENU_SELECTION].size = { 239.0f, 62.0f };
	//g_td[TEXTURE_BUY_MENU_SELECTION].pos = { POS_X_SELECTION_NO,700.0f };
	//g_td[TEXTURE_NO].size = { 134.0f, 37.0f };
	//g_td[TEXTURE_NO].pos = { 1224.0f,711.0f };
	//g_td[TEXTURE_NO].col = { 0.0f,0.0f,0.0f,1.0f };
	//g_td[TEXTURE_BUY].size = { 91.0f, 37.0f };
	//g_td[TEXTURE_BUY].pos = { 1580.0f,711.0f };
	//g_td[TEXTURE_BUY].col = { 0.0f,0.0f,0.0f,1.0f };

	g_InTransaction = FALSE;
	g_SuccessfulTransaction = FALSE;

	g_Money = MONEY_MAX; //SaveDataから読み込む予定
	g_Price = MONEY_MAX; //SaveDataから読み込む予定

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitWallet(void)
{
	if (g_Load == FALSE) return;


	UninitUI();

	//delete[] g_td;

	g_Load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateWallet(void)
{
	//UpdateUI();
	
	//if (!g_InTransaction)return;

	//if (GetKeyboardTrigger(DIK_A)) 
	//{// 買わないボタンを選択する場合
	//	g_td[TEXTURE_BUY_MENU_SELECTION].pos.x = POS_X_SELECTION_NO;
	//	g_td[TEXTURE_NO].col = COL_ORIGINAL;
	//	g_td[TEXTURE_BUY].col = COL_BLACK;
	//	
	//}
	//else if (GetKeyboardTrigger(DIK_D)) 
	//{// 買うボタンを選択する場合
	//	g_td[TEXTURE_BUY_MENU_SELECTION].pos.x = POS_X_SELECTION_BUY;
	//	g_td[TEXTURE_NO].col = COL_BLACK;
	//	g_td[TEXTURE_BUY].col = COL_ORIGINAL ;
	//}
	//else if (GetKeyboardTrigger(DIK_RETURN))
	//{
	//	if ((g_Money >= g_Price) && ((int)g_td[TEXTURE_BUY_MENU_SELECTION].pos.x == (int)POS_X_SELECTION_BUY))
	//	{//お金が持っている + 買うボタンを選択する場合
	//		g_Money -= g_Price;
	//		g_SuccessfulTransaction = TRUE;
	//	}
	//	else
	//	{
	//		g_InTransaction = FALSE;
	//	}
	//}
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawWallet(void)
{	
	//if (IsRocketSelectFinished()) return;

	DrawTexture2D(&g_td[UI_WALLET_BG]);
	DrawTexture2D(&g_td[UI_WALLET]);

	// プレイヤーのお金の描画処理
	ULONG64 money = g_Money;
	int count = 0;
	int comma = 0;
	while (money > 0)
	{
		if (count == 2)
		{
			g_td[UI_WALLET_DOT].pos.x = g_td[UI_WALLET_NUMBER].pos.x - (count - comma) * g_td[UI_WALLET_NUMBER].size.x - comma * g_td[UI_WALLET_COMMA].size.x;
			DrawTexture2D(&g_td[UI_WALLET_DOT]);
			count++;
			comma++;
		}
		else if (count == 6 || count == 10 || count == 14)
		{
			g_td[UI_WALLET_COMMA].pos.x = g_td[UI_WALLET_NUMBER].pos.x - (count - comma) * g_td[UI_WALLET_NUMBER].size.x - comma * g_td[UI_WALLET_COMMA].size.x;
			DrawTexture2D(&g_td[UI_WALLET_COMMA]);
			count++;
			comma++;
		}
		g_td[UI_WALLET_NUMBER].uv_pos.u = (float)(money % 10) * 0.1f;
		g_td[UI_WALLET_NUMBER].posAdd.x =  - (count - comma) * g_td[UI_WALLET_NUMBER].size.x - comma * g_td[UI_WALLET_COMMA].size.x;
		DrawTexture2D(&g_td[UI_WALLET_NUMBER]);
		money /= 10;
		count++;
	}
	while (count < 3)
	{
		if (count == 2)
		{
			g_td[UI_WALLET_DOT].pos.x = g_td[UI_WALLET_NUMBER].pos.x - (count - comma) * g_td[UI_WALLET_NUMBER].size.x - comma * g_td[UI_WALLET_COMMA].size.x;
			DrawTexture2D(&g_td[UI_WALLET_DOT]);
			count++;
			comma++;
		}

		g_td[UI_WALLET_NUMBER].uv_pos.u = 0.0f;
		g_td[UI_WALLET_NUMBER].posAdd.x = -(count - comma) * g_td[UI_WALLET_NUMBER].size.x - comma * g_td[UI_WALLET_COMMA].size.x;
		DrawTexture2D(&g_td[UI_WALLET_NUMBER]);
		money /= 10;
		count++;
	}
	g_td[UI_YEN].pos.x = g_td[UI_WALLET_NUMBER].pos.x - (count - comma) * g_td[UI_WALLET_NUMBER].size.x - comma * g_td[UI_WALLET_COMMA].size.x - 5.0f;
	DrawTexture2D(&g_td[UI_YEN]);


	// 消費のお金の描画処理
	money = g_Price;
	if (money == 0) return;

	count = 0;
	comma = 0;
	while (money > 0)
	{
		if (count == 2)
		{
			g_td[UI_MINUS_DOT].pos.x = g_td[UI_MINUS_NUMBER].pos.x - (count - comma) * g_td[UI_MINUS_NUMBER].size.x - comma * g_td[UI_MINUS_COMMA].size.x;
			DrawTexture2D(&g_td[UI_MINUS_DOT]);
			count++; comma++;
		}
		else if (count == 6 || count == 10 || count == 14)
		{
			g_td[UI_MINUS_COMMA].pos.x = g_td[UI_MINUS_NUMBER].pos.x - (count - comma) * g_td[UI_MINUS_NUMBER].size.x - comma * g_td[UI_MINUS_COMMA].size.x;
			DrawTexture2D(&g_td[UI_MINUS_COMMA]);
			count++; comma++;
		}
		g_td[UI_MINUS_NUMBER].uv_pos.u = (float)(money % 10) * 0.1f;
		g_td[UI_MINUS_NUMBER].posAdd.x = -(count - comma) * g_td[UI_MINUS_NUMBER].size.x - comma * g_td[UI_MINUS_COMMA].size.x;
		DrawTexture2D(&g_td[UI_MINUS_NUMBER]);
		money /= 10;
		count++;
	}
	g_td[UI_MINUS].pos.x = g_td[UI_MINUS_NUMBER].pos.x - (count - comma) * g_td[UI_MINUS_NUMBER].size.x - comma * g_td[UI_MINUS_COMMA].size.x;
	DrawTexture2D(&g_td[UI_MINUS]);


	if (!g_InTransaction)return;

	//DrawTexture2D(&g_td[TEXTURE_SHOP_MENU_OPACITY]);
	//DrawTexture2D(&g_td[TEXTURE_BUY_MENU]);
	//DrawTexture2D(&g_td[TEXTURE_BUY_MENU_SELECTION]);
	//DrawTexture2D(&g_td[TEXTURE_NO]);
	//DrawTexture2D(&g_td[TEXTURE_BUY]);
}

void SetPriceRocket(const ULONG64& price)
{
	g_Price = price;
}


void StartTransaction(void)
{
	//g_InTransaction = TRUE;
	//g_SuccessfulTransaction = FALSE;
	//g_td[TEXTURE_BUY_MENU_SELECTION].pos.x = POS_X_SELECTION_NO;
	//g_td[TEXTURE_NO].col = COL_ORIGINAL;
	//g_td[TEXTURE_BUY].col = COL_BLACK;
}
void ResetTransaction(void)
{
	g_InTransaction = FALSE;
}

BOOL InTransaction(void)
{
	return g_InTransaction;
}

BOOL SuccessfulTransaction(void)
{
	if (g_SuccessfulTransaction)
	{
		g_InTransaction = FALSE;
		return TRUE;
	}
	return FALSE;
}

void GainMoney(ULONG64& gain)
{
	g_Money += gain;
	if (g_Money > MONEY_MAX)
	{
		g_Money = MONEY_MAX;
	}
}

ULONG64& GetMoney(void)
{
	return g_Money;
}

void SetWalletPosAdd(XMFLOAT2& posAdd) {

}