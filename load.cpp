//=============================================================================
//
// ロード処理 [load.cpp]
// Author : 國江 翔太
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "texture2d.h"
#include "model.h"
#include "sound.h"
#include "fade.h"
#include "load.h"
#include "debugproc.h"
#include "load2.h"
#include "game.h"

// ロードするモノの数
#define LOAD_SUM (SOUND_LABEL_MAX + MODEL_MAX + TEXTURE_LABEL_MAX - TEXTURE_LABEL_COUNTDOWN3)

#define LOAD_Y					(110.0f)	// 
#define LOADMAP_Y				(0.0f)		// 
#define LOADMAP_ICON_SCALE		(0.18f)		// 

#define LOAD_ANIM_NUM	(12)
#define LOAD_ANIM_SPAN	(6)

/*******************************************************************************
* グローバル変数
*******************************************************************************/

// テクスチャ管理
enum
{
	TEXTURE_NOWLOADING = 0,
	TEXTURE_NOWLOADING2,
	TEXTURE_BAR_EMPTY,
	TEXTURE_BAR_FULL,
	TEXTURE_BG,
	TEXTURE_ROCKET,
	TEXTURE_NUMBER0,
	TEXTURE_NUMBER1,
	TEXTURE_NUMBER2,
	TEXTURE_NUMBER3,
	TEXTURE_NUMBER4,
	TEXTURE_NUMBER5,
	TEXTURE_NUMBER6,
	TEXTURE_NUMBER7,
	TEXTURE_NUMBER8,
	TEXTURE_NUMBER9,
	TEXTURE_PERCENT,
	TEXTURE_TRANSITION_UP,
	TEXTURE_TRANSITION_DOWN,
	TEXTURE_BG_UP,
	TEXTURE_BG_DOWN,
	TEXTURE_MAX,
};
static TEXTURE2D_DESC	g_td[TEXTURE_MAX];


// ロード用スレッド
static HANDLE	g_hThread = NULL;
DWORD WINAPI ThreadFuncLoad(LPVOID pParam)
{
	while (TRUE) UpdateLoad();
	return 0;
}

static int	g_LoadSum = 0;
void InitLoad(void)
{
	// テクスチャ生成
	for (int i = 0; i < TEXTURE_TRANSITION_DOWN; i++) {
		g_td[i].tex = (TEXTURE_LABEL)(TEXTURE_LABEL_LOAD_TEXT + i);
		g_td[i].size = GetTextureSize(g_td[i].tex);
	}
	// 詳細設定 
	//g_td[TEXTURE_LOAD_ANIM].size = { 175.0f * 0.5f, 300.0f * 0.5f };
	//g_td[TEXTURE_LOAD_ANIM].uv_pos.uw = 1.0f / (float)LOAD_ANIM_NUM;
	//g_td[TEXTURE_LOAD_ANIM].pos.y = LOADMAP_Y - 190.0f + LOAD_Y;

	g_td[TEXTURE_NOWLOADING].size = { 350.0f, 32.0f * 1.5f };
	g_td[TEXTURE_NOWLOADING].pos.y = LOADMAP_Y + 70.0f + LOAD_Y + 32.0f * 1.5f;

	g_td[TEXTURE_NOWLOADING2].size = { 150.0f, 32.0f * 1.5f };
	g_td[TEXTURE_NOWLOADING2].pos.y = LOADMAP_Y + 70.0f + LOAD_Y;
	g_td[TEXTURE_NOWLOADING2].uv_pos = { 0.0f, 0.0f, 1.0f, 1.0f / (float)LOAD_MAX };

	g_td[TEXTURE_BAR_EMPTY].pos.y = LOAD_Y;
	g_td[TEXTURE_BAR_FULL].pos.y = LOAD_Y;
	g_td[TEXTURE_ROCKET].pos.y = LOAD_Y;
	g_td[TEXTURE_ROCKET].posType = POSITION_LEFT;
	g_td[TEXTURE_ROCKET].ctrType = CENTER_LEFT;

	g_td[TEXTURE_TRANSITION_UP].ctrType = CENTER_BOTTOM;
	g_td[TEXTURE_TRANSITION_UP].uv_pos.vh = 0.0f;
	g_td[TEXTURE_TRANSITION_UP].pos.y = LOAD_Y;
	g_td[TEXTURE_TRANSITION_DOWN] = g_td[TEXTURE_TRANSITION_UP];
	g_td[TEXTURE_TRANSITION_DOWN].ctrType = CENTER_TOP;
	g_td[TEXTURE_TRANSITION_DOWN].rot = XM_PI;

	g_td[TEXTURE_BG_UP].tex = TEXTURE_LABEL_LOAD_BG;
	g_td[TEXTURE_BG_UP].size = g_td[TEXTURE_BG].size;
	g_td[TEXTURE_BG_UP].size.y = g_td[TEXTURE_BG_UP].size.y * 0.5f + LOAD_Y;
	g_td[TEXTURE_BG_UP].ctrType = CENTER_TOP;
	g_td[TEXTURE_BG_UP].posType = POSITION_TOP;
	g_td[TEXTURE_BG_UP].uv_pos.vh = g_td[TEXTURE_BG_UP].size.y / g_td[TEXTURE_BG].size.y;

	g_td[TEXTURE_BG_DOWN].tex = TEXTURE_LABEL_LOAD_BG;
	g_td[TEXTURE_BG_DOWN].size = g_td[TEXTURE_BG].size;
	g_td[TEXTURE_BG_DOWN].size.y = g_td[TEXTURE_BG_DOWN].size.y - g_td[TEXTURE_BG_UP].size.y;
	g_td[TEXTURE_BG_DOWN].ctrType = CENTER_BOTTOM;
	g_td[TEXTURE_BG_DOWN].posType = POSITION_BOTTOM;
	g_td[TEXTURE_BG_DOWN].uv_pos.v = g_td[TEXTURE_BG_UP].uv_pos.vh;
	g_td[TEXTURE_BG_DOWN].uv_pos.vh = 1.0f - g_td[TEXTURE_BG_DOWN].uv_pos.v;

	// 軽いものは先に読み込んじゃう
	//InitMap();

	// 別のスレッドを作成
	g_hThread = CreateThread(NULL, 0, ThreadFuncLoad, NULL, 0, NULL);
}

