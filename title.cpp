////=============================================================================
////
//// �`�[�����S��ʏ��� [title.cpp]
//// Author : ���] �đ�
////
////=============================================================================
#include "main.h"
#include "renderer.h"
#include "texture2d.h"
#include "title.h"
#include "fade.h"
#include "model.h"

//
////*****************************************************************************
//// �}�N����`
////*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH)	// �w�i�T�C�Y��
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)	// �w�i�T�C�Y�c

//#define TEXTURE_WIDTH_LOGO			(1154)			// ���S�T�C�Y��
//#define TEXTURE_HEIGHT_LOGO			(693)			// ���S�T�C�Y�c
#define TEXTURE_WIDTH_LOGO			(984)			// ���S�T�C�Y��
#define TEXTURE_HEIGHT_LOGO			(590)			// ���S�T�C�Y�c

#define TEAMLOGO_TIME				(100)			// ���S�\������
//
////*****************************************************************************
//// �O���[�o���ϐ�
////*****************************************************************************
static int				g_Time = 0;
static BOOL				g_Load = FALSE;
//
//// �e�N�X�`���Ǘ�
enum
{
	TEXTURE_BG = 0,
	TEXTURE_TEAMLOGO,
	TEXTURE_STAR,
	TEXTURE_MAX,
};
static TEXTURE2D_DESC	g_td[TEXTURE_MAX];
static ID3D11ShaderResourceView*	g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����
static char*	g_TextureName[TEXTURE_MAX] = {
	"data/TEXTURE/white.png",
	"data/TEXTURE/peanuts_logo_color.png",
	"data/MODEL/star1.jpg",
	//"data/TEXTURE/peanuts_logo_white.png",
	//"data/TEXTURE/peanuts_bg_1.png",
	//"data/TEXTURE/peanuts.png",
};

enum {
	MODEL_STAR = 0,
	MODEL_EARTH,
	MODEL_ROCKET,
	MODEL_MAX,
};
static MODEL_DATA	g_Model[MODEL_MAX];	// �v���C���[�̃��f���Ǘ�


////=============================================================================
//// ����������
HRESULT InitTitle(void)
{
	// �e�N�X�`������
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		D3DX11CreateShaderResourceViewFromFile(GetDevice(), g_TextureName[i], NULL, NULL, &g_Texture[i], NULL);
		g_td[i].tex = &g_Texture[i];
	}

	// �ڍאݒ�
	//g_td[TEXTURE_BG].col = { 1.0f, 1.0f, 1.0f, 1.0f };

	//g_td[TEXTURE_TEAMLOGO].size = { TEXTURE_WIDTH_LOGO, TEXTURE_HEIGHT_LOGO };
	//g_td[TEXTURE_TEAMLOGO].scl = { 0.5f, 0.5f };

	LoadModel("data/MODEL/skydome.obj", &g_Model[MODEL_STAR].model);
	//LoadModel("data/MODEL/earth01.obj", &g_Model[MODEL_EARTH].model);
	//LoadModel("data/MODEL/rocket01.obj", &g_Model[MODEL_ROCKET].model);

	g_Model[MODEL_STAR].pos = { 0.0f, 0.0f, 0.0f };
	g_Model[MODEL_EARTH].pos = { 25.0f, 0.0f, 100.0f };
	g_Model[MODEL_STAR].scl = { 20.3f, 20.3f, 20.3f };
	g_Model[MODEL_EARTH].scl = { 0.3f, 0.3f, 0.3f };
	g_Model[MODEL_ROCKET].scl = { 0.01f, 0.01f, 0.01f };

	g_Time = 0;

	g_Load = TRUE;
	return S_OK;
}

////=============================================================================
//// �I������
////=============================================================================
void UninitTitle(void)
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

////=============================================================================
//// �X�V����
////=============================================================================
void UpdateTitle(void)
{
	g_Model[MODEL_EARTH].rot.y -= 0.01f;
}

////=============================================================================
//// �`�揈��
////=============================================================================
void DrawTitle(void)
{

	SetDrawNoLighting();

	SetCullingMode(CULL_MODE_NONE);

	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	for (int testNo = 0; testNo < MODEL_MAX; testNo++)
	{
		// ���[���h�}�g���b�N�X�̏�����
		mtxWorld = XMMatrixIdentity();

		// �X�P�[���𔽉f
		mtxScl = XMMatrixScaling(g_Model[testNo].scl.x, g_Model[testNo].scl.y, g_Model[testNo].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// ��]�𔽉f�F�S�̂̊p�x
		mtxRot = XMMatrixRotationRollPitchYaw(g_Model[testNo].rot.x, g_Model[testNo].rot.y, g_Model[testNo].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// �ړ��𔽉f
		mtxTranslate = XMMatrixTranslation(g_Model[testNo].pos.x, g_Model[testNo].pos.y, g_Model[testNo].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ���[���h�}�g���b�N�X�̐ݒ�
		SetWorldBuffer(&mtxWorld);

		// �}�e���A���ݒ�
		MATERIAL material;
		ZeroMemory(&material, sizeof(material));
		material.Diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };

		// ���f���`��
		if (testNo != MODEL_STAR)
		{
			DrawModel(&g_Model[testNo].model, NULL, &material);
		}
		else
		{
			DrawModel(&g_Model[testNo].model, &g_Texture[TEXTURE_STAR], &material);
		}
	}

	SetCullingMode(CULL_MODE_BACK);


}
