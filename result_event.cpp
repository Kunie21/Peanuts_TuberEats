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
#include "player.h"
#include "tube.h"


//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH)	// 背景サイズ横
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)	// 背景サイズ縦

#define ROCKET_MAX					(6)
#define ROCKET_SPEED				(10)

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


XMFLOAT3 g_CameraPos, g_CameraAt, g_CameraUp;


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


	g_CameraPos = { 2500.0f, -500.0f, 2000.0f };
	g_CameraAt = { 0.0f, -500.0f, 2000.0f };
	g_CameraUp = { 0.0f, 1.0f, 0.0f };


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


	g_Load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateResultEvent(void)
{
	g_CameraPos.z -= ROCKET_SPEED;
	g_CameraAt.z -= ROCKET_SPEED;
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawResultEvent(void)
{
	//SetDraw2DTexture();
	
	//後でステージに分けて描画する
	DrawTexture2D(&g_td[TEXTURE_OOSAKA]);
	//DrawTexture2D(&g_td[TEXTURE_SHANGHAI]);
	//DrawTexture2D(&g_td[TEXTURE_PARIS]);
	//DrawTexture2D(&g_td[TEXTURE_NEWYORK]);

	DrawTexture2DAll(TRUE);
	
	//チューブの描画
	static CURVE_BUFFER curve;
	SetCurveBuffer(&curve);
	SetAfterRotation(&XMMatrixRotationRollPitchYaw(0.0f, 0.0f, 0.0f));

	SetViewBuffer(&XMMatrixLookAtLH(XMLoadFloat3(&g_CameraPos), XMLoadFloat3(&g_CameraAt), XMLoadFloat3(&g_CameraUp)));

	SetStencilReadLL(SHADER_TUBE);
	DrawTubeResult();

	//ロケットの描画
	SetStencilReadLL(SHADER_PLAYER);
	DrawPlayerResult();
	DrawFireResult();

}
