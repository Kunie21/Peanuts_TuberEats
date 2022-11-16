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
	SOUND_LABEL_BGM_TITLE,			// �^�C�g��
	SOUND_LABEL_BGM_START,			// �X�^�[�g
	SOUND_LABEL_BGM_HOME,			// �z�[��
	SOUND_LABEL_BGM_STAGE_SELECT,	// �X�e�[�W�Z���N�g
	SOUND_LABEL_BGM_STAGE_1_1,		// �X�e�[�W1-1
	//SOUND_LABEL_BGM_STAGE_1_2,		// �X�e�[�W1-2
	//SOUND_LABEL_BGM_STAGE_1_3,		// �X�e�[�W1-3
	//SOUND_LABEL_BGM_STAGE_2_1,		// �X�e�[�W2-1
	//SOUND_LABEL_BGM_STAGE_2_2,		// �X�e�[�W2-2
	//SOUND_LABEL_BGM_STAGE_2_3,		// �X�e�[�W2-3
	//SOUND_LABEL_BGM_STAGE_3_1,		// �X�e�[�W3-1
	//SOUND_LABEL_BGM_STAGE_3_2,		// �X�e�[�W3-2
	//SOUND_LABEL_BGM_STAGE_3_3,		// �X�e�[�W3-3
	//SOUND_LABEL_BGM_STAGE_4_1,		// �X�e�[�W4-1
	//SOUND_LABEL_BGM_STAGE_4_2,		// �X�e�[�W4-2
	//SOUND_LABEL_BGM_STAGE_4_3,		// �X�e�[�W4-3
	SOUND_LABEL_BGM_RESULT,			// ���U���g
	SOUND_LABEL_BGM_ENDING,			// �G���f�B���O
	SOUND_LABEL_SE_SELECT,			// �I���
	SOUND_LABEL_SE_DECIDE,			// ���艹
	SOUND_LABEL_SE_OPENING,			// �I�[�v�j���OSE?
	SOUND_LABEL_SE_ENGINE,			// �G���W����
	SOUND_LABEL_SE_COLLISION,		// �Փˉ�
	//SOUND_LABEL_SE_PLAYERVOICE,		// �v���C���[�{�C�X
	SOUND_LABEL_SE_AIRLEAK,			// ��C�R�ꉹ
	SOUND_LABEL_SE_DOOR_OPEN,		// �h�A���J����
	SOUND_LABEL_SE_ACROSS_RING,		// �����O�ʉ߉�
	SOUND_LABEL_SE_STAR,			// ���̉�
	SOUND_LABEL_SE_POINT_ADD,		// �|�C���g���Z

	SOUND_LABEL_MAX,
};


// �t�F�[�h�̏��
typedef enum
{
	AUDIOFADE_NONE = 0,				// ����Ȃ����
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
	ECHO,			// �G�R�[
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
// ������:AUDIOFADE_OUT or AUDIOFADE_IN ��w��
// ������:�t�F�[�h������������x����w�肷�� GetAudioLabel()�ł����Ǝv���܂��B
//*****************************************************************************
void SetAudioFade(AUDIOFADE audiofade,int label);

// ���ݖ��Ă���I�[�f�B�I���x���ԍ���擾
int GetSoundLabel(void);
// �\�[�X�{�C�X�̉��ʒ���
void SetSourceVolume(int label, float volume);
// �\�[�X�{�C�X�̃{�C�X�̍Đ��s�b�`����
void SetFrequencyRatio(int label, float pitch);
// �\�[�X�{�C�X�̈ꎞ��~
void PauseSound(int label);
// �\�[�X�{�C�X�̍ĊJ
void ReStartSound(int label);