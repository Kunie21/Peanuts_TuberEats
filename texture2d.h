//=============================================================================
//
// 2D画像描画処理 [texture2D.h]
// Author : 國江 翔太
//
//=============================================================================
#pragma once

enum TEXTURE_LABEL {
	TEXTURE_LABEL_WHITE = 0,
	TEXTURE_LABEL_TEAMLOGO,

	TEXTURE_LABEL_LOAD_TEXT,
	TEXTURE_LABEL_LOAD_TEXT2,
	TEXTURE_LABEL_LOAD_ANIM,
	TEXTURE_LABEL_LOAD_BAR_EMPTY,
	TEXTURE_LABEL_LOAD_BAR_FULL,
	TEXTURE_LABEL_LOAD_BG,

	TEXTURE_LABEL_COUNTDOWN3,
	TEXTURE_LABEL_COUNTDOWN2,
	TEXTURE_LABEL_COUNTDOWN1,
	TEXTURE_LABEL_COUNTDOWN0,
	TEXTURE_LABEL_GOAL,
	TEXTURE_LABEL_PARTICLE,

	TEXTURE_LABEL_TUBE_WAFER,
	TEXTURE_LABEL_TUBE_LIGHT,
		
	TEXTURE_LABEL_FUEL_EMPTY,
	TEXTURE_LABEL_FUEL_FONT,
	TEXTURE_LABEL_FUEL_FULL,
	TEXTURE_LABEL_GOALPIN,
	TEXTURE_LABEL_ICON,
	TEXTURE_LABEL_MAP,
	TEXTURE_LABEL_MAP_ROCKET,
	TEXTURE_LABEL_SPEED_EMPTY,
	TEXTURE_LABEL_SPEED_FONT,
	TEXTURE_LABEL_SPEED_GAUGE,
	TEXTURE_LABEL_TIMER_BACK,
	TEXTURE_LABEL_NUMBER,
	TEXTURE_LABEL_SEMICOLON,
	

	TEXTURE_LABEL_CHARACTER_01,
	TEXTURE_LABEL_CHARACTER_02,
	TEXTURE_LABEL_CHARACTER_03,
	TEXTURE_LABEL_CHARACTER_LAST,
	TEXTURE_LABEL_START_BUTTON_1,
	TEXTURE_LABEL_START_BUTTON_2,
	TEXTURE_LABEL_START_BUTTON_3,
	TEXTURE_LABEL_BG,
	
	TEXTURE_LABEL_SHOP_MENU,
	TEXTURE_LABEL_STATUSBAR,
	TEXTURE_LABEL_STATUSBAR_POINT,
	TEXTURE_LABEL_ROCKET_ICON_LOCK,
	TEXTURE_LABEL_ROCKET_ICON_NEW,
	TEXTURE_LABEL_ROCKET_ICON,
	TEXTURE_LABEL_ROCKET_ICON_EQUIP,
	TEXTURE_LABEL_ROCKET_1,
	TEXTURE_LABEL_ROCKET_2,
	TEXTURE_LABEL_ROCKET_3,
	TEXTURE_LABEL_ROCKET_4,
	TEXTURE_LABEL_ROCKETSELECT_1,
	TEXTURE_LABEL_ROCKETSELECT_2,
	TEXTURE_LABEL_ROCKETSELECT_3,
	TEXTURE_LABEL_ROCKETSELECT_4,
	TEXTURE_LABEL_SHOP,
	TEXTURE_LABEL_LEFT,
	TEXTURE_LABEL_RIGHT,
	
	TEXTURE_LABEL_WALLET,
	TEXTURE_LABEL_WALLET_TOTAL,
	TEXTURE_LABEL_WALLET_DOT,
	TEXTURE_LABEL_WALLET_COMMA,
	TEXTURE_LABEL_YEN,
	TEXTURE_LABEL_MINUS_TOTAL,
	TEXTURE_LABEL_MINUS_DOT,
	TEXTURE_LABEL_MINUS_COMMA,
	TEXTURE_LABEL_MINUS,
	TEXTURE_LABEL_SHOP_MENU_OPACITY,
	TEXTURE_LABEL_BUY_MENU,
	TEXTURE_LABEL_BUY_MENU_SELECTION,
	TEXTURE_LABEL_NO,
	TEXTURE_LABEL_BUY,

