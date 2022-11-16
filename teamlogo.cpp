//=============================================================================
//
// �`�[�����S��ʏ��� [teamlogo.cpp]
// Author : ���] �đ�
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "texture2d.h"
#include "teamlogo.h"
#include "fade.h"
#include "model.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH)	// �w�i�T�C�Y��
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)	// �w�i�T�C�Y�c

//#define TEXTURE_WIDTH_LOGO			(1154)			// ���S�T�C�Y��
//#define TEXTURE_HEIGHT_LOGO			(693)			// ���S�T�C�Y�c

#define TEAMLOGO_TIME				(100)			// ���S�\������

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static int				g_Time = 0;
static BOOL				g_Load = FALSE;

// �e�N�X�`���Ǘ�
enum
{
	TEXTURE_BG = 0,
	//TEXTURE_TEAMLOGO,
	TEXTURE_MAX,
};
static TEXTURE2D_DESC	g_td[TEXTURE_MAX];
static ID3D11ShaderResourceView*	g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����
static char*	g_TextureName[TEXTURE_MAX] = {
	//"data/TEXTURE/white.png",
	"data/TEXTURE/peanuts_bg_1.png",
};


enum {
	MODEL_STAR = 0,
	MODEL_EARTH,
	MODEL_ROCKET,
	MODEL_MAX,
};
static MODEL_DATA	g_Model[MODEL_MAX];	// �v���C���[�̃��f���Ǘ�

//=============================================================================
// ����������
//=============================================================================
HRESULT InitTeamLogo(void)
{
	// �e�N�X�`������
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		D3DX11CreateShaderResourceViewFromFile(GetDevice(), g_TextureName[i], NULL, NULL, &g_Texture[i], NULL);
		g_td[i].tex = &g_Texture[i];
	}

	// �ڍאݒ�
	g_td[TEXTURE_BG].col = { 1.0f, 1.0f, 1.0f, 1.0f };


	g_Time = 0;

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitTeamLogo(void)
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
void UpdateTeamLogo(void)
{
	if (g_Time++ > TEAMLOGO_TIME) { g_Time = 0; SetFade(FADE_OUT, MODE_GAME); }
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawTeamLogo(void)
{
	SetDraw2DTexture();
	DrawTexture2D(&g_td[TEXTURE_BG]);

	SetDrawNoLighting();

	SetCullingMode(CULL_MODE_NONE);

	//XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	//for (int testNo = 0; testNo < MODEL_MAX; testNo++)
	//{
	//	// ���[���h�}�g���b�N�X�̏�����
	//	mtxWorld = XMMatrixIdentity();

	//	// �X�P�[���𔽉f
	//	mtxScl = XMMatrixScaling(g_Model[testNo].srt.scl.x, g_Model[testNo].srt.scl.y, g_Model[testNo].srt.scl.z);
	//	mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

	//	// ��]�𔽉f�F�S�̂̊p�x	
	//	mtxRot = XMMatrixRotationRollPitchYaw(g_Model[testNo].srt.rot.x, g_Model[testNo].srt.rot.y, g_Model[testNo].srt.rot.z);
	//	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

	//	// �ړ��𔽉f
	//	mtxTranslate = XMMatrixTranslation(g_Model[testNo].srt.pos.x, g_Model[testNo].srt.pos.y, g_Model[testNo].srt.pos.z);
	//	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

	//	// ���[���h�}�g���b�N�X�̐ݒ�
	//	SetWorldBuffer(&mtxWorld);

	//	// �}�e���A���ݒ�
	//	MATERIAL material;
	//	ZeroMemory(&material, sizeof(material));
	//	material.Diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };

	//	// ���f���`��
	//	if (testNo != MODEL_STAR)
	//	{
	//		DrawModel(&g_Model[testNo].model, NULL, &material);
	//	}
	//	else
	//	{
	//		DrawModel(&g_Model[testNo].model, &g_Texture[TEXTURE_STAR], &material);
	//	}
	//}

	SetCullingMode(CULL_MODE_BACK);
}
