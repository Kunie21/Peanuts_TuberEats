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
	TEXTURE_SHANGHAI,
	TEXTURE_PARIS,
	TEXTURE_NEWYORK,
	TEXTURE_MAX,
};
static TEXTURE2D_DESC	g_td[TEXTURE_MAX];
//static ID3D11ShaderResourceView*	g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����
//static char*	g_TextureName[TEXTURE_MAX] = {
//	"data/TEXTURE/result_country/oosaka.jpg",
//	"data/TEXTURE/result_country/shanghai.jpg",
//	"data/TEXTURE/result_country/France.jpg",
//	"data/TEXTURE/result_country/America.jpg",
//
//};


XMFLOAT3 g_CameraPos, g_CameraAt, g_CameraUp;


//=============================================================================
// ����������
//=============================================================================
HRESULT InitResultEvent(void)
{
	// �e�N�X�`������
	for (int i = 0; i < TEXTURE_MAX; i++) {
		g_td[i].tex = (TEXTURE_LABEL)(TEXTURE_LABEL_OOSAKA + i);
	}

	// �ڍאݒ�
	g_td[TEXTURE_OOSAKA].size = { TEXTURE_WIDTH, TEXTURE_HEIGHT };

	g_td[TEXTURE_SHANGHAI].size = { 900, 600 };
	g_td[TEXTURE_SHANGHAI].scl = { 2.2f, 2.2f };

	g_td[TEXTURE_PARIS].size = { TEXTURE_WIDTH, TEXTURE_HEIGHT };

	g_td[TEXTURE_NEWYORK].size = { TEXTURE_WIDTH, TEXTURE_HEIGHT };


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


	g_Load = FALSE;
}

//=============================================================================
// �X�V����
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
// �`�揈��
//=============================================================================
void DrawResultEvent(void)
{
	//SetDraw2DTexture();
	
	//��ŃX�e�[�W�ɕ����ĕ`�悷��
	DrawTexture2D(&g_td[TEXTURE_OOSAKA]);
	//DrawTexture2D(&g_td[TEXTURE_SHANGHAI]);
	//DrawTexture2D(&g_td[TEXTURE_PARIS]);
	//DrawTexture2D(&g_td[TEXTURE_NEWYORK]);

	DrawTexture2DAll(TRUE);
	
	//�`���[�u�̕`��
	static CURVE_BUFFER curve;
	SetCurveBuffer(&curve);
	SetAfterRotation(&XMMatrixRotationRollPitchYaw(0.0f, 0.0f, 0.0f));

	SetViewBuffer(&XMMatrixLookAtLH(XMLoadFloat3(&g_CameraPos), XMLoadFloat3(&g_CameraAt), XMLoadFloat3(&g_CameraUp)));

	SetStencilReadLL(SHADER_TUBE);
	DrawTubeResult();

	//���P�b�g�̕`��
	SetStencilReadLL(SHADER_PLAYER);
	DrawPlayerResult();
	DrawFireResult();

}
