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

//*****************************************************************************
// グローバル変数
//*****************************************************************************

// テクスチャ管理
static TEXTURE2D_DESC	g_td;
static ID3D11ShaderResourceView*	g_Texture = NULL;	// テクスチャ情報
static char*	g_TextureName = "data/TEXTURE/white.png";

FADE			g_Fade = FADE_IN;	// フェードの状態
int				g_ModeNext;			// 次のモード

static BOOL		g_Load = FALSE;

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitFade(void)
{
	// テクスチャ生成
	D3DX11CreateShaderResourceViewFromFile(GetDevice(), g_TextureName, NULL, NULL, &g_Texture, NULL);
	g_td.tex = &g_Texture;
	g_td.col.w = 1.0f;

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

	if (g_Texture)
	{
		g_Texture->Release();
		g_Texture = NULL;
	}

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
			g_td.col.w += FADE_RATE;		// α値を加算して画面を消していく
			if (g_td.col.w >= 1.0f)
			{
				// フェードイン処理に切り替え
				g_td.col.w = 1.0f;
				SetFade(FADE_IN, g_ModeNext);
			}
		}
		else if (g_Fade == FADE_IN)
		{// フェードイン処理
			g_td.col.w -= FADE_RATE;		// α値を減算して画面を浮き上がらせる
			if (g_td.col.w <= 0.0f)
			{
				// フェード処理終了
				g_td.col.w = 0.0f;
				SetFade(FADE_NONE, g_ModeNext);
			}
		}
	}
}

BOOL CheckFadeIn(void)
{
	if (g_td.col.w <= 0.0f)
		return TRUE;
	return FALSE;
}
BOOL CheckFadeOut(void)
{
	if (g_td.col.w <= 0.0f)
		return TRUE;
	return FALSE;
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawFade(void)
{
	if (g_Fade == FADE_NONE) return;	// フェードしないのなら描画しない

	DrawTexture2D(&g_td);
}


//=============================================================================
// フェードの状態設定
//=============================================================================
void SetFade(FADE fade, int modeNext)
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



