//=============================================================================
//
// タイトル画面 [title.cpp]
// Author : サン
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "texture2d.h"
#include "title.h"
#include "fade.h"
#include "model.h"
#include "input.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TITLE_X					(150.0f)
#define TITLE_Y					(120.0f)
#define TITLE_BUTTON_X			(80.0f)
#define TITLE_BUTTON_Y			(510.0f)
#define COPYR_X					(1300.0f)
#define COPYR_Y					(954.0f)

#define OBJ_DIST				(200)

#define BUTTONLOGO_TIME			(100)
#define ALPHASPEED				(0.03f)

#define ROCKET_PARTS_MAX	(1)

// テクスチャ名
enum {
	TEXTURE_TITLE = 0,
	TEXTURE_BUTTON,
	TEXTURE_COPYR,
	TEXTURE_STAR,
	TEXTURE_MAX,
};

// モデル名
enum {
	MODEL_TITLE_STAR = 0,
	MODEL_TITLE_EARTH,
	MODEL_TITLE_ROCKET,
	MODEL_TITLE_FIRE,
	MODEL_TITLE_MAX,
};

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static BOOL				g_Load = FALSE;
static bool				g_Fade = true;
static TEXTURE2D_DESC	g_td[TEXTURE_MAX];
static MODEL_DATA		g_Model[MODEL_TITLE_MAX];

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitTitle(void)
{
	// テクスチャ設定
	for (int i = 0; i < TEXTURE_STAR; i++) {
		g_td[i].tex = (TEXTURE_LABEL)(TEXTURE_LABEL_TITLE + i);
		g_td[i].ctrType = CENTER_LEFTTOP;
		g_td[i].posType = POSITION_ABSOLUTE;
	}
	g_td[TEXTURE_STAR].tex = TEXTURE_LABEL_STAR;

	g_td[TEXTURE_TITLE].size = { 577.0f, 345.0f };
	g_td[TEXTURE_TITLE].pos = { TITLE_X, TITLE_Y };

	g_td[TEXTURE_BUTTON].size = { 739.0f, 67.0f };
	g_td[TEXTURE_BUTTON].pos = { TITLE_BUTTON_X, TITLE_BUTTON_Y };

	g_td[TEXTURE_COPYR].size = { 547.0f, 43.0f };
	g_td[TEXTURE_COPYR].pos = { COPYR_X, COPYR_Y };

	// モデル設定
	g_Model[MODEL_TITLE_STAR].model = MODEL_EARTH;
	g_Model[MODEL_TITLE_STAR].srt.pos = { 0.0f, 0.0f, 0.0f };
	g_Model[MODEL_TITLE_STAR].srt.scl = { 80.0f, 80.0f, 80.0f };

	g_Model[MODEL_TITLE_EARTH].model = MODEL_EARTH;
	g_Model[MODEL_TITLE_EARTH].srt.pos = { 0.0f, 0.0f, OBJ_DIST };
	g_Model[MODEL_TITLE_EARTH].srt.scl = { 20.0f, 20.0f, 20.0f };

	g_Model[MODEL_TITLE_ROCKET].model = MODEL_ROCKET1;
	g_Model[MODEL_TITLE_ROCKET].srt.pos = { 0.0f, 0.0f, OBJ_DIST };
	g_Model[MODEL_TITLE_ROCKET].srt.scl = { 0.15f, 0.15f, 0.15f };
	g_Model[MODEL_TITLE_ROCKET].srt.rot.y = XM_PI;

	g_Model[MODEL_TITLE_FIRE].model = MODEL_FIRE;
	g_Model[MODEL_TITLE_FIRE].srt.pos.z = -15.0f;

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitTitle(void)
{
	if (g_Load == FALSE) return;

	g_Load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateTitle(void)
{
	g_Model[MODEL_TITLE_EARTH].srt.rot.y -= 0.001f;
	g_Model[MODEL_TITLE_STAR].srt.rot.y += 0.0001f;
	if (g_Model[MODEL_TITLE_EARTH].srt.rot.y < -XM_2PI) g_Model[MODEL_TITLE_ROCKET].srt.rot.y += XM_2PI;
	if (g_Model[MODEL_TITLE_STAR].srt.rot.y > XM_2PI) g_Model[MODEL_TITLE_ROCKET].srt.rot.y -= XM_2PI;

	g_Model[MODEL_TITLE_ROCKET].srt.rot.y += 0.03f;
	g_Model[MODEL_TITLE_ROCKET].srt.rot.z -= 0.0002f;
	if (g_Model[MODEL_TITLE_ROCKET].srt.rot.y > XM_2PI) g_Model[MODEL_TITLE_ROCKET].srt.rot.y -= XM_2PI;
	if (g_Model[MODEL_TITLE_ROCKET].srt.rot.z < -XM_2PI) g_Model[MODEL_TITLE_ROCKET].srt.rot.z += XM_2PI;

	if (g_Fade)
	{
		g_td[TEXTURE_BUTTON].col.w -= ALPHASPEED;
		if (g_td[TEXTURE_BUTTON].col.w <= 0.0f)
			g_Fade = false;
	}
	else
	{
		g_td[TEXTURE_BUTTON].col.w += ALPHASPEED;
		if (g_td[TEXTURE_BUTTON].col.w >= 1.0f)
			g_Fade = true;
	}
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawTitle(void)
{
	XMMATRIX mtxWorld;
	SRT srt;

	SetDrawNoLighting();

	// ロケット
	mtxWorld = XMMatrixIdentity();
	srt = g_Model[MODEL_TITLE_ROCKET].srt;
	MulMtxScl(mtxWorld, srt.scl.x, srt.scl.y, srt.scl.z);	// スケールを反映
	MulMtxRot(mtxWorld, XM_PI, 0.0f, XM_PI);				// 回転を反映
	MulMtxPos(mtxWorld, -225.0f, 0.0f, 0.0f);				// 移動を反映
	MulMtxRot(mtxWorld, 0.0f, srt.rot.y, 0.0f);				// 回転を反映
	MulMtxPos(mtxWorld, srt.pos.x, srt.pos.y, srt.pos.z);	// 移動を反映
	MulMtxRot(mtxWorld, 0.0f, 0.0f, XM_PIDIV4 * 0.5f + srt.rot.z);		// 回転を反映
	DrawModel(&g_Model[MODEL_TITLE_ROCKET].model, &mtxWorld);

	// 地球
	DrawModel(&g_Model[MODEL_TITLE_EARTH].model, &g_Model[MODEL_TITLE_EARTH].srt);

	// 星
	SetCullingMode(CULL_MODE_NONE);
	DrawModel(&g_Model[MODEL_TITLE_STAR].model, &g_Model[MODEL_TITLE_STAR].srt, TEXTURE_LABEL_STAR);
	SetCullingMode(CULL_MODE_BACK);

	// ファイヤー
	SetBlendState(BLEND_MODE_ADD);
	SetDrawFire();
	g_Model[MODEL_TITLE_FIRE].srt.scl.x = (float)(rand() % 10) * 0.003f + 0.1f;
	g_Model[MODEL_TITLE_FIRE].srt.scl.y = (float)(rand() % 10) * 0.003f + 0.1f;
	g_Model[MODEL_TITLE_FIRE].srt.scl.z = (float)(rand() % 10) * 0.003f + 0.1f;
	mtxWorld = XMMatrixIdentity();
	srt = g_Model[MODEL_TITLE_ROCKET].srt;
	srt.scl = g_Model[MODEL_TITLE_FIRE].srt.scl;
	MulMtxScl(mtxWorld, srt.scl.x, srt.scl.y, srt.scl.z);	// スケールを反映
	MulMtxRot(mtxWorld, XM_PI, 0.0f, XM_PI);				// 回転を反映
	MulMtxPos(mtxWorld, -225.0f, 0.0f, g_Model[MODEL_TITLE_FIRE].srt.pos.z);	// 移動を反映
	MulMtxRot(mtxWorld, 0.0f, srt.rot.y, 0.0f);				// 回転を反映
	MulMtxPos(mtxWorld, srt.pos.x, srt.pos.y, srt.pos.z);	// 移動を反映
	MulMtxRot(mtxWorld, 0.0f, 0.0f, XM_PIDIV4 * 0.5f + srt.rot.z);		// 回転を反映
	DrawModel(&g_Model[MODEL_TITLE_FIRE].model, &mtxWorld);	// モデル描画
	SetBlendState(BLEND_MODE_ALPHABLEND);

	// タイトルロゴなど
	DrawTexture2D(&g_td[TEXTURE_TITLE], TRUE, TRUE);
	DrawTexture2D(&g_td[TEXTURE_BUTTON], TRUE, TRUE);
	DrawTexture2D(&g_td[TEXTURE_COPYR], FALSE, TRUE);
}

void SetTitleAlpha(float rate) {
	g_td[TEXTURE_TITLE].col.w = rate;
	g_td[TEXTURE_BUTTON].col.w = rate;
	g_td[TEXTURE_COPYR].col.w = rate;

	float dist = (1.0f - rate) * OBJ_DIST;
	g_Model[MODEL_TITLE_EARTH].srt.pos.x = dist;
	g_Model[MODEL_TITLE_ROCKET].srt.pos.x = dist;
	g_Model[MODEL_TITLE_EARTH].srt.pos.z = OBJ_DIST + dist * 0.5f;
	g_Model[MODEL_TITLE_ROCKET].srt.pos.z = OBJ_DIST + dist * 0.5f;
}