//=============================================================================
//
// ���T���g�̉��o���� [result_event.cpp]
// Author : � ��m
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
#include "stage.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH)	// �w�i�T�C�Y��
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)	// �w�i�T�C�Y�c

#define ROCKET_MAX					(6)
#define ROCKET_SPEED				(20)

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static int				g_Time = 0;
static BOOL				g_Load = FALSE;

// �e�N�X�`���Ǘ�
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
static TEXTURE2D_DESC*	g_td;

XMFLOAT3 g_CameraPos, g_CameraAt, g_CameraUp;


//=============================================================================
// ����������
//=============================================================================
HRESULT InitResultEvent(void)
{
	g_td = new TEXTURE2D_DESC[TEXTURE_MAX];

	for (int i = 0; i < TEXTURE_MAX; i++) {
		g_td[i].tex = (TEXTURE_LABEL)(TEXTURE_LABEL_BG_OOSAKA + i);
	}

	g_CameraPos = { 2500.0f, -500.0f, 2000.0f };
	g_CameraAt = { 0.0f, -500.0f, 2000.0f };
	g_CameraUp = { 0.0f, 1.0f, 0.0f };

	g_Time = 0;

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitResultEvent(void)
{
	if (g_Load == FALSE) return;

	delete[] g_td;

	g_Load = FALSE;
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateResultEvent(void)
{
	g_CameraPos.z -= ROCKET_SPEED;
	g_CameraAt.z -= ROCKET_SPEED;

	if (g_CameraPos.z < -2500.0f)
	{
		SetEventFinish();
	}
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawResultEvent(void)
{
	//��ŃX�e�[�W�ɕ����ĕ`�悷��
	DrawTexture2D(&g_td[GetStageNo()]);

	DrawTexture2DAll(TRUE);
	
	//�`���[�u�̕`��
	static CURVE_BUFFER curve;
	SetCurveBuffer(&curve);
	SetAfterRotation(&XMMatrixRotationRollPitchYaw(0.0f, 0.0f, 0.0f));

	SetViewBuffer(&XMMatrixLookAtLH(XMLoadFloat3(&g_CameraPos), XMLoadFloat3(&g_CameraAt), XMLoadFloat3(&g_CameraUp)));

	SetCullingMode(CULL_MODE_BACK);
	SetStencilReadLL(SHADER_TUBE);
	DrawTubeResult(g_CameraPos.z);

	//���P�b�g�̕`��
	SetStencilReadLL(SHADER_PLAYER);
	DrawPlayerResult();

	SetBlendState(BLEND_MODE_ADD);
	SetDrawFire();
	DrawFireResult();
	SetBlendState(BLEND_MODE_ALPHABLEND);
}
