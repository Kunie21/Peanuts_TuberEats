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
#define ANIM_SCALING	(0.1f)		// メニューが大きくなる倍率
#define ANIM_ALPHA		(0.1f)		// メニューが色づくスピード
#define ANIM_SLIDE		(80.0f)		// メニューがスライドしてくるスピード
#define OBJ_DIST		(220.0f)
#define COL_BLACK		{0.0f,0.0f,0.0f,1.0f}	// 黒い色
#define COL_ORIGINAL	{1.0f,1.0f,1.0f,1.0f}	// 元の色
#define SLIDE_X			(-750.0f)

// モデル名
enum {
	MODEL_TITLE_STAR = 0,
	MODEL_TITLE_EARTH,
	MODEL_TITLE_MAX,
};

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static float			g_AnimScl = 0.0f;				// アニメーション管理用
static float			g_AnimAlpha = 0.0f;				// アニメーション管理用
static float			g_AnimSlide = -SCREEN_WIDTH;	// アニメーション管理用
static float			g_PinScl = 0.0f;
static float			g_DDScl = 0.0f;	// ドロップダウンアニメーション管理用
static float			g_DDAlpha = 0.0f;	// ドロップダウンアニメーション管理用
static BOOL				g_Load = FALSE;
static MODEL_DATA		g_Model[MODEL_TITLE_MAX];
static int				g_SelectedStage = -1;

enum GPS_LABEL {
	GPS_JAPAN = 0,
	GPS_ASIA,
	GPS_EUROPE,
	GPS_NORTHAMERICA,

	GPS_OSAKA,
	GPS_HOKKAIDO,
	GPS_OKINAWA,

	GPS_CHINA,
	GPS_KOREA,
	GPS_SINGAPORE,

	GPS_FRANCE,
	GPS_ENGLAND,
	GPS_ITALY,

	GPS_AMERICA,
	GPS_CANADA,
	GPS_MEXICO,

	GPS_NUM
};
#define GPS {\
	{ 35.683333f, 139.76666f },\
	{ 30.0f, 130.f },\
	{ 47.0f, 5.0f },\
	{ 35.0f, -90.0f },\
\
	{ 34.683333f, 135.5f },\
	{ 43.06666f, 141.35f },\
	{ 26.5f, 128.0f },\
\
	{ 31.216666f, 121.46666f },\
	{ 37.55f, 126.96666f },\
	{ 1.283333f, 103.83333f },\
\
	{ 48.85f, 2.35f },\
	{ 51.5f, 0.116666f },\
	{ 41.9f, 12.483333f },\
\
	{ 40.7f, -74.0f },\
	{ 45.5f, -73.55f },\
	{ 19.43333f, -99.13333f },\
}
static XMFLOAT2*	g_gpsRef;
static XMFLOAT2		g_gps;

enum MENU_LABEL {
	MENU_MAIN = 0,
	MENU_JAPAN,
	MENU_ASIA,
	MENU_EUROPE,
	MENU_NORTHAMERICA,

	MENU_NUM
};
static MENU_LABEL g_menu = MENU_MAIN;
static MENU_LABEL g_menuDD = MENU_JAPAN;
#define REGION_NUM (4)
#define AREA_NUM (3)
#define TEX_NUM (3)

//*****************************************************************************
// UI定義
//*****************************************************************************
// UI名（描画順）
enum UI_LABEL {
	UI_BG = 0,
	UI_HEAD,
	UI_LINE,
	UI_CIRCLE,
	UI_BACK,
	UI_PIN,

	UI_JAPAN_BG_ALL,
	UI_ASIA_BG_ALL,
	UI_EUROPE_BG_ALL,
	UI_NORTHAMERICA_BG_ALL,

	// 日本
	UI_JAPAN,
	UI_JAPAN_B,
	UI_JAPAN_BG,

	UI_JAPAN_OSAKA,
	UI_JAPAN_OSAKA_B,
	UI_JAPAN_OSAKA_BG,

	UI_JAPAN_HOKKAIDO,
	UI_JAPAN_HOKKAIDO_B,
	UI_JAPAN_HOKKAIDO_BG,

	UI_JAPAN_OKINAWA,
	UI_JAPAN_OKINAWA_B,
	UI_JAPAN_OKINAWA_BG,

	// アジア
	UI_ASIA,
	UI_ASIA_B,
	UI_ASIA_BG,

	UI_ASIA_CHINA,
	UI_ASIA_CHINA_B,
	UI_ASIA_CHINA_BG,

	UI_ASIA_KOREA,
	UI_ASIA_KOREA_B,
	UI_ASIA_KOREA_BG,

	UI_ASIA_SINGAPORE,
	UI_ASIA_SINGAPORE_B,
	UI_ASIA_SINGAPORE_BG,

	// ヨーロッパ
	UI_EUROPE,
	UI_EUROPE_B,
	UI_EUROPE_BG,

	UI_EUROPE_FRANCE,
	UI_EUROPE_FRANCE_B,
	UI_EUROPE_FRANCE_BG,

	UI_EUROPE_ENGLAND,
	UI_EUROPE_ENGLAND_B,
	UI_EUROPE_ENGLAND_BG,

