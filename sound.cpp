//=============================================================================
//
// サウンド処理 [sound.cpp]
// Author : 星 克昌
//
//=============================================================================
#include "main.h"
#include "sound.h"
#include "debugproc.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	AUDIO_FADEOUT_SPEED		(0.01f)			// オーディオフェードインスピード
#define	AUDIO_FADEIN_SPEED		(0.01f)			// オーディオフェードアウトスピード

#define NO_AUDIO	// サウンド無し

//*****************************************************************************
// パラメータ構造体定義
//*****************************************************************************
typedef struct
{
	char *pFilename;	// ファイル名
	int nCntLoop;		// ループカウント
} SOUNDPARAM;

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT CheckChunk(HANDLE hFile, DWORD format, DWORD *pChunkSize, DWORD *pChunkDataPosition);
HRESULT ReadChunkData(HANDLE hFile, void *pBuffer, DWORD dwBuffersize, DWORD dwBufferoffset);

//*****************************************************************************
// グローバル変数
//*****************************************************************************
IXAudio2 *g_pXAudio2 = NULL;								// XAudio2オブジェクトへのインターフェイス
IXAudio2MasteringVoice *g_pMasteringVoice = NULL;			// マスターボイス
IXAudio2SourceVoice *g_apSourceVoice[SOUND_LABEL_MAX] = {};	// ソースボイス
BYTE *g_apDataAudio[SOUND_LABEL_MAX] = {};					// オーディオデータ
DWORD g_aSizeAudio[SOUND_LABEL_MAX] = {};					// オーディオデータサイズ

AUDIOFADE g_AudioFade = AUDIOFADE_IN;						// フェードの状態
float g_VolumeSound = 1.0f;									// 音量
int g_Label = 0;											// オーディオラベル番号の初期化

static int	g_LoadPoint = 0;

// 各音素材のパラメータ
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