	TEXTURE_LABEL_RESULT_BAR,
	TEXTURE_LABEL_RESULT_BG,
	TEXTURE_LABEL_RESULT_NO_STARS,
	TEXTURE_LABEL_RESULT_GREEN_STARS,
	TEXTURE_LABEL_RESULT_EXPRESSION_1,
	TEXTURE_LABEL_RESULT_EXPRESSION_2,
	TEXTURE_LABEL_RESULT_EXPRESSION_3,
	TEXTURE_LABEL_RESULT_SCROLL_BAR,
	TEXTURE_LABEL_RESULT_SCROLL,
	TEXTURE_LABEL_RESULT_DELIVERY_DISTAMCE,
	TEXTURE_LABEL_RESULT_DELIVERY_TIME,
	TEXTURE_LABEL_RESULT_DELIVERY_FEE,
	TEXTURE_LABEL_RESULT_TIP,
	TEXTURE_LABEL_RESULT_DAMAGE,
	TEXTURE_LABEL_RESULT_TOTAL_AMOUNT,
	TEXTURE_LABEL_RESULT_YEN,
	TEXTURE_LABEL_RESULT_MIUS_YEN,
	TEXTURE_LABEL_RESULT_TOTAL_YEN,
	TEXTURE_LABEL_RESULT_NUMBER,
	TEXTURE_LABEL_RESULT_MIUS_NUMBER,
	TEXTURE_LABEL_RESULT_TOTAL_NUMBER,
	TEXTURE_LABEL_RESULT_POINT,
	TEXTURE_LABEL_RESULT_MIUS_POINT,
	TEXTURE_LABEL_RESULT_TOTAL_POINT,
	TEXTURE_LABEL_RESULT_FIREWORK_1,
	TEXTURE_LABEL_RESULT_FIREWORK_2,
	TEXTURE_LABEL_RESULT_FIREWORK_3,
	TEXTURE_LABEL_RESULT_FIREWORK_4,
	TEXTURE_LABEL_RESULT_FIREWORK_5,

	//TEXTURE_LABEL_OOSAKA,
	//TEXTURE_LABEL_SHANGHAI,
	//TEXTURE_LABEL_PARIS,
	//TEXTURE_LABEL_NEWYORK,
	TEXTURE_LABEL_OOSAKA,
	TEXTURE_LABEL_HOKKAIDO,
	TEXTURE_LABEL_OKINAWA,
	TEXTURE_LABEL_SHANGHAI,
	TEXTURE_LABEL_KOREA,
	TEXTURE_LABEL_SINGAPORE,
	TEXTURE_LABEL_FRANCE,
	TEXTURE_LABEL_ENGLAND,
	TEXTURE_LABEL_ITALY,
	TEXTURE_LABEL_AMERICA,
	TEXTURE_LABEL_CANADA,
	TEXTURE_LABEL_MEXICO,

	TEXTURE_LABEL_STAR,

	TEXTURE_LABEL_MENUBOARD,
	TEXTURE_LABEL_MENU_LINE,
	TEXTURE_LABEL_START,
	TEXTURE_LABEL_START_01,
	TEXTURE_LABEL_START_JP,
	TEXTURE_LABEL_MENU_PANNEL_01,
	TEXTURE_LABEL_OPTION,
	TEXTURE_LABEL_OPTION_01,
	TEXTURE_LABEL_OPTION_JP,
	TEXTURE_LABEL_MENU_PANNEL_02,
	TEXTURE_LABEL_GALLERY,
	TEXTURE_LABEL_GALLERY_01,
	TEXTURE_LABEL_GALLERY_JP,
	TEXTURE_LABEL_MENU_PANNEL_03,
	TEXTURE_LABEL_CREDIT,
	TEXTURE_LABEL_CREDIT_01,
	TEXTURE_LABEL_CREDIT_JP,
	TEXTURE_LABEL_MENU_PANNEL_04,
	TEXTURE_LABEL_QUIT,
	TEXTURE_LABEL_QUIT_01,
	TEXTURE_LABEL_QUIT_JP,
	TEXTURE_LABEL_MENU_PANNEL_05,

	TEXTURE_LABEL_TITLE,
	TEXTURE_LABEL_BUTTON,
	TEXTURE_LABEL_COPYR,

	TEXTURE_LABEL_STAGE_SELECTION_MAP,
	TEXTURE_LABEL_STAGE_SELECTION_MAP_1,
	TEXTURE_LABEL_CIRCLE_SPIN,
	TEXTURE_LABEL_JAPAN,
	TEXTURE_LABEL_ASIA,
	TEXTURE_LABEL_EUROPE,
	TEXTURE_LABEL_NORTH_AMERICA,
	TEXTURE_LABEL_STAGE_SELECT_1,
	TEXTURE_LABEL_STAGE_SELECT_2,
	TEXTURE_LABEL_STAGE_SELECT_3,
	TEXTURE_LABEL_STAGE_SELECT_4,
	TEXTURE_LABEL_STAGE_SELECT_5,
	TEXTURE_LABEL_STAGE_SELECT_6,
	TEXTURE_LABEL_STAGE_SELECT_7,
	TEXTURE_LABEL_JAPAN_MENU,
	TEXTURE_LABEL_JAPAN_STAGE_1,
	TEXTURE_LABEL_JAPAN_STAGE_2,
	TEXTURE_LABEL_JAPAN_STAGE_3,
	TEXTURE_LABEL_ASIA_MENU,
	TEXTURE_LABEL_ASIA_STAGE_1,
	TEXTURE_LABEL_ASIA_STAGE_2,
	TEXTURE_LABEL_ASIA_STAGE_3,
	TEXTURE_LABEL_EUROPE_MENU,
	TEXTURE_LABEL_EUROPE_STAGE_1,
	TEXTURE_LABEL_EUROPE_STAGE_2,
	TEXTURE_LABEL_EUROPE_STAGE_3,
	TEXTURE_LABEL_NORTH_AMERICA_MENU,
	TEXTURE_LABEL_NORTH_AMERICA_STAGE_1,
	TEXTURE_LABEL_NORTH_AMERICA_STAGE_2,
	TEXTURE_LABEL_NORTH_AMERICA_STAGE_3,

