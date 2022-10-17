//=============================================================================
//
// �v���C���[���� [player.cpp]
// Author : ���] �đ�
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "texture2d.h"
#include "fade.h"
#include "input.h"
#include "player.h"
#include "tube.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH)	// �w�i�T�C�Y��

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static int				g_Time = 0;
static BOOL				g_Load = FALSE;

// �e�N�X�`���Ǘ�
enum
{
	TEXTURE_TEAMLOGO = 0,
	TEXTURE_MAX,
};
static TEXTURE2D_DESC	g_td[TEXTURE_MAX];
static ID3D11ShaderResourceView*	g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����
static char*	g_TextureName[TEXTURE_MAX] = {
	"data/TEXTURE/blueberry_.png",
};

//=============================================================================
// ����������
//=============================================================================
HRESULT InitPlayer(void)
{
	// �e�N�X�`������
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		D3DX11CreateShaderResourceViewFromFile(GetDevice(), g_TextureName[i], NULL, NULL, &g_Texture[i], NULL);
		g_td[i].tex = &g_Texture[i];
	}

	// �ڍאݒ�
	//g_td[TEXTURE_TEAMLOGO].tex = &g_Texture[TEXTURE_TEAMLOGO];

	g_Time = 0;

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitPlayer(void)
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
void UpdatePlayer(void)
{
	if (GetKeyboardPress(DIK_A))
	{
		TubeRotation(0.05f);
	}
	if (GetKeyboardPress(DIK_D))
	{
		TubeRotation(-0.05f);
	}
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawPlayer(void)
{
	//DrawTexture2D(&g_td[TEXTURE_TEAMLOGO]);
}
