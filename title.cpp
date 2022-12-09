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
#include "start.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TITLE_X					(150.0f)
#define TITLE_Y					(120.0f)
#define TITLE_BUTTON_X			(80.0f)
#define TITLE_BUTTON_Y			(510.0f)
#define COPYR_X					(1300.0f)
#define COPYR_Y					(954.0f)

#define OBJ_DIST				(230)

#define BUTTONLOGO_TIME			(100)
#define ALPHASPEED				(0.03f)

#define ROCKET_PARTS_MAX	(1)

#define EARTH_ANGLE				(0.0f)
//#define EARTH_ANGLE			(-XM_PIDIV4 * 0.3f)
#define ROCKET_ANGLE			(-XM_PIDIV4 * 0.2f)

#define DEBRIS_NUM			(10)
#define DEBRIS_DIST			(-280)
#define DEBRIS_X			(290)
#define DEBRIS_Y			(50)
#define DEBRIS_SPD_X		(0.2f)
#define DEBRIS_SPD_Y		(0.1f)
#define DEBRIS_ROTSPD		(0.02f)
#define DEBRIS_INTERVAL		(500)
#define DEBRIS_SCL			(0.25f)

// テクスチャ名
enum {
	TEXTURE_TITLE = 0,
	TEXTURE_BUTTON,
	TEXTURE_COPYR,
	TEXTURE_STAR,
	TEXTURE_LINE,
	TEXTURE_MAX,
};

// モデル名
enum {
	MODEL_TITLE_STAR = 0,
	MODEL_TITLE_EARTH,
	MODEL_TITLE_ROCKET,
	MODEL_TITLE_FIRE,
	MODEL_TITLE_TUBE,
	MODEL_TITLE_MAX,
};

enum {
	DEBRIS_SUSHI01 = 0,
	DEBRIS_SUSHI02,
	DEBRIS_LOLLIPOP01,
	DEBRIS_LOLLIPOP02,
	DEBRIS_RAMEN,
	DEBRIS_DONUT01,
	DEBRIS_MAX,
};

struct DEBRIS {
	XMFLOAT3 pos;
	XMFLOAT3 rot;
	XMFLOAT3 vpos;
	XMFLOAT3 vrot;
	int type;
	BOOL use = FALSE;
};

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static BOOL				g_Load = FALSE;
static TEXTURE2D_DESC	g_td[TEXTURE_MAX];
static MODEL_DATA		g_Model[MODEL_TITLE_MAX];

