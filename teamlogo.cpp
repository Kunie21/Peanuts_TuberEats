//=============================================================================
//
// �`�[�����S��ʏ��� [teamlogo.cpp]
// Author : ���] �đ�
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "fade.h"
#include "teamlogo.h"
#include "texture2d.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH)	// �w�i�T�C�Y��
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)	// �w�i�T�C�Y�c

#define TEXTURE_WIDTH_LOGO			(1154)			// ���S�T�C�Y��
#define TEXTURE_HEIGHT_LOGO			(693)			// ���S�T�C�Y�c

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
	TEXTURE_TEAMLOGO,
	TEXTURE_MAX,
};
static TEXTURE2D_DESC	g_td[TEXTURE_MAX];
static ID3D11ShaderResourceView*	g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����
static char*	g_TextureName[TEXTURE_MAX] = {
	"data/TEXTURE/white.png",
	"data/TEXTURE/peanuts.png",
};

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
	g_td[TEXTURE_TEAMLOGO].size = { TEXTURE_WIDTH_LOGO, TEXTURE_HEIGHT_LOGO };
	g_td[TEXTURE_TEAMLOGO].scl = { 0.5f, 0.5f };

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
	DrawTexture2D(&g_td[TEXTURE_BG]);
	DrawTexture2D(&g_td[TEXTURE_TEAMLOGO]);
}
