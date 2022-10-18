//=============================================================================
//
// ギミック処理 [gimmick.cpp]
// Author : 國江 翔太
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "texture2d.h"
#include "debugproc.h"
#include "model.h"
#include "fade.h"
#include "input.h"
#include "gimmick.h"
#include "tube.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MODEL_MAX		(1)
#define GIMMICK_NUM		(100)

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static int				g_Time = 0;
static BOOL				g_Load = FALSE;

struct GIMMICK
{
	int rotPosNo = 0;
	int zPosNo = 0;
};
static GIMMICK g_Gimmick[GIMMICK_NUM];

static DX11_MODEL	g_Model[MODEL_MAX];	// プレイヤーのモデル管理


static float		g_Rotation = 0.0f;

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitGimmick(void)
{
	for (int i = 0; i < MODEL_MAX; i++)
	{
		LoadModel("data/MODEL/ice_1.obj", &g_Model[i]);
	}

	for (int i = 0; i < GIMMICK_NUM; i++)
	{
		g_Gimmick[i].rotPosNo = (i / 10) % 8;
		g_Gimmick[i].zPosNo = i * 10;
	}

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitGimmick(void)
{
	if (g_Load == FALSE) return;

	for (int i = 0; i < MODEL_MAX; i++)
	{
		UnloadModel(&g_Model[i]);
	}

	g_Load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateGimmick(void)
{

#ifdef _DEBUG	// デバッグ情報を表示する
#endif
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawGimmick(void)
{
	static float d_pos = 0;
	d_pos -= 40.0f;
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	for (int i = 0; i < GIMMICK_NUM; i++)
	{
		// ワールドマトリックスの初期化
		mtxWorld = XMMatrixIdentity();

		// スケールを反映
		mtxScl = XMMatrixScaling(3.0f, 2.0f, 3.0f);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		float rot = XM_2PI * (float)g_Gimmick[i].rotPosNo / 8.0f + GetTubeRotation();

		// 回転を反映：全体の角度
		mtxRot = XMMatrixRotationRollPitchYaw(0.0f, XM_PIDIV2, 0.0f);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);
		mtxRot = XMMatrixRotationRollPitchYaw(0.0f, 0.0f, rot + XM_PIDIV2);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// 移動を反映
		mtxTranslate = XMMatrixTranslation(TUBE_RADIUS * 0.8f * cosf(rot), TUBE_RADIUS* 0.8f * sinf(rot), d_pos + MESH_SIZE * g_Gimmick[i].zPosNo);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ワールドマトリックスの設定
		SetWorldBuffer(&mtxWorld);

		//XMStoreFloat4x4(&g_Gimmick.mtxWorld, mtxWorld);

		// モデル描画
		DrawModel(&g_Model[0]);
	}
}
