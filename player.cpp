//=============================================================================
//
// プレイヤー処理 [player.cpp]
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
#include "player.h"
#include "tube.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MODEL_MAX		(1)

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

static DX11_MODEL	g_Model[MODEL_MAX];	// プレイヤーのモデル管理


static float		g_Rotation = 0.0f;

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

	for (int i = 0; i < MODEL_MAX; i++)
	{
		LoadModel("data/MODEL/ice_1.obj", &g_Model[i]);
		//LoadModel("data/MODEL/aloe.obj", &g_Model[i]);
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

	for (int i = 0; i < MODEL_MAX; i++)
	{
		UnloadModel(&g_Model[i]);
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
		if (g_Rotation < 0.05f) g_Rotation += 0.002f;
	}
	if (GetKeyboardPress(DIK_D))
	{
		if (g_Rotation > -0.05f) g_Rotation -= 0.002f;
	}
	g_Rotation *= 0.98f;

	RotateTube(g_Rotation);
	MoveTube(40.0f);
	//TestCurveTube(40.0f);

#ifdef _DEBUG	// デバッグ情報を表示する
	static int dZMove = 0;
	static int dTime = 0;
	dZMove += 40;
	dTime++;
	PrintDebugProc("Z_MOVE:%d\n", dZMove / (int)MESH_SIZE);
	PrintDebugProc("TUBES:%d\n", dZMove / (int)(MESH_SIZE * MESH_NUM_Z));
	PrintDebugProc("Time:%d\n", dTime / 60);
#endif
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawPlayer(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// ワールドマトリックスの初期化
	mtxWorld = XMMatrixIdentity();

	// スケールを反映
	mtxScl = XMMatrixScaling(0.8f, 0.8f, 0.8f);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

	// 回転を反映：全体の角度
	mtxRot = XMMatrixRotationRollPitchYaw(-XM_PIDIV2, 0.0f, 0.0f);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

	// 移動を反映
	mtxTranslate = XMMatrixTranslation(0.0f, -40.0f, 200.0f);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

	// ワールドマトリックスの設定
	SetWorldMatrix(&mtxWorld);

	//XMStoreFloat4x4(&g_Player.mtxWorld, mtxWorld);

	// マテリアル設定
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = { 1.0f, 1.0f, 1.0f, 0.5f };

	// モデル描画
	DrawModel(&g_Model[0], NULL, &material);
}
