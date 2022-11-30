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
#include "result.h"


//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH)	// 背景サイズ横
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)	// 背景サイズ縦

#define ROCKET_MAX					(6)
#define ROCKET_SPEED				(15)

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static int				g_Time = 0;
static BOOL				g_Load = FALSE;

// テクスチャ管理
enum
{
	TEXTURE_OOSAKA = 0,
	TEXTURE_HOKKAIDO,
	TEXTURE_OKINAWA,
	TEXTURE_SHANGHAI,
	TEXTURE_KOREA,
	TEXTURE_SINGAPORE,
	TEXTURE_FRANCE,
	TEXTURE_ENGLAND,
	TEXTURE_ITALY,
	TEXTURE_AMERICA,
	TEXTURE_CANADA,
	TEXTURE_MEXICO,
	TEXTURE_MAX,
};
static TEXTURE2D_DESC	g_td[TEXTURE_MAX];
<<<<<<< HEAD
//static ID3D11ShaderResourceView*	g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報
//static char*	g_TextureName[TEXTURE_MAX] = {
//	"data/TEXTURE/result_country/oosaka.jpg",
//	"data/TEXTURE/result_country/shanghai.jpg",
//	"data/TEXTURE/result_country/France.jpg",
//	"data/TEXTURE/result_country/America.jpg",
//
//};
=======
static ID3D11ShaderResourceView*	g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報
static char*	g_TextureName[TEXTURE_MAX] = {
	"data/TEXTURE/result_country/Oosaka.jpg",
	"data/TEXTURE/result_country/Hokkaido.jpg",
	"data/TEXTURE/result_country/Okinawa.jpg",
	"data/TEXTURE/result_country/shanghai.jpg",
	"data/TEXTURE/result_country/Seoul.jpg",
	"data/TEXTURE/result_country/Singapore.jpg",
	"data/TEXTURE/result_country/France.jpg",
	"data/TEXTURE/result_country/London.jpg",
	"data/TEXTURE/result_country/Italy.jpg",
	"data/TEXTURE/result_country/america.jpg",
	"data/TEXTURE/result_country/Canada.jpg",
	"data/TEXTURE/result_country/Mexico.jpg",

};
>>>>>>> Peanuts_TuberEats/result


