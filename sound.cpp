//=============================================================================
//
// �T�E���h���� [sound.cpp]
// Author : ���� �D��
//
//=============================================================================
#include "main.h"
#include "sound.h"
#include "debugproc.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	AUDIO_FADEOUT_SPEED		(0.01f)			// �I�[�f�B�I�t�F�[�h�C���X�s�[�h
#define	AUDIO_FADEIN_SPEED		(0.01f)			// �I�[�f�B�I�t�F�[�h�A�E�g�X�s�[�h

#define NO_AUDIO	// �T�E���h����(��������΃��[�h�͂���)

//*****************************************************************************
// �p�����[�^�\���̒�`
//*****************************************************************************
typedef struct
{
	char *pFilename;	// �t�@�C����
	int nCntLoop;		// ���[�v�J�E���g
	//BOOL UseFilter;		// �G�t�F�N�g�g�����g��Ȃ����H�H
	//int type;			// �T�E���h�̎��

} SOUNDPARAM;

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT CheckChunk(HANDLE hFile, DWORD format, DWORD *pChunkSize, DWORD *pChunkDataPosition);
HRESULT ReadChunkData(HANDLE hFile, void *pBuffer, DWORD dwBuffersize, DWORD dwBufferoffset);


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
IXAudio2 *g_pXAudio2 = NULL;								// XAudio2�I�u�W�F�N�g�ւ̃C���^�[�t�F�C�X
IXAudio2MasteringVoice *g_pMasteringVoice = NULL;			// �}�X�^�[�{�C�X


// �T�u�~�b�N�X�{�C�X�̍쐬
IXAudio2SubmixVoice *g_apDrySubmixVoice;					//�T�u�~�b�N�X�{�C�X(�G�t�F�N�g�Ȃ�)
IXAudio2SubmixVoice *g_apWetSubmixVoice;					//�T�u�~�b�N�X�{�C�X(�G�t�F�N�g����)

IXAudio2SourceVoice *g_apSourceVoice[SOUND_LABEL_MAX] = {};	// �\�[�X�{�C�X
BYTE *g_apDataAudio[SOUND_LABEL_MAX] = {};					// �I�[�f�B�I�f�[�^
DWORD g_aSizeAudio[SOUND_LABEL_MAX] = {};					// �I�[�f�B�I�f�[�^�T�C�Y

AUDIOFADE g_AudioFade = AUDIOFADE_IN;						// �t�F�[�h�̏��
float g_VolumeSound = 1.0f;									// ����
int g_Label = 0;											// �I�[�f�B�I���x���ԍ��̏�����

//���ʊǗ�
float				g_VolParam[SOUND_TYPE_MAX];

static int	g_LoadPoint = 0;