	UI_EUROPE_ITALY,
	UI_EUROPE_ITALY_B,
	UI_EUROPE_ITALY_BG,

	// 北アメリカ
	UI_NORTHAMERICA,
	UI_NORTHAMERICA_B,
	UI_NORTHAMERICA_BG,

	UI_NORTHAMERICA_AMERICA,
	UI_NORTHAMERICA_AMERICA_B,
	UI_NORTHAMERICA_AMERICA_BG,

	UI_NORTHAMERICA_CANADA,
	UI_NORTHAMERICA_CANADA_B,
	UI_NORTHAMERICA_CANADA_BG,

	UI_NORTHAMERICA_MEXICO,
	UI_NORTHAMERICA_MEXICO_B,
	UI_NORTHAMERICA_MEXICO_BG,

	UI_NUM,
};
#define ALPHA_START (UI_BACK)
#define ALPHA_END (UI_NORTHAMERICA_MEXICO_BG)
// 参照テクスチャ名（UI名順）
#define REF_TL {\
	TEXTURE_LABEL_STAGE_SELECTION_BG,\
	TEXTURE_LABEL_STAGE_SELECTION_HEAD,\
	TEXTURE_LABEL_STAGE_SELECTION_LINE,\
	TEXTURE_LABEL_CIRCLE_SPIN,\
	TEXTURE_LABEL_BACK,\
	TEXTURE_LABEL_GOALPIN,\
\
	TEXTURE_LABEL_JAPAN_BG_ALL,\
	TEXTURE_LABEL_ASIA_BG_ALL,\
	TEXTURE_LABEL_EUROPE_BG_ALL,\
	TEXTURE_LABEL_AMERICA_BG_ALL,\
\
	TEXTURE_LABEL_JAPAN,\
	TEXTURE_LABEL_JAPAN,\
	TEXTURE_LABEL_JAPAN_BG,\
	TEXTURE_LABEL_JAPAN1,\
	TEXTURE_LABEL_JAPAN1,\
	TEXTURE_LABEL_JAPAN1_BG,\
	TEXTURE_LABEL_JAPAN2,\
	TEXTURE_LABEL_JAPAN2,\
	TEXTURE_LABEL_JAPAN2_BG,\
	TEXTURE_LABEL_JAPAN3,\
	TEXTURE_LABEL_JAPAN3,\
	TEXTURE_LABEL_JAPAN3_BG,\
\
	TEXTURE_LABEL_ASIA,\
	TEXTURE_LABEL_ASIA,\
	TEXTURE_LABEL_ASIA_BG,\
	TEXTURE_LABEL_ASIA1,\
	TEXTURE_LABEL_ASIA1,\
	TEXTURE_LABEL_ASIA1_BG,\
	TEXTURE_LABEL_ASIA2,\
	TEXTURE_LABEL_ASIA2,\
	TEXTURE_LABEL_ASIA2_BG,\
	TEXTURE_LABEL_ASIA3,\
	TEXTURE_LABEL_ASIA3,\
	TEXTURE_LABEL_ASIA3_BG,\
\
	TEXTURE_LABEL_EUROPE,\
	TEXTURE_LABEL_EUROPE,\
	TEXTURE_LABEL_EUROPE_BG,\
	TEXTURE_LABEL_EUROPE1,\
	TEXTURE_LABEL_EUROPE1,\
	TEXTURE_LABEL_EUROPE1_BG,\
	TEXTURE_LABEL_EUROPE2,\
	TEXTURE_LABEL_EUROPE2,\
	TEXTURE_LABEL_EUROPE2_BG,\
	TEXTURE_LABEL_EUROPE3,\
	TEXTURE_LABEL_EUROPE3,\
	TEXTURE_LABEL_EUROPE3_BG,\
\
	TEXTURE_LABEL_AMERICA,\
	TEXTURE_LABEL_AMERICA,\
	TEXTURE_LABEL_AMERICA_BG,\
	TEXTURE_LABEL_AMERICA1,\
	TEXTURE_LABEL_AMERICA1,\
	TEXTURE_LABEL_AMERICA1_BG,\
	TEXTURE_LABEL_AMERICA2,\
	TEXTURE_LABEL_AMERICA2,\
	TEXTURE_LABEL_AMERICA2_BG,\
	TEXTURE_LABEL_AMERICA3,\
	TEXTURE_LABEL_AMERICA3,\
	TEXTURE_LABEL_AMERICA3_BG,\
\
}
// UI詳細管理
static TEXTURE2D_DESC* g_td_ss = NULL;


//*****************************************************************************
// ボタン定義
//*****************************************************************************
// ボタン名（優先順）
enum BT_LABEL_MAIN {
	BT_MAIN_JAPAN = 0,
	BT_MAIN_ASIA,
	BT_MAIN_EUROPE,
	BT_MAIN_NORTHAMERICA,

	BT_MAIN_BACK,

	BT_MAIN_NUM
};
// 参照UI名（ボタン名順）
#define REF_UL_MAIN {\
	UI_JAPAN_BG,\
	UI_ASIA_BG,\
	UI_EUROPE_BG,\
	UI_NORTHAMERICA_BG,\
	UI_BACK,\
}
enum BT_LABEL_JAPAN {
	BT_JAPAN_OSAKA = 0,
	BT_JAPAN_HOKKAIDO,
	BT_JAPAN_OKINAWA,