//=============================================================================
// 初期化処理
//=============================================================================
BOOL InitSound(HWND hWnd)
{
#ifdef NO_AUDIO
	return TRUE;
#endif

	HRESULT hr;

	// COMライブラリの初期化
	CoInitializeEx(NULL, COINIT_MULTITHREADED);

	// XAudio2オブジェクトの作成
	hr = XAudio2Create(&g_pXAudio2, 0);
	if (FAILED(hr))
	{
		MessageBox(hWnd, "XAudio2オブジェクトの作成に失敗！", "警告！", MB_ICONWARNING);

		// COMライブラリの終了処理
		CoUninitialize();

		return FALSE;
	}

	// マスターボイスの生成
	hr = g_pXAudio2->CreateMasteringVoice(&g_pMasteringVoice);
	if (FAILED(hr))
	{
		MessageBox(hWnd, "マスターボイスの生成に失敗！", "警告！", MB_ICONWARNING);

		if (g_pXAudio2)
		{
			// XAudio2オブジェクトの開放
			g_pXAudio2->Release();
			g_pXAudio2 = NULL;
		}

		// COMライブラリの終了処理
		CoUninitialize();

		return FALSE;
	}

	//// サウンドデータの初期化
	//for (int nCntSound = 0; nCntSound < SOUND_LABEL_MAX; nCntSound++)
	//{
	//	HANDLE hFile;
	//	DWORD dwChunkSize = 0;
	//	DWORD dwChunkPosition = 0;
	//	DWORD dwFiletype;
	//	WAVEFORMATEXTENSIBLE wfx;
	//	XAUDIO2_BUFFER buffer;


	//	// バッファのクリア
	//	memset(&wfx, 0, sizeof(WAVEFORMATEXTENSIBLE));
	//	memset(&buffer, 0, sizeof(XAUDIO2_BUFFER));

	//	// サウンドデータファイルの生成
	//	hFile = CreateFile(g_aParam[nCntSound].pFilename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	//	if (hFile == INVALID_HANDLE_VALUE)
	//	{
	//		MessageBox(hWnd, "サウンドデータファイルの生成に失敗！(1)", "警告！", MB_ICONWARNING);
	//		return FALSE;
	//	}
	//	if (SetFilePointer(hFile, 0, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
	//	{// ファイルポインタを先頭に移動
	//		MessageBox(hWnd, "サウンドデータファイルの生成に失敗！(2)", "警告！", MB_ICONWARNING);
	//		return FALSE;
	//	}

	//	// WAVEファイルのチェック
	//	hr = CheckChunk(hFile, 'FFIR', &dwChunkSize, &dwChunkPosition);
	//	if (FAILED(hr))
	//	{
	//		MessageBox(hWnd, "WAVEファイルのチェックに失敗！(1)", "警告！", MB_ICONWARNING);
	//		return FALSE;
	//	}
	//	hr = ReadChunkData(hFile, &dwFiletype, sizeof(DWORD), dwChunkPosition);
	//	if (FAILED(hr))
	//	{
	//		MessageBox(hWnd, "WAVEファイルのチェックに失敗！(2)", "警告！", MB_ICONWARNING);
	//		return FALSE;
	//	}
	//	if (dwFiletype != 'EVAW')
	//	{
	//		MessageBox(hWnd, "WAVEファイルのチェックに失敗！(3)", "警告！", MB_ICONWARNING);
	//		return FALSE;
	//	}

	//	// フォーマットチェック
	//	hr = CheckChunk(hFile, ' tmf', &dwChunkSize, &dwChunkPosition);
	//	if (FAILED(hr))
	//	{
	//		MessageBox(hWnd, "フォーマットチェックに失敗！(1)", "警告！", MB_ICONWARNING);
	//		return FALSE;
	//	}
	//	hr = ReadChunkData(hFile, &wfx, dwChunkSize, dwChunkPosition);
	//	if (FAILED(hr))
	//	{
	//		MessageBox(hWnd, "フォーマットチェックに失敗！(2)", "警告！", MB_ICONWARNING);
	//		return FALSE;
	//	}

	//	// オーディオデータ読み込み
	//	hr = CheckChunk(hFile, 'atad', &g_aSizeAudio[nCntSound], &dwChunkPosition);
	//	if (FAILED(hr))
	//	{
	//		MessageBox(hWnd, "オーディオデータ読み込みに失敗！(1)", "警告！", MB_ICONWARNING);
	//		return FALSE;
	//	}
	//	g_apDataAudio[nCntSound] = (BYTE*)malloc(g_aSizeAudio[nCntSound]);
	//	hr = ReadChunkData(hFile, g_apDataAudio[nCntSound], g_aSizeAudio[nCntSound], dwChunkPosition);
	//	if (FAILED(hr))
	//	{
	//		MessageBox(hWnd, "オーディオデータ読み込みに失敗！(2)", "警告！", MB_ICONWARNING);
	//		return FALSE;
	//	}

	//	// ソースボイスの生成
	//	hr = g_pXAudio2->CreateSourceVoice(&g_apSourceVoice[nCntSound], &(wfx.Format));
	//	if (FAILED(hr))
	//	{
	//		MessageBox(hWnd, "ソースボイスの生成に失敗！", "警告！", MB_ICONWARNING);
	//		return FALSE;
	//	}

	//	// バッファの値設定
	//	memset(&buffer, 0, sizeof(XAUDIO2_BUFFER));
	//	buffer.AudioBytes = g_aSizeAudio[nCntSound];
	//	buffer.pAudioData = g_apDataAudio[nCntSound];
	//	buffer.Flags = XAUDIO2_END_OF_STREAM;
	//	buffer.LoopCount = g_aParam[nCntSound].nCntLoop;

	//	// オーディオバッファの登録
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

	// サウンドデータの初期化
	HANDLE hFile;
	DWORD dwChunkSize = 0;
	DWORD dwChunkPosition = 0;
	DWORD dwFiletype;
	WAVEFORMATEXTENSIBLE wfx;
	XAUDIO2_BUFFER buffer;


	// バッファのクリア
	memset(&wfx, 0, sizeof(WAVEFORMATEXTENSIBLE));
	memset(&buffer, 0, sizeof(XAUDIO2_BUFFER));

	// サウンドデータファイルの生成
	hFile = CreateFile(g_aParam[g_LoadPoint].pFilename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		MessageBox(hWnd, "サウンドデータファイルの生成に失敗！(1)", "警告！", MB_ICONWARNING);
		return FALSE;
	}
	if (SetFilePointer(hFile, 0, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
	{// ファイルポインタを先頭に移動
		MessageBox(hWnd, "サウンドデータファイルの生成に失敗！(2)", "警告！", MB_ICONWARNING);
		return FALSE;
	}

	// WAVEファイルのチェック
	hr = CheckChunk(hFile, 'FFIR', &dwChunkSize, &dwChunkPosition);
	if (FAILED(hr))
	{
		MessageBox(hWnd, "WAVEファイルのチェックに失敗！(1)", "警告！", MB_ICONWARNING);
		return FALSE;
	}
	hr = ReadChunkData(hFile, &dwFiletype, sizeof(DWORD), dwChunkPosition);
	if (FAILED(hr))
	{
		MessageBox(hWnd, "WAVEファイルのチェックに失敗！(2)", "警告！", MB_ICONWARNING);
		return FALSE;
	}
	if (dwFiletype != 'EVAW')
	{
		MessageBox(hWnd, "WAVEファイルのチェックに失敗！(3)", "警告！", MB_ICONWARNING);
		return FALSE;
	}

	// フォーマットチェック
	hr = CheckChunk(hFile, ' tmf', &dwChunkSize, &dwChunkPosition);
	if (FAILED(hr))
	{
		MessageBox(hWnd, "フォーマットチェックに失敗！(1)", "警告！", MB_ICONWARNING);
		return FALSE;
	}
	hr = ReadChunkData(hFile, &wfx, dwChunkSize, dwChunkPosition);
	if (FAILED(hr))
	{
		MessageBox(hWnd, "フォーマットチェックに失敗！(2)", "警告！", MB_ICONWARNING);
		return FALSE;
	}

	// オーディオデータ読み込み
	hr = CheckChunk(hFile, 'atad', &g_aSizeAudio[g_LoadPoint], &dwChunkPosition);
	if (FAILED(hr))
	{
		MessageBox(hWnd, "オーディオデータ読み込みに失敗！(1)", "警告！", MB_ICONWARNING);
		return FALSE;
	}
	g_apDataAudio[g_LoadPoint] = (BYTE*)malloc(g_aSizeAudio[g_LoadPoint]);
	hr = ReadChunkData(hFile, g_apDataAudio[g_LoadPoint], g_aSizeAudio[g_LoadPoint], dwChunkPosition);
	if (FAILED(hr))
	{
		MessageBox(hWnd, "オーディオデータ読み込みに失敗！(2)", "警告！", MB_ICONWARNING);
		return FALSE;
	}

	// ソースボイスの生成
	hr = g_pXAudio2->CreateSourceVoice(&g_apSourceVoice[g_LoadPoint], &(wfx.Format));
	if (FAILED(hr))
	{
		MessageBox(hWnd, "ソースボイスの生成に失敗！", "警告！", MB_ICONWARNING);
		return FALSE;
	}

	// バッファの値設定
	memset(&buffer, 0, sizeof(XAUDIO2_BUFFER));
	buffer.AudioBytes = g_aSizeAudio[g_LoadPoint];
	buffer.pAudioData = g_apDataAudio[g_LoadPoint];
	buffer.Flags = XAUDIO2_END_OF_STREAM;
	buffer.LoopCount = g_aParam[g_LoadPoint].nCntLoop;

	// オーディオバッファの登録
	g_apSourceVoice[g_LoadPoint]->SubmitSourceBuffer(&buffer);

	return TRUE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateAudioFade(void)
{
#ifdef NO_AUDIO
	return;
#endif

	if (g_AudioFade != AUDIOFADE_NONE)
	{// 音量フェード処理中
		if (g_AudioFade == AUDIOFADE_OUT)
		{// 音量フェードアウト処理
			g_VolumeSound -= AUDIO_FADEOUT_SPEED;					// ボリューム値を加算して音量を徐々に上げている
			if (g_VolumeSound <= 0.0f)
			{
				g_VolumeSound = 0.0f;
				g_AudioFade = AUDIOFADE_NONE;
			}
			g_apSourceVoice[g_Label]->SetVolume(g_VolumeSound);		// 音量を変えている
		}
		else if (g_AudioFade == AUDIOFADE_IN)
		{// フェードイン処理
			g_VolumeSound += AUDIO_FADEIN_SPEED;					// ボリューム値を減算して音量を徐々に下げている
			if (g_VolumeSound >= 1.0f)
			{
				// フェード処理終了
				g_VolumeSound = 1.0f;
				g_AudioFade = AUDIOFADE_NONE;
			}
			g_apSourceVoice[g_Label]->SetVolume(g_VolumeSound);		// 音量を変えている

		}
	}
	PrintDebugProc("音量: %f\n", g_VolumeSound);
	PrintDebugProc("サウンド番号: %d\n", g_Label);
}



//=============================================================================
// 終了処理
//=============================================================================
void UninitSound(void)
{
#ifdef NO_AUDIO
	return;
#endif

	// 一時停止
	for (int nCntSound = 0; nCntSound < SOUND_LABEL_MAX; nCntSound++)
	{
		if (g_apSourceVoice[nCntSound])
		{
			// 一時停止
			g_apSourceVoice[nCntSound]->Stop(0);

			// ソースボイスの破棄
			g_apSourceVoice[nCntSound]->DestroyVoice();
			g_apSourceVoice[nCntSound] = NULL;

			// オーディオデータの開放
			free(g_apDataAudio[nCntSound]);
			g_apDataAudio[nCntSound] = NULL;
		}
	}

	// マスターボイスの破棄
	g_pMasteringVoice->DestroyVoice();
	g_pMasteringVoice = NULL;

	if (g_pXAudio2)
	{
		// XAudio2オブジェクトの開放
		g_pXAudio2->Release();
		g_pXAudio2 = NULL;
	}

	// COMライブラリの終了処理
	CoUninitialize();
}

//=============================================================================
// セグメント再生(再生中なら停止)
//=============================================================================
void PlaySound(int label, float volume)
{
#ifdef NO_AUDIO
	return;
#endif

	g_Label = label;					// 現在鳴らしているラベル番号を保存

	XAUDIO2_VOICE_STATE xa2state;
	XAUDIO2_BUFFER buffer;

	// バッファの値設定
	memset(&buffer, 0, sizeof(XAUDIO2_BUFFER));
	buffer.AudioBytes = g_aSizeAudio[label];
	buffer.pAudioData = g_apDataAudio[label];
	buffer.Flags = XAUDIO2_END_OF_STREAM;
	buffer.LoopCount = g_aParam[label].nCntLoop;

	// 状態取得
	g_apSourceVoice[label]->GetState(&xa2state);
	if (xa2state.BuffersQueued != 0)
	{// 再生中
		// 一時停止
		g_apSourceVoice[label]->Stop(0);

		// オーディオバッファの削除
		g_apSourceVoice[label]->FlushSourceBuffers();
	}

	// オーディオバッファの登録
	g_apSourceVoice[label]->SubmitSourceBuffer(&buffer);

	// 再生
	g_apSourceVoice[label]->Start(0);

	// 音量を変えている
	g_apSourceVoice[label]->SetVolume(g_VolumeSound);

	if (volume >= 0.0f)
	{
		g_apSourceVoice[label]->SetVolume(volume);
	}
}

//=============================================================================
// セグメント停止(ラベル指定)
//=============================================================================
void StopSound(int label)
{
#ifdef NO_AUDIO
	return;
#endif

	XAUDIO2_VOICE_STATE xa2state;

	// 状態取得
	g_apSourceVoice[label]->GetState(&xa2state);
	if (xa2state.BuffersQueued != 0)
	{// 再生中
		// 一時停止
		g_apSourceVoice[label]->Stop(0);

		// オーディオバッファの削除
		g_apSourceVoice[label]->FlushSourceBuffers();
	}
}

//=============================================================================
// セグメント停止(全て)
//=============================================================================
void StopSound(void)
{
#ifdef NO_AUDIO
	return;
#endif

	// 一時停止
	for (int nCntSound = 0; nCntSound < SOUND_LABEL_MAX; nCntSound++)
	{
		if (g_apSourceVoice[nCntSound])
		{
			// 一時停止
			g_apSourceVoice[nCntSound]->Stop(0);
		}
	}
}

//=============================================================================
// チャンクのチェック
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
	{// ファイルポインタを先頭に移動
		return HRESULT_FROM_WIN32(GetLastError());
	}

	while (hr == S_OK)
	{
		if (ReadFile(hFile, &dwChunkType, sizeof(DWORD), &dwRead, NULL) == 0)
		{// チャンクの読み込み
			hr = HRESULT_FROM_WIN32(GetLastError());
		}

		if (ReadFile(hFile, &dwChunkDataSize, sizeof(DWORD), &dwRead, NULL) == 0)
		{// チャンクデータの読み込み
			hr = HRESULT_FROM_WIN32(GetLastError());
		}

		switch (dwChunkType)
		{
		case 'FFIR':
			dwRIFFDataSize = dwChunkDataSize;
			dwChunkDataSize = 4;
			if (ReadFile(hFile, &dwFileType, sizeof(DWORD), &dwRead, NULL) == 0)
			{// ファイルタイプの読み込み
				hr = HRESULT_FROM_WIN32(GetLastError());
			}
			break;

		default:
			if (SetFilePointer(hFile, dwChunkDataSize, NULL, FILE_CURRENT) == INVALID_SET_FILE_POINTER)
			{// ファイルポインタをチャンクデータ分移動
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
// チャンクデータの読み込み
//=============================================================================
HRESULT ReadChunkData(HANDLE hFile, void *pBuffer, DWORD dwBuffersize, DWORD dwBufferoffset)
{
#ifdef NO_AUDIO
	return S_OK;
#endif

	DWORD dwRead;

	if (SetFilePointer(hFile, dwBufferoffset, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
	{// ファイルポインタを指定位置まで移動
		return HRESULT_FROM_WIN32(GetLastError());
	}

	if (ReadFile(hFile, pBuffer, dwBuffersize, &dwRead, NULL) == 0)
	{// データの読み込み
		return HRESULT_FROM_WIN32(GetLastError());
	}

	return S_OK;
}


//=============================================================================
// サウンドフェードの状態の設定
//=============================================================================
void SetAudioFade(AUDIOFADE audiofade, int label)
{
#ifdef NO_AUDIO
	return;
#endif

	if (audiofade == AUDIOFADE_IN)
	{
		g_VolumeSound = 0.0f;		// 音量0からスタートさせている
		g_Label = label;			// 現在鳴らしているラベル番号を保存
		g_AudioFade = audiofade;	// フェードの状態をセット
	}
	if (audiofade == AUDIOFADE_OUT)
	{
		g_Label = label;			// 現在鳴らしているラベル番号を保存
		g_AudioFade = audiofade;	// フェードの状態をセット
	}
}

//=============================================================================
// 現在鳴っているラベル番号取得
//=============================================================================
int GetAudioLabel(void)
{
	return g_Label;
}