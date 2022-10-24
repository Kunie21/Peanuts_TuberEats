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
#include "player.h"
#include "ui_game.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
//#define MODEL_MAX		(1)
#define ICE_NUM		(50)
#define RING_NUM	(20)

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static int				g_Time = 0;
static BOOL				g_Load = FALSE;

enum GIMMICK_TYPE
{
	GIMMICK_ICE = 0,
	GIMMICK_RING,

	GIMMICK_MAX
};

static GIMMICK g_GmIce[ICE_NUM];
static GIMMICK g_GmRing[RING_NUM];

static DX11_MODEL	g_Model[GIMMICK_MAX];	// プレイヤーのモデル管理


static float		g_Rotation = 0.0f;

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitGimmick(void)
{
	LoadModel("data/MODEL/ice_1.obj", &g_Model[GIMMICK_ICE]);
	LoadModel("data/MODEL/ring_1.obj", &g_Model[GIMMICK_RING]);

	for (int i = 0; i < ICE_NUM; i++)
	{
		g_GmIce[i].rotPosNo = (i * 5) % 8;
		g_GmIce[i].zPosNo = i * 20;
	}
	for (int i = 0; i < RING_NUM; i++)
	{
		g_GmRing[i].rotPosNo = (i * 5) % 8;
		g_GmRing[i].zPosNo = i * 50;
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

	for (int i = 0; i < GIMMICK_MAX; i++)
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
	d_pos -= GetPlayerSpeed();
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// 氷
	for (int i = 0; i < ICE_NUM; i++)
	{
		float zPos = d_pos + MESH_SIZE * g_GmIce[i].zPosNo;
		if (-100.0f > zPos || zPos > 10000.0f)
			continue;

		// ワールドマトリックスの初期化
		mtxWorld = XMMatrixIdentity();

		// スケールを反映
		mtxScl = XMMatrixScaling(4.0f, 3.0f, 4.0f);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		float rot = XM_2PI * (float)g_GmIce[i].rotPosNo / (float)MESH_NUM_X + GetTubeRotation();

		// 回転を反映：全体の角度
		mtxRot = XMMatrixRotationRollPitchYaw(0.0f, XM_PIDIV2, 0.0f);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// 移動を反映
		mtxTranslate = XMMatrixTranslation(0.0f, 120.0f, 0.0f);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// 回転を反映：個々の角度
		mtxRot = XMMatrixRotationRollPitchYaw(0.0f, 0.0f, rot + XM_PIDIV2);
		//SetAfterRotation(&mtxRot);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// 移動を反映
		mtxTranslate = XMMatrixTranslation(TUBE_RADIUS * 0.8f * cosf(rot), TUBE_RADIUS* 0.8f * sinf(rot), zPos);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ワールドマトリックスの設定
		SetWorldBuffer(&mtxWorld);

		//XMStoreFloat4x4(&g_Gimmick.mtxWorld, mtxWorld);

		// モデル描画
		DrawModel(&g_Model[GIMMICK_ICE]);
	}
	// リング
	for (int i = 0; i < RING_NUM; i++)
	{
		float zPos = d_pos + MESH_SIZE * g_GmRing[i].zPosNo;
		if (-100.0f > zPos || zPos > 10000.0f) continue;

		// ワールドマトリックスの初期化
		mtxWorld = XMMatrixIdentity();

		// スケールを反映
		mtxScl = XMMatrixScaling(1.0f, 1.0f, 1.0f);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		float rot = XM_2PI * (float)g_GmRing[i].rotPosNo / (float)MESH_NUM_X + GetTubeRotation();

		// 回転を反映：全体の角度
		mtxRot = XMMatrixRotationRollPitchYaw(XM_PIDIV2, 0.0f, 0.0f);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// 移動を反映
		mtxTranslate = XMMatrixTranslation(0.0f, 50.0f, 0.0f);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// 回転を反映：個々の角度
		mtxRot = XMMatrixRotationRollPitchYaw(0.0f, 0.0f, rot + XM_PIDIV2);
		//SetAfterRotation(&mtxRot);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// 移動を反映
		mtxTranslate = XMMatrixTranslation(TUBE_RADIUS * 0.8f * cosf(rot), TUBE_RADIUS* 0.8f * sinf(rot), zPos);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ワールドマトリックスの設定
		SetWorldBuffer(&mtxWorld);

		//XMStoreFloat4x4(&g_Gimmick.mtxWorld, mtxWorld);

		// モデル描画
		DrawModel(&g_Model[GIMMICK_RING]);
	}
}

bool CollisionGimmick(float oldZ, float newZ, float oldRot, float newRot)
{
	float oldZPosNoFloat = oldZ / MESH_SIZE;
	float newZPosNoFloat = newZ / MESH_SIZE;
	int oldZPosNoInt = (int)oldZPosNoFloat;
	int newZPosNoInt = (int)newZPosNoFloat;
	if (oldZPosNoInt == (int)newZPosNoInt) return false;
	float length = newZPosNoFloat - oldZPosNoFloat;
	while (oldZPosNoInt <= newZPosNoInt)
	{
		float rate = (1.0f - oldZPosNoFloat + (float)oldZPosNoInt) / length;
		int colZPosNo = oldZPosNoInt + 1;
		float colRot = oldRot + (newRot - oldRot) * rate;
		for (int i = 0; i < ICE_NUM; i++)
		{
			if (g_GmIce[i].zPosNo == colZPosNo)
			{
				float rot = colRot + XM_PIDIV2 + XM_2PI * (float)g_GmIce[i].rotPosNo / (float)MESH_NUM_X;
				while (rot < 0.0f) rot += XM_2PI;
				while (rot > XM_2PI) rot -= XM_2PI;
				if (rot < g_GmIce[i].rotSizeHalf ||
					XM_2PI - g_GmIce[i].rotSizeHalf < rot)
				{
					SetDamageEffect();
					SetPlayerCollisionIce();
					return true;
				}
			}
		}
		for (int i = 0; i < RING_NUM; i++)
		{
			if (g_GmRing[i].zPosNo == colZPosNo)
			{
				float rot = colRot + XM_PIDIV2 + XM_2PI * (float)g_GmRing[i].rotPosNo / (float)MESH_NUM_X;
				while (rot < 0.0f) rot += XM_2PI;
				while (rot > XM_2PI) rot -= XM_2PI;
				if (rot < g_GmRing[i].rotSizeHalf ||
					XM_2PI - g_GmRing[i].rotSizeHalf < rot)
				{
					SetBoostEffect();
					SetPlayerThroughRing();
					return true;
				}
			}
		}
		oldZPosNoInt++;
	}
	return false;
}