static MODEL_DATA		g_ModelDebris[DEBRIS_MAX];
static DEBRIS			g_Debris[DEBRIS_NUM];

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitTitle(void)
{
	if (g_Load) return S_OK;

	// テクスチャ設定
	for (int i = 0; i < TEXTURE_STAR; i++) {
		g_td[i].tex = (TEXTURE_LABEL)(TEXTURE_LABEL_TITLE + i);
		g_td[i].size = GetTextureSize(g_td[i].tex);
		g_td[i].ctrType = CENTER_LEFTTOP;
		g_td[i].posType = POSITION_LEFTTOP;
	}
	g_td[TEXTURE_STAR].tex = TEXTURE_LABEL_STAR;

	g_td[TEXTURE_LINE].tex = TEXTURE_LABEL_WHITE;
	g_td[TEXTURE_LINE].ctrType = CENTER_LEFTTOP;
	g_td[TEXTURE_LINE].posType = POSITION_LEFTTOP;
	g_td[TEXTURE_LINE].col = { 0.0f, 0.3f, 0.0f, 1.0f };
	g_td[TEXTURE_LINE].size = { 50.0f, g_td[TEXTURE_TITLE].size.y + 25.0f + g_td[TEXTURE_BUTTON].size.y };
	g_td[TEXTURE_LINE].pos = { TITLE_X - 80.0f, TITLE_Y };

	//g_td[TEXTURE_TITLE].size = { 577.0f, 345.0f };
	//g_td[TEXTURE_TITLE].pos = { TITLE_X, TITLE_Y };
	g_td[TEXTURE_TITLE].ctrType = CENTER_TOP;
	g_td[TEXTURE_TITLE].pos = { TITLE_X + g_td[TEXTURE_TITLE].size.x * 0.5f, TITLE_Y };
	g_td[TEXTURE_TITLE].sd_pos = { 6.0f, 6.0f };
	g_td[TEXTURE_TITLE].sd_col = { 0.0f, 0.15f, 0.0f, 0.5f };

	//g_td[TEXTURE_BUTTON].size = { 739.0f, 67.0f };
	//g_td[TEXTURE_BUTTON].pos = { TITLE_BUTTON_X, TITLE_BUTTON_Y };
	g_td[TEXTURE_BUTTON].ctrType = CENTER_TOP;
	g_td[TEXTURE_BUTTON].pos = { g_td[TEXTURE_TITLE].pos.x + 15.0f, g_td[TEXTURE_TITLE].pos.y + g_td[TEXTURE_TITLE].size.y + 60.0f };
	g_td[TEXTURE_BUTTON].scl = { 0.9f, 0.9f };

	//g_td[TEXTURE_COPYR].size = { 547.0f, 43.0f };
	g_td[TEXTURE_COPYR].ctrType = CENTER_RIGHTBOTTOM;
	g_td[TEXTURE_COPYR].posType = POSITION_RIGHTBOTTOM;
	g_td[TEXTURE_COPYR].pos = { -50.0f, -50.0f };

	// モデル設定
	g_Model[MODEL_TITLE_STAR].model = MODEL_EARTH;
	g_Model[MODEL_TITLE_STAR].srt.pos = { 0.0f, 0.0f, 0.0f };
	g_Model[MODEL_TITLE_STAR].srt.scl = { 200.0f, 200.0f, 200.0f };

	g_Model[MODEL_TITLE_EARTH].model = MODEL_EARTH;
	g_Model[MODEL_TITLE_EARTH].srt.pos = { 0.0f, 0.0f, OBJ_DIST };
	g_Model[MODEL_TITLE_EARTH].srt.scl = { 20.0f, 20.0f, 20.0f };
	g_Model[MODEL_TITLE_EARTH].srt.rot.y = -XM_PIDIV4;

	g_Model[MODEL_TITLE_ROCKET].model = MODEL_ROCKET1;
	g_Model[MODEL_TITLE_ROCKET].srt.pos = { 0.0f, 0.0f, OBJ_DIST };
	g_Model[MODEL_TITLE_ROCKET].srt.scl = { 0.15f, 0.15f, 0.15f };
	g_Model[MODEL_TITLE_ROCKET].srt.rot.y = XM_PI;
	g_Model[MODEL_TITLE_ROCKET].srt.rot.z = XM_PIDIV4 * 0.5f;

	g_Model[MODEL_TITLE_FIRE].model = MODEL_FIRE;
	g_Model[MODEL_TITLE_FIRE].srt.pos.z = -15.0f;

	g_Model[MODEL_TITLE_TUBE].model = MODEL_RING;
	g_Model[MODEL_TITLE_TUBE].srt.pos = { 0.0f, 0.0f, OBJ_DIST };
	g_Model[MODEL_TITLE_TUBE].srt.scl = { 3.06f, 3.06f, 3.06f };
	g_Model[MODEL_TITLE_TUBE].srt.rot = { 0.0f, 0.0f, 0.0f };
	

	for (int i = 0; i < DEBRIS_MAX; i++) {
		g_ModelDebris[i].model = (MODEL_LABEL)(MODEL_SUSHI01 + i);
		g_ModelDebris[i].srt.scl = { DEBRIS_SCL, DEBRIS_SCL, DEBRIS_SCL };
	}

	// 乱数の初期化
	srand((unsigned int)timeGetTime());

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

void NewDebris(void) {
	for (int i = 0; i < DEBRIS_NUM; i++) {
		if (g_Debris[i].use) continue;
		g_Debris[i].type = rand() % DEBRIS_MAX;
		g_Debris[i].pos = { DEBRIS_X, DEBRIS_Y * (float)(rand() % 10) * 0.2f - DEBRIS_Y, DEBRIS_DIST };
		g_Debris[i].vpos = { -DEBRIS_SPD_X - DEBRIS_SPD_X * (float)(rand() % 10) * 0.1f, DEBRIS_SPD_Y * (float)(rand() % 10) * 0.2f - DEBRIS_SPD_Y, 0.0f };
		g_Debris[i].rot = { 0.0f, 0.0f, 0.0f };
		g_Debris[i].vrot = { DEBRIS_ROTSPD * (float)(rand() % 10) * 0.1f, DEBRIS_ROTSPD * (float)(rand() % 10) * 0.1f, DEBRIS_ROTSPD * (float)(rand() % 10) * 0.1f };
		g_Debris[i].use = TRUE;
		return;
	}
}
void UpdateDebris(void) {
	static int time = 0;
	if(!time) NewDebris();
	for (int i = 0; i < DEBRIS_NUM; i++) {
		if (!g_Debris[i].use) continue;
		g_Debris[i].pos.x += g_Debris[i].vpos.x;
		g_Debris[i].pos.y += g_Debris[i].vpos.y;
		g_Debris[i].pos.z += g_Debris[i].vpos.z;
		g_Debris[i].rot.x += g_Debris[i].vrot.x;
		g_Debris[i].rot.y += g_Debris[i].vrot.y;
		g_Debris[i].rot.z += g_Debris[i].vrot.z;
		if (g_Debris[i].pos.x < -DEBRIS_X) g_Debris[i].use = FALSE;
	}
	time = (time + 1) % DEBRIS_INTERVAL;
}
void DrawDebris(void) {
	//SetCullingMode(CULL_MODE_NONE);
	XMMATRIX mtxWorld;
	SRT srt;
	for (int i = 0; i < DEBRIS_NUM; i++) {
		if (!g_Debris[i].use) continue;
		mtxWorld = XMMatrixIdentity();
		srt = g_ModelDebris[g_Debris[i].type].srt;
		srt.pos = g_Model[MODEL_TITLE_EARTH].srt.pos;
		MulMtxScl(mtxWorld, srt.scl.x, srt.scl.y, srt.scl.z);	// スケールを反映
		MulMtxRot(mtxWorld, srt.rot.x, srt.rot.y, srt.rot.z);	// 回転を反映
		MulMtxRot(mtxWorld, g_Debris[i].rot.x, g_Debris[i].rot.y, g_Debris[i].rot.z);					// 回転を反映
		MulMtxPos(mtxWorld, srt.pos.x + g_Debris[i].pos.x, srt.pos.y + g_Debris[i].pos.y, srt.pos.z + g_Debris[i].pos.z);	// 移動を反映
		DrawModel(&g_ModelDebris[g_Debris[i].type].model, &mtxWorld);
	}
	//SetCullingMode(CULL_MODE_BACK);
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateTitle(void)
{
	UpdateDebris();

	g_Model[MODEL_TITLE_EARTH].srt.rot.y -= 0.001f;
	g_Model[MODEL_TITLE_STAR].srt.rot.y += 0.0001f;
	if (g_Model[MODEL_TITLE_EARTH].srt.rot.y < -XM_2PI) g_Model[MODEL_TITLE_ROCKET].srt.rot.y += XM_2PI;
	if (g_Model[MODEL_TITLE_STAR].srt.rot.y > XM_2PI) g_Model[MODEL_TITLE_ROCKET].srt.rot.y -= XM_2PI;

	g_Model[MODEL_TITLE_ROCKET].srt.rot.y += 0.02f;
	//g_Model[MODEL_TITLE_ROCKET].srt.rot.z -= 0.0002f;
	if (g_Model[MODEL_TITLE_ROCKET].srt.rot.y > XM_2PI) g_Model[MODEL_TITLE_ROCKET].srt.rot.y -= XM_2PI;
	if (g_Model[MODEL_TITLE_ROCKET].srt.rot.z < -XM_2PI) g_Model[MODEL_TITLE_ROCKET].srt.rot.z += XM_2PI;

	static float time = 0.0f;
	time += 0.05f; if(time > XM_2PI) time -= XM_2PI;
	g_td[TEXTURE_BUTTON].col.w = 1.0f + sinf(time);

	if (IsMouseLeftTriggered() || IsMouseRightTriggered()) {
		PressedAnyButton();
	}
}

void DrawEarthRocket(void) {
	XMMATRIX mtxWorld;
	SRT srt;
	MATERIAL material;

	material.Specular = { 0.0f, 0.0f, 0.0f, 0.0f };
	mtxWorld = XMMatrixIdentity();
	srt = g_Model[MODEL_TITLE_EARTH].srt;
	MulMtxScl(mtxWorld, srt.scl.x, srt.scl.y, srt.scl.z);	// スケールを反映
	MulMtxRot(mtxWorld, 0.0f, srt.rot.y, 0.0f);				// 回転を反映
	MulMtxRot(mtxWorld, 0.0f, 0.0f, EARTH_ANGLE);			// 回転を反映
	MulMtxRot(mtxWorld, EARTH_ANGLE, 0.0f, 0.0f);			// 回転を反映
	MulMtxPos(mtxWorld, srt.pos.x, srt.pos.y, srt.pos.z);	// 移動を反映
	DrawModel(&g_Model[MODEL_TITLE_EARTH].model, &mtxWorld, &material);

	// ロケット
	mtxWorld = XMMatrixIdentity();
	srt = g_Model[MODEL_TITLE_ROCKET].srt;
	MulMtxScl(mtxWorld, srt.scl.x, srt.scl.y, srt.scl.z);	// スケールを反映
	MulMtxRot(mtxWorld, XM_PI, 0.0f, XM_PI);				// 回転を反映
	MulMtxPos(mtxWorld, -225.0f, 0.0f, 0.0f);				// 移動を反映
	MulMtxRot(mtxWorld, 0.0f, srt.rot.y, 0.0f);				// 回転を反映
	MulMtxRot(mtxWorld, 0.0f, 0.0f, srt.rot.z);		// 回転を反映
	MulMtxRot(mtxWorld, ROCKET_ANGLE, 0.0f, 0.0f);		// 回転を反映
	MulMtxRot(mtxWorld, 0.0f, 0.0f, EARTH_ANGLE);			// 回転を反映
	MulMtxRot(mtxWorld, EARTH_ANGLE, 0.0f, 0.0f);			// 回転を反映
	MulMtxPos(mtxWorld, srt.pos.x, srt.pos.y, srt.pos.z);	// 移動を反映
	DrawModel(&g_Model[MODEL_TITLE_ROCKET].model, &mtxWorld);

	// パイプ（位置はロケットを参照）
	//SetBlendState(BLEND_MODE_ADD);
	//mtxWorld = XMMatrixIdentity();
	//srt = g_Model[MODEL_TITLE_ROCKET].srt;
	//srt.scl = g_Model[MODEL_TITLE_TUBE].srt.scl;
	//MulMtxScl(mtxWorld, srt.scl.x, srt.scl.y, srt.scl.z);	// スケールを反映
	//MulMtxRot(mtxWorld, XM_PI, 0.0f, XM_PI);				// 回転を反映
	////MulMtxRot(mtxWorld, 0.0f, srt.rot.y, 0.0f);				// 回転を反映
	//MulMtxRot(mtxWorld, 0.0f, 0.0f, srt.rot.z);		// 回転を反映
	//MulMtxRot(mtxWorld, ROCKET_ANGLE, 0.0f, 0.0f);		// 回転を反映
	//MulMtxRot(mtxWorld, 0.0f, 0.0f, EARTH_ANGLE);			// 回転を反映
	//MulMtxRot(mtxWorld, EARTH_ANGLE, 0.0f, 0.0f);			// 回転を反映
	//MulMtxPos(mtxWorld, srt.pos.x, srt.pos.y, srt.pos.z);	// 移動を反映
	//material.Specular = { 1.0f, 1.0f, 1.0f, 1.0f };
	//material.Diffuse = { 0.0f, 0.3f, 0.0f, 0.7f };	// 半透明にする
	//DrawModel(&g_Model[MODEL_TITLE_TUBE].model, &mtxWorld, TEXTURE_LABEL_WHITE, &material);
	//SetBlendState(BLEND_MODE_ALPHABLEND);

}
//=============================================================================
// 描画処理
//=============================================================================
void DrawTitle(void)
{
	XMMATRIX mtxWorld;
	SRT srt;
	MATERIAL material;

	SetDrawNoLighting();

	// 星
	SetCullingMode(CULL_MODE_FRONT);	// 裏面も描画
	DrawModel(&g_Model[MODEL_TITLE_STAR].model, &g_Model[MODEL_TITLE_STAR].srt, TEXTURE_LABEL_STAR);
	SetCullingMode(CULL_MODE_BACK);

	// 黒塗り
	SetDrawFillBlackPlayer();
	DrawEarthRocket();
	DrawDebris();

	// シャドウボリューム
	//SetStencilWriteDL();
	//DrawEarthRocket();
	//DrawDebris();

	SetBlendState(BLEND_MODE_ADD);

	// ディレクショナルライト
	SetLightNo(0);
	SetStencilReadDL();
	DrawEarthRocket();
	DrawDebris();

	// アンビエントライト
	SetStencilNoneAL();
	DrawEarthRocket();
	DrawDebris();

	SetBlendState(BLEND_MODE_ALPHABLEND);

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
	MulMtxRot(mtxWorld, 0.0f, 0.0f, srt.rot.z);	// 回転を反映
	MulMtxRot(mtxWorld, ROCKET_ANGLE, 0.0f, 0.0f);		// 回転を反映
	MulMtxRot(mtxWorld, 0.0f, 0.0f, EARTH_ANGLE);			// 回転を反映
	MulMtxRot(mtxWorld, EARTH_ANGLE, 0.0f, 0.0f);			// 回転を反映
	MulMtxPos(mtxWorld, srt.pos.x, srt.pos.y, srt.pos.z);	// 移動を反映
	DrawModel(&g_Model[MODEL_TITLE_FIRE].model, &mtxWorld);	// モデル描画

	// パイプ（位置はロケットを参照）
	//SetCullingMode(CULL_MODE_NONE);	// 裏面も描画
	//SetStencilReadDL();
	mtxWorld = XMMatrixIdentity();
	srt = g_Model[MODEL_TITLE_ROCKET].srt;
	srt.scl = g_Model[MODEL_TITLE_TUBE].srt.scl;
	MulMtxScl(mtxWorld, srt.scl.x, srt.scl.y, srt.scl.z);	// スケールを反映
	MulMtxRot(mtxWorld, XM_PI, 0.0f, XM_PI);				// 回転を反映
	//MulMtxRot(mtxWorld, 0.0f, srt.rot.y, 0.0f);				// 回転を反映
	MulMtxRot(mtxWorld, 0.0f, 0.0f, srt.rot.z);		// 回転を反映
	MulMtxRot(mtxWorld, ROCKET_ANGLE, 0.0f, 0.0f);		// 回転を反映
	MulMtxRot(mtxWorld, 0.0f, 0.0f, EARTH_ANGLE);			// 回転を反映
	MulMtxRot(mtxWorld, EARTH_ANGLE, 0.0f, 0.0f);			// 回転を反映
	MulMtxPos(mtxWorld, srt.pos.x, srt.pos.y, srt.pos.z);	// 移動を反映
	material.Diffuse = { 0.0f, 0.3f, 0.0f, 0.7f };	// 半透明にする
	DrawModel(&g_Model[MODEL_TITLE_TUBE].model, &mtxWorld, TEXTURE_LABEL_WHITE, &material);
	//SetCullingMode(CULL_MODE_BACK);	// 裏面も描画

	SetBlendState(BLEND_MODE_ALPHABLEND);
	

	// タイトルロゴなど
	DrawTexture2D(&g_td[TEXTURE_TITLE], TRUE);
	DrawTexture2D(&g_td[TEXTURE_BUTTON]);
	DrawTexture2D(&g_td[TEXTURE_COPYR]);
	//DrawTexture2D(&g_td[TEXTURE_LINE], FALSE);
}

void SetTitleAlpha(float rate) {
	g_td[TEXTURE_TITLE].col.w = rate;
	g_td[TEXTURE_BUTTON].col.w = rate;
	g_td[TEXTURE_COPYR].col.w = rate;

	g_td[TEXTURE_TITLE].sd_col.w = rate * 0.5f;
	g_td[TEXTURE_BUTTON].sd_col.w = rate * 0.5f;
	g_td[TEXTURE_COPYR].sd_col.w = rate * 0.5f;

	float dist = (1.0f - rate) * OBJ_DIST;
	g_Model[MODEL_TITLE_EARTH].srt.pos.x = dist;
	g_Model[MODEL_TITLE_ROCKET].srt.pos.x = dist;
	g_Model[MODEL_TITLE_EARTH].srt.pos.z = OBJ_DIST + dist * 0.5f;
	g_Model[MODEL_TITLE_ROCKET].srt.pos.z = OBJ_DIST + dist * 0.5f;
}