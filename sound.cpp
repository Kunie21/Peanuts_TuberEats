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

#define NO_AUDIO	// �T�E���h����

//*****************************************************************************
// �p�����[�^�\���̒�`
//*****************************************************************************
typedef struct
{
	char *pFilename;	// �t�@�C����
	int nCntLoop;		// ���[�v�J�E���g
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

IXAudio2SourceVoice *g_apSourceVoice[SOUND_LABEL_MAX] = {};	// �\�[�X�{�C�X
BYTE *g_apDataAudio[SOUND_LABEL_MAX] = {};					// �I�[�f�B�I�f�[�^
DWORD g_aSizeAudio[SOUND_LABEL_MAX] = {};					// �I�[�f�B�I�f�[�^�T�C�Y

AUDIOFADE g_AudioFade = AUDIOFADE_IN;						// �t�F�[�h�̏��
float g_VolumeSound = 1.0f;									// ����
int g_Label = 0;											// �I�[�f�B�I���x���ԍ��̏�����

static int	g_LoadPoint = 0;


//*****************************************************************************
// �e���f�ނ̃p�����[�^
//*****************************************************************************
SOUNDPARAM g_aParam[SOUND_LABEL_MAX] =
{
	{ (char*)"data/BGM/TITLE.wav", -1 },		// 
	{ (char*)"data/BGM/STAGESELECT.wav", -1 },	// 
	{ (char*)"data/BGM/TUTORIAL.wav", -1 },		// 
	{ (char*)"data/BGM/NORMAL.wav", -1 },		// 
	{ (char*)"data/BGM/ICE.wav", -1 },			// 
	{ (char*)"data/BGM/SAND.wav", -1 },			// 
	{ (char*)"data/BGM/MAGMA.wav", -1 },		// 
	{ (char*)"data/BGM/BONE.wav", -1 },			// 
	{ (char*)"data/BGM/SECRET.wav", -1 },		// 
	{ (char*)"data/BGM/RESULT.wav", -1 },		// 
	{ (char*)"data/BGM/ENDING.wav", -1 },		// 
	{ (char*)"data/SE/DECIDE.wav", 0 },			// 
	{ (char*)"data/SE/DECIDE_FROG.wav", 0 },	// 
	{ (char*)"data/SE/DECIDE_KIRA.wav", 0 },	// 
	{ (char*)"data/SE/DECIDE_KORO.wav", 0 },	// 
	{ (char*)"data/SE/DECIDE_GAO.wav", 0 },		// 
	{ (char*)"data/SE/BACK.wav", 0 },			// 
	{ (char*)"data/SE/CLEAR.wav", 0 },			// 
	{ (char*)"data/SE/SWIPE.wav", 0 },			// 
	{ (char*)"data/SE/BUBBLE.wav", 0 },			// 
	{ (char*)"data/SE/FALL.wav", 0 },			// 
	{ (char*)"data/SE/HOLD.wav", 0 },			// 
	{ (char*)"data/SE/INTODARK.wav", 0 },		// 
	{ (char*)"data/SE/INTOMAGMA.wav", 0 },		// 
	{ (char*)"data/SE/JUMP.wav", 0 },			// 
	{ (char*)"data/SE/MARVELOUS.wav", 0 },		// 
	{ (char*)"data/SE/GOAL.wav", 0 },			// 
	{ (char*)"data/SE/TIMEUP.wav", 0 },			// 
	{ (char*)"data/SE/MEDAL.wav", 0 },			// 
	{ (char*)"data/SE/SCORE.wav", 0 },			// 
	{ (char*)"data/SE/CHECKPOINT.wav", 0 },			// 
};

//*****************************************************************************
// �G�t�F�N�g�֘A
//*****************************************************************************

//���o�[�u

//�G�R�[

//EQ




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

	//// �T�E���h�f�[�^�̏�����
	//for (int nCntSound = 0; nCntSound < SOUND_LABEL_MAX; nCntSound++)
	//{
	//	HANDLE hFile;
	//	DWORD dwChunkSize = 0;
	//	DWORD dwChunkPosition = 0;
	//	DWORD dwFiletype;
	//	WAVEFORMATEXTENSIBLE wfx;
	//	XAUDIO2_BUFFER buffer;


	//	// �o�b�t�@�̃N���A
	//	memset(&wfx, 0, sizeof(WAVEFORMATEXTENSIBLE));
	//	memset(&buffer, 0, sizeof(XAUDIO2_BUFFER));

	//	// �T�E���h�f�[�^�t�@�C���̐���
	//	hFile = CreateFile(g_aParam[nCntSound].pFilename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	//	if (hFile == INVALID_HANDLE_VALUE)
	//	{
	//		MessageBox(hWnd, "�T�E���h�f�[�^�t�@�C���̐����Ɏ��s�I(1)", "�x���I", MB_ICONWARNING);
	//		return FALSE;
	//	}
	//	if (SetFilePointer(hFile, 0, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
	//	{// �t�@�C���|�C���^��擪�Ɉړ�
	//		MessageBox(hWnd, "�T�E���h�f�[�^�t�@�C���̐����Ɏ��s�I(2)", "�x���I", MB_ICONWARNING);
	//		return FALSE;
	//	}

	//	// WAVE�t�@�C���̃`�F�b�N
	//	hr = CheckChunk(hFile, 'FFIR', &dwChunkSize, &dwChunkPosition);
	//	if (FAILED(hr))
	//	{
	//		MessageBox(hWnd, "WAVE�t�@�C���̃`�F�b�N�Ɏ��s�I(1)", "�x���I", MB_ICONWARNING);
	//		return FALSE;
	//	}
	//	hr = ReadChunkData(hFile, &dwFiletype, sizeof(DWORD), dwChunkPosition);
	//	if (FAILED(hr))
	//	{
	//		MessageBox(hWnd, "WAVE�t�@�C���̃`�F�b�N�Ɏ��s�I(2)", "�x���I", MB_ICONWARNING);
	//		return FALSE;
	//	}
	//	if (dwFiletype != 'EVAW')
	//	{
	//		MessageBox(hWnd, "WAVE�t�@�C���̃`�F�b�N�Ɏ��s�I(3)", "�x���I", MB_ICONWARNING);
	//		return FALSE;
	//	}

	//	// �t�H�[�}�b�g�`�F�b�N
	//	hr = CheckChunk(hFile, ' tmf', &dwChunkSize, &dwChunkPosition);
	//	if (FAILED(hr))
	//	{
	//		MessageBox(hWnd, "�t�H�[�}�b�g�`�F�b�N�Ɏ��s�I(1)", "�x���I", MB_ICONWARNING);
	//		return FALSE;
	//	}
	//	hr = ReadChunkData(hFile, &wfx, dwChunkSize, dwChunkPosition);
	//	if (FAILED(hr))
	//	{
	//		MessageBox(hWnd, "�t�H�[�}�b�g�`�F�b�N�Ɏ��s�I(2)", "�x���I", MB_ICONWARNING);
	//		return FALSE;
	//	}

	//	// �I�[�f�B�I�f�[�^�ǂݍ���
	//	hr = CheckChunk(hFile, 'atad', &g_aSizeAudio[nCntSound], &dwChunkPosition);
	//	if (FAILED(hr))
	//	{
	//		MessageBox(hWnd, "�I�[�f�B�I�f�[�^�ǂݍ��݂Ɏ��s�I(1)", "�x���I", MB_ICONWARNING);
	//		return FALSE;
	//	}
	//	g_apDataAudio[nCntSound] = (BYTE*)malloc(g_aSizeAudio[nCntSound]);
	//	hr = ReadChunkData(hFile, g_apDataAudio[nCntSound], g_aSizeAudio[nCntSound], dwChunkPosition);
	//	if (FAILED(hr))
	//	{
	//		MessageBox(hWnd, "�I�[�f�B�I�f�[�^�ǂݍ��݂Ɏ��s�I(2)", "�x���I", MB_ICONWARNING);
	//		return FALSE;
	//	}

	//	// �\�[�X�{�C�X�̐���
	//	hr = g_pXAudio2->CreateSourceVoice(&g_apSourceVoice[nCntSound], &(wfx.Format));
	//	if (FAILED(hr))
	//	{
	//		MessageBox(hWnd, "�\�[�X�{�C�X�̐����Ɏ��s�I", "�x���I", MB_ICONWARNING);
	//		return FALSE;
	//	}

	//	// �o�b�t�@�̒l�ݒ�
	//	memset(&buffer, 0, sizeof(XAUDIO2_BUFFER));
	//	buffer.AudioBytes = g_aSizeAudio[nCntSound];
	//	buffer.pAudioData = g_apDataAudio[nCntSound];
	//	buffer.Flags = XAUDIO2_END_OF_STREAM;
	//	buffer.LoopCount = g_aParam[nCntSound].nCntLoop;

	//	// �I�[�f�B�I�o�b�t�@�̓o�^
	//	g_apSourceVoice[nCntSound]->SubmitSourceBuffer(&buffer);
	//}

	g_LoadPoint = 0;

	return TRUE;
}

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
	hr = g_pXAudio2->CreateSourceVoice(&g_apSourceVoice[g_LoadPoint], &(wfx.Format));
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
			g_VolumeSound -= AUDIO_FADEOUT_SPEED;					// �{�����[���l�����Z���ĉ��ʂ����X�ɏグ�Ă���
			if (g_VolumeSound <= 0.0f)
			{
				g_VolumeSound = 0.0f;
				g_AudioFade = AUDIOFADE_NONE;
			}
			g_apSourceVoice[g_Label]->SetVolume(g_VolumeSound);		// ���ʂ�ς��Ă���
		}
		else if (g_AudioFade == AUDIOFADE_IN)
		{// �t�F�[�h�C������
			g_VolumeSound += AUDIO_FADEIN_SPEED;					// �{�����[���l�����Z���ĉ��ʂ����X�ɉ����Ă���
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

	g_Label = label;					// ���ݖ炵�Ă��郉�x���ԍ���ۑ�

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

	// �I�[�f�B�I�o�b�t�@�̓o�^
	g_apSourceVoice[label]->SubmitSourceBuffer(&buffer);

	// �Đ�
	g_apSourceVoice[label]->Start(0);

	// ���ʂ�ς��Ă���
	g_apSourceVoice[label]->SetVolume(g_VolumeSound);

	if (volume >= 0.0f)
	{
		g_apSourceVoice[label]->SetVolume(volume);
	}
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
			{// �t�@�C���|�C���^���`�����N�f�[�^���ړ�
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
	{// �t�@�C���|�C���^���w��ʒu�܂ňړ�
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
		g_AudioFade = audiofade;	// �t�F�[�h�̏�Ԃ��Z�b�g
	}
	if (audiofade == AUDIOFADE_OUT)
	{
		g_Label = label;			// ���ݖ炵�Ă��郉�x���ԍ���ۑ�
		g_AudioFade = audiofade;	// �t�F�[�h�̏�Ԃ��Z�b�g
	}
}

//=============================================================================
// ���ݖ��Ă��郉�x���ԍ��擾
//=============================================================================
int GetAudioLabel(void)
{
	return g_Label;
}