//*****************************************************************************
// �e���f�ނ̃p�����[�^
//*****************************************************************************
SOUNDPARAM g_aParam[SOUND_LABEL_MAX] =
{
	{ (char*)"data/BGM/title.wav", -1 },			// �^�C�g��
	{ (char*)"data/BGM/STAGESELECT.wav", -1 },		// �X�^�[�g
	{ (char*)"data/BGM/home.wav", -1 },				// �z�[��
	{ (char*)"data/BGM/stage_select.wav", -1 },		// �X�e�[�W�Z���N�g
	{ (char*)"data/BGM/stage_1_1.wav", -1 },		// �X�e�[�W1-1
	//{ (char*)"data/BGM/stage_1_2.wav", -1 },		// �X�e�[�W1-2
	//{ (char*)"data/BGM/stage_1_3.wav", -1 },		// �X�e�[�W1-3
	//{ (char*)"data/BGM/stage_2_1.wav", -1 },		// �X�e�[�W2-1
	//{ (char*)"data/BGM/stage_2_2.wav", -1 },		// �X�e�[�W2-2
	//{ (char*)"data/BGM/stage_2_3.wav", -1 },		// �X�e�[�W2-3
	//{ (char*)"data/BGM/stage_3_1.wav", -1 },		// �X�e�[�W3-1
	//{ (char*)"data/BGM/stage_3_2.wav", -1 },		// �X�e�[�W3-2
	//{ (char*)"data/BGM/stage_3_3.wav", -1 },		// �X�e�[�W3-3
	//{ (char*)"data/BGM/stage_4_1.wav", -1 },		// �X�e�[�W4-1
	//{ (char*)"data/BGM/stage_4_2.wav", -1 },		// �X�e�[�W4-2
	//{ (char*)"data/BGM/stage_4_3.wav", -1 },		// �X�e�[�W4-3
	{ (char*)"data/BGM/result.wav", 0 },			// ���U���g
	{ (char*)"data/BGM/ending.wav", 0 },			// �G���f�B���O
	{ (char*)"data/SE/select.wav", 0 },			// �I���
	{ (char*)"data/SE/decide.wav", 0 },			// ���艹
	{ (char*)"data/SE/decide.wav", 0 },			// �I�[�v�j���OSE?
	{ (char*)"data/SE/engine.wav", 0 },			// �G���W����
	{ (char*)"data/SE/collision.wav", 0 },		// �Փˉ�
	//{ (char*)"data/SE/INTOMAGMA.wav", 0 },		// �v���C���[�{�C�X
	{ (char*)"data/SE/airleak.wav", 0 },		// ��C�R�ꉹ
	{ (char*)"data/SE/door_open.wav", 0 },		// �h�A���J����
	{ (char*)"data/SE/across_ring.wav", 0 },	// �����O�ʉ߉�
	{ (char*)"data/SE/star.wav", 0 },			// ���̉�
	{ (char*)"data/SE/point_add.wav", 0 },		// �|�C���g���Z
};

//*****************************************************************************
// �G�t�F�N�g�֘A
//*****************************************************************************
//���o�[�u
IUnknown *g_apXPO_Reverb;
XAUDIO2_EFFECT_DESCRIPTOR g_Descriptior[SOUND_EFFECT_MAX];
XAUDIO2_EFFECT_CHAIN g_Chain;

//���o�[�u�̃p�����[�^
FXECHO_PARAMETERS g_ReverbParam;
XAUDIO2FX_REVERB_I3DL2_PARAMETERS g_i3dl2Param = XAUDIO2FX_I3DL2_PRESET_BATHROOM;
XAUDIO2FX_REVERB_PARAMETERS g_reverbParam;

//�G�R�[
IUnknown *g_apXPO_Echo;

//�G�R�[�̃p�����[�^
FXECHO_PARAMETERS g_EchoParam;

//EQ
IUnknown *g_apXPO_EQ;

//EQ�̃p�����[�^
FXEQ_PARAMETERS g_EQParam;

////LowPass
//XAUDIO2_FILTER_PARAMETERS FilterParams;

// �}�X�^�����O���~�b�^�[����

