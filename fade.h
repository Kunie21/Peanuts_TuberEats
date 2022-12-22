//=============================================================================
//
// �t�F�[�h���� [fade.h]
// Author : ���] �đ�
//
//=============================================================================
#pragma once

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	FADE_COLOR		{0.0f, 0.0f, 0.0f}

// �t�F�[�h�̏��
typedef enum
{
	FADE_NONE = 0,		// �����Ȃ����
	FADE_IN,			// �t�F�[�h�C������
	FADE_OUT,			// �t�F�[�h�A�E�g����
	FADE_MAX
} FADE;

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitFade(void);
void UninitFade(void);
void UpdateFade(void);
void DrawFade(void);

void SetFade(FADE fade, MODE_LABEL modeNext, BOOL load = FALSE);
void SetFadeColor(XMFLOAT3 color = FADE_COLOR);
FADE GetFade(void);

BOOL CheckFadeIn(void);
BOOL CheckFadeOut(void);