	TEXTURE_LABEL_BACK,

	TEXTURE_LABEL_MAX,
};

// 2D画像描画用データ
struct TEXTURE2D_DESC
{
	UV_POSITION uv_pos = { 0.0f, 0.0f, 1.0f, 1.0f };	// UV座標

	XMFLOAT4 col = { 1.0f, 1.0f, 1.0f, 1.0f };			// 色

	XMFLOAT4 outline = { 100.0f, 100.0f, 0.0f, 1.0f };	// 色

	XMFLOAT4 sd_col = { 0.0f, 0.0f, 0.0f, 0.5f };		// 影色
	XMFLOAT2 sd_pos = { 5.0f, 5.0f };					// 影位置（ずらす量）

	XMFLOAT2 size = { SCREEN_WIDTH, SCREEN_HEIGHT };	// サイズ

	XMFLOAT2 pos = { 0.0f, 0.0f };						// 固定位置
	XMFLOAT2 posAdd = { 0.0f, 0.0f };					// 位置（固定位置からずらしたいとき）

	XMFLOAT2 scl = { 1.0f, 1.0f };						// 拡大率

	POSITION_TYPE posType = POSITION_CENTER;			// 位置の種類
	CENTER_TYPE ctrType = CENTER_CENTER;				// 中心の種類

	//ID3D11ShaderResourceView** tex = NULL;			// テクスチャ
	TEXTURE_LABEL	tex = TEXTURE_LABEL_WHITE;			// テクスチャ

	float wight = 5.0f;
	BOOL b_useOutline = FALSE;
	BOOL b_outline = FALSE;

	float rot = 0.0f;									// 回転
};

struct BUTTON_DESC {
	XMFLOAT4 col_on = { 1.0f, 1.0f, 1.0f, 1.0f };	// 色
	XMFLOAT4 col_off = { 0.8f, 0.8f, 0.8f, 1.0f };	// 色OFF
	XMFLOAT2 scl_on = { 1.0f, 1.0f };	// 拡大率
	XMFLOAT2 scl_off = { 0.9f, 0.9f };	// 拡大率OFF
	XMFLOAT2 pos = { 0.0f, 0.0f };	// 左上から
	XMFLOAT2 size = { 0.0f, 0.0f };
	TEXTURE_LABEL tex_on = TEXTURE_LABEL_WHITE;			// テクスチャ
	TEXTURE_LABEL tex_off = TEXTURE_LABEL_WHITE;			// テクスチャ
	TEXTURE2D_DESC* p_td = NULL;
	BOOL b_on = FALSE;
};

struct BUTTON_TABLE {
	int* tbl = NULL;
	int tbl_x = 0;
	int tbl_y = 0;

	BUTTON_DESC* bd = NULL;
	int num = 0;

	XMINT2* cursor;
};

//struct OUTLINE_DESC {
//	XMFLOAT4 col = { 1.0f, 1.0f, 0.0f, 1.0f };	// 色
//	float wight = 5.0f;
//};

XMFLOAT2 ConvertToAbsolutePosition(XMFLOAT2 pos, XMFLOAT2 size, POSITION_TYPE pt, CENTER_TYPE ct);
void SetUIButton(BUTTON_DESC* ub, TEXTURE2D_DESC* td);
void UpdateButton(BUTTON_TABLE* bt, void bp(int b));

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitTexture2D(void);
HRESULT LoadTexture2D(void);
void UninitTexture2D(void);
void UpdateTexture2D(void);
void DrawTexture2D(TEXTURE2D_DESC* td, BOOL bShadow = FALSE, BOOL bUV = FALSE, BOOL bOutline = FALSE);
void SetUVTexture2D(UV_POSITION* uv);
void ResetUVTexture2D(void);
void DrawTexture2DAll(BOOL bInterrupt = FALSE);	// TRUEで割り込み（途中で描画する）
ID3D11ShaderResourceView** GetTexture(TEXTURE_LABEL texture);

BOOL LoadTextureKernel(void);

XMFLOAT2 GetTextureSize(TEXTURE_LABEL label);