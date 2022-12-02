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
#include "ui_game.h"
#include "stage.h"
#include "teamlogo.h"
#include "missile.h"
#include "texture2d.h"
#include "gate.h"
#include "anim_start.h"
#include "particle.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static BOOL		g_bPause = TRUE;		// ポーズON/OFF
static BOOL		g_bStopTime = FALSE;	// ポーズON/OFF
static BOOL		g_Load = FALSE;

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitGame(void)
{
	InitTube();
	InitParticle();
	InitGimmick();
	InitPlayer();
	InitGameUI();
	InitGate();
	InitStage();
	InitMissile();
	InitAnimStart();

	SetCameraAnimStart();

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitGame(void)
{
	if (g_Load == FALSE) return;

	UninitAnimStart();
	UninitStage();
	UninitGate();
	UninitMissile();
	UninitGameUI();
	UninitPlayer();
	UninitGimmick();
	UninitParticle();
	UninitTube();

	g_Load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateGame(void)
{
#ifdef _DEBUG
	static LARGE_INTEGER Shadow_S, Shadow_E;
	static int oldTime, nowTime;
	nowTime++;
	if (nowTime - oldTime >= 20) { QueryPerformanceCounter(&Shadow_S); }
#endif
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
	UpdateParticle();

	UpdatePlayer();
	UpdateGameUI();
	UpdateGate();
	UpdateStage();
	UpdateMissile();
	UpdateAnimStart();

	if (GetKeyboardPress(DIK_K)) { SetStartGate(); }
#ifdef _DEBUG
	if (nowTime - oldTime >= 20) { QueryPerformanceCounter(&Shadow_E); }
	if (nowTime - oldTime >= 20) oldTime = nowTime;
	PrintDebugProc("UpdateTimeB:%d\n", Shadow_E.QuadPart - Shadow_S.QuadPart);
#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawAllObjects(void)
{
	DrawTube();
	DrawGimmick(GIMMICK_ICE);
	DrawGimmick(GIMMICK_RING);
	DrawPlayer();
}
void DrawGame(void)
{

	DrawAnimStart();

	//SetDrawNoLighting();
	//DrawPlayer();

	//DrawGameUI();
	//DrawTexture2DAll(TRUE);


#ifdef _DEBUG
	static LARGE_INTEGER Shadow_S, Shadow_E, Shade_S, Shade_E, Light_S, Light_E;
	static int oldTime, nowTime;
	nowTime++;
#endif

#ifdef _DEBUG
	if (nowTime - oldTime >= 20) { QueryPerformanceCounter(&Light_S); }
#endif
	{	// ALL 25000 → 15000
		//// アウトラインを引く 3000
		//SetDrawOutline(0.8f, { 1.0f, 0.0f, 0.0f, 1.0f });
		//DrawGimmickInstancing(GIMMICK_ICE, TRUE);

		//SetDrawOutline(0.8f, { 1.0f, 1.0f, 0.0f, 1.0f });
		//DrawMissile(MISSILE_TYPE_01);
		//DrawMissile(MISSILE_TYPE_02);

		// 環境光で下塗りする 3000
		{
			//SetStencilNoneAL(SHADER_TUBE);
			//DrawTube();	// 800
			//SetStencilNoneAL(SHADER_GIMMICK);
			//DrawGimmick(GIMMICK_ICE);	// 1500
			//SetStencilNoneAL(SHADER_PLAYER);
			//DrawPlayer();	// 900

			// 黒塗りする 3000
			//SetDrawFillBlack(SHADER_TUBE);
			//DrawTube();
			//SetDrawFillBlack(SHADER_GIMMICK);
			//DrawGimmick(GIMMICK_ICE);
			//SetDrawFillBlack(SHADER_PLAYER);
			//DrawPlayer();
		}

		// 加算合成モードにする
		//SetBlendState(BLEND_MODE_ADD);

		{
			// シャドウステンシルを描画 3000
		{
			// 影になる部分のステンシルを作成
			////SetStencilWriteLL(SHADER_TUBE);
			////DrawTube();
			//SetStencilWriteLL(SHADER_GIMMICK);
			//DrawGimmick(GIMMICK_ICE);
			////SetStencilWritePL();
			//SetStencilWriteLL(SHADER_PLAYER);
			//DrawPlayer();
		}

#ifdef _DEBUG
		if (nowTime - oldTime >= 20) { QueryPerformanceCounter(&Shade_S); }
#endif
			// ステンシルテストを使って影以外の部分を加算合成で描画
			// ラインライトの光 3000 → 5000
			SetStencilReadLL(SHADER_TUBE);
			DrawTube();


			//DrawDoor();
			SetStencilReadLLGimmick();
			//SetStencilReadLL(SHADER_GIMMICK);
			DrawGimmickInstancing(GIMMICK_ICE);
			DrawMissile(MISSILE_TYPE_01);
			DrawMissile(MISSILE_TYPE_02);
			SetStencilReadLL(SHADER_PLAYER);
			DrawPlayer();

#ifdef _DEBUG
		if (nowTime - oldTime >= 20) { QueryPerformanceCounter(&Shade_E); }
#endif
			// ステンシルを初期化
			//ClearStencil();

#ifdef _DEBUG
			if (nowTime - oldTime >= 20) { QueryPerformanceCounter(&Shadow_S); }
#endif
			// 光るもの描画 3000
			{
				SetBlendState(BLEND_MODE_ADD);

				//SetDrawTubeLight();

				SetDrawInstancingOnlyTex();
				DrawGimmickInstancing(GIMMICK_RING);

				//SetDrawLight();
				//DrawTubeLight();

				//SetBlendState(BLEND_MODE_ADD);
				SetDrawTubeLight();
				DrawTubeLight();
				DrawTubeLight2();
				//SetBlendState(BLEND_MODE_ALPHABLEND);


				ApplyLightToTarget();

				SetDrawFire();
				DrawFire();

				SetDrawMissileFire();
				DrawMissileFire();

				DrawGate();

				DrawParticle();

				SetBlendState(BLEND_MODE_ALPHABLEND);


				// アウトラインを引く 3000
				SetDrawOutline(0.8f, { 1.0f, 0.0f, 0.0f, 1.0f });
				DrawGimmickInstancing(GIMMICK_ICE, TRUE);

			}
#ifdef _DEBUG
			if (nowTime - oldTime >= 20) { QueryPerformanceCounter(&Shadow_E); }
#endif

		}

		// 加算合成モードを終了する
		//SetBlendState(BLEND_MODE_ALPHABLEND);
	}

#ifdef _DEBUG
	static bool blur = true;
	if (GetKeyboardTrigger(DIK_F1)) blur = blur ? false : true;
	if (blur)
#endif
	ApplyMotionBlur();

	//ApplyFilter(FILTER_MODE_LAPLACIAN);
	//	FILTER_MODE_NONE,			// フィルタなし
	//	FILTER_MODE_AVERAGING,		// 平均化フィルタ
	//	FILTER_MODE_GAUSSIAN,		// ガウシアンフィルタ
	//	FILTER_MODE_SHARPNING,		// 鮮鋭化フィルタ弱
	//	FILTER_MODE_SHARPNING_HIGH,	// 鮮鋭化フィルタ強
	//	FILTER_MODE_LAPLACIAN,		// ラプラシアンフィルタ
	//	FILTER_MODE_LAPLACIAN_COLOR,// ラプラシアンフィルタカラー
	//	FILTER_MODE_PREWITT_X,		// プリューウィットフィルタ横
	//	FILTER_MODE_PREWITT_Y,		// プリューウィットフィルタ縦
	//	FILTER_MODE_SOBEL_X,		// ソーベルフィルタ横
	//	FILTER_MODE_SOBEL_Y,		// ソーベルフィルタ縦

	//// UI描画 15000 → 150（インスタンシング使用）
	//SetDraw2DTexture();
	//DrawGameUI();

	//// バックバッファをターゲットにして描画 1000
	//DrawTarget();

	// UI描画 15000 → 150（インスタンシング使用）
	//SetDraw2DTexture();
	DrawGameUI();
	//DrawTexture2DAll();
	//ClearDepth();

#ifdef _DEBUG
	if (nowTime - oldTime >= 20) { QueryPerformanceCounter(&Light_E); }
#endif

#ifdef _DEBUG
	if (nowTime - oldTime >= 20) oldTime = nowTime;
	PrintDebugProc("DrawTimeLight:%d\n", Shadow_E.QuadPart - Shadow_S.QuadPart);
	PrintDebugProc("DrawTimeMain:%d\n", Shade_E.QuadPart - Shade_S.QuadPart);
	PrintDebugProc("DrawTimeALL:%d\n", Light_E.QuadPart - Light_S.QuadPart);
#endif


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