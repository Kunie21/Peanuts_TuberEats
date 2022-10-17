////=============================================================================
////
//// チームロゴ画面処理 [teamlogo.cpp]
//// Author : 國江 翔太
////
////=============================================================================
//#include "main.h"
//#include "renderer.h"
//#include "fade.h"
//#include "teamlogo.h"
//#include "texture2d.h"
//
////*****************************************************************************
//// マクロ定義
////*****************************************************************************
//#define TEXTURE_WIDTH				(SCREEN_WIDTH)	// 背景サイズ横
//#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)	// 背景サイズ縦
//
//#define TEXTURE_WIDTH_LOGO			(371)			// ロゴサイズ横
//#define TEXTURE_HEIGHT_LOGO			(160)			// ロゴサイズ縦
//
//#define TEAMLOGO_TIME				(100)			// ロゴ表示時間
//
////*****************************************************************************
//// グローバル変数
////*****************************************************************************
//static int				g_Time = 0;
//static BOOL				g_Load = FALSE;
//
//// テクスチャ管理
//enum
//{
//	TEXTURE_TEAMLOGO = 0,
//	TEXTURE_MAX,
//};
//static TEXTURE2D_DESC	g_td[TEXTURE_MAX];
//static ID3D11ShaderResourceView*	g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報
//static char*	g_TextureName[TEXTURE_MAX] = {
//	"data/TEXTURE/blueberry_.png",
//};
//
////=============================================================================
//// 初期化処理
////=============================================================================
//HRESULT InitTeamLogo(void)
//{
//	// テクスチャ生成
//	for (int i = 0; i < TEXTURE_MAX; i++)
//	{
//		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
//			g_TextureName[i],
//			NULL,
//			NULL,
//			&g_Texture[i],
//			NULL);
//	}
//
//	// 詳細設定
//	g_td[TEXTURE_TEAMLOGO].size = { TEXTURE_WIDTH_LOGO, TEXTURE_HEIGHT_LOGO };
//	g_td[TEXTURE_TEAMLOGO].tex = &g_Texture[TEXTURE_TEAMLOGO];
//
//	g_Time = 0;
//
//	g_Load = TRUE;
//	return S_OK;
//}
//
////=============================================================================
//// 終了処理
////=============================================================================
//void UninitTeamLogo(void)
//{
//	if (g_Load == FALSE) return;
//
//	for (int i = 0; i < TEXTURE_MAX; i++)
//	{
//		if (g_Texture[i])
//		{
//			g_Texture[i]->Release();
//			g_Texture[i] = NULL;
//		}
//	}
//
//	g_Load = FALSE;
//}
//
////=============================================================================
//// 更新処理
////=============================================================================
//void UpdateTeamLogo(void)
//{
//	if(g_Time++ >TEAMLOGO_TIME) { SetFade(FADE_OUT, MODE_LOADING); }
//}
//
////=============================================================================
//// 描画処理
////=============================================================================
//void DrawTeamLogo(void)
//{
//	DrawTexture2D(&g_td[TEXTURE_TEAMLOGO]);
//}