//=============================================================================
// ����������
//=============================================================================
BOOL InitSound(HWND hWnd)
{
#ifdef NO_AUDIO
	return TRUE;
#endif

	HRESULT hr;

	// COM���C�u�����̏�����
	CoInitializeEx(NULL, COINIT_MULTITHREADED);

	// XAudio2�I�u�W�F�N�g�̍쐬
	hr = XAudio2Create(&g_pXAudio2, 0);
	if (FAILED(hr))
	{
		MessageBox(hWnd, "XAudio2�I�u�W�F�N�g�̍쐬�Ɏ��s�I", "�x���I", MB_ICONWARNING);

		// COM���C�u�����̏I������
		CoUninitialize();

		return FALSE;
	}

	// �}�X�^�[�{�C�X�̐���
	hr = g_pXAudio2->CreateMasteringVoice(&g_pMasteringVoice);
	if (FAILED(hr))
	{
		MessageBox(hWnd, "�}�X�^�[�{�C�X�̐����Ɏ��s�I", "�x���I", MB_ICONWARNING);

		if (g_pXAudio2)
		{
			// XAudio2�I�u�W�F�N�g�̊J��
			g_pXAudio2->Release();
			g_pXAudio2 = NULL;
		}

		// COM���C�u�����̏I������
		CoUninitialize();

		return FALSE;
	}

	//�T�u�~�b�N�X�{�C�X�i�G�t�F�N�g����j�̍쐬
	hr = g_pXAudio2->CreateSubmixVoice(&g_apWetSubmixVoice, 2, 48000);
	if (FAILED(hr))
	{
		MessageBox(hWnd, "�T�u�~�b�N�X�i�G�t�F�N�g����j�̐����Ɏ��s�I", "�x���I", MB_ICONWARNING);
		return FALSE;
	}

	//�T�u�~�b�N�X�{�C�X�i�G�t�F�N�g�Ȃ��j�̍쐬
	hr = g_pXAudio2->CreateSubmixVoice(&g_apDrySubmixVoice, 2, 48000);
	if (FAILED(hr))
	{
		MessageBox(hWnd, "�T�u�~�b�N�X�i�G�t�F�N�g�Ȃ��j�̐����Ɏ��s�I", "�x���I", MB_ICONWARNING);
		return FALSE;
	}

	g_LoadPoint = 0;

	return TRUE;
}

//=============================================================================
// �f�[�^����
//=============================================================================
BOOL LoadSoundKernel(float* loadPalam, int* loadSum)
{
#ifdef NO_AUDIO
	return TRUE;
#endif

	if (g_LoadPoint < SOUND_LABEL_MAX)
	{
		if(LoadSound())
		{
			g_LoadPoint++;
			*loadSum = *loadSum + 1;
		}
		if (g_LoadPoint == SOUND_LABEL_MAX)
		{
			*loadPalam = 0.0f;
			return TRUE;
		}
	}
	*loadPalam = (float)g_LoadPoint / (float)SOUND_LABEL_MAX;
	return FALSE;
}