static int g_LoadPoint = 0;
void UpdateLoad(void)
{
	if (CheckFadeIn() && g_LoadPoint < LOAD_MAX)
	{
		if (LoadingNow()) g_LoadPoint++;
		if (g_LoadPoint == LOAD_MAX)
		{
			g_LoadPoint--;
			CloseHandle(g_hThread);	// スレッドを閉じる

			InitLoad2();

			InitGame();

			// ロード完了
			//SetFade(FADE_OUT, MODE_TITLE_START);
			SetMode(POST_LOAD_MODE);
		}
	}
}

BOOL LoadingNow(void)
{
	switch (g_LoadPoint)
	{
	case LOAD_TEXTURE:
		return LoadTextureKernel();

	case LOAD_SOUND:
		return LoadSoundKernel();

	case LOAD_MODEL:
		return LoadModelKernel();
	}
	return FALSE;
}

void AddLoadSum(void) {
	g_LoadSum++;
}

void DrawLoad(void)
{
	// 背景
	{
		DrawTexture2D(&g_td[TEXTURE_BG]);
	}

	// Animation
	{
		//static int time = 0;
		//float anim = (float)((int)(time++ / LOAD_ANIM_SPAN) % LOAD_ANIM_NUM) / (float)LOAD_ANIM_NUM;
		//g_td[TEXTURE_LOAD_ANIM].uv_pos.u = anim;
		//DrawTexture2D(&g_td[TEXTURE_LOAD_ANIM], TRUE, TRUE);
	}

	// テキスト表示
	{
		static float time = 0.0f;
		time += 0.05f;
		g_td[TEXTURE_NOWLOADING2].uv_pos.v = (float)g_LoadPoint / (float)LOAD_MAX;
		g_td[TEXTURE_NOWLOADING2].col.w = 1.1f + sinf(time);
		g_td[TEXTURE_NOWLOADING].col.w = 1.1f + sinf(time);
		DrawTexture2D(&g_td[TEXTURE_NOWLOADING2], TRUE, TRUE);
		DrawTexture2D(&g_td[TEXTURE_NOWLOADING], TRUE);
	}

	// バー
	{
		// 空バー
		DrawTexture2D(&g_td[TEXTURE_BAR_EMPTY]);

		// バー
		float rate = (float)g_LoadSum / (float)LOAD_SUM;
		g_td[TEXTURE_BAR_FULL].size.x = GetTextureSize(g_td[TEXTURE_BAR_FULL].tex).x * rate;
		g_td[TEXTURE_BAR_FULL].pos.x = -GetTextureSize(g_td[TEXTURE_BAR_FULL].tex).x * 0.5f * (1.0f - rate);
		g_td[TEXTURE_BAR_FULL].uv_pos.uw = rate;
		DrawTexture2D(&g_td[TEXTURE_BAR_FULL], FALSE, TRUE);

		// ロケット
		g_td[TEXTURE_ROCKET].pos.x = g_td[TEXTURE_BAR_FULL].size.x - 10.0f;
		DrawTexture2D(&g_td[TEXTURE_ROCKET], TRUE);


		// パーセンテージ
		if (rate < 0.1f) // 1桁
		{
			int num = TEXTURE_NUMBER0 + (int)(rate * 100.0f) % 10;
			g_td[num].pos.x = -g_td[num].size.x * 0.5f;
			DrawTexture2D(&g_td[num], TRUE);

			g_td[TEXTURE_PERCENT].pos.x = g_td[TEXTURE_PERCENT].size.x * 0.5f;
			DrawTexture2D(&g_td[TEXTURE_PERCENT], TRUE);
		}
		else if (rate < 1.0f) // 2桁
		{
			int num1 = TEXTURE_NUMBER0 + (int)(rate * 100.0f) % 10;
			int num10 = TEXTURE_NUMBER0 + (int)(rate * 10.0f) % 10;

			g_td[num10].pos.x = -(g_td[num10].size.x * 0.5f + g_td[num1].size.x * 0.5f);
			DrawTexture2D(&g_td[num10], TRUE);

			g_td[num1].pos.x = 0.0f;
			DrawTexture2D(&g_td[num1], TRUE);

			g_td[TEXTURE_PERCENT].pos.x = g_td[TEXTURE_PERCENT].size.x * 0.5f + g_td[num1].size.x * 0.5f;
			DrawTexture2D(&g_td[TEXTURE_PERCENT], TRUE);
		}
		else // 3桁
		{
			int num1 = TEXTURE_NUMBER0 + (int)(rate * 100.0f) % 10;
			int num10 = TEXTURE_NUMBER0 + (int)(rate * 10.0f) % 10;
			int num100 = TEXTURE_NUMBER0 + (int)rate % 10;

			g_td[num100].pos.x = -(g_td[num10].size.x + g_td[num100].size.x * 0.5f);
			DrawTexture2D(&g_td[num100], TRUE);

			g_td[num10].pos.x = -g_td[num10].size.x * 0.5f;
			DrawTexture2D(&g_td[num10], TRUE);

			g_td[num1].pos.x = g_td[num1].size.x * 0.5f;
			DrawTexture2D(&g_td[num1], TRUE);

			g_td[TEXTURE_PERCENT].pos.x = g_td[TEXTURE_PERCENT].size.x * 0.5f + g_td[num1].size.x;
			DrawTexture2D(&g_td[TEXTURE_PERCENT], TRUE);
		}
	}

	//#ifdef _DEBUG	// デバッグ情報を表示する
	//	PrintDebugProc("LoadPoint:%f\n", (float)g_LoadPoint + g_LoadPalam);
	//	PrintDebugProc("LoadRate:%f\n", (float)g_LoadSum / (float)LOAD_SUM);
	//#endif
}

