//=============================================================================
//
// リサルト画面処理 [countDown.cpp]
// Author : 葉　志成
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "model.h"
#include "fade.h"
#include "input.h"
#include "countDown.h"
#include "texture2d.h"
#include "ui_game.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(600.0f)	// 背景サイズ横
#define TEXTURE_HEIGHT				(800.0f)	// 背景サイズ縦

#define DOOR_SCL					{15.0f,15.0f,1.0f}
#define DOOR_MOVE					(6.0f)
#define DOOR_MOVEMENT_RANGE			(300.0f)

#define TIME_FRAME					(1.0f)
#define TIME_START					(4)

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static int				g_Time = 0;
static BOOL				g_Load = FALSE;

// テクスチャ管理
enum
{
	TEXTURE_COUNTDOWN_03 = 0,
	TEXTURE_COUNTDOWN_02,
	TEXTURE_COUNTDOWN_01,
	TEXTURE_COUNTDOWN_GO,
	TEXTURE_COUNTDOWN_DISPLAY,
	TEXTURE_MAX,
};
static TEXTURE2D_DESC	g_td[TEXTURE_MAX];
static ID3D11ShaderResourceView*	g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報
static char*	g_TextureName[TEXTURE_MAX] = {
	"data/TEXTURE/game_UI/countdown_3_1.png",
	"data/TEXTURE/game_UI/countdown_2_1.png",
	"data/TEXTURE/game_UI/countdown_1_1.png",
	"data/TEXTURE/game_UI/go_1.png",
	"data/TEXTURE/white.png",
};

enum {
	MODEL_DOOR_LEFT = 0,
	MODEL_DOOR_RIGHT,
	MODEL_MAX
};

static MODEL_DATA	g_Model[MODEL_MAX];	// プレイヤーのモデル管理
static BOOL			g_IsDoorOpen = FALSE;

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitCountDown(void)
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

	//テキスチャーの初期化
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		g_td[i].tex = &g_Texture[i];
		g_td[i].pos = { SCREEN_WIDTH*0.34f, SCREEN_HEIGHT * 0.05f };
		g_td[i].posType = POSITION_ABSOLUTE;
		g_td[i].ctrType = CENTER_LEFTTOP;
	}
	g_td[TEXTURE_COUNTDOWN_03].size = { 487.0f, 557.0f };
	g_td[TEXTURE_COUNTDOWN_02].size = { 430.0f,494.0f };
	g_td[TEXTURE_COUNTDOWN_01].size = { 576.0f,421.0f };
	g_td[TEXTURE_COUNTDOWN_01].pos.y = SCREEN_HEIGHT * 0.1f;
	g_td[TEXTURE_COUNTDOWN_GO].size = { 911.0f,402.0f };
	g_td[TEXTURE_COUNTDOWN_GO].pos = { SCREEN_WIDTH*0.25f, SCREEN_HEIGHT * 0.1f };
	
	g_td[TEXTURE_COUNTDOWN_DISPLAY].tex = g_td[TEXTURE_COUNTDOWN_03].tex;
	g_td[TEXTURE_COUNTDOWN_DISPLAY].pos = g_td[TEXTURE_COUNTDOWN_03].pos;
	g_td[TEXTURE_COUNTDOWN_DISPLAY].size = g_td[TEXTURE_COUNTDOWN_03].size;


	//モデルの初期化
	LoadModel("data/MODEL/door_left.obj", &g_Model[MODEL_DOOR_LEFT].model);
	LoadModel("data/MODEL/door_right.obj", &g_Model[MODEL_DOOR_RIGHT].model);

	g_Model[MODEL_DOOR_LEFT].srt.pos = { 0.0f,0.0f,0.0f };
	g_Model[MODEL_DOOR_LEFT].srt.scl = DOOR_SCL;
	g_Model[MODEL_DOOR_RIGHT].srt.pos = { 0.0f,0.0f,0.0f };
	g_Model[MODEL_DOOR_RIGHT].srt.scl = DOOR_SCL;

	// 詳細設定
	g_IsDoorOpen = FALSE;

	SetTimer(0.0f);

	g_Time = 0;

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitCountDown(void)
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

	for (int i = 0; i < MODEL_MAX; i++)
	{
		UnloadModel(&g_Model[i].model);
	}

	g_Load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateCountDown(void)
{
	if (!g_IsDoorOpen)
	{
		g_Model[MODEL_DOOR_LEFT].srt.pos.x -= DOOR_MOVE;
		g_Model[MODEL_DOOR_RIGHT].srt.pos.x += DOOR_MOVE;

		if (g_Model[MODEL_DOOR_RIGHT].srt.pos.x > DOOR_MOVEMENT_RANGE)
		{
			g_IsDoorOpen = TRUE;
		}
	}
	else
	//if (g_Time < TIME_START)
	{
		OnTimer();

		if (GetTime() >= TIME_FRAME)
		{
			SetTimer(0.0f);
			g_Time++;

			if (g_Time >= TIME_START)return;

			//画像のアップデート
			g_td[TEXTURE_COUNTDOWN_DISPLAY].tex = g_td[g_Time].tex;
			g_td[TEXTURE_COUNTDOWN_DISPLAY].pos = g_td[g_Time].pos;
			g_td[TEXTURE_COUNTDOWN_DISPLAY].size = g_td[g_Time].size;
		}
	}

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawCountDown(void)
{
	if (g_Time >= TIME_START)return;


	SetCullingMode(CULL_MODE_NONE);

	// ロケットの描画処理
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	for (int i = 0; i < MODEL_MAX; i++)
	{
		// モデル描画
		DrawModel(&g_Model[i].model, &g_Model[i].srt);
	}

	SetCullingMode(CULL_MODE_BACK);


	SetDraw2DTexture();
	DrawTexture2D(&g_td[TEXTURE_COUNTDOWN_DISPLAY], FALSE, TRUE);
}

BOOL IsGameStart(void)
{
	return g_Time >= TIME_START;
	//return g_IsDoorOpen;
}


