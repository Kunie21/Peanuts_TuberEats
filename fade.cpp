//=============================================================================
//
// フェード処理 [fade.cpp]
// Author : 國江 翔太
//
//=============================================================================
#include "main.h"
#include "game.h"
#include "renderer.h"
#include "texture2d.h"
#include "fade.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	FADE_RATE		(0.02f)			// フェード係数
//#define SHUTTER			(60)

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static FADE				g_Fade = FADE_IN;	// フェードの状態
static MODE_LABEL		g_ModeNext;			// 次のモード
static TEXTURE2D_DESC	g_td;
#ifdef SHUTTER
static TEXTURE2D_DESC	g_td2;
#endif
static BOOL				g_Load = FALSE;

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitFade(void)
{
#ifdef SHUTTER
	g_td.tex = TEXTURE_LABEL_TEAMLOGO;
	g_td.ctrType = CENTER_BOTTOM;
	g_td.posType = POSITION_TOP;
	g_td.scl.y = 0.5f;
	g_td.uv_pos.vh = 0.5f;

	g_td2.tex = TEXTURE_LABEL_TEAMLOGO;
	g_td2.ctrType = CENTER_TOP;
	g_td2.posType = POSITION_BOTTOM;
	g_td2.scl.y = 0.5f;
	g_td2.uv_pos.v = 0.5f;
	g_td2.uv_pos.vh = 0.5f;
#else
	g_td.tex = TEXTURE_LABEL_WHITE;
	//g_td.col = { 0.0f, 0.0f, 0.0f, 1.0f };
#endif

	g_Fade = FADE_IN;

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitFade(void)
{
	if (g_Load == FALSE) return;

	g_Load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateFade(void)
{
	if (g_Fade != FADE_NONE)
	{// フェード処理中
		if (g_Fade == FADE_OUT)
		{// フェードアウト処理
#ifdef SHUTTER
			g_td.pos.y += g_td.size.y * FADE_RATE;
			if (g_td.pos.y >= g_td.size.y * 0.75f)
			{
				g_td.pos.y = g_td.size.y * 0.75f;
				static int wait = 0;
				++wait;
				if (wait < SHUTTER) return;
				else wait = 0;
#else
			g_td.col.w += FADE_RATE;		// α値を加算して画面を消していく
			if (g_td.col.w >= 1.0f)
			{
				g_td.col.w = 1.0f;
#endif
				// 鳴っている曲を全部止める
				//StopSound();

				// フェードイン処理に切り替え
				SetFade(FADE_IN, g_ModeNext);

				// モードを設定
				SetMode(g_ModeNext);
			}
		}
		else if (g_Fade == FADE_IN)
		{// フェードイン処理
#ifdef SHUTTER
			g_td.pos.y -= g_td.size.y * FADE_RATE;
			if (g_td.pos.y <= 0.0f)
			{
				g_td.pos.y = 0.0f;
				static int wait = 0;
				++wait;
				if (wait < SHUTTER) return;
				else wait = 0;
#else
			g_td.col.w -= FADE_RATE;		// α値を減算して画面を浮き上がらせる
			if (g_td.col.w <= 0.0f)
			{
				g_td.col.w = 0.0f;
#endif
				// フェード処理終了
				SetFade(FADE_NONE, g_ModeNext);
			}
		}
	}
}

BOOL CheckFadeIn(void)
{
#ifdef SHUTTER
	if (g_td.pos.y <= 0.0f)
#else
	if (g_td.col.w <= 0.0f)
#endif
		return TRUE;
	return FALSE;
}
BOOL CheckFadeOut(void)
{
#ifdef SHUTTER
	if (g_td.pos.y <= 0.0f)
#else
	if (g_td.col.w <= 0.0f)
#endif
		return TRUE;
	return FALSE;
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawFade(void)
{
	if (g_Fade == FADE_NONE) return;	// フェードしないのなら描画しない

#ifdef SHUTTER
	g_td2.pos.y = -g_td.pos.y;
	DrawTexture2D(&g_td, FALSE, TRUE);
	DrawTexture2D(&g_td2, FALSE, TRUE);
#else
	DrawTexture2D(&g_td);
#endif
}


//=============================================================================
// フェードの状態設定
//=============================================================================
void SetFade(FADE fade, MODE_LABEL modeNext)
{
	g_Fade = fade;
	g_ModeNext = modeNext;
}

//=============================================================================
// フェードの状態取得
//=============================================================================
FADE GetFade(void)
{
	return g_Fade;
}



