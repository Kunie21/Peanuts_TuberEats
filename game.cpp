//=============================================================================
//
// �Q�[������ [game.cpp]
// Author : ���] �đ�
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
// �}�N����`
//*****************************************************************************

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static BOOL		g_bPause = TRUE;		// �|�[�YON/OFF
static BOOL		g_bStopTime = FALSE;	// �|�[�YON/OFF
static BOOL		g_Load = FALSE;

//=============================================================================
// ����������
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
// �I������
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
// �X�V����
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
	
	// �|�[�Y�؂�ւ�
	if (GetKeyboardTrigger(DIK_P))
	{
		g_bPause = g_bPause ? FALSE : TRUE;
	}

	if(g_bPause == FALSE)
		return;	// �|�[�Y��

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
// �`�揈��
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
	{	// ALL 25000 �� 15000
		//// �A�E�g���C�������� 3000
		//SetDrawOutline(0.8f, { 1.0f, 0.0f, 0.0f, 1.0f });
		//DrawGimmickInstancing(GIMMICK_ICE, TRUE);

		//SetDrawOutline(0.8f, { 1.0f, 1.0f, 0.0f, 1.0f });
		//DrawMissile(MISSILE_TYPE_01);
		//DrawMissile(MISSILE_TYPE_02);

		// �����ŉ��h�肷�� 3000
		{
			//SetStencilNoneAL(SHADER_TUBE);
			//DrawTube();	// 800
			//SetStencilNoneAL(SHADER_GIMMICK);
			//DrawGimmick(GIMMICK_ICE);	// 1500
			//SetStencilNoneAL(SHADER_PLAYER);
			//DrawPlayer();	// 900

			// ���h�肷�� 3000
			//SetDrawFillBlack(SHADER_TUBE);
			//DrawTube();
			//SetDrawFillBlack(SHADER_GIMMICK);
			//DrawGimmick(GIMMICK_ICE);
			//SetDrawFillBlack(SHADER_PLAYER);
			//DrawPlayer();
		}

		// ���Z�������[�h�ɂ���
		//SetBlendState(BLEND_MODE_ADD);

		{
			// �V���h�E�X�e���V����`�� 3000
		{
			// �e�ɂȂ镔���̃X�e���V�����쐬
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
			// �X�e���V���e�X�g���g���ĉe�ȊO�̕��������Z�����ŕ`��
			// ���C�����C�g�̌� 3000 �� 5000
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
			// �X�e���V����������
			//ClearStencil();

#ifdef _DEBUG
			if (nowTime - oldTime >= 20) { QueryPerformanceCounter(&Shadow_S); }
#endif
			// ������̕`�� 3000
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


				// �A�E�g���C�������� 3000
				SetDrawOutline(0.8f, { 1.0f, 0.0f, 0.0f, 1.0f });
				DrawGimmickInstancing(GIMMICK_ICE, TRUE);

			}
#ifdef _DEBUG
			if (nowTime - oldTime >= 20) { QueryPerformanceCounter(&Shadow_E); }
#endif

		}

		// ���Z�������[�h���I������
		//SetBlendState(BLEND_MODE_ALPHABLEND);
	}

#ifdef _DEBUG
	static bool blur = true;
	if (GetKeyboardTrigger(DIK_F1)) blur = blur ? false : true;
	if (blur)
