//=============================================================================
//
// �h�A���� [door.cpp]
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
#include "door.h"
#include "tube.h"
#include "gimmick.h"
#include "stage.h"
#include "player.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define DOOR_SPD	(20.0f)		// �J�����x
#define DOOR_OPN	(110.0f)	// �ǂ��܂ŊJ����
#define DOOR_SCL	(20.0f)		// �傫��
#define DOOR_POS	(250.0f)	// Z���W
#define DOOR_OFX	(0.3f)		// X���W�̏����ʒu����
#define DOOR_ROT	(0.0f)		// �΂߂ɂ���H
#define DOOR_SHK	(0.0f)		// �K�^�K�^������H

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static BOOL				g_Load = FALSE;

// �e�N�X�`���Ǘ�
enum {
	TEXTURE_TEAMLOGO = 0,
	//TEXTURE_STAR,
	TEXTURE_MAX,
};
static TEXTURE2D_DESC	g_td[TEXTURE_MAX];
static ID3D11ShaderResourceView*	g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����
static char*	g_TextureName[TEXTURE_MAX] = {
	"data/TEXTURE/blueberry_.png",
	//"data/MODEL/star1.jpg",
};

enum {
	MODEL_DOOR_LEFT = 0,
	MODEL_DOOR_RIGHT,
	MODEL_MAX,
};
static MODEL_DATA	g_Model[MODEL_MAX];	// �v���C���[�̃��f���Ǘ�


//=============================================================================
// ����������
//=============================================================================
HRESULT InitDoor(void)
{
	// �e�N�X�`������
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		D3DX11CreateShaderResourceViewFromFile(GetDevice(), g_TextureName[i], NULL, NULL, &g_Texture[i], NULL);
		g_td[i].tex = &g_Texture[i];
	}

	// ���f���ǂݍ���
	{
		LoadModel("data/MODEL/door_left.obj", &g_Model[MODEL_DOOR_LEFT].model);
		LoadModel("data/MODEL/door_right.obj", &g_Model[MODEL_DOOR_RIGHT].model);

		g_Model[MODEL_DOOR_LEFT].srt.pos = { DOOR_OFX, 0.0f, DOOR_POS };
		g_Model[MODEL_DOOR_LEFT].srt.rot = { DOOR_ROT, 0.0f, 0.0f };
		g_Model[MODEL_DOOR_LEFT].srt.scl = { DOOR_SCL, DOOR_SCL, DOOR_SCL };
		g_Model[MODEL_DOOR_RIGHT].srt.pos = { -DOOR_OFX, 0.0f, DOOR_POS };
		g_Model[MODEL_DOOR_RIGHT].srt.rot = { DOOR_ROT, 0.0f, 0.0f };
		g_Model[MODEL_DOOR_RIGHT].srt.scl = { DOOR_SCL, DOOR_SCL, DOOR_SCL };
	}

	// �ڍאݒ�
	//g_td[TEXTURE_TEAMLOGO].tex = &g_Texture[TEXTURE_TEAMLOGO];

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitDoor(void)
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
void UpdateDoor(void)
{
	static float door_open = 0.0f;
	if (door_open < DOOR_OPN) door_open += DOOR_SPD;
	if (GetKeyboardPress(DIK_K)) { door_open = 0.0f; }
	float d_pos = -GetPlayerPosition();
	g_Model[MODEL_DOOR_LEFT].srt.pos = { DOOR_OFX - door_open, (float)(rand() % 10) * DOOR_SHK, DOOR_POS + d_pos + (float)(rand() % 10) * DOOR_SHK };
	g_Model[MODEL_DOOR_RIGHT].srt.pos = { -DOOR_OFX + door_open, (float)(rand() % 10) * DOOR_SHK, DOOR_POS + d_pos + (float)(rand() % 10) * DOOR_SHK };
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawDoor(void) {
	MATERIAL material;
	DrawModel(&g_Model[MODEL_DOOR_LEFT].model, &g_Model[MODEL_DOOR_LEFT].srt, NULL, &material);	// ���f���`��
	DrawModel(&g_Model[MODEL_DOOR_RIGHT].model, &g_Model[MODEL_DOOR_RIGHT].srt, NULL, &material);	// ���f���`��
}