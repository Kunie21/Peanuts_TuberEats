//=============================================================================
//
// チームロゴ画面処理 [teamlogo.cpp]
// Author : 國江 翔太
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "texture2d.h"
#include "teamlogo.h"
#include "fade.h"
#include "model.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH)	// 背景サイズ横
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)	// 背景サイズ縦

//#define TEXTURE_WIDTH_LOGO			(1154)			// ロゴサイズ横
//#define TEXTURE_HEIGHT_LOGO			(693)			// ロゴサイズ縦
#define TEXTURE_WIDTH_LOGO			(984)			// ロゴサイズ横
#define TEXTURE_HEIGHT_LOGO			(590)			// ロゴサイズ縦

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
	TEXTURE_STAR,
	TEXTURE_MAX,
};
static TEXTURE2D_DESC	g_td[TEXTURE_MAX];
static ID3D11ShaderResourceView*	g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報
static char*	g_TextureName[TEXTURE_MAX] = {
	"data/TEXTURE/white.png",
	"data/TEXTURE/peanuts_logo_color.png",
	"data/MODEL/star1.jpg",
	//"data/TEXTURE/peanuts_logo_white.png",
	//"data/TEXTURE/peanuts_bg_1.png",
	//"data/TEXTURE/peanuts.png",
};


enum {
	MODEL_STAR = 0,
	MODEL_EARTH,
	MODEL_ROCKET,
	MODEL_MAX,
};
static MODEL_DATA	g_Model[MODEL_MAX];	// プレイヤーのモデル管理

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
	//g_td[TEXTURE_BG].col = { 0.0f, 0.0f, 0.0f, 1.0f };

	g_td[TEXTURE_TEAMLOGO].size = { TEXTURE_WIDTH_LOGO, TEXTURE_HEIGHT_LOGO };
	g_td[TEXTURE_TEAMLOGO].scl = { 0.5f, 0.5f };
	//g_td[TEXTURE_TEAMLOGO].scl = { 1.0f, 1.0f };

	LoadModel("data/MODEL/skydome.obj", &g_Model[MODEL_STAR].model);
	//LoadModel("data/MODEL/skydome.obj", &g_Model[MODEL_EARTH].model);
	LoadModel("data/MODEL/earth01.obj", &g_Model[MODEL_EARTH].model);
	LoadModel("data/MODEL/rocket01.obj", &g_Model[MODEL_ROCKET].model);

	g_Model[MODEL_STAR].srt.pos = { 0.0f, 0.0f, 0.0f };
	g_Model[MODEL_EARTH].srt.pos = { 25.0f, 0.0f, 100.0f };
	g_Model[MODEL_STAR].srt.scl = { 20.3f, 20.3f, 20.3f };
	g_Model[MODEL_EARTH].srt.scl = { 0.3f, 0.3f, 0.3f };
	g_Model[MODEL_ROCKET].srt.scl = { 0.01f, 0.01f, 0.01f };

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
	g_Model[MODEL_EARTH].srt.rot.y -= 0.01f;
	//if (g_Time++ > TEAMLOGO_TIME) { g_Time = 0; SetFade(FADE_OUT, MODE_GAME); }
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawTeamLogo(void)
{
	//SetDraw2DTexture();
	//DrawTexture2D(&g_td[TEXTURE_BG]);
	//DrawTexture2D(&g_td[TEXTURE_TEAMLOGO]);

	//SetDrawNoLighting();

	//SetCullingMode(CULL_MODE_NONE);

	//XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	//for (int testNo = 0; testNo < MODEL_MAX; testNo++)
	//{
	//	// ワールドマトリックスの初期化
	//	mtxWorld = XMMatrixIdentity();

	//	// スケールを反映
	//	mtxScl = XMMatrixScaling(g_Model[testNo].scl.x, g_Model[testNo].scl.y, g_Model[testNo].scl.z);
	//	mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

	//	// 回転を反映：全体の角度
	//	mtxRot = XMMatrixRotationRollPitchYaw(g_Model[testNo].rot.x, g_Model[testNo].rot.y, g_Model[testNo].rot.z);
	//	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

	//	// 移動を反映
	//	mtxTranslate = XMMatrixTranslation(g_Model[testNo].pos.x, g_Model[testNo].pos.y, g_Model[testNo].pos.z);
	//	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

	//	// ワールドマトリックスの設定
	//	SetWorldBuffer(&mtxWorld);

	//	// マテリアル設定
	//	MATERIAL material;
	//	ZeroMemory(&material, sizeof(material));
	//	material.Diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };

	//	// モデル描画
	//	if (testNo != MODEL_STAR)
	//	{
	//		DrawModel(&g_Model[testNo].model, NULL, &material);
	//	}
	//	else
	//	{
	//		DrawModel(&g_Model[testNo].model, &g_Texture[TEXTURE_STAR], &material);
	//	}
	//}

	//SetCullingMode(CULL_MODE_BACK);
}
