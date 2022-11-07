//=============================================================================
//
// �t�F�[�h���� [fade.cpp]
// Author : ���] �đ�
//
//=============================================================================
#include "main.h"
#include "game.h"
#include "renderer.h"
#include "texture2d.h"
#include "fade.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	FADE_RATE		(0.02f)			// �t�F�[�h�W��

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************

// �e�N�X�`���Ǘ�
static TEXTURE2D_DESC	g_td;
static ID3D11ShaderResourceView*	g_Texture = NULL;	// �e�N�X�`�����
static char*	g_TextureName = "data/TEXTURE/white.png";

FADE			g_Fade = FADE_IN;	// �t�F�[�h�̏��
int				g_ModeNext;			// ���̃��[�h

static BOOL		g_Load = FALSE;

//=============================================================================
// ����������
//=============================================================================
HRESULT InitFade(void)
{
	// �e�N�X�`������
	D3DX11CreateShaderResourceViewFromFile(GetDevice(), g_TextureName, NULL, NULL, &g_Texture, NULL);
	g_td.tex = &g_Texture;
	g_td.col.w = 1.0f;

	g_Fade = FADE_IN;

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitFade(void)
{
	if (g_Load == FALSE) return;

	if (g_Texture)
	{
		g_Texture->Release();
		g_Texture = NULL;
	}

	g_Load = FALSE;
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateFade(void)
{
	if (g_Fade != FADE_NONE)
	{// �t�F�[�h������
		if (g_Fade == FADE_OUT)
		{// �t�F�[�h�A�E�g����
			g_td.col.w += FADE_RATE;		// ���l����Z���ĉ�ʂ�����Ă���
			if (g_td.col.w >= 1.0f)
			{
				// �t�F�[�h�C�������ɐ؂�ւ�
				g_td.col.w = 1.0f;
				SetFade(FADE_IN, g_ModeNext);
			}
		}
		else if (g_Fade == FADE_IN)
		{// �t�F�[�h�C������
			g_td.col.w -= FADE_RATE;		// ���l����Z���ĉ�ʂ𕂂��オ�点��
			if (g_td.col.w <= 0.0f)
			{
				// �t�F�[�h�����I��
				g_td.col.w = 0.0f;
				SetFade(FADE_NONE, g_ModeNext);
			}
		}
	}
}

BOOL CheckFadeIn(void)
{
	if (g_td.col.w <= 0.0f)
		return TRUE;
	return FALSE;
}
BOOL CheckFadeOut(void)
{
	if (g_td.col.w <= 0.0f)
		return TRUE;
	return FALSE;
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawFade(void)
{
	if (g_Fade == FADE_NONE) return;	// �t�F�[�h���Ȃ��̂Ȃ�`�悵�Ȃ�

	DrawTexture2D(&g_td);
}


//=============================================================================
// �t�F�[�h�̏�Ԑݒ�
//=============================================================================
void SetFade(FADE fade, int modeNext)
{
	g_Fade = fade;
	g_ModeNext = modeNext;
}

//=============================================================================
// �t�F�[�h�̏�Ԏ擾
//=============================================================================
FADE GetFade(void)
{
	return g_Fade;
}



