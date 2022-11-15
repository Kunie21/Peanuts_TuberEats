//=============================================================================
//
// �X�^�[�g�A�j���[�V�������� [anim_start.cpp]
// Author : ���] �đ�
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "texture2d.h"
#include "debugproc.h"
#include "model.h"
#include "fade.h"
#include "input.h"
#include "player.h"
#include "tube.h"
#include "ui_game.h"
#include "gimmick.h"
#include "stage.h"
#include "missile.h"
#include "anim_start.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define DEFAULT_SPEED	(40.0f)
//#define DEFAULT_POS		(310.0f)
#define MAX_SPEED		(70.0f)

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static BOOL				g_Load = FALSE;

// �e�N�X�`���Ǘ�
enum {
	TEXTURE_NO3 = 0,
	TEXTURE_NO2,
	TEXTURE_NO1,
	TEXTURE_GO,
	TEXTURE_MAX,
};
static TEXTURE2D_DESC	g_td[TEXTURE_MAX];
static ID3D11ShaderResourceView*	g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����
static char*	g_TextureName[TEXTURE_MAX] = {
	"data/TEXTURE/blueberry_.png",
	//"data/MODEL/star1.jpg",
};

enum {
	MODEL_ROCKET1 = 0,
	MODEL_ROCKET2,
	MODEL_ROCKET3,
	MODEL_ROCKET4,
	MODEL_ROCKET5,
	MODEL_FIRE,
	MODEL_MAX,
};
static MODEL_DATA	g_Model[MODEL_MAX];	// �v���C���[�̃��f���Ǘ�



//=============================================================================
// ����������
//=============================================================================
HRESULT InitAnimStart(void)
{
	// �e�N�X�`������
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		D3DX11CreateShaderResourceViewFromFile(GetDevice(), g_TextureName[i], NULL, NULL, &g_Texture[i], NULL);
		g_td[i].tex = &g_Texture[i];
	}

	//for (int i = 0; i < MODEL_MAX; i++)
	{
		//LoadModel("data/MODEL/earth01.obj", &g_Model[0].model);
		LoadModel("data/MODEL/rocket01.obj", &g_Model[0].model);
		LoadModel("data/MODEL/rocket02.obj", &g_Model[1].model);
		LoadModel("data/MODEL/rocket03.obj", &g_Model[2].model);
		LoadModel("data/MODEL/rocket04.obj", &g_Model[3].model);
		LoadModel("data/MODEL/rocket05.obj", &g_Model[4].model);
		LoadModel("data/MODEL/fire01.obj", &g_Model[5].model);
		for (int i = 0; i < MODEL_MAX; i++) {
			g_Model[i].srt.pos = { 0.0f, -60.0f, 0.0f };
			g_Model[i].srt.rot = { XM_PI, 0.0f, XM_PI };
			g_Model[i].srt.scl = { 0.3f, 0.3f, 0.3f };
		}
		g_Model[MODEL_FIRE].srt.pos.z = -30.0f;
	}

	// �ڍאݒ�
	//g_td[TEXTURE_TEAMLOGO].tex = &g_Texture[TEXTURE_TEAMLOGO];

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitAnimStart(void)
{
	if (g_Load == FALSE) return;

	for (int i = 0; i < TEXTURE_MAX; i++) {
		if (g_Texture[i]) {
			g_Texture[i]->Release();
			g_Texture[i] = NULL;
		}
	}

	for (int i = 0; i < MODEL_MAX; i++) { UnloadModel(&g_Model[i].model); }

	g_Load = FALSE;
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateAnimStart(void)
{
	
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawAnimStart(void) {

}