	BT_JAPAN_NUM
};
// 参照UI名（ボタン名順）
#define REF_UL_JAPAN {\
	UI_JAPAN_OSAKA_BG,\
	UI_JAPAN_HOKKAIDO_BG,\
	UI_JAPAN_OKINAWA_BG,\
}
enum BT_LABEL_ASIA {
	BT_ASIA_CHINA = 0,
	BT_ASIA_KOREA,
	BT_ASIA_SINGAPORE,

	BT_ASIA_NUM
};
// 参照UI名（ボタン名順）
#define REF_UL_ASIA {\
	UI_ASIA_CHINA_BG,\
	UI_ASIA_KOREA_BG,\
	UI_ASIA_SINGAPORE_BG,\
}
enum BT_LABEL_EUROPE {
	BT_EUROPE_FRANCE = 0,
	BT_EUROPE_ENGLAND,
	BT_EUROPE_ITALY,

	BT_EUROPE_NUM
};
// 参照UI名（ボタン名順）
#define REF_UL_EUROPE {\
	UI_EUROPE_FRANCE_BG,\
	UI_EUROPE_ENGLAND_BG,\
	UI_EUROPE_ITALY_BG,\
}
enum BT_LABEL_NORTHAMERICA {
	BT_NORTHAMERICA_AMERICA = 0,
	BT_NORTHAMERICA_CANADA,
	BT_NORTHAMERICA_MEXICO,

	BT_NORTHAMERICA_NUM
};
// 参照UI名（ボタン名順）
#define REF_UL_NORTHAMERICA {\
	UI_NORTHAMERICA_AMERICA_BG,\
	UI_NORTHAMERICA_CANADA_BG,\
	UI_NORTHAMERICA_MEXICO_BG,\
}
// ボタン詳細管理
static BUTTON_DESC* g_bd_main = NULL;
static BUTTON_DESC* g_bd_japan = NULL;
static BUTTON_DESC* g_bd_asia = NULL;
static BUTTON_DESC* g_bd_europe = NULL;
static BUTTON_DESC* g_bd_northamerica = NULL;
// ボタン表
#define BT_NUM_X_MAIN 1
#define BT_NUM_Y_MAIN 5
static int g_btTbl_main[BT_NUM_Y_MAIN][BT_NUM_X_MAIN] = {
	{BT_MAIN_JAPAN},
	{BT_MAIN_ASIA},
	{BT_MAIN_EUROPE},
	{BT_MAIN_NORTHAMERICA},
	{BT_MAIN_BACK},
};
#define BT_NUM_X_ELSE 1
#define BT_NUM_Y_ELSE 3
static int g_btTbl_japan[BT_NUM_Y_ELSE][BT_NUM_X_ELSE] = {
	{BT_JAPAN_OSAKA},
	{BT_JAPAN_HOKKAIDO},
	{BT_JAPAN_OKINAWA},
};
static int g_btTbl_asia[BT_NUM_Y_ELSE][BT_NUM_X_ELSE] = {
	{BT_ASIA_CHINA},
	{BT_ASIA_KOREA},
	{BT_ASIA_SINGAPORE},
};
static int g_btTbl_europe[BT_NUM_Y_ELSE][BT_NUM_X_ELSE] = {
	{BT_EUROPE_FRANCE},
	{BT_EUROPE_ENGLAND},
	{BT_EUROPE_ITALY},
};
static int g_btTbl_northamerica[BT_NUM_Y_ELSE][BT_NUM_X_ELSE] = {
	{BT_NORTHAMERICA_AMERICA},
	{BT_NORTHAMERICA_CANADA},
	{BT_NORTHAMERICA_MEXICO},
};
static BUTTON_TABLE* g_bt_main = NULL;
static BUTTON_TABLE* g_bt_japan = NULL;
static BUTTON_TABLE* g_bt_asia = NULL;
static BUTTON_TABLE* g_bt_europe = NULL;
static BUTTON_TABLE* g_bt_northamerica = NULL;
// カーソル位置
static XMINT2 g_cursor = { 0, 0 };

//*****************************************************************************
// ローカル関数
//*****************************************************************************
// メニューの種類と部品名からテクスチャ名を取得
static int GetTexNo(void) {
	if (g_menu == MENU_MAIN) return UI_JAPAN + g_cursor.y * 12;
	else return UI_JAPAN + (g_menu - 1) * 12 + (g_cursor.y + 1) * 3;
}
static int GetGpsNo(void) {
	if (g_menu == MENU_MAIN) return g_cursor.y;
	else return GPS_OSAKA + (g_menu - 1) * 3 + g_cursor.y;
}
// パネルのアニメーション
static void PannelAnim(void)
{
	int index = GetTexNo();

	++index;
	g_td_ss[index].scl.y = g_AnimScl;
	g_td_ss[index].uv_pos.v = 0.5f - g_AnimScl * 0.5f;
	g_td_ss[index].uv_pos.vh = g_AnimScl;

	++index;
	g_td_ss[index].scl.y = g_AnimScl;
	g_td_ss[index].uv_pos.v = 0.5f - g_AnimScl * 0.5f;
	g_td_ss[index].uv_pos.vh = g_AnimScl;
}
static void PannelAnimDD(void)
{
	int index = UI_JAPAN_BG_ALL + (g_menuDD - 1);
	g_td_ss[index].scl.y = g_DDScl;
	g_td_ss[index].posAdd.y = (g_DDScl - 1.0f) * g_td_ss[index].size.y * 0.5f;
	g_td_ss[index].uv_pos.vh = g_DDScl;

	g_td_ss[UI_BACK].col.w = 1.0f - g_DDScl;
}
static void AlphaAnimDD(void)
{
	int index = UI_JAPAN_OSAKA + 12 * (g_menuDD - 1);

	g_td_ss[index].col.w = g_DDAlpha;
	g_td_ss[index + 3].col.w = g_DDAlpha;
	g_td_ss[index + 6].col.w = g_DDAlpha;
}

