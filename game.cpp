//=============================================================================
//
// ゲーム処理 [game.cpp]
// Author : 國江 翔太
//
//=============================================================================
#include "main.h"
#include "game.h"
#include "renderer.h"
#include "debugproc.h"
#include "camera.h"
#include "light.h"
#include "input.h"
#include "sound.h"
#include "fade.h"
#include "file.h"
#include "tube.h"
#include "gimmick.h"
#include "player.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static BOOL		g_bPause = TRUE;		// ポーズON/OFF
static BOOL		g_bStopTime = FALSE;	// ポーズON/OFF
static BOOL		g_Load = FALSE;
static int		g_LoadPoint = 0;

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitGame(void)
{
	g_LoadPoint = 0;

	//LoadData();

	InitTube();
	InitGimmick();
	InitPlayer();

	g_Load = TRUE;
	return S_OK;
}

BOOL LoadGameTextureKernel(float* loadPalam, int* loadSum)
{
	//if (g_LoadPoint < TEXTURE_MAX)
	//{
	//	if (LoadGameTexture())
	//	{
	//		g_LoadPoint++;
	//		*loadSum = *loadSum + 1;
	//	}
	//	if (g_LoadPoint == TEXTURE_MAX)
	//	{
	//		*loadPalam = 0.0f;
	//		g_Load = TRUE;
	//		return TRUE;
	//	}
	//}
	//*loadPalam = (float)g_LoadPoint / (float)TEXTURE_MAX;
	//*loadPalam = 1;
	return TRUE;
}
BOOL LoadGameTexture(void)
{
	//g_Texture[g_LoadPoint] = NULL;
	//D3DX11CreateShaderResourceViewFromFile(GetDevice(), g_TextureName[g_LoadPoint], NULL, NULL, &g_Texture[g_LoadPoint], NULL);
	return TRUE;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitGame(void)
{
	if (g_Load == FALSE) return;

	UninitPlayer();
	UninitGimmick();
	UninitTube();

	g_Load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateGame(void)
{
#ifdef _DEBUG
	
	// ポーズ切り替え
	if (GetKeyboardTrigger(DIK_P))
	{
		g_bPause = g_bPause ? FALSE : TRUE;
	}

	if(g_bPause == FALSE)
		return;	// ポーズ中

#endif

	UpdateTube();
	UpdateGimmick();
	UpdatePlayer();

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawAllObjects(void)
{
	DrawTube();
	DrawGimmick();
	DrawPlayer();
}
void DrawGame(void)
{
	SetDrawNoLighting();
	SetDrawTube();
	DrawTube();
	SetDrawGimmick();
	//DrawGimmick();
	DrawPlayer();

	// アウトラインを引く
	//if (g_bOutline)
	//{
	//	SetDrawOutline(10.0f, GetRainbowColorNegative());
	//	//SetDrawOutline(5.0f);
	//	DrawPolygon(TRUE);
	//	DrawPolygonSphere(TRUE);
	//}

	//// 加算合成で明るくしていくために黒で塗りつぶす＋物体の前後関係（深度値）を記録する
	//SetDrawFillBlack();
	//DrawAllObjects();

	//// 加算合成モードにする
	//SetBlendState(BLEND_MODE_ADD);

	//// 平行光源に照らされた部分の描画
	//for (int i = 0; i < LIGHT_DIRECTIONAL_MAX; i++)
	//{
	//	// ライトの使用確認
	//	if (GetDLUse(i) == FALSE)
	//		continue;

	//	// ライト番号をセット
	//	SetLightNo(i);

	//	// 影になる部分のステンシルを作成
	//	SetStencilWriteDL();
	//	DrawAllObjects();

	//	// ステンシルテストを使って影以外の部分を加算合成で描画
	//	SetStencilReadDL();
	//	DrawAllObjects();

	//	// ステンシルを初期化
	//	ClearStencil();
	//}

	//// 点光源に照らされた部分の描画
	//for (int i = 0; i < LIGHT_POINT_MAX; i++)
	//{
	//	// ライトの使用確認
	//	if (GetPLUse(i) == FALSE)
	//		continue;

	//	// ライト番号をセット
	//	SetLightNo(i);

	//	// 影になる部分のステンシルを作成
	//	SetStencilWritePL();
	//	DrawAllObjects();

	//	// ステンシルテストを使って影以外の部分を加算合成で描画
	//	SetStencilReadPL();
	//	DrawAllObjects();

	//	// ステンシルを初期化
	//	ClearStencil();
	//}

	//// スポットライトに照らされた部分の描画
	//for (int i = 0; i < LIGHT_SPOT_MAX; i++)
	//{
	//	// ライトの使用確認
	//	if (GetSLUse(i) == FALSE)
	//		continue;

	//	// ライト番号をセット
	//	SetLightNo(i);

	//	// 影になる部分のステンシルを作成
	//	SetStencilWriteSL();
	//	DrawAllObjects();

	//	// ステンシルテストを使って影以外の部分を加算合成で描画
	//	SetStencilReadSL();
	//	DrawAllObjects();

	//	// ステンシルを初期化
	//	ClearStencil();
	//}

	//// アンビエントライトに照らされた部分の描画
	//if (GetALUse())
	//{
	//	SetStencilNoneAL();
	//	DrawAllObjects();
	//}

	//// 加算合成モードを終わる
	//SetBlendState(BLEND_MODE_ALPHABLEND);

	//// ノーマルマップ作成
	//if (g_bNormalMap)
	//{
	//	SetTargetNormalMap(TRUE);	// ターゲットビューに直接書き込む
	//	DrawAllObjects();
	//	//DrawNormalMap();
	//}
	//// デプスマップ作成
	//if (g_bDepthMap)
	//{
	//	SetTargetDepthMap(TRUE);	// ターゲットビューに直接書き込む
	//	DrawAllObjects();
	//	//DrawDepthMap();
	//}

	// 特殊なポストエフェクト
	//if (g_bChromaticAberration)	ApplyChromaticAberration();
	//if (g_bMonitoring)			ApplyMonitoring();
	//if (g_bOldGame)				ApplyOldGame();

	// モザイク処理
	//if (g_mosaicSize > 1)	ApplyMosaic(g_mosaicSize);

	// カーネルを使ったフィルタ処理
	//if (g_bAveraging)	ApplyFilter(FILTER_MODE_AVERAGING);
	//if (g_bGaussian)	ApplyFilter(FILTER_MODE_GAUSSIAN);
	//if (g_bSharpning)	ApplyFilter(FILTER_MODE_SHARPNING);
	//if (g_bLaplacian)	ApplyFilter(FILTER_MODE_LAPLACIAN);
	//ApplyFilter(FILTER_MODE_SHARPNING_HIGH);
	//ApplyFilter(FILTER_MODE_LAPLACIAN_COLOR);
	//ApplyFilter(FILTER_MODE_PREWITT_X);
	//ApplyFilter(FILTER_MODE_PREWITT_Y);
	//ApplyFilter(FILTER_MODE_SOBEL_X);
	//ApplyFilter(FILTER_MODE_SOBEL_Y);

	// バックバッファをターゲットにして描画
	//DrawTarget();
}