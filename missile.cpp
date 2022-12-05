//=============================================================================
//
// ミサイル処理 [missile.cpp]
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
#include "tube.h"
#include "player.h"
#include "ui_game.h"
#include "missile.h"
#include "gimmick.h"
#include "stage.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MISSILE_NUM			(50)
#define MISSILE_SCL			(0.5f)
#define MISSILE_FIRE_SCL	(0.1f)
#define MISSILE_RADIUS		(40.0f)

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static int			g_Time = 0;
static BOOL			g_Load = FALSE;

static MISSILE		g_Ms[MISSILE_NUM];

//static DX11_MODEL	g_Model[MISSILE_TYPE_MAX];	// プレイヤーのモデル管理
static MODEL_LABEL	g_Model[MISSILE_TYPE_MAX];	// プレイヤーのモデル管理


static float		g_Rot = 0.0f;

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitMissile(void)
{
	g_Model[MISSILE_TYPE_01] = MODEL_SUSHI01;
	//g_Model[MISSILE_TYPE_01] = MODEL_MISSILE1;
	g_Model[MISSILE_TYPE_02] = MODEL_MISSILE2;
	g_Model[MISSILE_TYPE_FIRE] = MODEL_FIRE;

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitMissile(void)
{
	if (g_Load == FALSE) return;

	//for (int i = 0; i < MISSILE_TYPE_MAX; i++)
	//{
	//	UnloadModel(&g_Model[i]);
	//}

	g_Load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateMissile(void)
{
	g_Rot += 0.03f;
	if(g_Rot > XM_2PI) g_Rot -= XM_2PI;

	for (int i = 0; i < MISSILE_NUM; i++)
	{
		if (!g_Ms[i].use) continue;
		g_Ms[i].zPos += g_Ms[i].zSpd;
		g_Ms[i].zSpd += g_Ms[i].zAcl;
		g_Ms[i].zRot += g_Ms[i].zRotSpd;

		// コリジョン
		if (CollisionMissile(GetPlayerPosition() + g_Ms[i].zPosOld, GetPlayerPosition() + g_Ms[i].zPos, -g_Ms[i].zRotOld + XM_PI, -g_Ms[i].zRot + XM_PI)) {
			g_Ms[i].use = FALSE;
			//g_Have++;
		}

		g_Ms[i].zPosOld = g_Ms[i].zPos;
		g_Ms[i].zRotOld = g_Ms[i].zRot;

		if (DRAW_DIST_FAR < g_Ms[i].zPos) {
			g_Ms[i].use = FALSE;
			//g_Have++;
		}
	}

#ifdef _DEBUG	// デバッグ情報を表示する
#endif
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawMissile(MISSILE_TYPE gimmick)
{
	// インスタンス情報を登録
	D3D11_MAPPED_SUBRESOURCE msr;
	GetDeviceContext()->Map(GetInstanceBuffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
	INSTANCE* b_pInstance = (INSTANCE*)msr.pData;
	int instCount = 0;

	float zPos, rot;
	for (int i = 0; i < MISSILE_NUM; i++)
	{
		if (!g_Ms[i].use) continue;
		if (g_Ms[i].type != gimmick) continue;
		if (g_Ms[i].zPos < DRAW_DIST_NEAR || DRAW_DIST_FAR < g_Ms[i].zPos) continue;

		zPos = g_Ms[i].zPos;
		rot = g_Ms[i].zRot + GetTubeRotation() + XM_PIDIV2;

		b_pInstance->scl[instCount] = { MISSILE_SCL, MISSILE_SCL, MISSILE_SCL, 0.0f };
		b_pInstance->rot[instCount] = { 0.0f, XM_PIDIV2, rot, 0.0f };
		b_pInstance->pos[instCount] = { (TUBE_RADIUS - 120.0f) * 0.8f * cosf(rot), (TUBE_RADIUS - 120.0f) * 0.8f * sinf(rot), zPos, 0.0f };
		b_pInstance->col[instCount] = { 1.0f, 1.0f, 1.0f, 1.0f };

		instCount++;	// インスタンス数を更新
	}
	GetDeviceContext()->Unmap(GetInstanceBuffer(), 0);

	if (instCount > 0)
	{
		SetWorldBuffer(&XMMatrixIdentity());	// ワールドマトリックスの設定
		static MATERIAL material;
		DrawModelInstanced(&g_Model[gimmick], instCount, &material);	// モデル描画
	}
}
void DrawMissile(void)
{
	// インスタンス情報を登録
	D3D11_MAPPED_SUBRESOURCE msr;
	GetDeviceContext()->Map(GetInstanceBuffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
	INSTANCE* b_pInstance = (INSTANCE*)msr.pData;
	int instCount = 0;

	float zPos, rot;
	for (int i = 0; i < MISSILE_NUM; i++)
	{
		if (!g_Ms[i].use) continue;
		if (g_Ms[i].zPos < DRAW_DIST_NEAR || DRAW_DIST_FAR < g_Ms[i].zPos) continue;

		zPos = g_Ms[i].zPos;
		rot = g_Ms[i].zRot + GetTubeRotation() + XM_PIDIV2;

		b_pInstance->scl[instCount] = { MISSILE_SCL, MISSILE_SCL, MISSILE_SCL, 0.0f };
		b_pInstance->rot[instCount] = { 0.0f, XM_PIDIV2, rot + XM_PIDIV2, 0.0f };
		b_pInstance->pos[instCount] = { (TUBE_RADIUS - 120.0f) * 0.8f * cosf(rot), (TUBE_RADIUS - 120.0f) * 0.8f * sinf(rot), zPos, 0.0f };
		b_pInstance->col[instCount] = { 1.0f, 1.0f, 1.0f, 1.0f };
		instCount++;	// インスタンス数を更新
	}
	GetDeviceContext()->Unmap(GetInstanceBuffer(), 0);

	if (instCount > 0)
	{
		SetWorldBuffer(&XMMatrixIdentity());	// ワールドマトリックスの設定
		static MATERIAL material;
		DrawModelInstanced(&g_Model[MISSILE_TYPE_01], instCount, &material);	// モデル描画
	}
}
void DrawMissileHave(void)
{
	float rot;
	SRT srt;

	// 所持ミサイルの描画
	for (int i = 0; i < GetPlayerMissiles(); i++)
	{
		rot = g_Rot + (XM_2PI / GetPlayerMissiles()) * (float)i;

		srt.scl = { MISSILE_SCL, MISSILE_SCL, MISSILE_SCL };
		srt.rot = { 0.0f, XM_PIDIV2, 0.0f };
		srt.pos = { MISSILE_RADIUS * cosf(rot), ROCKET_Y + (MISSILE_RADIUS * sinf(rot)), 0.0f };
		DrawModel(&g_Model[MISSILE_TYPE_01], &srt);	// モデル描画
	}
}
void DrawMissileFire(void) {
	// インスタンス情報を登録
	D3D11_MAPPED_SUBRESOURCE msr;
	GetDeviceContext()->Map(GetInstanceBuffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
	INSTANCE* b_pInstance = (INSTANCE*)msr.pData;
	int instCount = 0;

	float zPos, rot;
	for (int i = 0; i < MISSILE_NUM; i++)
	{
		if (!g_Ms[i].use) continue;
		if (g_Ms[i].zPos < DRAW_DIST_NEAR || DRAW_DIST_FAR < g_Ms[i].zPos) continue;

		zPos = g_Ms[i].zPos;
		rot = g_Ms[i].zRot + GetTubeRotation() + XM_PIDIV2;

		b_pInstance->scl[instCount] = { MISSILE_FIRE_SCL + (float)(rand() % 10) * 0.003f, MISSILE_FIRE_SCL + (float)(rand() % 10) * 0.003f, MISSILE_FIRE_SCL + (float)(rand() % 10) * 0.003f, 0.0f };
		b_pInstance->rot[instCount] = { XM_PI, 0.0f, rot, 0.0f };
		b_pInstance->pos[instCount] = { (TUBE_RADIUS - 120.0f) * 0.8f * cosf(rot), (TUBE_RADIUS - 120.0f) * 0.8f * sinf(rot), zPos - 40.0f, 0.0f };
		b_pInstance->col[instCount] = { 1.0f, 1.0f, 1.0f, 1.0f };

		instCount++;	// インスタンス数を更新
	}
	GetDeviceContext()->Unmap(GetInstanceBuffer(), 0);

	if (instCount > 0)
	{
		SetWorldBuffer(&XMMatrixIdentity());	// ワールドマトリックスの設定
		DrawModelInstanced(&g_Model[MISSILE_TYPE_FIRE], instCount);	// モデル描画
	}
}

BOOL LaunchMissile(MISSILE_TYPE type, float zPos, float zSpd, float zRot, float zRotSpd)
{
	for (int i = 0; i < MISSILE_NUM; i++)
	{
		if (g_Ms[i].use) continue;
		g_Ms[i].type = type;

		g_Ms[i].zPos = zPos + 75.0f;
		g_Ms[i].zPosOld = g_Ms[i].zPos;
		g_Ms[i].zSpd = zSpd;

		g_Ms[i].zRot = zRot;
		g_Ms[i].zRotOld = zRot;
		g_Ms[i].zRotSpd = zRotSpd;

		g_Ms[i].use = TRUE;

		return TRUE;
	}
	return FALSE;
}