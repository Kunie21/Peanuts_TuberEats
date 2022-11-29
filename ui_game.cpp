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
	TEXTURE_TIMER_BACK,
	TEXTURE_NUMBER,
	TEXTURE_SEMICOLON,
	TEXTURE_WHITE,
	TEXTURE_MAX,
};
static TEXTURE2D_DESC	g_td[TEXTURE_MAX];

static float g_Timer = 0.0f;

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitGameUI(void)
{
	// テクスチャ生成
	for (int i = 0; i < TEXTURE_WHITE; i++)
	{
		//D3DX11CreateShaderResourceViewFromFile(GetDevice(), g_TextureName[i], NULL, NULL, &g_Texture[i], NULL);
		g_td[i].tex = (TEXTURE_LABEL)(TEXTURE_LABEL_FUEL_EMPTY + i);
	}
	g_td[TEXTURE_WHITE].tex = TEXTURE_LABEL_WHITE;

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
	g_td[TEXTURE_GOALPIN].pos = { MAP_EDGE, MAP_LINE_Y - 40.0f };

	// タイマー
	g_td[TEXTURE_TIMER_BACK].size = { 169.0f, 79.0f };
	g_td[TEXTURE_TIMER_BACK].pos = { TIMER_CENTER_X, MAP_LINE_Y };
	g_td[TEXTURE_SEMICOLON].size = { 20.0f, 90.0f };
	g_td[TEXTURE_SEMICOLON].pos = { TIMER_CENTER_X, MAP_LINE_Y };
	g_td[TEXTURE_NUMBER].size = { 255.0f, 90.0f };
	g_td[TEXTURE_NUMBER].pos = { TIMER_CENTER_X - 25.0f, MAP_LINE_Y };
	g_td[TEXTURE_NUMBER].scl = { 0.1f, 1.0f };
	g_td[TEXTURE_NUMBER].uv_pos = { 0.1f, 0.0f, 0.1f, 1.0f };

	// スクリーンエフェクト用
	g_td[TEXTURE_WHITE].col = { 0.0f, 0.0f, 0.0f, 0.0f };

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitGameUI(void)
{
	if (g_Load == FALSE) return;

	g_Load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateGameUI(void)
{
	g_td[TEXTURE_WHITE].col.w *= 0.97f;
	OnTimer();
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawGameUI(void)
{
	int time;
	float digit;
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		switch (i)
		{
		case TEXTURE_FUEL_FULL:
		case TEXTURE_SPEED_GAUGE:
			DrawTexture2D(&g_td[i], FALSE, TRUE);
			break;
		case TEXTURE_WHITE:
			DrawTexture2D(&g_td[i]);
			break;
		case TEXTURE_NUMBER:
			time = (int)(g_Timer * 100.0f);
			digit = 1.5f;
			while (time)
			{
				g_td[i].uv_pos.u = (float)(time % 10) * 0.1f;
				if (digit > 0.0f)
				{
					g_td[i].pos.x = TIMER_CENTER_X + digit * 25.5f + 10.0f;
				}
				else
				{
					g_td[i].pos.x = TIMER_CENTER_X + digit * 25.5f - 10.0f;
				}
				DrawTexture2D(&g_td[i], FALSE, TRUE);
				digit -= 1.0f;

				time /= 10;
			}
			if (g_Timer < 1.0f)	// 0秒を描画
			{
				g_td[i].uv_pos.u = 0.0f;
				g_td[i].pos.x = TIMER_CENTER_X - 0.5f * 25.5f - 10.0f;
				DrawTexture2D(&g_td[i], FALSE, TRUE);
			}
			break;
		default:
			DrawTexture2D(&g_td[i], TRUE);
			break;
		}
	}
}

void SetMapPosition(float rate)
{
	static float rateBuffer = 0.0f;
	if (rate < 0.0f) rate = 0.0f;
	if (rate > 1.0f) rate = 1.0f;
	rateBuffer += (rate - rateBuffer) * 0.5f;
	g_td[TEXTURE_MAP_ROCKET].pos.x = -MAP_EDGE + MAP_SIZE * rateBuffer;
}

void SetFuelMeter(float rate)
{
	static float rateBuffer = 0.0f;
	if (rate < 0.0f) rate = 0.0f;
	if (rate > 1.0f) rate = 1.0f;
	rateBuffer += (rate - rateBuffer) * 0.5f;
	g_td[TEXTURE_FUEL_FULL].scl.x = rateBuffer;
	g_td[TEXTURE_FUEL_FULL].uv_pos.uw = rateBuffer;
	g_td[TEXTURE_FUEL_FULL].pos.x = FUEL_CENTER_X + (rateBuffer - 1.0f) * g_td[TEXTURE_FUEL_FULL].size.x * 0.5f;
	if (rate < 0.25f)
	{
		g_td[TEXTURE_FUEL_EMPTY].col = { 1.0f, 0.7f, 0.7f, 1.0f };
	}
	else if (rate < 0.5f)
	{
		g_td[TEXTURE_FUEL_EMPTY].col = { 1.0f, 1.0f, 0.7f, 1.0f };
	}
}
void SetSpeedMeter(float rate)
{
	static float rateBuffer = 0.0f;
	if (rate < 0.0f) rate = 0.0f;
	if (rate > 1.0f) rate = 1.0f;
	rateBuffer += (rate - rateBuffer) * 0.5f;
	g_td[TEXTURE_SPEED_GAUGE].scl.y = rateBuffer;
	g_td[TEXTURE_SPEED_GAUGE].uv_pos.v = 1.0f - rateBuffer;
	g_td[TEXTURE_SPEED_GAUGE].uv_pos.vh = rateBuffer;
	g_td[TEXTURE_SPEED_GAUGE].pos.y = SPEED_CENTER_Y + (1.0f - rateBuffer) * g_td[TEXTURE_SPEED_GAUGE].size.y * 0.5f;
}

void SetDamageEffect(void)
{
	g_td[TEXTURE_WHITE].col = { 1.0f, 0.0f, 0.0f, 0.5f };
}
void SetBoostEffect(void)
{
	g_td[TEXTURE_WHITE].col = { 1.0f, 1.0f, 1.0f, 0.5f };
}
void SetTimer(float time)
{
	g_Timer = time;
}
void OnTimer(void)
{
	g_Timer += 0.01666666f;
}

float GetTime(void)
{
	return g_Timer;
}