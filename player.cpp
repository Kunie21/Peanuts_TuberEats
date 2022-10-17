//=============================================================================
//
// プレイヤー処理 [player.cpp]
// Author : 國江 翔太
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "texture2d.h"
#include "fade.h"
#include "input.h"
#include "player.h"
#include "tube.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH)	// 背景サイズ横

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static int				g_Time = 0;
static BOOL				g_Load = FALSE;

// テクスチャ管理
enum
{
	TEXTURE_TEAMLOGO = 0,
	TEXTURE_MAX,
};
static TEXTURE2D_DESC	g_td[TEXTURE_MAX];
static ID3D11ShaderResourceView*	g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報
static char*	g_TextureName[TEXTURE_MAX] = {
	"data/TEXTURE/blueberry_.png",
};

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitPlayer(void)
{
	// テクスチャ生成
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		D3DX11CreateShaderResourceViewFromFile(GetDevice(), g_TextureName[i], NULL, NULL, &g_Texture[i], NULL);
		g_td[i].tex = &g_Texture[i];
	}

	// 詳細設定
	//g_td[TEXTURE_TEAMLOGO].tex = &g_Texture[TEXTURE_TEAMLOGO];

	g_Time = 0;

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitPlayer(void)
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
void UpdatePlayer(void)
{
	if (GetKeyboardPress(DIK_A))
	{
		TubeRotation(0.05f);
	}
	if (GetKeyboardPress(DIK_D))
	{
		TubeRotation(-0.05f);
	}
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawPlayer(void)
{
	//DrawTexture2D(&g_td[TEXTURE_TEAMLOGO]);
}
