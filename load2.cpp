//=============================================================================
//
// ロード処理 [load2.cpp]
// Author : 國江 翔太
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "texture2d.h"
#include "sound.h"
#include "fade.h"
#include "load2.h"

#define ANIM_NUM		(48)
//#define ANIM_FPS		(24)
#define ANIM_FPS		(2.5f)
#define RIGHT_BOTTOM	(-50.0f)
#ifdef _DEBUG
#define BAR_MAX			(0.0f)
#else
#define BAR_MAX			(200.0f)
#endif

/*******************************************************************************
* グローバル変数
*******************************************************************************/
static unsigned int g_time = 0;
static MODE_LABEL	g_ModeNext;

enum
{
	TEXTURE_BG = 0,
	TEXTURE_DOT,
	TEXTURE_EMPTY,
	TEXTURE_FULL,
	TEXTURE_ANIM,
	TEXTURE_WHITE,
	TEXTURE_MAX,
};
static TEXTURE2D_DESC	g_tdLoad2[TEXTURE_MAX];

void InitLoad2(void)
{
	for (int i = 0; i < TEXTURE_MAX - 1; i++) {
		g_tdLoad2[i].tex = (TEXTURE_LABEL)(TEXTURE_LABEL_LOAD2_BG + i);
		g_tdLoad2[i].size = GetTextureSize(g_tdLoad2[i].tex);
	}

	g_tdLoad2[TEXTURE_EMPTY].ctrType = CENTER_RIGHTBOTTOM;
	g_tdLoad2[TEXTURE_EMPTY].posType = POSITION_RIGHTBOTTOM;
	g_tdLoad2[TEXTURE_EMPTY].pos = { RIGHT_BOTTOM, RIGHT_BOTTOM };

	g_tdLoad2[TEXTURE_FULL].ctrType = CENTER_LEFTBOTTOM;
	g_tdLoad2[TEXTURE_FULL].posType = g_tdLoad2[TEXTURE_EMPTY].posType;
	g_tdLoad2[TEXTURE_FULL].pos = g_tdLoad2[TEXTURE_EMPTY].pos;
	g_tdLoad2[TEXTURE_FULL].pos.x -= g_tdLoad2[TEXTURE_EMPTY].size.x;

	g_tdLoad2[TEXTURE_ANIM].ctrType = g_tdLoad2[TEXTURE_EMPTY].ctrType;
	g_tdLoad2[TEXTURE_ANIM].posType = g_tdLoad2[TEXTURE_EMPTY].posType;
	g_tdLoad2[TEXTURE_ANIM].pos = g_tdLoad2[TEXTURE_EMPTY].pos;
	g_tdLoad2[TEXTURE_ANIM].pos.y -= 20.0f;
	g_tdLoad2[TEXTURE_ANIM].size = { 450.0f, 225.0f };
	g_tdLoad2[TEXTURE_ANIM].uv_pos.uw = 1.0f / (float)ANIM_NUM;

	g_tdLoad2[TEXTURE_DOT].ctrType = g_tdLoad2[TEXTURE_EMPTY].ctrType;
	g_tdLoad2[TEXTURE_DOT].posType = g_tdLoad2[TEXTURE_EMPTY].posType;
	g_tdLoad2[TEXTURE_DOT].pos = g_tdLoad2[TEXTURE_EMPTY].pos;
	g_tdLoad2[TEXTURE_DOT].pos.x -= 160.0f;
	g_tdLoad2[TEXTURE_DOT].pos.y -= g_tdLoad2[TEXTURE_EMPTY].size.y + 1.0f;

	g_tdLoad2[TEXTURE_WHITE].tex = TEXTURE_LABEL_WHITE;
	g_tdLoad2[TEXTURE_WHITE].col = { 0.777f, 0.898f, 0.914f, 1.0f };
}

BOOL DrawLoad2(void)
{
	float rate = (float)g_time / BAR_MAX;

	if (rate >= 1.0f) {
		SetFade(FADE_OUT, g_ModeNext);
		rate = 1.0f;
		//return TRUE;
	}

	DrawTexture2D(&g_tdLoad2[TEXTURE_WHITE]);

	// Animation
	{
		float anim = (float)((int)((float)g_time / ANIM_FPS) % ANIM_NUM) / (float)ANIM_NUM;
		g_tdLoad2[TEXTURE_ANIM].uv_pos.u = anim;
		DrawTexture2D(&g_tdLoad2[TEXTURE_ANIM], FALSE, TRUE);
	}

	// バー
	{
		// 空バー
		DrawTexture2D(&g_tdLoad2[TEXTURE_EMPTY]);

		// バー
		g_tdLoad2[TEXTURE_FULL].size.x = GetTextureSize(g_tdLoad2[TEXTURE_FULL].tex).x * rate;
		//g_tdLoad2[TEXTURE_FULL].pos.x = -GetTextureSize(g_tdLoad2[TEXTURE_FULL].tex).x * 0.5f * (1.0f - rate);
		g_tdLoad2[TEXTURE_FULL].uv_pos.uw = rate;
		DrawTexture2D(&g_tdLoad2[TEXTURE_FULL], FALSE, TRUE);
	}

	// ドット
	{
		int anim = (int)(g_time / 24) % 4;
		for (int i = 0; i < anim; i++) {
			g_tdLoad2[TEXTURE_DOT].posAdd.x = i * g_tdLoad2[TEXTURE_DOT].size.x;
			DrawTexture2D(&g_tdLoad2[TEXTURE_DOT]);
		}
	}

	DrawTexture2D(&g_tdLoad2[TEXTURE_BG]);

	g_time++;

	return FALSE;
}

void SetDrawLoad2(MODE_LABEL modeNext)
{
	g_time = 0;
	g_ModeNext = modeNext;
}