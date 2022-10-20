//=============================================================================
//
// ゲームUI処理 [ui_game.cpp]
// Author : 國江 翔太
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "texture2d.h"
#include "ui_game.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MAP_LINE_Y		(-450.0f)
#define MAP_EDGE		(700.0f)
#define MAP_SIZE		(1400.0f)
#define SPEED_CENTER_X	(-800.0f)
#define SPEED_CENTER_Y	(300.0f)
#define FUEL_LINE_Y		(SPEED_CENTER_Y + 180.0f)
#define FUEL_CENTER_X	(-600.0f)
#define TIMER_CENTER_X	(MAP_EDGE + 125.0f)

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static BOOL				g_Load = FALSE;

// テクスチャ管理
enum
{
	TEXTURE_FUEL_EMPTY = 0,
	TEXTURE_FUEL_FONT,
	TEXTURE_FUEL_FULL,
	TEXTURE_GOALPIN,
	TEXTURE_ICON,
	TEXTURE_MAP,
	TEXTURE_MAP_ROCKET,
	TEXTURE_SPEED_EMPTY,
	TEXTURE_SPEED_FONT,
	TEXTURE_SPEED_GAUGE,
	TEXTURE_NUMBER,
	TEXTURE_TIMER_BACK,
	TEXTURE_SEMICOLON, 
	TEXTURE_MAX,
};
static TEXTURE2D_DESC	g_td[TEXTURE_MAX];
static ID3D11ShaderResourceView*	g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報
static char*	g_TextureName[TEXTURE_MAX] = {
	"data/TEXTURE/game_UI/fuel_empty.png",
	"data/TEXTURE/game_UI/fuel_font.png",
	"data/TEXTURE/game_UI/fuel_full.png",
	"data/TEXTURE/game_UI/goalpin_UI.png",
	"data/TEXTURE/game_UI/icon_UI.png",
	"data/TEXTURE/game_UI/map_UI.png",
	"data/TEXTURE/game_UI/rocket_map.png",
	"data/TEXTURE/game_UI/speed_empty.png",
	"data/TEXTURE/game_UI/speed_font.png",
	"data/TEXTURE/game_UI/speed_gauge.png",
	"data/TEXTURE/game_UI/timer_UI.png",
	"data/TEXTURE/game_UI/timer_backdrop.png",
	"data/TEXTURE/game_UI/timer_semicolon.png",
};

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitGameUI(void)
{
	// テクスチャ生成
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		D3DX11CreateShaderResourceViewFromFile(GetDevice(), g_TextureName[i], NULL, NULL, &g_Texture[i], NULL);
		g_td[i].tex = &g_Texture[i];
	}

	// 詳細設定
	// スピードメーター
	g_td[TEXTURE_SPEED_FONT].size = { 82.0f, 23.0f };
	g_td[TEXTURE_SPEED_FONT].pos = { SPEED_CENTER_X + 149.0f, SPEED_CENTER_Y + 154.0f };
	g_td[TEXTURE_SPEED_EMPTY].size = { 218.0f, 333.0f };
	g_td[TEXTURE_SPEED_EMPTY].pos = { SPEED_CENTER_X, SPEED_CENTER_Y };
	g_td[TEXTURE_SPEED_GAUGE].size = { 218.0f, 333.0f };
	g_td[TEXTURE_SPEED_GAUGE].pos = { SPEED_CENTER_X, SPEED_CENTER_Y };

	// 燃料メーター
	g_td[TEXTURE_FUEL_FONT].size = { 62.0f, 23.0f };
	g_td[TEXTURE_FUEL_FONT].pos = { FUEL_CENTER_X - 310.0f, FUEL_LINE_Y };
	g_td[TEXTURE_FUEL_EMPTY].size = { 554.0f, 24.0f };
	g_td[TEXTURE_FUEL_EMPTY].pos = { FUEL_CENTER_X, FUEL_LINE_Y };
	g_td[TEXTURE_FUEL_FULL].size = { 554.0f, 24.0f };
	g_td[TEXTURE_FUEL_FULL].pos = { FUEL_CENTER_X, FUEL_LINE_Y };

	// プレイヤーアイコン
	g_td[TEXTURE_ICON].size = { 183.0f, 183.0f };
	g_td[TEXTURE_ICON].pos = { -700.0f, 350.0f };

	// マップ
	g_td[TEXTURE_MAP].size = { 1425.0f, 25.0f };
	g_td[TEXTURE_MAP].pos = { 0.0f, MAP_LINE_Y };
	g_td[TEXTURE_MAP_ROCKET].size = { 91.0f, 40.0f };
	g_td[TEXTURE_MAP_ROCKET].pos = { -MAP_EDGE, MAP_LINE_Y };
	g_td[TEXTURE_GOALPIN].size = { 36.0f, 48.0f };
	g_td[TEXTURE_GOALPIN].pos = { MAP_EDGE, MAP_LINE_Y - 50.0f };

	// タイマー
	g_td[TEXTURE_TIMER_BACK].size = { 169.0f, 79.0f };
	g_td[TEXTURE_TIMER_BACK].pos = { TIMER_CENTER_X, MAP_LINE_Y };
	g_td[TEXTURE_SEMICOLON].size = { 20.0f, 90.0f };
	g_td[TEXTURE_SEMICOLON].pos = { TIMER_CENTER_X, MAP_LINE_Y };
	g_td[TEXTURE_NUMBER].size = { 267.0f, 90.0f };
	g_td[TEXTURE_NUMBER].pos = { TIMER_CENTER_X, MAP_LINE_Y };
	g_td[TEXTURE_NUMBER].scl = { 0.0f, 0.0f };

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitGameUI(void)
{
	if (g_Load == FALSE) return;

	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		if (g_Texture[i])
		{
			g_Texture[i]->Release();
			g_Texture[i] = NULL;
		}
	}

	g_Load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateGameUI(void)
{
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawGameUI(void)
{
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		if(i== TEXTURE_FUEL_FULL || i == TEXTURE_SPEED_GAUGE)
		{
			DrawTexture2D(&g_td[i], FALSE, TRUE);
		}
		else
		{
			DrawTexture2D(&g_td[i], TRUE);
		}
	}
}

void SetMapPosition(float rate)
{
	g_td[TEXTURE_MAP_ROCKET].pos.x = -MAP_EDGE + MAP_SIZE * rate;
}

void SetFuelMeter(float rate)
{
	g_td[TEXTURE_FUEL_FULL].scl.x = rate;
	g_td[TEXTURE_FUEL_FULL].uv_pos.uw = rate;
	g_td[TEXTURE_FUEL_FULL].pos.x = FUEL_CENTER_X + (rate - 1.0f) * g_td[TEXTURE_FUEL_FULL].size.x * 0.5f;
}
void SetSpeedMeter(float rate)
{
	g_td[TEXTURE_SPEED_GAUGE].scl.y = rate;
	g_td[TEXTURE_SPEED_GAUGE].uv_pos.v = 1.0f - rate;
	g_td[TEXTURE_SPEED_GAUGE].uv_pos.vh = rate;
	g_td[TEXTURE_SPEED_GAUGE].pos.y = SPEED_CENTER_Y + (1.0f - rate) * g_td[TEXTURE_SPEED_GAUGE].size.y * 0.5f;
}
