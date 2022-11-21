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


//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH)	// �w�i�T�C�Y��
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)	// �w�i�T�C�Y�c

#define ROCKET_MAX					(6)
#define ROCKET_SPEED				(10)

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
static ID3D11ShaderResourceView*	g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����
static char*	g_TextureName[TEXTURE_MAX] = {
	"data/TEXTURE/result_country/oosaka.jpg",
	"data/TEXTURE/result_country/shanghai.jpg",
	"data/TEXTURE/result_country/France.jpg",
	"data/TEXTURE/result_country/America.jpg",

};


XMFLOAT3 g_CameraPos, g_CameraAt, g_CameraUp;


//=============================================================================
// ����������
//=============================================================================
HRESULT InitResultEvent(void)
{
	// �e�N�X�`������
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_TextureName[i],
			NULL,
			NULL,
			&g_Texture[i],
			NULL);
	}

	// �ڍאݒ�
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
// �I������
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
// �X�V����
//=============================================================================
void UpdateResultEvent(void)
{
	g_CameraPos.z -= ROCKET_SPEED;
	g_CameraAt.z -= ROCKET_SPEED;
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