static BOOL	g_bLoadAfter = FALSE;
BOOL GetLoadAfter(void) { return g_bLoadAfter; }
void DrawLoadAfter(void)
{
	if (g_td[TEXTURE_TRANSITION_DOWN].posAdd.y >= g_td[TEXTURE_BG].size.y)
	{
		g_bLoadAfter = TRUE;
		return;
	}

	static float slide = 0.0f;
	static float spd = 0.0f;

	// 背景
	{
		g_td[TEXTURE_BG_UP].posAdd.y = -slide;
		g_td[TEXTURE_BG_DOWN].posAdd.y = slide;
		DrawTexture2D(&g_td[TEXTURE_BG_UP]);
		DrawTexture2D(&g_td[TEXTURE_BG_DOWN]);
	}

	// バー
	{
		// バー
		if (slide < g_td[TEXTURE_BG].size.y) {
			DrawTexture2D(&g_td[TEXTURE_BAR_FULL], FALSE, TRUE);
		}

		// パーセンテージ
		int num1 = TEXTURE_NUMBER0;
		int num10 = TEXTURE_NUMBER0;
		int num100 = TEXTURE_NUMBER1;

		g_td[num100].pos.x = -(g_td[num10].size.x + g_td[num100].size.x * 0.5f);
		g_td[num100].posAdd.y = -slide;
		DrawTexture2D(&g_td[num100], TRUE);

		g_td[num10].pos.x = -g_td[num10].size.x * 0.5f;
		g_td[num10].posAdd.y = -slide;
		DrawTexture2D(&g_td[num10], TRUE);

		g_td[num1].pos.x = g_td[num1].size.x * 0.5f;
		g_td[num1].posAdd.y = -slide;
		DrawTexture2D(&g_td[num1], TRUE);

		g_td[TEXTURE_PERCENT].pos.x = g_td[TEXTURE_PERCENT].size.x * 0.5f + g_td[num1].size.x;
		g_td[TEXTURE_PERCENT].posAdd.y = -slide;
		DrawTexture2D(&g_td[TEXTURE_PERCENT], TRUE);
	}

	// 幕
	{
		if (slide < g_td[TEXTURE_BG].size.y)
		{
			g_td[TEXTURE_TRANSITION_UP].size.y = slide;
			g_td[TEXTURE_TRANSITION_UP].uv_pos.vh = g_td[TEXTURE_TRANSITION_UP].size.y / g_td[TEXTURE_BG].size.y;

			g_td[TEXTURE_TRANSITION_DOWN].size.y = slide;
			g_td[TEXTURE_TRANSITION_DOWN].uv_pos.vh = g_td[TEXTURE_TRANSITION_UP].uv_pos.vh;
		}
		else
		{
			g_td[TEXTURE_TRANSITION_DOWN].posAdd.y = slide - g_td[TEXTURE_BG].size.y;
			g_td[TEXTURE_TRANSITION_UP].posAdd.y = -g_td[TEXTURE_TRANSITION_DOWN].posAdd.y;
		}
		DrawTexture2D(&g_td[TEXTURE_TRANSITION_UP]);
		DrawTexture2D(&g_td[TEXTURE_TRANSITION_DOWN]);
	}

	slide += spd;
	spd += 0.2f;

	g_bLoadAfter = FALSE;
}
