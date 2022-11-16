//=============================================================================
//
// ï¿½Qï¿½[ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ [game.cpp]
// Author : ï¿½ï¿½ï¿½] ï¿½Ä‘ï¿½
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
#include "door.h"
#include "anim_start.h"

//*****************************************************************************
// ï¿½}ï¿½Nï¿½ï¿½ï¿½ï¿½`
//*****************************************************************************

//*****************************************************************************
// ï¿½Oï¿½ï¿½ï¿½[ï¿½oï¿½ï¿½ï¿½Ïï¿½
//*****************************************************************************
static BOOL		g_bPause = TRUE;		// ï¿½|ï¿½[ï¿½YON/OFF
static BOOL		g_bStopTime = FALSE;	// ï¿½|ï¿½[ï¿½YON/OFF
static BOOL		g_Load = FALSE;
static int		g_LoadPoint = 0;

//=============================================================================
// ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½
//=============================================================================
HRESULT InitGame(void)
{
	g_LoadPoint = 0;

	//LoadData();

	InitTube();
	InitGimmick();
	InitPlayer();
	InitGameUI();
	InitStage();
	InitMissile();
	InitDoor();
	InitAnimStart();

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
// ï¿½Iï¿½ï¿½ï¿½ï¿½ï¿½ï¿½
//=============================================================================
void UninitGame(void)
{
	if (g_Load == FALSE) return;

	UninitAnimStart();
	UninitDoor();
	UninitMissile();
	UninitStage();
	UninitGameUI();
	UninitPlayer();
	UninitGimmick();
	UninitTube();

	g_Load = FALSE;
}

//=============================================================================
// ï¿½Xï¿½Vï¿½ï¿½ï¿½ï¿½
//=============================================================================
void UpdateGame(void)
{
#ifdef _DEBUG
	
	// ï¿½|ï¿½[ï¿½Yï¿½Ø‚ï¿½Ö‚ï¿½
	if (GetKeyboardTrigger(DIK_P))
	{
		g_bPause = g_bPause ? FALSE : TRUE;
	}

	if(g_bPause == FALSE)
		return;	// ï¿½|ï¿½[ï¿½Yï¿½ï¿½

#endif

	UpdateTube();
	UpdateGimmick();
	UpdatePlayer();
	UpdateGameUI();
	UpdateStage();
	UpdateMissile();
	UpdateDoor();
	UpdateAnimStart();

}

//=============================================================================
// ï¿½`ï¿½æˆï¿½ï¿½
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
<<<<<<< HEAD
	
	{
		// ï¿½ï¿½ï¿½hï¿½è‚·ï¿½ï¿½
=======

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
	{	// ALL 25000 ¨ 15000
		// ƒAƒEƒgƒ‰ƒCƒ“‚ğˆø‚­ 3000
		SetDrawOutline(0.8f, { 1.0f, 0.0f, 0.0f, 1.0f });
		DrawGimmickInstancing(GIMMICK_ICE, TRUE);
		//SetDrawOutline(0.8f, { 1.0f, 1.0f, 0.0f, 1.0f });
		//DrawMissile(MISSILE_TYPE_01);
		//DrawMissile(MISSILE_TYPE_02);

		// ŠÂ‹«Œõ‚Å‰º“h‚è‚·‚é 3000
>>>>>>> 0c9843ec89b09e54e6e9db2a49eaa9ffee69c53e
		{
			//SetStencilNoneAL(SHADER_TUBE);
			//DrawTube();	// 800
			//SetStencilNoneAL(SHADER_GIMMICK);
			//DrawGimmick(GIMMICK_ICE);	// 1500
			//SetStencilNoneAL(SHADER_PLAYER);
			//DrawPlayer();	// 900

			// •“h‚è‚·‚é 3000
			//SetDrawFillBlack(SHADER_TUBE);
			//DrawTube();
			//SetDrawFillBlack(SHADER_GIMMICK);
			//DrawGimmick(GIMMICK_ICE);
			//SetDrawFillBlack(SHADER_PLAYER);
			//DrawPlayer();
		}

<<<<<<< HEAD
		// ï¿½ï¿½ï¿½Zï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½[ï¿½hï¿½É‚ï¿½ï¿½ï¿½
		SetBlendState(BLEND_MODE_ADD);

		{
			// ï¿½eï¿½É‚È‚é•”ï¿½ï¿½ï¿½ÌƒXï¿½eï¿½ï¿½ï¿½Vï¿½ï¿½ï¿½ï¿½ì¬
			//SetStencilWriteLL(SHADER_TUBE);
			//DrawTube();
			SetStencilWriteLL(SHADER_GIMMICK);
			DrawGimmick(GIMMICK_ICE);
			//SetStencilWritePL();
			SetStencilWriteLL(SHADER_PLAYER);
			DrawPlayer();

			// ï¿½Xï¿½eï¿½ï¿½ï¿½Vï¿½ï¿½ï¿½eï¿½Xï¿½gï¿½ï¿½gï¿½ï¿½ï¿½Ä‰eï¿½ÈŠOï¿½Ì•ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Zï¿½ï¿½ï¿½ï¿½ï¿½Å•`ï¿½ï¿½

			// ï¿½ï¿½ï¿½Cï¿½ï¿½ï¿½ï¿½ï¿½Cï¿½g
=======
		// ‰ÁZ‡¬ƒ‚[ƒh‚É‚·‚é
		//SetBlendState(BLEND_MODE_ADD);

		{
			// ƒVƒƒƒhƒEƒXƒeƒ“ƒVƒ‹‚ğ•`‰æ 3000
		{
			// ‰e‚É‚È‚é•”•ª‚ÌƒXƒeƒ“ƒVƒ‹‚ğì¬
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
			// ƒXƒeƒ“ƒVƒ‹ƒeƒXƒg‚ğg‚Á‚Ä‰eˆÈŠO‚Ì•”•ª‚ğ‰ÁZ‡¬‚Å•`‰æ
			// ƒ‰ƒCƒ“ƒ‰ƒCƒg‚ÌŒõ 3000 ¨ 5000
>>>>>>> 0c9843ec89b09e54e6e9db2a49eaa9ffee69c53e
			SetStencilReadLL(SHADER_TUBE);
			DrawTube();
			DrawDoor();
			SetStencilReadLLGimmick();
			//SetStencilReadLL(SHADER_GIMMICK);
			DrawGimmickInstancing(GIMMICK_ICE);
			DrawMissile(MISSILE_TYPE_01);
			DrawMissile(MISSILE_TYPE_02);
			SetStencilReadLL(SHADER_PLAYER);
			DrawPlayer();

<<<<<<< HEAD
			// ï¿½Xï¿½eï¿½ï¿½ï¿½Vï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½
			ClearStencil();

			// ï¿½Â‹ï¿½ï¿½ï¿½
			SetStencilNoneAL(SHADER_TUBE);
			DrawTube();
			SetStencilNoneAL(SHADER_GIMMICK);
			DrawGimmick(GIMMICK_ICE);
			SetStencilNoneAL(SHADER_PLAYER);
			DrawPlayer();

			//SetDrawTube();
			//DrawTube();

			//SetDrawGimmick();

			//SetDrawPlayer();
			//DrawPlayer();

=======
#ifdef _DEBUG
		if (nowTime - oldTime >= 20) { QueryPerformanceCounter(&Shade_E); }
#endif
			// ƒXƒeƒ“ƒVƒ‹‚ğ‰Šú‰»
			ClearStencil();

#ifdef _DEBUG
			if (nowTime - oldTime >= 20) { QueryPerformanceCounter(&Shadow_S); }
#endif
			// Œõ‚é‚à‚Ì•`‰æ 3000
>>>>>>> 0c9843ec89b09e54e6e9db2a49eaa9ffee69c53e
			{
				//SetDrawTubeLight();

				SetBlendState(BLEND_MODE_ADD);

				SetDrawInstancingOnlyTex();
				DrawGimmickInstancing(GIMMICK_RING);

				SetDrawLight();
				DrawTubeLight();

				ApplyLightToTarget();

				SetDrawFire();
				DrawFire();

				SetDrawMissileFire();
				DrawMissileFire();

				SetBlendState(BLEND_MODE_ALPHABLEND);
			}
#ifdef _DEBUG
			if (nowTime - oldTime >= 20) { QueryPerformanceCounter(&Shadow_E); }
#endif

		}

<<<<<<< HEAD
		// ï¿½ï¿½ï¿½Zï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½[ï¿½hï¿½ï¿½Iï¿½ï¿½ï¿½ï¿½ï¿½ï¿½
		SetBlendState(BLEND_MODE_ALPHABLEND);
=======
		// ‰ÁZ‡¬ƒ‚[ƒh‚ğI—¹‚·‚é
		//SetBlendState(BLEND_MODE_ALPHABLEND);
>>>>>>> 0c9843ec89b09e54e6e9db2a49eaa9ffee69c53e
	}

	//ApplyFilter(FILTER_MODE_LAPLACIAN);
	//	FILTER_MODE_NONE,			// ƒtƒBƒ‹ƒ^‚È‚µ
	//	FILTER_MODE_AVERAGING,		// •½‹Ï‰»ƒtƒBƒ‹ƒ^
	//	FILTER_MODE_GAUSSIAN,		// ƒKƒEƒVƒAƒ“ƒtƒBƒ‹ƒ^
	//	FILTER_MODE_SHARPNING,		// ‘N‰s‰»ƒtƒBƒ‹ƒ^ã
	//	FILTER_MODE_SHARPNING_HIGH,	// ‘N‰s‰»ƒtƒBƒ‹ƒ^‹­
	//	FILTER_MODE_LAPLACIAN,		// ƒ‰ƒvƒ‰ƒVƒAƒ“ƒtƒBƒ‹ƒ^
	//	FILTER_MODE_LAPLACIAN_COLOR,// ƒ‰ƒvƒ‰ƒVƒAƒ“ƒtƒBƒ‹ƒ^ƒJƒ‰[
	//	FILTER_MODE_PREWITT_X,		// ƒvƒŠƒ…[ƒEƒBƒbƒgƒtƒBƒ‹ƒ^‰¡
	//	FILTER_MODE_PREWITT_Y,		// ƒvƒŠƒ…[ƒEƒBƒbƒgƒtƒBƒ‹ƒ^c
	//	FILTER_MODE_SOBEL_X,		// ƒ\[ƒxƒ‹ƒtƒBƒ‹ƒ^‰¡
	//	FILTER_MODE_SOBEL_Y,		// ƒ\[ƒxƒ‹ƒtƒBƒ‹ƒ^c

<<<<<<< HEAD
	// ï¿½oï¿½bï¿½Nï¿½oï¿½bï¿½tï¿½@ï¿½ï¿½^ï¿½[ï¿½Qï¿½bï¿½gï¿½É‚ï¿½ï¿½Ä•`ï¿½ï¿½
	DrawTarget();
=======
	//// UI•`‰æ 15000 ¨ 150iƒCƒ“ƒXƒ^ƒ“ƒVƒ“ƒOg—pj
	//SetDraw2DTexture();
	//DrawGameUI();
>>>>>>> 0c9843ec89b09e54e6e9db2a49eaa9ffee69c53e

	//// ƒoƒbƒNƒoƒbƒtƒ@‚ğƒ^[ƒQƒbƒg‚É‚µ‚Ä•`‰æ 1000
	//DrawTarget();

	// UI•`‰æ 15000 ¨ 150iƒCƒ“ƒXƒ^ƒ“ƒVƒ“ƒOg—pj
	//SetDraw2DTexture();
	DrawGameUI();
	//DrawTexture2DAll();
	//ClearDepth();

#ifdef _DEBUG
	if (nowTime - oldTime >= 20) { QueryPerformanceCounter(&Light_E); }
#endif

#ifdef _DEBUG
	if (nowTime - oldTime >= 20) oldTime = nowTime;
	PrintDebugProc("LightDrawTime:%d\n", Shadow_E.QuadPart - Shadow_S.QuadPart);
	PrintDebugProc("ShadingDrawTime:%d\n", Shade_E.QuadPart - Shade_S.QuadPart);
	PrintDebugProc("ALLDrawTime:%d\n", Light_E.QuadPart - Light_S.QuadPart);
#endif


	// ï¿½Aï¿½Eï¿½gï¿½ï¿½ï¿½Cï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½
	//if (g_bOutline)
	//{
	//	SetDrawOutline(10.0f, GetRainbowColorNegative());
	//	//SetDrawOutline(5.0f);
	//	DrawPolygon(TRUE);
	//	DrawPolygonSphere(TRUE);
	//}

	//// ï¿½ï¿½ï¿½Zï¿½ï¿½ï¿½ï¿½ï¿½Å–ï¿½ï¿½é‚­ï¿½ï¿½ï¿½Ä‚ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ß‚Éï¿½ï¿½Å“hï¿½ï¿½Â‚Ô‚ï¿½ï¿½{ï¿½ï¿½ï¿½Ì‚Ì‘Oï¿½ï¿½ÖŒWï¿½iï¿½[ï¿½xï¿½lï¿½jï¿½ï¿½Lï¿½^ï¿½ï¿½ï¿½ï¿½
	//SetDrawFillBlack();
	//DrawAllObjects();

	//// ï¿½ï¿½ï¿½Zï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½[ï¿½hï¿½É‚ï¿½ï¿½ï¿½
	//SetBlendState(BLEND_MODE_ADD);

	//// ï¿½ï¿½ï¿½sï¿½ï¿½ï¿½ï¿½ï¿½ÉÆ‚ç‚³ï¿½ê‚½ï¿½ï¿½ï¿½ï¿½ï¿½Ì•`ï¿½ï¿½
	//for (int i = 0; i < LIGHT_DIRECTIONAL_MAX; i++)
	//{
	//	// ï¿½ï¿½ï¿½Cï¿½gï¿½Ìgï¿½pï¿½mï¿½F
	//	if (GetDLUse(i) == FALSE)
	//		continue;

	//	// ï¿½ï¿½ï¿½Cï¿½gï¿½Ôï¿½ï¿½ï¿½Zï¿½bï¿½g
	//	SetLightNo(i);

	//	// ï¿½eï¿½É‚È‚é•”ï¿½ï¿½ï¿½ÌƒXï¿½eï¿½ï¿½ï¿½Vï¿½ï¿½ï¿½ï¿½ì¬
	//	SetStencilWriteDL();
	//	DrawAllObjects();

	//	// ï¿½Xï¿½eï¿½ï¿½ï¿½Vï¿½ï¿½ï¿½eï¿½Xï¿½gï¿½ï¿½gï¿½ï¿½ï¿½Ä‰eï¿½ÈŠOï¿½Ì•ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Zï¿½ï¿½ï¿½ï¿½ï¿½Å•`ï¿½ï¿½
	//	SetStencilReadDL();
	//	DrawAllObjects();

	//	// ï¿½Xï¿½eï¿½ï¿½ï¿½Vï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½
	//	ClearStencil();
	//}

	//// ï¿½_ï¿½ï¿½ï¿½ï¿½ï¿½ÉÆ‚ç‚³ï¿½ê‚½ï¿½ï¿½ï¿½ï¿½ï¿½Ì•`ï¿½ï¿½
	//for (int i = 0; i < LIGHT_POINT_MAX; i++)
	//{
	//	// ï¿½ï¿½ï¿½Cï¿½gï¿½Ìgï¿½pï¿½mï¿½F
	//	if (GetPLUse(i) == FALSE)
	//		continue;

	//	// ï¿½ï¿½ï¿½Cï¿½gï¿½Ôï¿½ï¿½ï¿½Zï¿½bï¿½g
	//	SetLightNo(i);

	//	// ï¿½eï¿½É‚È‚é•”ï¿½ï¿½ï¿½ÌƒXï¿½eï¿½ï¿½ï¿½Vï¿½ï¿½ï¿½ï¿½ì¬
	//	SetStencilWritePL();
	//	DrawAllObjects();

	//	// ï¿½Xï¿½eï¿½ï¿½ï¿½Vï¿½ï¿½ï¿½eï¿½Xï¿½gï¿½ï¿½gï¿½ï¿½ï¿½Ä‰eï¿½ÈŠOï¿½Ì•ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Zï¿½ï¿½ï¿½ï¿½ï¿½Å•`ï¿½ï¿½
	//	SetStencilReadPL();
	//	DrawAllObjects();

	//	// ï¿½Xï¿½eï¿½ï¿½ï¿½Vï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½
	//	ClearStencil();
	//}

	//// ï¿½Xï¿½|ï¿½bï¿½gï¿½ï¿½ï¿½Cï¿½gï¿½ÉÆ‚ç‚³ï¿½ê‚½ï¿½ï¿½ï¿½ï¿½ï¿½Ì•`ï¿½ï¿½
	//for (int i = 0; i < LIGHT_SPOT_MAX; i++)
	//{
	//	// ï¿½ï¿½ï¿½Cï¿½gï¿½Ìgï¿½pï¿½mï¿½F
	//	if (GetSLUse(i) == FALSE)
	//		continue;

	//	// ï¿½ï¿½ï¿½Cï¿½gï¿½Ôï¿½ï¿½ï¿½Zï¿½bï¿½g
	//	SetLightNo(i);

	//	// ï¿½eï¿½É‚È‚é•”ï¿½ï¿½ï¿½ÌƒXï¿½eï¿½ï¿½ï¿½Vï¿½ï¿½ï¿½ï¿½ì¬
	//	SetStencilWriteSL();
	//	DrawAllObjects();

	//	// ï¿½Xï¿½eï¿½ï¿½ï¿½Vï¿½ï¿½ï¿½eï¿½Xï¿½gï¿½ï¿½gï¿½ï¿½ï¿½Ä‰eï¿½ÈŠOï¿½Ì•ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Zï¿½ï¿½ï¿½ï¿½ï¿½Å•`ï¿½ï¿½
	//	SetStencilReadSL();
	//	DrawAllObjects();

	//	// ï¿½Xï¿½eï¿½ï¿½ï¿½Vï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½
	//	ClearStencil();
	//}

	//// ï¿½Aï¿½ï¿½ï¿½rï¿½Gï¿½ï¿½ï¿½gï¿½ï¿½ï¿½Cï¿½gï¿½ÉÆ‚ç‚³ï¿½ê‚½ï¿½ï¿½ï¿½ï¿½ï¿½Ì•`ï¿½ï¿½
	//if (GetALUse())
	//{
	//	SetStencilNoneAL();
	//	DrawAllObjects();
	//}

	//// ï¿½ï¿½ï¿½Zï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½[ï¿½hï¿½ï¿½Iï¿½ï¿½ï¿½
	//SetBlendState(BLEND_MODE_ALPHABLEND);

	//// ï¿½mï¿½[ï¿½}ï¿½ï¿½ï¿½}ï¿½bï¿½vï¿½ì¬
	//if (g_bNormalMap)
	//{
	//	SetTargetNormalMap(TRUE);	// ï¿½^ï¿½[ï¿½Qï¿½bï¿½gï¿½rï¿½ï¿½ï¿½[ï¿½É’ï¿½ï¿½Úï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½
	//	DrawAllObjects();
	//	//DrawNormalMap();
	//}
	//// ï¿½fï¿½vï¿½Xï¿½}ï¿½bï¿½vï¿½ì¬
	//if (g_bDepthMap)
	//{
	//	SetTargetDepthMap(TRUE);	// ï¿½^ï¿½[ï¿½Qï¿½bï¿½gï¿½rï¿½ï¿½ï¿½[ï¿½É’ï¿½ï¿½Úï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½
	//	DrawAllObjects();
	//	//DrawDepthMap();
	//}

	// ï¿½ï¿½ï¿½ï¿½Èƒ|ï¿½Xï¿½gï¿½Gï¿½tï¿½Fï¿½Nï¿½g
	//if (g_bChromaticAberration)	ApplyChromaticAberration();
	//if (g_bMonitoring)			ApplyMonitoring();
	//if (g_bOldGame)				ApplyOldGame();

	// ï¿½ï¿½ï¿½Uï¿½Cï¿½Nï¿½ï¿½ï¿½ï¿½
	//if (g_mosaicSize > 1)	ApplyMosaic(g_mosaicSize);

	// ï¿½Jï¿½[ï¿½lï¿½ï¿½ï¿½ï¿½gï¿½ï¿½ï¿½ï¿½ï¿½tï¿½Bï¿½ï¿½ï¿½^ï¿½ï¿½ï¿½ï¿½
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

	// ï¿½oï¿½bï¿½Nï¿½oï¿½bï¿½tï¿½@ï¿½ï¿½^ï¿½[ï¿½Qï¿½bï¿½gï¿½É‚ï¿½ï¿½Ä•`ï¿½ï¿½
	//DrawTarget();
}