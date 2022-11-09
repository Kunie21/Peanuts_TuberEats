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

//*****************************************************************************
// �}�N����`
//*****************************************************************************

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static BOOL		g_bPause = TRUE;		// �|�[�YON/OFF
static BOOL		g_bStopTime = FALSE;	// �|�[�YON/OFF
static BOOL		g_Load = FALSE;
static int		g_LoadPoint = 0;

//=============================================================================
// ����������
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
// �I������
//=============================================================================
void UninitGame(void)
{
	if (g_Load == FALSE) return;

	UninitStage();
	UninitGameUI();
	UninitPlayer();
	UninitGimmick();
	UninitTube();

	g_Load = FALSE;
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateGame(void)
{
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
	UpdatePlayer();
	UpdateGameUI();
	UpdateStage();

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawAllObjects(void)
{
	DrawTube();
	//DrawGimmick();
	DrawPlayer();
}
void DrawGame(void)
{
	//SetDrawNoLighting();
	//DrawPlayer();
	
	{
		// ���h�肷��
		{
			SetDrawFillBlack(SHADER_TUBE);
			DrawTube();
			SetDrawFillBlack(SHADER_GIMMICK);
			DrawGimmick(GIMMICK_ICE);
			SetDrawFillBlack(SHADER_PLAYER);
			DrawPlayer();
		}

		// ���Z�������[�h�ɂ���
		SetBlendState(BLEND_MODE_ADD);

		{
			// �e�ɂȂ镔���̃X�e���V����쐬
			//SetStencilWriteLL(SHADER_TUBE);
			//DrawTube();
			SetStencilWriteLL(SHADER_GIMMICK);
			DrawGimmick(GIMMICK_ICE);
			//SetStencilWritePL();
			SetStencilWriteLL(SHADER_PLAYER);
			DrawPlayer();

			// �X�e���V���e�X�g��g���ĉe�ȊO�̕�������Z�����ŕ`��

			// ���C�����C�g
			SetStencilReadLL(SHADER_TUBE);
			DrawTube();
			SetStencilReadLL(SHADER_GIMMICK);
			DrawGimmick(GIMMICK_ICE);
			SetStencilReadLL(SHADER_PLAYER);
			DrawPlayer();

			// �X�e���V���������
			ClearStencil();

			// ����
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

			{
				SetDrawLight();
				DrawTubeLight();
				DrawGimmick(GIMMICK_RING);
				ApplyLightToTarget();
			}

			SetDrawFire();
			DrawFire();
		}

		// ���Z�������[�h��I������
		SetBlendState(BLEND_MODE_ALPHABLEND);
	}

	//ApplyFilter(FILTER_MODE_GAUSSIAN);

	// �o�b�N�o�b�t�@��^�[�Q�b�g�ɂ��ĕ`��
	DrawTarget();

	SetDraw2DTexture();
	DrawGameUI();



	// �A�E�g���C�������
	//if (g_bOutline)
	//{
	//	SetDrawOutline(10.0f, GetRainbowColorNegative());
	//	//SetDrawOutline(5.0f);
	//	DrawPolygon(TRUE);
	//	DrawPolygonSphere(TRUE);
	//}

	//// ���Z�����Ŗ��邭���Ă������߂ɍ��œh��Ԃ��{���̂̑O��֌W�i�[�x�l�j��L�^����
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

	//	// ���C�g�ԍ���Z�b�g
	//	SetLightNo(i);

	//	// �e�ɂȂ镔���̃X�e���V����쐬
	//	SetStencilWriteDL();
	//	DrawAllObjects();

	//	// �X�e���V���e�X�g��g���ĉe�ȊO�̕�������Z�����ŕ`��
	//	SetStencilReadDL();
	//	DrawAllObjects();

	//	// �X�e���V���������
	//	ClearStencil();
	//}

	//// �_�����ɏƂ炳�ꂽ�����̕`��
	//for (int i = 0; i < LIGHT_POINT_MAX; i++)
	//{
	//	// ���C�g�̎g�p�m�F
	//	if (GetPLUse(i) == FALSE)
	//		continue;

	//	// ���C�g�ԍ���Z�b�g
	//	SetLightNo(i);

	//	// �e�ɂȂ镔���̃X�e���V����쐬
	//	SetStencilWritePL();
	//	DrawAllObjects();

	//	// �X�e���V���e�X�g��g���ĉe�ȊO�̕�������Z�����ŕ`��
	//	SetStencilReadPL();
	//	DrawAllObjects();

	//	// �X�e���V���������
	//	ClearStencil();
	//}

	//// �X�|�b�g���C�g�ɏƂ炳�ꂽ�����̕`��
	//for (int i = 0; i < LIGHT_SPOT_MAX; i++)
	//{
	//	// ���C�g�̎g�p�m�F
	//	if (GetSLUse(i) == FALSE)
	//		continue;

	//	// ���C�g�ԍ���Z�b�g
	//	SetLightNo(i);

	//	// �e�ɂȂ镔���̃X�e���V����쐬
	//	SetStencilWriteSL();
	//	DrawAllObjects();

	//	// �X�e���V���e�X�g��g���ĉe�ȊO�̕�������Z�����ŕ`��
	//	SetStencilReadSL();
	//	DrawAllObjects();

	//	// �X�e���V���������
	//	ClearStencil();
	//}

	//// �A���r�G���g���C�g�ɏƂ炳�ꂽ�����̕`��
	//if (GetALUse())
	//{
	//	SetStencilNoneAL();
	//	DrawAllObjects();
	//}

	//// ���Z�������[�h��I���
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

	// �J�[�l����g�����t�B���^����
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

	// �o�b�N�o�b�t�@��^�[�Q�b�g�ɂ��ĕ`��
	//DrawTarget();
}