// ボタンごとの処理
static void ButtonPressedMain(int b)
{
	if (g_DDScl > 0.0f) return;
	switch (b)
	{
	case BT_MAIN_JAPAN:
	case BT_MAIN_ASIA:
	case BT_MAIN_EUROPE:
	case BT_MAIN_NORTHAMERICA:

		g_menu =(MENU_LABEL)(b + 1);
		g_cursor.y = 0;
		g_AnimScl = 0.0f;
		PannelAnim();

		g_menuDD = g_menu;

		g_DDScl = 0.0f;
		PannelAnimDD();

		g_DDAlpha = 0.0f;
		AlphaAnimDD();

		g_PinScl = 0.0f;
		g_td_ss[UI_PIN].scl = { g_PinScl, g_PinScl };

		break;

	case BT_MAIN_BACK:
	case BT_BACKSPACE:
		SetFade(FADE_OUT, MODE_HOME);
		break;

	default:
		break;
	}
}
static void ButtonPressedJapan(int b)
{
	if (g_DDAlpha < 1.0f) return;
	switch (b)
	{
	case BT_JAPAN_OSAKA:
	case BT_JAPAN_HOKKAIDO:
	case BT_JAPAN_OKINAWA:
		//g_SelectedStage = 
		SetFade(FADE_OUT, MODE_GAME);
		break;

	default:
		g_menu = MENU_MAIN;
		g_cursor.y = 0;
		g_AnimScl = 0.0f;
		PannelAnim();
		break;
	}
}
static void ButtonPressedAsia(int b)
{
	if (g_DDAlpha < 1.0f) return;
	switch (b)
	{
	case BT_ASIA_CHINA:
	case BT_ASIA_KOREA:
	case BT_ASIA_SINGAPORE:
		break;

	default:
		g_menu = MENU_MAIN;
		g_cursor.y = 1;
		g_AnimScl = 0.0f;
		PannelAnim();
		break;
	}
}
static void ButtonPressedEurope(int b)
{
	if (g_DDAlpha < 1.0f) return;
	switch (b)
	{
	case BT_EUROPE_FRANCE:
	case BT_EUROPE_ENGLAND:
	case BT_EUROPE_ITALY:
		break;

	default:
		g_menu = MENU_MAIN;
		g_cursor.y = 2;
		g_AnimScl = 0.0f;
		PannelAnim();
		break;
	}
}
static void ButtonPressedNorthAmerica(int b)
{
	if (g_DDAlpha < 1.0f) return;
	switch (b)
	{
	case BT_NORTHAMERICA_AMERICA:
	case BT_NORTHAMERICA_CANADA:
	case BT_NORTHAMERICA_MEXICO:
		break;

	default:
		g_menu = MENU_MAIN;
		g_cursor.y = 3;
		g_AnimScl = 0.0f;
		PannelAnim();
		break;
	}
}

