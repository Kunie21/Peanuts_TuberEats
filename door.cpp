//=============================================================================
//
// ドア処理 [door.cpp]
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
#include "door.h"
#include "tube.h"
#include "gimmick.h"
#include "stage.h"
#include "player.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define DOOR_SPD	(20.0f)		// 開く速度
#define DOOR_OPN	(110.0f)	// どこまで開くか
#define DOOR_SCL	(20.0f)		// 大きさ
#define DOOR_POS	(250.0f)	// Z座標
#define DOOR_OFX	(0.3f)		// X座標の初期位置調整
#define DOOR_ROT	(0.0f)		// 斜めにする？
#define DOOR_SHK	(0.0f)		// ガタガタさせる？

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static BOOL				g_Load = FALSE;

// テクスチャ管理
enum {
	TEXTURE_TEAMLOGO = 0,
	//TEXTURE_STAR,
	TEXTURE_MAX,
};
static TEXTURE2D_DESC	g_td[TEXTURE_MAX];
static ID3D11ShaderResourceView*	g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報
static char*	g_TextureName[TEXTURE_MAX] = {
	"data/TEXTURE/blueberry_.png",
	//"data/MODEL/star1.jpg",
};

enum {
	MODEL_DOOR_LEFT = 0,
	MODEL_DOOR_RIGHT,
	MODEL_MAX,
};
static MODEL_DATA	g_Model[MODEL_MAX];	// プレイヤーのモデル管理


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitDoor(void)
{
	// テクスチャ生成
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		D3DX11CreateShaderResourceViewFromFile(GetDevice(), g_TextureName[i], NULL, NULL, &g_Texture[i], NULL);
		g_td[i].tex = &g_Texture[i];
	}

	// モデル読み込み
	{
		LoadModel("data/MODEL/door_left.obj", &g_Model[MODEL_DOOR_LEFT].model);
		LoadModel("data/MODEL/door_right.obj", &g_Model[MODEL_DOOR_RIGHT].model);

		g_Model[MODEL_DOOR_LEFT].srt.pos = { DOOR_OFX, 0.0f, DOOR_POS };
		g_Model[MODEL_DOOR_LEFT].srt.rot = { DOOR_ROT, 0.0f, 0.0f };
		g_Model[MODEL_DOOR_LEFT].srt.scl = { DOOR_SCL, DOOR_SCL, DOOR_SCL };
		g_Model[MODEL_DOOR_RIGHT].srt.pos = { -DOOR_OFX, 0.0f, DOOR_POS };
		g_Model[MODEL_DOOR_RIGHT].srt.rot = { DOOR_ROT, 0.0f, 0.0f };
		g_Model[MODEL_DOOR_RIGHT].srt.scl = { DOOR_SCL, DOOR_SCL, DOOR_SCL };
	}

	// 詳細設定
	//g_td[TEXTURE_TEAMLOGO].tex = &g_Texture[TEXTURE_TEAMLOGO];

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitDoor(void)
{
	if (g_Load == FALSE) return;

	for (int i = 0; i < TEXTURE_MAX; i++) {
		if (g_Texture[i]) {
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
void UpdateDoor(void)
{
	static float door_open = 0.0f;
	if (door_open < DOOR_OPN) door_open += DOOR_SPD;
	if (GetKeyboardPress(DIK_K)) { door_open = 0.0f; }
	float d_pos = -GetPlayerPosition();
	g_Model[MODEL_DOOR_LEFT].srt.pos = { DOOR_OFX - door_open, (float)(rand() % 10) * DOOR_SHK, DOOR_POS + d_pos + (float)(rand() % 10) * DOOR_SHK };
	g_Model[MODEL_DOOR_RIGHT].srt.pos = { -DOOR_OFX + door_open, (float)(rand() % 10) * DOOR_SHK, DOOR_POS + d_pos + (float)(rand() % 10) * DOOR_SHK };
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawDoor(void) {
	MATERIAL material;
	DrawModel(&g_Model[MODEL_DOOR_LEFT].model, &g_Model[MODEL_DOOR_LEFT].srt, NULL, &material);	// モデル描画
	DrawModel(&g_Model[MODEL_DOOR_RIGHT].model, &g_Model[MODEL_DOOR_RIGHT].srt, NULL, &material);	// モデル描画
}