XMFLOAT3 g_CameraPos, g_CameraAt, g_CameraUp;


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitResultEvent(void)
{
	// テクスチャ生成
	for (int i = 0; i < TEXTURE_MAX; i++) {
		g_td[i].tex = (TEXTURE_LABEL)(TEXTURE_LABEL_OOSAKA + i);
	}

	// 詳細設定
<<<<<<< HEAD
	g_td[TEXTURE_OOSAKA].size = { TEXTURE_WIDTH, TEXTURE_HEIGHT };

	g_td[TEXTURE_SHANGHAI].size = { 900, 600 };
	g_td[TEXTURE_SHANGHAI].scl = { 2.2f, 2.2f };

	g_td[TEXTURE_PARIS].size = { TEXTURE_WIDTH, TEXTURE_HEIGHT };

	g_td[TEXTURE_NEWYORK].size = { TEXTURE_WIDTH, TEXTURE_HEIGHT };
=======
	g_td[TEXTURE_OOSAKA].size = { 641, 426 };
	g_td[TEXTURE_OOSAKA].scl = { 3.0f, 3.0f };
	g_td[TEXTURE_OOSAKA].tex = &g_Texture[TEXTURE_OOSAKA];

	g_td[TEXTURE_HOKKAIDO].size = { 641, 427 };
	g_td[TEXTURE_HOKKAIDO].scl = { 3.0f, 3.0f };
	g_td[TEXTURE_HOKKAIDO].tex = &g_Texture[TEXTURE_HOKKAIDO];

	g_td[TEXTURE_OKINAWA].size = { 641, 427 };
	g_td[TEXTURE_OKINAWA].scl = { 3.0f, 3.0f };
	g_td[TEXTURE_OKINAWA].tex = &g_Texture[TEXTURE_OKINAWA];

	g_td[TEXTURE_SHANGHAI].size = { 641, 427 };
	g_td[TEXTURE_SHANGHAI].scl = { 3.0f, 3.0f };
	g_td[TEXTURE_SHANGHAI].tex = &g_Texture[TEXTURE_SHANGHAI];

	g_td[TEXTURE_KOREA].size = { 641, 427 };
	g_td[TEXTURE_KOREA].scl = { 3.0f, 3.0f };
	g_td[TEXTURE_KOREA].tex = &g_Texture[TEXTURE_KOREA];

	g_td[TEXTURE_SINGAPORE].size = { 641, 424 };
	g_td[TEXTURE_SINGAPORE].scl = { 3.0f, 3.0f };
	g_td[TEXTURE_SINGAPORE].tex = &g_Texture[TEXTURE_SINGAPORE];

	g_td[TEXTURE_FRANCE].size = { 641, 360 };
	g_td[TEXTURE_FRANCE].scl = { 3.0f, 3.0f };
	g_td[TEXTURE_FRANCE].tex = &g_Texture[TEXTURE_FRANCE];

	g_td[TEXTURE_ENGLAND].size = { 641, 441 };
	g_td[TEXTURE_ENGLAND].scl = { 3.0f, 3.0f };
	g_td[TEXTURE_ENGLAND].tex = &g_Texture[TEXTURE_ENGLAND];

	g_td[TEXTURE_ITALY].size = { 641, 360 };
	g_td[TEXTURE_ITALY].scl = { 3.0f, 3.0f };
	g_td[TEXTURE_ITALY].tex = &g_Texture[TEXTURE_ITALY];
>>>>>>> Peanuts_TuberEats/result

	g_td[TEXTURE_AMERICA].size = { 641, 427 };
	g_td[TEXTURE_AMERICA].scl = { 3.0f, 3.0f };
	g_td[TEXTURE_AMERICA].tex = &g_Texture[TEXTURE_AMERICA];

	g_td[TEXTURE_CANADA].size = { 640, 408 };
	g_td[TEXTURE_CANADA].scl = { 3.0f, 3.0f };
	g_td[TEXTURE_CANADA].tex = &g_Texture[TEXTURE_CANADA];

	g_td[TEXTURE_MEXICO].size = { 640, 420 };
	g_td[TEXTURE_MEXICO].scl = { 3.0f, 3.0f };
	g_td[TEXTURE_MEXICO].tex = &g_Texture[TEXTURE_MEXICO];

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


	g_Load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateResultEvent(void)
{
	g_CameraPos.z -= ROCKET_SPEED;
	g_CameraAt.z -= ROCKET_SPEED;

	if (g_CameraPos.z < (-2500))
	{
		SetEventFinish();
	}
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawResultEvent(void)
{

	//後でステージに分けて描画する
	DrawTexture2D(&g_td[TEXTURE_OOSAKA]);
	DrawTexture2D(&g_td[TEXTURE_HOKKAIDO]);
	DrawTexture2D(&g_td[TEXTURE_OKINAWA]);
	DrawTexture2D(&g_td[TEXTURE_SHANGHAI]);
	DrawTexture2D(&g_td[TEXTURE_KOREA]);
	DrawTexture2D(&g_td[TEXTURE_SINGAPORE]);
	DrawTexture2D(&g_td[TEXTURE_FRANCE]);
	DrawTexture2D(&g_td[TEXTURE_ENGLAND]);
	DrawTexture2D(&g_td[TEXTURE_ITALY]);
	DrawTexture2D(&g_td[TEXTURE_AMERICA]);
	DrawTexture2D(&g_td[TEXTURE_CANADA]);
	DrawTexture2D(&g_td[TEXTURE_MEXICO]);

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
