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
	TEXTURE_LOAD_ANIM,
	TEXTURE_BAR_EMPTY,
	TEXTURE_BAR_FULL,
	TEXTURE_BG,
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
	for (int i = 0; i < TEXTURE_MAX; i++) {
		g_td[i].tex = (TEXTURE_LABEL)(TEXTURE_LABEL_LOAD_TEXT + i);
	}

	// 詳細設定 
	g_td[TEXTURE_LOAD_ANIM].size = { 175.0f * 0.5f, 300.0f * 0.5f };
	g_td[TEXTURE_LOAD_ANIM].uv_pos.uw = 1.0f / (float)LOAD_ANIM_NUM;
	g_td[TEXTURE_LOAD_ANIM].pos.y = LOADMAP_Y - 190.0f + LOAD_Y;

	g_td[TEXTURE_NOWLOADING].size = { 350.0f, 32.0f * 1.5f };
	g_td[TEXTURE_NOWLOADING].pos.y = LOADMAP_Y + 70.0f + LOAD_Y + 32.0f * 1.5f;

	g_td[TEXTURE_NOWLOADING2].size = { 150.0f, 32.0f * 1.5f };
	g_td[TEXTURE_NOWLOADING2].pos.y = LOADMAP_Y + 70.0f + LOAD_Y;
	g_td[TEXTURE_NOWLOADING2].uv_pos = { 0.0f, 0.0f, 1.0f, 1.0f / (float)LOAD_MAX };

	g_td[TEXTURE_BAR_EMPTY].size = { 800.0f, 55.0f };
	g_td[TEXTURE_BAR_EMPTY].pos.y = LOAD_Y;
	g_td[TEXTURE_BAR_FULL].size = { 800.0f, 55.0f };
	g_td[TEXTURE_BAR_FULL].pos.y = LOAD_Y;


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

			// ロード完了
			//SetFade(FADE_OUT, MODE_TITLE_START);
			SetFade(FADE_OUT, MODE_GAME);
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
		static int time = 0;
		float anim = (float)((int)(time++ / LOAD_ANIM_SPAN) % LOAD_ANIM_NUM) / (float)LOAD_ANIM_NUM;
		g_td[TEXTURE_LOAD_ANIM].uv_pos.u = anim;
		DrawTexture2D(&g_td[TEXTURE_LOAD_ANIM], TRUE, TRUE);
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
		DrawTexture2D(&g_td[TEXTURE_BAR_EMPTY], TRUE);

		float rate = (float)g_LoadSum / (float)LOAD_SUM;
		g_td[TEXTURE_BAR_FULL].size.x = 800.0f * rate;
		g_td[TEXTURE_BAR_FULL].pos.x = -800.0f * 0.5f * (1.0f - rate);
		g_td[TEXTURE_BAR_FULL].uv_pos.uw = rate;
		DrawTexture2D(&g_td[TEXTURE_BAR_FULL], FALSE, TRUE);
	}

//#ifdef _DEBUG	// デバッグ情報を表示する
//	PrintDebugProc("LoadPoint:%f\n", (float)g_LoadPoint + g_LoadPalam);
//	PrintDebugProc("LoadRate:%f\n", (float)g_LoadSum / (float)LOAD_SUM);
//#endif
}