#endif
	ApplyMotionBlur();

	//ApplyFilter(FILTER_MODE_LAPLACIAN);
	//	FILTER_MODE_NONE,			// �t�B���^�Ȃ�
	//	FILTER_MODE_AVERAGING,		// ���ω��t�B���^
	//	FILTER_MODE_GAUSSIAN,		// �K�E�V�A���t�B���^
	//	FILTER_MODE_SHARPNING,		// �N�s���t�B���^��
	//	FILTER_MODE_SHARPNING_HIGH,	// �N�s���t�B���^��
	//	FILTER_MODE_LAPLACIAN,		// ���v���V�A���t�B���^
	//	FILTER_MODE_LAPLACIAN_COLOR,// ���v���V�A���t�B���^�J���[
	//	FILTER_MODE_PREWITT_X,		// �v�����[�E�B�b�g�t�B���^��
	//	FILTER_MODE_PREWITT_Y,		// �v�����[�E�B�b�g�t�B���^�c
	//	FILTER_MODE_SOBEL_X,		// �\�[�x���t�B���^��
	//	FILTER_MODE_SOBEL_Y,		// �\�[�x���t�B���^�c

	//// UI�`�� 15000 �� 150�i�C���X�^���V���O�g�p�j
	//SetDraw2DTexture();
	//DrawGameUI();

	//// �o�b�N�o�b�t�@���^�[�Q�b�g�ɂ��ĕ`�� 1000
	//DrawTarget();

	// UI�`�� 15000 �� 150�i�C���X�^���V���O�g�p�j
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


	// �A�E�g���C��������
	//if (g_bOutline)
	//{
	//	SetDrawOutline(10.0f, GetRainbowColorNegative());
	//	//SetDrawOutline(5.0f);
	//	DrawPolygon(TRUE);
	//	DrawPolygonSphere(TRUE);
	//}

	//// ���Z�����Ŗ��邭���Ă������߂ɍ��œh��Ԃ��{���̂̑O��֌W�i�[�x�l�j���L�^����
	//SetDrawFillBlack();
	//DrawAllObjects();

	//// ���Z�������[�h�ɂ���
	//SetBlendState(BLEND_MODE_ADD);

	//// ���s�����ɏƂ炳�ꂽ�����̕`��
	//for (int i = 0; i < LIGHT_DIRECTIONAL_MAX; i++)
	//{
	//	// ���C�g�̎g�p�m�F
	//	if (GetDLUse(i) == FALSE)
	//		continue;

	//	// ���C�g�ԍ����Z�b�g
	//	SetLightNo(i);

	//	// �e�ɂȂ镔���̃X�e���V�����쐬
	//	SetStencilWriteDL();
	//	DrawAllObjects();

	//	// �X�e���V���e�X�g���g���ĉe�ȊO�̕��������Z�����ŕ`��
	//	SetStencilReadDL();
	//	DrawAllObjects();

	//	// �X�e���V����������
	//	ClearStencil();
	//}

	//// �_�����ɏƂ炳�ꂽ�����̕`��
	//for (int i = 0; i < LIGHT_POINT_MAX; i++)
	//{
	//	// ���C�g�̎g�p�m�F
	//	if (GetPLUse(i) == FALSE)
	//		continue;

	//	// ���C�g�ԍ����Z�b�g
	//	SetLightNo(i);

	//	// �e�ɂȂ镔���̃X�e���V�����쐬
	//	SetStencilWritePL();
	//	DrawAllObjects();

	//	// �X�e���V���e�X�g���g���ĉe�ȊO�̕��������Z�����ŕ`��
	//	SetStencilReadPL();
	//	DrawAllObjects();

	//	// �X�e���V����������
	//	ClearStencil();
	//}

	//// �X�|�b�g���C�g�ɏƂ炳�ꂽ�����̕`��
	//for (int i = 0; i < LIGHT_SPOT_MAX; i++)
	//{
	//	// ���C�g�̎g�p�m�F
	//	if (GetSLUse(i) == FALSE)
	//		continue;

	//	// ���C�g�ԍ����Z�b�g
	//	SetLightNo(i);

	//	// �e�ɂȂ镔���̃X�e���V�����쐬
	//	SetStencilWriteSL();
	//	DrawAllObjects();

	//	// �X�e���V���e�X�g���g���ĉe�ȊO�̕��������Z�����ŕ`��
	//	SetStencilReadSL();
	//	DrawAllObjects();

	//	// �X�e���V����������
	//	ClearStencil();
	//}

	//// �A���r�G���g���C�g�ɏƂ炳�ꂽ�����̕`��
	//if (GetALUse())
	//{
	//	SetStencilNoneAL();
	//	DrawAllObjects();
	//}

	//// ���Z�������[�h���I���
	//SetBlendState(BLEND_MODE_ALPHABLEND);

	//// �m�[�}���}�b�v�쐬
	//if (g_bNormalMap)
	//{
	//	SetTargetNormalMap(TRUE);	// �^�[�Q�b�g�r���[�ɒ��ڏ�������
	//	DrawAllObjects();
	//	//DrawNormalMap();
	//}
	//// �f�v�X�}�b�v�쐬
	//if (g_bDepthMap)
	//{
	//	SetTargetDepthMap(TRUE);	// �^�[�Q�b�g�r���[�ɒ��ڏ�������
	//	DrawAllObjects();
	//	//DrawDepthMap();
	//}

	// ����ȃ|�X�g�G�t�F�N�g
	//if (g_bChromaticAberration)	ApplyChromaticAberration();
	//if (g_bMonitoring)			ApplyMonitoring();
	//if (g_bOldGame)				ApplyOldGame();

	// ���U�C�N����
	//if (g_mosaicSize > 1)	ApplyMosaic(g_mosaicSize);

	// �J�[�l�����g�����t�B���^����
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

	// �o�b�N�o�b�t�@���^�[�Q�b�g�ɂ��ĕ`��
	//DrawTarget();
}