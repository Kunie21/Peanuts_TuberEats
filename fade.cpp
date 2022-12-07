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
//#define SHUTTER			(60)

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static FADE				g_Fade = FADE_IN;	// �t�F�[�h�̏��
static MODE_LABEL		g_ModeNext;			// ���̃��[�h
static TEXTURE2D_DESC	g_td;
#ifdef SHUTTER
static TEXTURE2D_DESC	g_td2;
#endif
static BOOL				g_Load = FALSE;

//=============================================================================
// ����������
//=============================================================================
HRESULT InitFade(void)
{
#ifdef SHUTTER
	g_td.tex = TEXTURE_LABEL_TEAMLOGO;
	g_td.ctrType = CENTER_BOTTOM;
	g_td.posType = POSITION_TOP;
	g_td.scl.y = 0.5f;
	g_td.uv_pos.vh = 0.5f;

	g_td2.tex = TEXTURE_LABEL_TEAMLOGO;
	g_td2.ctrType = CENTER_TOP;
	g_td2.posType = POSITION_BOTTOM;
	g_td2.scl.y = 0.5f;
	g_td2.uv_pos.v = 0.5f;
	g_td2.uv_pos.vh = 0.5f;
#else
	g_td.tex = TEXTURE_LABEL_WHITE;
	//g_td.col = { 0.0f, 0.0f, 0.0f, 1.0f };
#endif

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
#ifdef SHUTTER
			g_td.pos.y += g_td.size.y * FADE_RATE;
			if (g_td.pos.y >= g_td.size.y * 0.75f)
			{
				g_td.pos.y = g_td.size.y * 0.75f;
				static int wait = 0;
				++wait;
				if (wait < SHUTTER) return;
				else wait = 0;
#else
			g_td.col.w += FADE_RATE;		// ���l�����Z���ĉ�ʂ������Ă���
			if (g_td.col.w >= 1.0f)
			{
				g_td.col.w = 1.0f;
#endif
				// ���Ă���Ȃ�S���~�߂�
				//StopSound();

				// �t�F�[�h�C�������ɐ؂�ւ�
				SetFade(FADE_IN, g_ModeNext);

				// ���[�h��ݒ�
				SetMode(g_ModeNext);
			}
		}
		else if (g_Fade == FADE_IN)
		{// �t�F�[�h�C������
#ifdef SHUTTER
			g_td.pos.y -= g_td.size.y * FADE_RATE;
			if (g_td.pos.y <= 0.0f)
			{
				g_td.pos.y = 0.0f;
				static int wait = 0;
				++wait;
				if (wait < SHUTTER) return;
				else wait = 0;
#else
			g_td.col.w -= FADE_RATE;		// ���l�����Z���ĉ�ʂ𕂂��オ�点��
			if (g_td.col.w <= 0.0f)
			{
				g_td.col.w = 0.0f;
#endif
				// �t�F�[�h�����I��
				SetFade(FADE_NONE, g_ModeNext);
			}
		}
	}
}

BOOL CheckFadeIn(void)
{
#ifdef SHUTTER
	if (g_td.pos.y <= 0.0f)
#else
	if (g_td.col.w <= 0.0f)
#endif
		return TRUE;
	return FALSE;
}
BOOL CheckFadeOut(void)
{
#ifdef SHUTTER
	if (g_td.pos.y <= 0.0f)
#else
	if (g_td.col.w <= 0.0f)
#endif
		return TRUE;
	return FALSE;
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawFade(void)
{
	if (g_Fade == FADE_NONE) return;	// �t�F�[�h���Ȃ��̂Ȃ�`�悵�Ȃ�

#ifdef SHUTTER
	g_td2.pos.y = -g_td.pos.y;
	DrawTexture2D(&g_td, FALSE, TRUE);
	DrawTexture2D(&g_td2, FALSE, TRUE);
#else
	DrawTexture2D(&g_td);
#endif
}


//=============================================================================
// �t�F�[�h�̏�Ԑݒ�
//=============================================================================
void SetFade(FADE fade, MODE_LABEL modeNext)
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



