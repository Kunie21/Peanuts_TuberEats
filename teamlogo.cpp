//=============================================================================
//
// チームロゴ画面処理 [teamlogo.cpp]
// Author : 國江 翔太
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "fade.h"
#include "teamlogo.h"
#include "texture2d.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH)	// 背景サイズ横
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)	// 背景サイズ縦

#define TEXTURE_WIDTH_LOGO			(1154)			// ロゴサイズ横
#define TEXTURE_HEIGHT_LOGO			(693)			// ロゴサイズ縦

#define TEAMLOGO_TIME				(100)			// ロゴ表示時間

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static int				g_Time = 0;
static BOOL				g_Load = FALSE;

// テクスチャ管理
enum
{
	TEXTURE_BG = 0,
	TEXTURE_TEAMLOGO,
	TEXTURE_MAX,
};
static TEXTURE2D_DESC	g_td[TEXTURE_MAX];
static ID3D11ShaderResourceView*	g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報
static char*	g_TextureName[TEXTURE_MAX] = {
	"data/TEXTURE/white.png",
	"data/TEXTURE/peanuts.png",
};

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitTeamLogo(void)
{
	// テクスチャ生成
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		D3DX11CreateShaderResourceViewFromFile(GetDevice(), g_TextureName[i], NULL, NULL, &g_Texture[i], NULL);
		g_td[i].tex = &g_Texture[i];
	}

	// 詳細設定
	g_td[TEXTURE_BG].col = { 1.0f, 1.0f, 1.0f, 1.0f };
	g_td[TEXTURE_TEAMLOGO].size = { TEXTURE_WIDTH_LOGO, TEXTURE_HEIGHT_LOGO };
	g_td[TEXTURE_TEAMLOGO].scl = { 0.5f, 0.5f };

	g_Time = 0;

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitTeamLogo(void)
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
void UpdateTeamLogo(void)
{
	if (g_Time++ > TEAMLOGO_TIME) { g_Time = 0; SetFade(FADE_OUT, MODE_GAME); }
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawTeamLogo(void)
{
	DrawTexture2D(&g_td[TEXTURE_BG]);
	DrawTexture2D(&g_td[TEXTURE_TEAMLOGO]);
}