// 初期化
static void InitUI(void)
{
	// メモリ確保
	g_td_ss = new TEXTURE2D_DESC[UI_NUM];
	g_bd_main = new BUTTON_DESC[BT_MAIN_NUM];
	g_bd_japan = new BUTTON_DESC[BT_JAPAN_NUM];
	g_bd_asia = new BUTTON_DESC[BT_ASIA_NUM];
	g_bd_europe = new BUTTON_DESC[BT_EUROPE_NUM];
	g_bd_northamerica = new BUTTON_DESC[BT_NORTHAMERICA_NUM];

	// UI詳細設定
	TEXTURE_LABEL tl[UI_NUM] = REF_TL;
	for (int i = 0; i < UI_NUM; i++)
	{
		// テクスチャの対応付け
		g_td_ss[i].tex = tl[i];

		// テクスチャサイズの取得
		g_td_ss[i].size = GetTextureSize(g_td_ss[i].tex);

		g_td_ss[i].posType = POSITION_LEFTTOP;
		g_td_ss[i].ctrType = CENTER_LEFTTOP;
	}

	g_td_ss[UI_LINE].pos.x -= 6.0f;

	g_td_ss[UI_PIN].posType = POSITION_CENTER;
	g_td_ss[UI_PIN].ctrType = CENTER_BOTTOM;
	g_td_ss[UI_PIN].pos.x = SCREEN_WIDTH * 0.2f;

	g_td_ss[UI_CIRCLE].posType = POSITION_RIGHT;
	g_td_ss[UI_CIRCLE].ctrType = CENTER_CENTER;
	g_td_ss[UI_CIRCLE].pos = { -320.0f, -60.0f };

	g_td_ss[UI_BACK].posType = POSITION_RIGHTTOP;
	g_td_ss[UI_BACK].ctrType = CENTER_RIGHTTOP;
	g_td_ss[UI_BACK].pos = { -30.0f, 20.0f };
	g_td_ss[UI_BACK].sd_pos = { 2.0f, 2.0f };
	g_bd_main[BT_MAIN_BACK].col_on = { 100.0f, 100.0f, 0.0f, 1.0f };
	g_bd_main[BT_MAIN_BACK].col_off = { 1.0f, 1.0f, 1.0f, 1.0f };

	int d = 0;
	for (int j = 0; j < REGION_NUM; j++)
	{
		g_td_ss[UI_JAPAN_BG_ALL + j].pos.y = g_td_ss[UI_HEAD].size.y + g_td_ss[UI_JAPAN_BG].size.y * (j + 1);

		for (int i = 0; i < AREA_NUM + 1; i++)
		{
			d = TEX_NUM * (AREA_NUM + 1) * j + TEX_NUM * i;

			g_td_ss[UI_JAPAN + d].ctrType = CENTER_TOP;
			g_td_ss[UI_JAPAN + d].pos.x = g_td_ss[UI_BG].size.x * 0.35f;
			g_td_ss[UI_JAPAN + d].pos.y = g_td_ss[UI_HEAD].size.y + g_td_ss[UI_JAPAN_BG].size.y * (j + i);

			g_td_ss[UI_JAPAN_B + d] = g_td_ss[UI_JAPAN + d];
			g_td_ss[UI_JAPAN_B + d].col = { 0.0f, 0.2f, 0.0f, 1.0f };

			g_td_ss[UI_JAPAN_BG + d].pos.y = g_td_ss[UI_JAPAN + d].pos.y;
		}
	}

	// ボタン詳細設定
	UI_LABEL ul_main[BT_MAIN_NUM] = REF_UL_MAIN;
	for (int i = 0; i < BT_MAIN_BACK; i++)
	{
		g_bd_main[i].col_off = g_bd_main[i].col_on;
		g_bd_main[i].scl_off = g_bd_main[i].scl_on;
		SetUIButton(&g_bd_main[i], &g_td_ss[ul_main[i]]);
	}
	SetUIButton(&g_bd_main[BT_MAIN_BACK], &g_td_ss[ul_main[BT_MAIN_BACK]]);
	UI_LABEL ul_japan[BT_JAPAN_NUM] = REF_UL_JAPAN;
	for (int i = 0; i < BT_JAPAN_NUM; i++)
	{
		g_bd_japan[i].col_off = g_bd_japan[i].col_on;
		g_bd_japan[i].scl_off = g_bd_japan[i].scl_on;
		SetUIButton(&g_bd_japan[i], &g_td_ss[ul_japan[i]]);
	}
	UI_LABEL ul_asia[BT_ASIA_NUM] = REF_UL_ASIA;
	for (int i = 0; i < BT_ASIA_NUM; i++)
	{
		g_bd_asia[i].col_off = g_bd_asia[i].col_on;
		g_bd_asia[i].scl_off = g_bd_asia[i].scl_on;
		SetUIButton(&g_bd_asia[i], &g_td_ss[ul_asia[i]]);
	}
	UI_LABEL ul_europe[BT_EUROPE_NUM] = REF_UL_EUROPE;
	for (int i = 0; i < BT_EUROPE_NUM; i++)
	{
		g_bd_europe[i].col_off = g_bd_europe[i].col_on;
		g_bd_europe[i].scl_off = g_bd_europe[i].scl_on;
		SetUIButton(&g_bd_europe[i], &g_td_ss[ul_europe[i]]);
	}
	UI_LABEL ul_northamerica[BT_NORTHAMERICA_NUM] = REF_UL_NORTHAMERICA;
	for (int i = 0; i < BT_NORTHAMERICA_NUM; i++)
	{
		g_bd_northamerica[i].col_off = g_bd_northamerica[i].col_on;
		g_bd_northamerica[i].scl_off = g_bd_northamerica[i].scl_on;
		SetUIButton(&g_bd_northamerica[i], &g_td_ss[ul_northamerica[i]]);
	}

	// ボタンテーブルへの登録
	g_bt_main = new BUTTON_TABLE;
	*g_bt_main = { &g_btTbl_main[0][0], BT_NUM_X_MAIN, BT_NUM_Y_MAIN, &g_bd_main[0], BT_MAIN_NUM, &g_cursor };
	
	g_bt_japan = new BUTTON_TABLE;
	*g_bt_japan = { &g_btTbl_japan[0][0], BT_NUM_X_ELSE, BT_NUM_Y_ELSE, &g_bd_japan[0], BT_JAPAN_NUM, &g_cursor };
	
	g_bt_asia = new BUTTON_TABLE;
	*g_bt_asia = { &g_btTbl_asia[0][0], BT_NUM_X_ELSE, BT_NUM_Y_ELSE, &g_bd_asia[0], BT_ASIA_NUM, &g_cursor };
	
	g_bt_europe = new BUTTON_TABLE;
	*g_bt_europe = { &g_btTbl_europe[0][0], BT_NUM_X_ELSE, BT_NUM_Y_ELSE, &g_bd_europe[0], BT_EUROPE_NUM, &g_cursor };
	
	g_bt_northamerica = new BUTTON_TABLE;
	*g_bt_northamerica = { &g_btTbl_northamerica[0][0], BT_NUM_X_ELSE, BT_NUM_Y_ELSE, &g_bd_northamerica[0], BT_NORTHAMERICA_NUM, &g_cursor };
}
// 更新
static void UpdateUI(void)
{
	if (g_AnimScl < 1.0f) return;
	switch (g_menu)
	{
	case MENU_MAIN:
		UpdateButton(g_bt_main, ButtonPressedMain);
		break;

	case MENU_JAPAN:
		UpdateButton(g_bt_japan, ButtonPressedJapan);
		break;

	case MENU_ASIA:
		UpdateButton(g_bt_asia, ButtonPressedAsia);
		break;

	case MENU_EUROPE:
		UpdateButton(g_bt_europe, ButtonPressedEurope);
		break;

	case MENU_NORTHAMERICA:
		UpdateButton(g_bt_northamerica, ButtonPressedNorthAmerica);
		break;
	}
}
// 描画
static void DrawUI(void)
{
	DrawTexture2D(&g_td_ss[UI_BG]);
	DrawTexture2D(&g_td_ss[UI_HEAD]);
	DrawTexture2D(&g_td_ss[UI_BACK], TRUE);

	DrawTexture2D(&g_td_ss[UI_JAPAN]);
	DrawTexture2D(&g_td_ss[UI_ASIA]);
	DrawTexture2D(&g_td_ss[UI_EUROPE]);
	DrawTexture2D(&g_td_ss[UI_NORTHAMERICA]);

	int index;
	if (g_menu != MENU_MAIN)
	{
		DrawTexture2D(&g_td_ss[UI_JAPAN_BG_ALL + (g_menu - 1)]);

		index = (g_menu - 1) * 12;

		g_td_ss[UI_JAPAN_B + index].scl.x = 1.0f;
		g_td_ss[UI_JAPAN_B + index].scl.y = 1.0f;

		DrawTexture2D(&g_td_ss[UI_JAPAN_B + index]);
		DrawTexture2D(&g_td_ss[UI_JAPAN_OSAKA + index]);
		DrawTexture2D(&g_td_ss[UI_JAPAN_HOKKAIDO + index]);
		DrawTexture2D(&g_td_ss[UI_JAPAN_OKINAWA + index]);

		DrawTexture2D(&g_td_ss[UI_PIN], TRUE);
	}
	else if (g_DDScl > 0.0f)
	{
		DrawTexture2D(&g_td_ss[UI_JAPAN_BG_ALL + (g_menuDD - 1)]);

		index = (g_menuDD - 1) * 12;
		DrawTexture2D(&g_td_ss[UI_JAPAN_OSAKA + index]);
		DrawTexture2D(&g_td_ss[UI_JAPAN_HOKKAIDO + index]);
		DrawTexture2D(&g_td_ss[UI_JAPAN_OKINAWA + index]);
	}


	if (g_cursor.y < BT_NUM_Y_MAIN - 1) {

		static float time = 0.0f;
		time += 0.05f; if (time > XM_2PI) time -= XM_2PI;
		if (g_AnimScl >= 1.0f) {
			g_td_ss[GetTexNo() + 1].scl.x = 1.0f + 0.025f * sinf(time);
			g_td_ss[GetTexNo() + 1].scl.y = 1.0f + 0.025f * sinf(time);
		}

		DrawTexture2D(&g_td_ss[GetTexNo() + 2], FALSE, TRUE);
		DrawTexture2D(&g_td_ss[GetTexNo() + 1], TRUE, TRUE);
	}

	DrawTexture2D(&g_td_ss[UI_LINE]);
	DrawTexture2D(&g_td_ss[UI_CIRCLE]);
}
// 終了
static void UninitUI(void)
{
	delete[] g_td_ss, g_bd_main, g_bd_japan, g_bd_asia, g_bd_europe, g_bd_northamerica;
	delete g_bt_main, g_bt_japan, g_bt_asia, g_bt_europe, g_bt_northamerica;
}

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitStageSelect(void)
{
	if (g_Load) return S_OK;

	InitUI();

	// アニメーション初期設定
	g_AnimSlide = SLIDE_X;
	g_AnimScl = 0.0f;
	g_AnimAlpha = 0.0f;
	for (int i = ALPHA_START; i <= ALPHA_END; i++) {
		g_td_ss[i].col.w = g_AnimAlpha;
	}
	g_td_ss[UI_HEAD].posAdd.x = g_AnimSlide;
	PannelAnim();

	g_DDAlpha = 0.0f;
	AlphaAnimDD();
	g_DDScl = 0.0f;
	PannelAnimDD();

	g_PinScl = 0.0f;
	g_td_ss[UI_PIN].scl = { g_PinScl, g_PinScl };

	g_Model[MODEL_TITLE_STAR].model = MODEL_EARTH;
	g_Model[MODEL_TITLE_STAR].srt.scl = { 80.0f, 80.0f, 80.0f };

	g_Model[MODEL_TITLE_EARTH].model = MODEL_EARTH;
	g_Model[MODEL_TITLE_EARTH].srt.pos.z = (1.3f - g_DDScl * 0.3f) * OBJ_DIST;
	g_Model[MODEL_TITLE_EARTH].srt.scl = { 20.0f, 20.0f, 20.0f };

	g_gpsRef = new XMFLOAT2[GPS_NUM];
	XMFLOAT2 gps[GPS_NUM] = GPS;
	for (int i = 0; i < GPS_NUM; i++) {
		g_gpsRef[i].x = -(XM_PIDIV2 * gps[i].x / 90.0f) - 0.015f;
		g_gpsRef[i].y = XM_PI + (XM_PI * gps[i].y / 180.0f) - 0.07f;
	}
	
	g_gps = g_gpsRef[0];

	g_Load = TRUE;

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitStageSelect(void)
{
	if (g_Load == FALSE) return;

	UninitUI();

	delete[] g_gpsRef;

	g_Load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateStageSelect(void)
{
	if (g_AnimAlpha > 0.0f) UpdateUI();

	static int old_cur_y = g_cursor.y;
	if (old_cur_y != g_cursor.y) {
		if (!(g_menu == MENU_MAIN && g_cursor.y == BT_NUM_Y_MAIN - 1)) {
			g_AnimScl = 0.0f;
			PannelAnim();
			old_cur_y = g_cursor.y;

			g_PinScl =0.0f;
			g_td_ss[UI_PIN].scl = { g_PinScl, g_PinScl };
		}
	}

	if (GetMode() == MODE_STAGESELECT)
	{	// 出てくるとき
		if (g_AnimSlide < 0.0f) {
			g_AnimSlide = min(g_AnimSlide + ANIM_SLIDE, 0.0f);
			g_td_ss[UI_HEAD].posAdd.x = g_AnimSlide;
		}
		else if (g_AnimAlpha < 1.0f)
		{
			g_AnimAlpha += ANIM_ALPHA;
			for (int i = ALPHA_START; i <= ALPHA_END; i++) {
				g_td_ss[i].col.w = g_AnimAlpha;
			}
		}
		else if (g_AnimScl < 1.0f)
		{
			if (g_menu != MENU_MAIN) {
				if (g_DDAlpha >= 1.0f) {
					g_AnimScl += ANIM_SCALING;
					PannelAnim();
				}
			}
			else {
				g_AnimScl += ANIM_SCALING;
				PannelAnim();
			}
		}
	}
	else
	{	// しまうとき
		if (g_AnimScl > 0.0f)
		{
			g_AnimScl -= ANIM_SCALING;
			PannelAnim();
		}
		else if (g_AnimAlpha > 0.0f)
		{
			g_AnimAlpha -= ANIM_ALPHA;
			for (int i = ALPHA_START; i <= ALPHA_END; i++) {
				g_td_ss[i].col.w = g_AnimAlpha;
			}
		}
		else if (g_AnimSlide > SLIDE_X) {
			g_AnimSlide = max(g_AnimSlide - ANIM_SLIDE, SLIDE_X);
			g_td_ss[UI_HEAD].posAdd.x = g_AnimSlide;
		}
		return;
	}

	// ドロップダウンアニメーション
	if (g_menu != MENU_MAIN)
	{
		if (g_DDScl < 1.0f)
		{
			g_DDScl = min(g_DDScl + ANIM_SCALING, 1.0f);
			PannelAnimDD();
			g_Model[MODEL_TITLE_EARTH].srt.pos.z = (1.3f - g_DDScl * 0.3f) * OBJ_DIST;
		}
		else if (g_DDAlpha < 1.0f)
		{
			g_DDAlpha = min(g_DDAlpha + ANIM_ALPHA, 1.0f);
			AlphaAnimDD();
		}
	}
	else
	{
		if (g_DDAlpha > 0.0f)
		{
			g_DDAlpha = max(g_DDAlpha - ANIM_ALPHA, 0.0f);
			AlphaAnimDD();
		}
		else if (g_DDScl > 0.0f)
		{
			g_DDScl = max(g_DDScl - ANIM_SCALING, 0.0f);
			PannelAnimDD();
			g_Model[MODEL_TITLE_EARTH].srt.pos.z = (1.3f - g_DDScl * 0.3f) * OBJ_DIST;
		}
	}

	g_td_ss[UI_CIRCLE].rot -= 0.0005f;
	if (g_td_ss[UI_CIRCLE].rot < -XM_2PI) g_td_ss[UI_CIRCLE].rot += XM_2PI;

	g_Model[MODEL_TITLE_STAR].srt.rot.y += 0.0001f;
	if (g_Model[MODEL_TITLE_STAR].srt.rot.y > XM_2PI) g_Model[MODEL_TITLE_STAR].srt.rot.y -= XM_2PI;


	if (g_cursor.y < BT_NUM_Y_MAIN - 1)
	{
		float dy = g_gpsRef[GetGpsNo()].y - g_Model[MODEL_TITLE_EARTH].srt.rot.y;
		float dx = g_gpsRef[GetGpsNo()].x - g_Model[MODEL_TITLE_EARTH].srt.rot.x;
			g_Model[MODEL_TITLE_EARTH].srt.rot.y += dy * 0.08f;
			g_Model[MODEL_TITLE_EARTH].srt.rot.x += dx * 0.08f;
		if ((g_PinScl < 1.0f) && (fabs(dy) < 0.005f && fabs(dx) < 0.005f))
		{
			g_PinScl = min(g_PinScl + ANIM_SCALING, 1.0f);
			g_td_ss[UI_PIN].scl = { g_PinScl, g_PinScl };
		}

		//g_Model[MODEL_TITLE_EARTH].srt.rot.x -= g_gpsRef[GetGpsNo()].y;
	}
	else
	{
		g_Model[MODEL_TITLE_EARTH].srt.rot.y -= 0.001f;
		if (g_Model[MODEL_TITLE_EARTH].srt.rot.y < -XM_2PI) g_Model[MODEL_TITLE_EARTH].srt.rot.y += XM_2PI;

	}

	static float g_x = 0.0f;
	static float g_y = 0.0f;
	static const float d = 0.005f;
	if (GetKeyboardTrigger(DIK_1)) {
		for (int i = 0; i < GPS_NUM; i++) {
			g_gpsRef[i].x += d;
		}
		g_x += d;
	}
	if (GetKeyboardTrigger(DIK_2)) {
		for (int i = 0; i < GPS_NUM; i++) {
			g_gpsRef[i].x -= d;
		}
		g_x -= d;
	}
	if (GetKeyboardTrigger(DIK_3)) {
		for (int i = 0; i < GPS_NUM; i++) {
			g_gpsRef[i].y += d;
		}
		g_y += d;
	}
	if (GetKeyboardTrigger(DIK_4)) {
		for (int i = 0; i < GPS_NUM; i++) {
			g_gpsRef[i].y -= d;
		}
		g_y -= d;
	}
}

//=============================================================================
// 
//=============================================================================
void DrawStageSelect(void)
{
	DrawUI();


	SetDrawNoLighting();

	// 星
	SetCullingMode(CULL_MODE_FRONT);
	DrawModel(&g_Model[MODEL_TITLE_STAR].model, &g_Model[MODEL_TITLE_STAR].srt, TEXTURE_LABEL_STAR);
	SetCullingMode(CULL_MODE_BACK);

	// 地球
	SetViewPortStageSelect();

	XMMATRIX mtxWorld;
	SRT srt;
	MATERIAL material;

	material.Specular = { 0.0f, 0.0f, 0.0f, 0.0f };
	mtxWorld = XMMatrixIdentity();
	srt = g_Model[MODEL_TITLE_EARTH].srt;
	MulMtxScl(mtxWorld, srt.scl.x, srt.scl.y, srt.scl.z);	// スケールを反映
	MulMtxRot(mtxWorld, 0.0f, srt.rot.y, 0.0f);				// 回転を反映
	MulMtxRot(mtxWorld, srt.rot.x, 0.0f, 0.0f);				// 回転を反映
	MulMtxPos(mtxWorld, srt.pos.x, srt.pos.y, srt.pos.z);	// 移動を反映
	DrawModel(&g_Model[MODEL_TITLE_EARTH].model, &mtxWorld, &material);

	ResetViewPort();
}
//
//void DownColChange(int a, int b,int c)
//{
//	g_td_ss[g_Menu + a].col = COL_BLACK;
//	g_td_ss[g_Menu + a - 1].col = COL_ORIGINAL;
//	if (g_Menu == b)
//	{
//		g_td_ss[c].col = COL_ORIGINAL;
//	}
//
//}
//
//void UpColChange(int a, int b)
//{
//	g_td_ss[g_Menu + a].col = COL_BLACK;
//	g_td_ss[g_Menu + a + 1].col = COL_ORIGINAL;
//	if (g_Menu == b)
//	{
//		g_td_ss[a].col = COL_ORIGINAL;
//	}
//
//}
//
//void PannelPlus(void)
//{
//	g_pannel++;
//	g_pannelNow = g_pannel;
//	g_pressSelect = 0;
//}
//
//int GetStageNumber(void)
//{
//	switch (g_Menu)
//	{
//	case STAGE_JAPAN:
//
//		return test2 + g_pressSelect - TEXTURE_JAPAN_STAGE_1;
//
//		break;
//	case STAGE_ASIA:
//
//		return test2 + g_pressSelect - TEXTURE_JAPAN_STAGE_1 - 1;
//
//		break;
//	case STAGE_EUROPE:
//
//		return test2 + g_pressSelect - TEXTURE_JAPAN_STAGE_1 - 2;
//
//		break;
//	case STAGE_NORTH_AMERICA:
//
//		return test2 + g_pressSelect - TEXTURE_JAPAN_STAGE_1 - 3;
//
//		break;
//
//	default:
//		break;
//	}
//	return 0;
//}

int GetSelectedStage(void) {
	return g_SelectedStage;
}