//=============================================================================
// �T�E���h�f�[�^����
//=============================================================================
BOOL LoadSound(void)
{
#ifdef NO_AUDIO
	return TRUE;
#endif

	HWND hWnd = GetWindowHandle();

	HRESULT hr;

	// �T�E���h�f�[�^�̏�����
	HANDLE hFile;
	DWORD dwChunkSize = 0;
	DWORD dwChunkPosition = 0;
	DWORD dwFiletype;
	WAVEFORMATEXTENSIBLE wfx;
	XAUDIO2_BUFFER buffer;


	// �o�b�t�@�̃N���A
	memset(&wfx, 0, sizeof(WAVEFORMATEXTENSIBLE));
	memset(&buffer, 0, sizeof(XAUDIO2_BUFFER));

	// �T�E���h�f�[�^�t�@�C���̐���
	hFile = CreateFile(g_aParam[g_LoadPoint].pFilename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		MessageBox(hWnd, "�T�E���h�f�[�^�t�@�C���̐����Ɏ��s�I(1)", "�x���I", MB_ICONWARNING);
		return FALSE;
	}
	if (SetFilePointer(hFile, 0, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
	{// �t�@�C���|�C���^��擪�Ɉړ�
		MessageBox(hWnd, "�T�E���h�f�[�^�t�@�C���̐����Ɏ��s�I(2)", "�x���I", MB_ICONWARNING);
		return FALSE;
	}

	// WAVE�t�@�C���̃`�F�b�N
	hr = CheckChunk(hFile, 'FFIR', &dwChunkSize, &dwChunkPosition);
	if (FAILED(hr))
	{
		MessageBox(hWnd, "WAVE�t�@�C���̃`�F�b�N�Ɏ��s�I(1)", "�x���I", MB_ICONWARNING);
		return FALSE;
	}
	hr = ReadChunkData(hFile, &dwFiletype, sizeof(DWORD), dwChunkPosition);
	if (FAILED(hr))
	{
		MessageBox(hWnd, "WAVE�t�@�C���̃`�F�b�N�Ɏ��s�I(2)", "�x���I", MB_ICONWARNING);
		return FALSE;
	}
	if (dwFiletype != 'EVAW')
	{
		MessageBox(hWnd, "WAVE�t�@�C���̃`�F�b�N�Ɏ��s�I(3)", "�x���I", MB_ICONWARNING);
		return FALSE;
	}

	// �t�H�[�}�b�g�`�F�b�N
	hr = CheckChunk(hFile, ' tmf', &dwChunkSize, &dwChunkPosition);
	if (FAILED(hr))
	{
		MessageBox(hWnd, "�t�H�[�}�b�g�`�F�b�N�Ɏ��s�I(1)", "�x���I", MB_ICONWARNING);
		return FALSE;
	}
	hr = ReadChunkData(hFile, &wfx, dwChunkSize, dwChunkPosition);
	if (FAILED(hr))
	{
		MessageBox(hWnd, "�t�H�[�}�b�g�`�F�b�N�Ɏ��s�I(2)", "�x���I", MB_ICONWARNING);
		return FALSE;
	}

	// �I�[�f�B�I�f�[�^�ǂݍ���
	hr = CheckChunk(hFile, 'atad', &g_aSizeAudio[g_LoadPoint], &dwChunkPosition);
	if (FAILED(hr))
	{
		MessageBox(hWnd, "�I�[�f�B�I�f�[�^�ǂݍ��݂Ɏ��s�I(1)", "�x���I", MB_ICONWARNING);
		return FALSE;
	}
	g_apDataAudio[g_LoadPoint] = (BYTE*)malloc(g_aSizeAudio[g_LoadPoint]);
	hr = ReadChunkData(hFile, g_apDataAudio[g_LoadPoint], g_aSizeAudio[g_LoadPoint], dwChunkPosition);
	if (FAILED(hr))
	{
		MessageBox(hWnd, "�I�[�f�B�I�f�[�^�ǂݍ��݂Ɏ��s�I(2)", "�x���I", MB_ICONWARNING);
		return FALSE;
	}

	// �\�[�X�{�C�X�̐���
	hr = g_pXAudio2->CreateSourceVoice(&g_apSourceVoice[g_LoadPoint], &(wfx.Format), XAUDIO2_VOICE_USEFILTER, 16.0f);
	if (FAILED(hr))
	{
		MessageBox(hWnd, "�\�[�X�{�C�X�̐����Ɏ��s�I", "�x���I", MB_ICONWARNING);
		return FALSE;
	}

	// �o�b�t�@�̒l�ݒ�
	memset(&buffer, 0, sizeof(XAUDIO2_BUFFER));
	buffer.AudioBytes = g_aSizeAudio[g_LoadPoint];
	buffer.pAudioData = g_apDataAudio[g_LoadPoint];
	buffer.Flags = XAUDIO2_END_OF_STREAM;
	buffer.LoopCount = g_aParam[g_LoadPoint].nCntLoop;

	// �I�[�f�B�I�o�b�t�@�̓o�^
	g_apSourceVoice[g_LoadPoint]->SubmitSourceBuffer(&buffer);

	////�G�t�F�N�g�̐���
	//{
	//	//���o�[�u�̐���
	//	XAudio2CreateReverb(&g_apXPO_Reverb);

	//	//EFFECT_DESCRIPTOR(���o�[�u)�̏�����
	//	g_Descriptior[REVERB].InitialState = FALSE;			//�L����Ԃ�
	//	g_Descriptior[REVERB].OutputChannels = 2;				//2ch�̃G�t�F�N�g
	//	g_Descriptior[REVERB].pEffect = g_apXPO_Reverb;		//�G�t�F�N�g�{��

	//	//�G�R�[�̐���
	//	CreateFX(_uuidof(FXEcho), &g_apXPO_Echo);

	//	//EFFECT_DESCRIPTOR(�G�R�[)�̏�����
	//	g_Descriptior[ECHO].InitialState = FALSE;				//������Ԃ�
	//	g_Descriptior[ECHO].OutputChannels = 2;				//2ch�̃G�t�F�N�g
	//	g_Descriptior[ECHO].pEffect = g_apXPO_Echo;			//�G�t�F�N�g�{��

	//	//EQ�̐���
	//	CreateFX(_uuidof(FXEQ), &g_apXPO_EQ);

	//	//EFFECT_DESCRIPTOR(EQ)�̏�����
	//	g_Descriptior[EQ].InitialState = FALSE;				//�L����Ԃ�
	//	g_Descriptior[EQ].OutputChannels = 2;					//2ch�̃G�t�F�N�g
	//	g_Descriptior[EQ].pEffect = g_apXPO_EQ;				//�G�t�F�N�g�{��

	//	//EFFECT_CHAIN�̍쐬
	//	g_Chain.EffectCount = SOUND_EFFECT_MAX;								//�}���̂�SOUND_EFFECT_MAX��
	//	g_Chain.pEffectDescriptors = g_Descriptior;							//�������̍\���̂�w��

	//	//�{�C�X��EFFECT_CHAIN��}��
	//	g_apWetSubmixVoice->SetEffectChain(&g_Chain);

	//	//Release
	//	g_apXPO_Reverb->Release();
	//	g_apXPO_Echo->Release();
	//	g_apXPO_EQ->Release();
	//}


	////�p�����[�^�[�̏�����
	//{
	//	// ���o�[�u
	//	// I3DL2_REVERB_PARAM �ϊ�
	//	ReverbConvertI3DL2ToNative(&g_i3dl2Param, &g_reverbParam);

	//	//�G�t�F�N�g�ɒʒm����
	//	g_apWetSubmixVoice->SetEffectParameters(REVERB, &g_reverbParam, sizeof(g_reverbParam));

	//	// �G�R�[
	//	g_EchoParam.WetDryMix = FXECHO_DEFAULT_WETDRYMIX;
	//	g_EchoParam.Delay = FXECHO_DEFAULT_DELAY;
	//	g_EchoParam.Feedback = FXECHO_DEFAULT_FEEDBACK;

	//	//�G�t�F�N�g�ɒʒm����
	//	g_apWetSubmixVoice->SetEffectParameters(ECHO, &g_EchoParam, sizeof(g_EchoParam));

	//	//EQ
	//	g_EQParam.FrequencyCenter0 = FXEQ_DEFAULT_FREQUENCY_CENTER_0;
	//	g_EQParam.FrequencyCenter1 = FXEQ_DEFAULT_FREQUENCY_CENTER_1;
	//	g_EQParam.FrequencyCenter2 = FXEQ_DEFAULT_FREQUENCY_CENTER_2;
	//	g_EQParam.FrequencyCenter3 = FXEQ_DEFAULT_FREQUENCY_CENTER_3;

	//	g_EQParam.Bandwidth0 = FXEQ_DEFAULT_BANDWIDTH;
	//	g_EQParam.Bandwidth1 = FXEQ_DEFAULT_BANDWIDTH;
	//	g_EQParam.Bandwidth2 = FXEQ_DEFAULT_BANDWIDTH;
	//	g_EQParam.Bandwidth3 = FXEQ_DEFAULT_BANDWIDTH;

	//	g_EQParam.Gain0 = FXEQ_DEFAULT_GAIN;
	//	g_EQParam.Gain1 = FXEQ_DEFAULT_GAIN;
	//	g_EQParam.Gain2 = FXEQ_DEFAULT_GAIN;
	//	g_EQParam.Gain3 = FXEQ_DEFAULT_GAIN;

	//	//�G�t�F�N�g�ɒʒm����
	//	g_apWetSubmixVoice->SetEffectParameters(EQ, &g_EQParam, sizeof(g_EQParam));
	//}


	////���ʂ̏�����
	//{
	//	g_VolParam[MASTER] = 1.0f;
	//	g_VolParam[BGM] = 1.0f;
	//	g_VolParam[SE] = 1.0f;
	//	g_VolParam[VOICE] = 1.0f;
	//}



	return TRUE;
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateAudioFade(void)
{
#ifdef NO_AUDIO
	return;
#endif

	if (g_AudioFade != AUDIOFADE_NONE)
	{// ���ʃt�F�[�h������
		if (g_AudioFade == AUDIOFADE_OUT)
		{// ���ʃt�F�[�h�A�E�g����
			g_VolumeSound -= AUDIO_FADEOUT_SPEED;					// �{�����[���l����Z���ĉ��ʂ���X�ɏグ�Ă���
			if (g_VolumeSound <= 0.0f)
			{
				g_VolumeSound = 0.0f;
				g_AudioFade = AUDIOFADE_NONE;
			}
			g_apSourceVoice[g_Label]->SetVolume(g_VolumeSound);		// ���ʂ�ς��Ă���
		}
		else if (g_AudioFade == AUDIOFADE_IN)
		{// �t�F�[�h�C������
			g_VolumeSound += AUDIO_FADEIN_SPEED;					// �{�����[���l����Z���ĉ��ʂ���X�ɉ����Ă���
			if (g_VolumeSound >= 1.0f)
			{
				// �t�F�[�h�����I��
				g_VolumeSound = 1.0f;
				g_AudioFade = AUDIOFADE_NONE;
			}
			g_apSourceVoice[g_Label]->SetVolume(g_VolumeSound);		// ���ʂ�ς��Ă���
		}
	}
	PrintDebugProc("����: %f\n", g_VolumeSound);
	PrintDebugProc("�T�E���h�ԍ�: %d\n", g_Label);
}

//=============================================================================
// �I������
//=============================================================================
void UninitSound(void)
{
#ifdef NO_AUDIO
	return;
#endif

	// �ꎞ��~
	for (int nCntSound = 0; nCntSound < SOUND_LABEL_MAX; nCntSound++)
	{
		if (g_apSourceVoice[nCntSound])
		{
			// �ꎞ��~
			g_apSourceVoice[nCntSound]->Stop(0);

			// �\�[�X�{�C�X�̔j��
			g_apSourceVoice[nCntSound]->DestroyVoice();
			g_apSourceVoice[nCntSound] = NULL;

			// �I�[�f�B�I�f�[�^�̊J��
			free(g_apDataAudio[nCntSound]);
			g_apDataAudio[nCntSound] = NULL;
		}
	}

	// �T�u�~�b�N�X�{�C�X�̔j��
	g_apWetSubmixVoice->DestroyVoice();
	g_apDrySubmixVoice->DestroyVoice();

	// �}�X�^�[�{�C�X�̔j��
	g_pMasteringVoice->DestroyVoice();
	g_pMasteringVoice = NULL;

	if (g_pXAudio2)
	{
		// XAudio2�I�u�W�F�N�g�̊J��
		g_pXAudio2->Release();
		g_pXAudio2 = NULL;
	}

	// COM���C�u�����̏I������
	CoUninitialize();
}

//=============================================================================
// �Z�O�����g�Đ�(�Đ����Ȃ��~)
//=============================================================================
void PlaySound(int label, float volume)
{
#ifdef NO_AUDIO
	return;
#endif

	//if (g_aParam[label].type == BGM)
	{
		g_Label = label;					// ���ݖ炵�Ă���BGM���x���ԍ���ۑ�
	}

	XAUDIO2_VOICE_STATE xa2state;
	XAUDIO2_BUFFER buffer;

	// �o�b�t�@�̒l�ݒ�
	memset(&buffer, 0, sizeof(XAUDIO2_BUFFER));
	buffer.AudioBytes = g_aSizeAudio[label];
	buffer.pAudioData = g_apDataAudio[label];
	buffer.Flags = XAUDIO2_END_OF_STREAM;
	buffer.LoopCount = g_aParam[label].nCntLoop;

	// ��Ԏ擾
	g_apSourceVoice[label]->GetState(&xa2state);
	if (xa2state.BuffersQueued != 0)
	{// �Đ���
		// �ꎞ��~
		g_apSourceVoice[label]->Stop(0);

		// �I�[�f�B�I�o�b�t�@�̍폜
		g_apSourceVoice[label]->FlushSourceBuffers();
	}

	//if (g_aParam[label].UseFilter == TRUE)
	{
		//�T�u�~�b�N�X�{�C�X�ɑ��M
		XAUDIO2_SEND_DESCRIPTOR Send[2] = { 0, g_apWetSubmixVoice,
											0, g_apDrySubmixVoice };
		XAUDIO2_VOICE_SENDS SendList = { 2, Send };
		g_apSourceVoice[label]->SetOutputVoices(&SendList);
	}
	// �I�[�f�B�I�o�b�t�@�̓o�^
	g_apSourceVoice[label]->SubmitSourceBuffer(&buffer);

	// �Đ�
	g_apSourceVoice[label]->Start(0);

	// �Đ�
	//g_apSourceVoice[label]->SetVolume(g_VolParam[g_aParam[label].type]);}
}

//=============================================================================
// �Z�O�����g��~(���x���w��)
//=============================================================================
void StopSound(int label)
{
#ifdef NO_AUDIO
	return;
#endif

	XAUDIO2_VOICE_STATE xa2state;

	// ��Ԏ擾
	g_apSourceVoice[label]->GetState(&xa2state);
	if (xa2state.BuffersQueued != 0)
	{// �Đ���
		// �ꎞ��~
		g_apSourceVoice[label]->Stop(0);

		// �I�[�f�B�I�o�b�t�@�̍폜
		g_apSourceVoice[label]->FlushSourceBuffers();
	}
}

//=============================================================================
// �Z�O�����g��~(�S��)
//=============================================================================
void StopSound(void)
{
#ifdef NO_AUDIO
	return;
#endif

	// �ꎞ��~
	for (int nCntSound = 0; nCntSound < SOUND_LABEL_MAX; nCntSound++)
	{
		if (g_apSourceVoice[nCntSound])
		{
			// �ꎞ��~
			g_apSourceVoice[nCntSound]->Stop(0);
		}
	}
}

//=============================================================================
// �`�����N�̃`�F�b�N
//=============================================================================
HRESULT CheckChunk(HANDLE hFile, DWORD format, DWORD *pChunkSize, DWORD *pChunkDataPosition)
{
#ifdef NO_AUDIO
	return S_OK;
#endif

	HRESULT hr = S_OK;
	DWORD dwRead;
	DWORD dwChunkType;
	DWORD dwChunkDataSize;
	DWORD dwRIFFDataSize = 0;
	DWORD dwFileType;
	DWORD dwBytesRead = 0;
	DWORD dwOffset = 0;

	if (SetFilePointer(hFile, 0, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
	{// �t�@�C���|�C���^��擪�Ɉړ�
		return HRESULT_FROM_WIN32(GetLastError());
	}

	while (hr == S_OK)
	{
		if (ReadFile(hFile, &dwChunkType, sizeof(DWORD), &dwRead, NULL) == 0)
		{// �`�����N�̓ǂݍ���
			hr = HRESULT_FROM_WIN32(GetLastError());
		}

		if (ReadFile(hFile, &dwChunkDataSize, sizeof(DWORD), &dwRead, NULL) == 0)
		{// �`�����N�f�[�^�̓ǂݍ���
			hr = HRESULT_FROM_WIN32(GetLastError());
		}

		switch (dwChunkType)
		{
		case 'FFIR':
			dwRIFFDataSize = dwChunkDataSize;
			dwChunkDataSize = 4;
			if (ReadFile(hFile, &dwFileType, sizeof(DWORD), &dwRead, NULL) == 0)
			{// �t�@�C���^�C�v�̓ǂݍ���
				hr = HRESULT_FROM_WIN32(GetLastError());
			}
			break;

		default:
			if (SetFilePointer(hFile, dwChunkDataSize, NULL, FILE_CURRENT) == INVALID_SET_FILE_POINTER)
			{// �t�@�C���|�C���^��`�����N�f�[�^���ړ�
				return HRESULT_FROM_WIN32(GetLastError());
			}
		}

		dwOffset += sizeof(DWORD) * 2;
		if (dwChunkType == format)
		{
			*pChunkSize = dwChunkDataSize;
			*pChunkDataPosition = dwOffset;

			return S_OK;
		}

		dwOffset += dwChunkDataSize;
		if (dwBytesRead >= dwRIFFDataSize)
		{
			return S_FALSE;
		}
	}

	return S_OK;
}

//=============================================================================
// �`�����N�f�[�^�̓ǂݍ���
//=============================================================================
HRESULT ReadChunkData(HANDLE hFile, void *pBuffer, DWORD dwBuffersize, DWORD dwBufferoffset)
{
#ifdef NO_AUDIO
	return S_OK;
#endif

	DWORD dwRead;

	if (SetFilePointer(hFile, dwBufferoffset, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
	{// �t�@�C���|�C���^��w��ʒu�܂ňړ�
		return HRESULT_FROM_WIN32(GetLastError());
	}

	if (ReadFile(hFile, pBuffer, dwBuffersize, &dwRead, NULL) == 0)
	{// �f�[�^�̓ǂݍ���
		return HRESULT_FROM_WIN32(GetLastError());
	}

	return S_OK;
}


//=============================================================================
// �T�E���h�t�F�[�h�̏�Ԃ̐ݒ�
//=============================================================================
void SetAudioFade(AUDIOFADE audiofade, int label)
{
#ifdef NO_AUDIO
	return;
#endif

	if (audiofade == AUDIOFADE_IN)
	{
		g_VolumeSound = 0.0f;		// ����0����X�^�[�g�����Ă���
		g_Label = label;			// ���ݖ炵�Ă��郉�x���ԍ���ۑ�
		g_AudioFade = audiofade;	// �t�F�[�h�̏�Ԃ�Z�b�g
	}
	if (audiofade == AUDIOFADE_OUT)
	{
		g_Label = label;			// ���ݖ炵�Ă��郉�x���ԍ���ۑ�
		g_AudioFade = audiofade;	// �t�F�[�h�̏�Ԃ�Z�b�g
	}
}

//=============================================================================
// ���ݖ��Ă��郉�x���ԍ��擾
//=============================================================================
int GetSoundLabel(void)
{
	return g_Label;
}

//=============================================================================
// �\�[�X�{�C�X�̉��ʒ���
//=============================================================================
void SetSourceVolume(int label, float volume)
{
	// ���ʂ�1.0f�ȉ��̎���2��J�[�u
	if (volume <= 1.0f)
	{
		volume *= volume;
	}
	g_apSourceVoice[label]->SetVolume(volume);
	return;
}

//=============================================================================
// �\�[�X�{�C�X�̃{�C�X�̍Đ��s�b�`����
//=============================================================================
void SetFrequencyRatio(int label, float pitch)
{
	g_apSourceVoice[label]->SetFrequencyRatio(pitch);
	return;
}

//=============================================================================
// �\�[�X�{�C�X�̈ꎞ��~
//=============================================================================
void PauseSound(int label)
{
	//if (g_aParam[label].type == BGM)
	{
		g_apSourceVoice[label]->Stop(XAUDIO2_PLAY_TAILS);
	}
	return;
}

//=============================================================================
// �\�[�X�{�C�X�̍ĊJ
//=============================================================================
void ReStartSound(int label)
{
	g_apSourceVoice[label]->Start();
	return;
}

