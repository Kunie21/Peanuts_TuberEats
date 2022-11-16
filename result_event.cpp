//=============================================================================
//
// リサルトの演出処理 [result_event.cpp]
// Author : 譚 倩盈
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "fade.h"
#include "teamlogo.h"
#include "texture2d.h"
#include "model.h"


//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH)	// 背景サイズ横
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)	// 背景サイズ縦

#define ROCKET_MAX					(6)
#define ROCKET_SPEED				(2)

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static int				g_Time = 0;
static BOOL				g_Load = FALSE;

// テクスチャ管理
enum
{
	TEXTURE_OOSAKA = 0,
	TEXTURE_SHANGHAI,
	TEXTURE_PARIS,
	TEXTURE_NEWYORK,
	TEXTURE_MAX,
};
static TEXTURE2D_DESC	g_td[TEXTURE_MAX];
static ID3D11ShaderResourceView*	g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報
static char*	g_TextureName[TEXTURE_MAX] = {
	"data/TEXTURE/result_country/oosaka.jpg",
	"data/TEXTURE/result_country/shanghai.jpg",
	"data/TEXTURE/result_country/France.jpg",
	"data/TEXTURE/result_country/America.jpg",

};

enum {
	MODEL_ROCKET1 = 0,
	MODEL_ROCKET2,
	MODEL_ROCKET3,
	MODEL_ROCKET4,
	MODEL_ROCKET5,
	MODEL_FIRE,
	MODEL_OOSAKA,
	MODEL_TUBE,
	MODEL_MAX,
};

static MODEL_DATA	g_Model[MODEL_MAX];	// プレイヤーのモデル管理

static int testNo = 0;


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitResultEvent(void)
{
	// テクスチャ生成
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_TextureName[i],
			NULL,
			NULL,
			&g_Texture[i],
			NULL);
	}

	// 詳細設定
	g_td[TEXTURE_OOSAKA].size = { TEXTURE_WIDTH, TEXTURE_HEIGHT };
	g_td[TEXTURE_OOSAKA].tex = &g_Texture[TEXTURE_OOSAKA];

	g_td[TEXTURE_SHANGHAI].size = { 900, 600 };
	g_td[TEXTURE_SHANGHAI].scl = { 2.2f, 2.2f };
	g_td[TEXTURE_SHANGHAI].tex = &g_Texture[TEXTURE_SHANGHAI];

	g_td[TEXTURE_PARIS].size = { TEXTURE_WIDTH, TEXTURE_HEIGHT };
	g_td[TEXTURE_PARIS].tex = &g_Texture[TEXTURE_PARIS];

	g_td[TEXTURE_NEWYORK].size = { TEXTURE_WIDTH, TEXTURE_HEIGHT };
	g_td[TEXTURE_NEWYORK].tex = &g_Texture[TEXTURE_NEWYORK];

	LoadModel("data/MODEL/rocket01.obj", &g_Model[0].model);
	LoadModel("data/MODEL/rocket02.obj", &g_Model[1].model);
	LoadModel("data/MODEL/rocket03.obj", &g_Model[2].model);
	LoadModel("data/MODEL/rocket04.obj", &g_Model[3].model);
	LoadModel("data/MODEL/rocket05.obj", &g_Model[4].model);
	LoadModel("data/MODEL/fire01.obj", &g_Model[5].model);
	LoadModel("data/MODEL/oosaka.obj", &g_Model[6].model);
	LoadModel("data/MODEL/tube.obj", &g_Model[7].model);


	for (int i = 0; i < ROCKET_MAX; i++) {
		g_Model[i].srt.pos = { -300.0f, 50.0f, 0.0f };
		g_Model[i].srt.rot = { 0.0f, -XM_PIDIV2, 0.0f };
		g_Model[i].srt.scl = { 0.2f, 0.2f, 0.2f };
	}
	g_Model[MODEL_FIRE].srt.pos.x -= 20.0f;

	g_Model[MODEL_OOSAKA].srt.pos = { 0.0f, 0.0f, 100.0f };
	g_Model[MODEL_OOSAKA].srt.scl = { 5.0f, 5.0f, 5.0f };
	g_Model[MODEL_OOSAKA].srt.rot = { 0.0f, 0.0f, XM_PI };

	g_Model[MODEL_TUBE].srt.pos = { 0.0f, 50.0f, 0.0f };
	g_Model[MODEL_TUBE].srt.scl = { 0.03f, 0.03f, 0.03f };
	g_Model[MODEL_TUBE].srt.rot = { 0.0f, XM_PIDIV2, 0.0f };


	g_Time = 0;

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitResultEvent(void)
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

	for (int i = 0; i < MODEL_MAX; i++) { UnloadModel(&g_Model[i].model); }

	g_Load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateResultEvent(void)
{
	for (int i = 0; i < ROCKET_MAX; i++)
	{
		g_Model[i].srt.pos.x += ROCKET_SPEED;
	}
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawResultEvent(void)
{
	SetDraw2DTexture();
	
	//後でステージに分けて描画する
	DrawTexture2D(&g_td[TEXTURE_OOSAKA]);
	DrawTexture2D(&g_td[TEXTURE_SHANGHAI]);
	DrawTexture2D(&g_td[TEXTURE_PARIS]);
	DrawTexture2D(&g_td[TEXTURE_NEWYORK]);

	//SetDrawNoLighting();

	//SetCullingMode(CULL_MODE_NONE);
	//MATERIAL material;
	//DrawModel(&g_Model[MODEL_OOSAKA].model, &g_Model[MODEL_OOSAKA].srt, NULL, &material);	// モデル描画
	//

	//DrawModel(&g_Model[testNo].model, &g_Model[testNo].srt, NULL, &material);	// モデル描画
	//
	//g_Model[MODEL_FIRE].srt.scl.x = (float)(rand() % 10) * 0.003f + 0.2f;
	//g_Model[MODEL_FIRE].srt.scl.y = (float)(rand() % 10) * 0.003f + 0.2f;
	//g_Model[MODEL_FIRE].srt.scl.z = (float)(rand() % 10) * 0.003f + 0.2f;
	//material.Shininess = 1.0f;
	//material.Diffuse.w = 1.0f;
	//DrawModel(&g_Model[MODEL_FIRE].model, &g_Model[MODEL_FIRE].srt, NULL, &material);	// モデル描画
	//SetCullingMode(CULL_MODE_BACK);

	//material.Diffuse.w = 0.6f;
	//DrawModel(&g_Model[MODEL_TUBE].model, &g_Model[MODEL_TUBE].srt, NULL, &material);	// モデル描画


}
