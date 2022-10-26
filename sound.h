//=============================================================================
//
// �T�E���h���� [sound.h]
// Author : ���� �D��
//
//=============================================================================
#pragma once

#include <windows.h>
#include "xaudio2.h"			// �T�E���h�����ŕK�v
#include "xaudio2fx.h"			// �T�E���h�̃G�t�F�N�g�����ŕK�v
#include <XAPOFX.h>				// �T�E���h�̃G�t�F�N�g�����ŕK�v

#pragma comment (lib, "xapofx.lib")


//*****************************************************************************
// �T�E���h�t�@�C��
//*****************************************************************************
enum 
{
	SOUND_LABEL_BGM_TITLE,			// 
	SOUND_LABEL_BGM_STAGESELECT,	// 
	SOUND_LABEL_BGM_TUTORIAL,		// 
	SOUND_LABEL_BGM_NORMAL,			// 
	SOUND_LABEL_BGM_ICE,			// 
	SOUND_LABEL_BGM_SAND,			// 
	SOUND_LABEL_BGM_MAGMA,			// 
	SOUND_LABEL_BGM_BONE,			// 
	SOUND_LABEL_BGM_SECRET,			// 
	SOUND_LABEL_BGM_RESULT,			// 
	SOUND_LABEL_BGM_ENDING,			// 
	SOUND_LABEL_SE_DECIDE,			// 
	SOUND_LABEL_SE_DECIDE_FROG,		// 
	SOUND_LABEL_SE_DECIDE_KIRA,		// 
	SOUND_LABEL_SE_DECIDE_KORO,		// 
	SOUND_LABEL_SE_DECIDE_GAO,		// 
	SOUND_LABEL_SE_BACK,			// 
	SOUND_LABEL_SE_CLEAR,			// 
	SOUND_LABEL_SE_SWIPE,			// 
	SOUND_LABEL_SE_BUBBLE,			// 
	SOUND_LABEL_SE_FALL,			// 
	SOUND_LABEL_SE_HOLD,			// 
	SOUND_LABEL_SE_INTODARK,		// 
	SOUND_LABEL_SE_INTOMAGMA,		// 
	SOUND_LABEL_SE_JUMP,			// 
	SOUND_LABEL_SE_MARVELOUS,		// 
	SOUND_LABEL_SE_GOAL,			// 
	SOUND_LABEL_SE_TIMEUP,			// 
	SOUND_LABEL_SE_MEDAL,			// 
	SOUND_LABEL_SE_SCORE,			// 
	SOUND_LABEL_SE_CHECKPOINT,		// 


	SOUND_LABEL_MAX,
};


// �t�F�[�h�̏��
typedef enum
{
	AUDIOFADE_NONE = 0,				// �����Ȃ����
	AUDIOFADE_IN,					// �t�F�[�h�C������
	AUDIOFADE_OUT,					// �t�F�[�h�A�E�g����
	AUDIOFADE_MAX
} AUDIOFADE;


// �g�p����T�E���h�̎��
enum {
	MASTER,
	BGM,
	SE,
	VOICE,
	SOUND_TYPE_MAX,				// �T�E���h��ނ�MAX��
};

// �g�p����G�t�F�N�g
typedef enum
{
	REVERB = 0,		// ���o�[�u
	ECHO,				// �G�R�[
	EQ,				// �C�R���C�U�[
	SOUND_EFFECT_MAX,				// �G�t�F�N�g��MAX��
} SOUND_EFFECT;


#define BGM_VOLUME (0.7f)			//BGM�̉���
#define MAX_VOLUME (1.6f)			//BGM��MAX����
#define MIN_VOLUME (0.0f)			//BGM��MIN����

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
BOOL InitSound(HWND hWnd);
void UninitSound(void);
void PlaySound(int label, float volume = -1.0f);
void StopSound(int label);
void StopSound(void);

void UpdateAudioFade(void);

BOOL LoadSoundKernel(float* loadPalam, int* loadSum);
BOOL LoadSound(void);

//*****************************************************************************
// ���ʂ̃t�F�[�h�C���E�A�E�g����
// ������:AUDIOFADE_OUT or AUDIOFADE_IN ���w��
// ������:�t�F�[�h�������������x�����w�肷�� GetAudioLabel()�ł����Ǝv���܂��B
//*****************************************************************************
void SetAudioFade(AUDIOFADE audiofade,int label);

// ���ݖ��Ă���I�[�f�B�I���x���ԍ����擾
int GetSoundLabel(void);
// �\�[�X�{�C�X�̉��ʒ���
void SetSourceVolume(int label, float volume);
// �\�[�X�{�C�X�̃{�C�X�̍Đ��s�b�`����
void SetFrequencyRatio(int label, float Pitch);
// �\�[�X�{�C�X�̈ꎞ��~
void PauseSound(int label);
// �\�[�X�{�C�X�̍ĊJ
void ReStartSound(int label);