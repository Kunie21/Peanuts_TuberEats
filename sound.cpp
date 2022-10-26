//=============================================================================
//
// サウンド処理 [sound.cpp]
// Author : 武藤 颯太
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

#define NO_AUDIO	// サウンド無し(これ消せばロードはいる)

//*****************************************************************************
// パラメータ構造体定義
//*****************************************************************************
typedef struct
{
	char *pFilename;	// ファイル名
	int nCntLoop;		// ループカウント
	//BOOL UseFilter;		//エフェクト使うか使わないか？？
	//int type;			// サウンドの種類

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


// サブミックスボイスの作成
IXAudio2SubmixVoice *g_apDrySubmixVoice;					//サブミックスボイス(エフェクトなし)
IXAudio2SubmixVoice *g_apWetSubmixVoice;					//サブミックスボイス(エフェクトあり)

IXAudio2SourceVoice *g_apSourceVoice[SOUND_LABEL_MAX] = {};	// ソースボイス
BYTE *g_apDataAudio[SOUND_LABEL_MAX] = {};					// オーディオデータ
DWORD g_aSizeAudio[SOUND_LABEL_MAX] = {};					// オーディオデータサイズ

AUDIOFADE g_AudioFade = AUDIOFADE_IN;						// フェードの状態
float g_VolumeSound = 1.0f;									// 音量
int g_Label = 0;											// オーディオラベル番号の初期化

//音量管理
float				g_VolParam[SOUND_TYPE_MAX];

static int	g_LoadPoint = 0;


//*****************************************************************************
// 各音素材のパラメータ
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
	{ (char*)"data/SE/CHECKPOINT.wav", 0 },		// 
};

//*****************************************************************************
// エフェクト関連
//*****************************************************************************
//リバーブ
IUnknown *g_apXPO_Reverb;
XAUDIO2_EFFECT_DESCRIPTOR g_Descriptior[SOUND_EFFECT_MAX];
XAUDIO2_EFFECT_CHAIN g_Chain;

//リバーブのパラメータ
FXECHO_PARAMETERS g_ReverbParam;
XAUDIO2FX_REVERB_I3DL2_PARAMETERS g_i3dl2Param = XAUDIO2FX_I3DL2_PRESET_BATHROOM;
XAUDIO2FX_REVERB_PARAMETERS g_reverbParam;

//エコー
IUnknown *g_apXPO_Echo;

//エコーのパラメータ
FXECHO_PARAMETERS g_EchoParam;

//EQ
IUnknown *g_apXPO_EQ;

//EQのパラメータ
FXEQ_PARAMETERS g_EQParam;


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

	//サブミックスボイス（エフェクトあり）の作成
	hr = g_pXAudio2->CreateSubmixVoice(&g_apWetSubmixVoice, 2, 48000);
	if (FAILED(hr))
	{
		MessageBox(hWnd, "サブミックス（エフェクトあり）の生成に失敗！", "警告！", MB_ICONWARNING);
		return FALSE;
	}

	//サブミックスボイス（エフェクトなし）の作成
	hr = g_pXAudio2->CreateSubmixVoice(&g_apDrySubmixVoice, 2, 48000);
	if (FAILED(hr))
	{
		MessageBox(hWnd, "サブミックス（エフェクトなし）の生成に失敗！", "警告！", MB_ICONWARNING);
		return FALSE;
	}

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
	hr = g_pXAudio2->CreateSourceVoice(&g_apSourceVoice[g_LoadPoint], &(wfx.Format), XAUDIO2_VOICE_USEFILTER, 16.0f);
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

	////エフェクトの生成
	//{
	//	//リバーブの生成
	//	XAudio2CreateReverb(&g_apXPO_Reverb);

	//	//EFFECT_DESCRIPTOR(リバーブ)の初期化
	//	g_Descriptior[REVERB].InitialState = FALSE;			//有効状態に
	//	g_Descriptior[REVERB].OutputChannels = 2;				//2chのエフェクト
	//	g_Descriptior[REVERB].pEffect = g_apXPO_Reverb;		//エフェクト本体

	//	//エコーの生成
	//	CreateFX(_uuidof(FXEcho), &g_apXPO_Echo);

	//	//EFFECT_DESCRIPTOR(エコー)の初期化
	//	g_Descriptior[ECHO].InitialState = FALSE;				//無効状態に
	//	g_Descriptior[ECHO].OutputChannels = 2;				//2chのエフェクト
	//	g_Descriptior[ECHO].pEffect = g_apXPO_Echo;			//エフェクト本体

	//	//EQの生成
	//	CreateFX(_uuidof(FXEQ), &g_apXPO_EQ);

	//	//EFFECT_DESCRIPTOR(EQ)の初期化
	//	g_Descriptior[EQ].InitialState = FALSE;				//有効状態に
	//	g_Descriptior[EQ].OutputChannels = 2;					//2chのエフェクト
	//	g_Descriptior[EQ].pEffect = g_apXPO_EQ;				//エフェクト本体

	//	//EFFECT_CHAINの作成
	//	g_Chain.EffectCount = SOUND_EFFECT_MAX;								//挿すのはSOUND_EFFECT_MAX分
	//	g_Chain.pEffectDescriptors = g_Descriptior;							//さっきの構造体を指示

	//	//ボイスにEFFECT_CHAINを挿す
	//	g_apWetSubmixVoice->SetEffectChain(&g_Chain);

	//	//Release
	//	g_apXPO_Reverb->Release();
	//	g_apXPO_Echo->Release();
	//	g_apXPO_EQ->Release();
	//}


	////パラメーターの初期化
	//{
	//	// リバーブ
	//	// I3DL2_REVERB_PARAM 変換
	//	ReverbConvertI3DL2ToNative(&g_i3dl2Param, &g_reverbParam);

	//	//エフェクトに通知する
	//	g_apWetSubmixVoice->SetEffectParameters(REVERB, &g_reverbParam, sizeof(g_reverbParam));

	//	// エコー
	//	g_EchoParam.WetDryMix = FXECHO_DEFAULT_WETDRYMIX;
	//	g_EchoParam.Delay = FXECHO_DEFAULT_DELAY;
	//	g_EchoParam.Feedback = FXECHO_DEFAULT_FEEDBACK;

	//	//エフェクトに通知する
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

	//	//エフェクトに通知する
	//	g_apWetSubmixVoice->SetEffectParameters(EQ, &g_EQParam, sizeof(g_EQParam));
	//}


	////音量の初期化
	//{
	//	g_VolParam[MASTER] = 1.0f;
	//	g_VolParam[BGM] = 1.0f;
	//	g_VolParam[SE] = 1.0f;
	//	g_VolParam[VOICE] = 1.0f;
	//}



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

	// サブミックスボイスの破棄
	g_apWetSubmixVoice->DestroyVoice();
	g_apDrySubmixVoice->DestroyVoice();

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

	//if (g_aParam[label].UseFilter == TRUE)
	{
		//サブミックスボイスに送信
		XAUDIO2_SEND_DESCRIPTOR Send[2] = { 0, g_apWetSubmixVoice,
											0, g_apDrySubmixVoice };
		XAUDIO2_VOICE_SENDS SendList = { 2, Send };
		g_apSourceVoice[label]->SetOutputVoices(&SendList);
	}
	// オーディオバッファの登録
	g_apSourceVoice[label]->SubmitSourceBuffer(&buffer);

	// 再生
	g_apSourceVoice[label]->Start(0);

	// 再生
	//g_apSourceVoice[label]->SetVolume(g_VolParam[g_aParam[label].type]);}
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
int GetSoundLabel(void)
{
	return g_Label;
}

//=============================================================================
// ソースボイスの音量調整
//=============================================================================
void SetSourceVolume(int label, float volume)
{
	volume *= volume;
	g_apSourceVoice[label]->SetVolume(volume);
	return;
}

//=============================================================================
// ソースボイスのボイスの再生ピッチ調整
//=============================================================================
void SetFrequencyRatio(int label, float Pitch)
{
	g_apSourceVoice[label]->SetFrequencyRatio(Pitch);
	return;
}

//=============================================================================
// ソースボイスの一時停止
//=============================================================================
void PauseSound(int label)
{
	g_apSourceVoice[label]->Stop(XAUDIO2_PLAY_TAILS);
	return;
}

//=============================================================================
// ソースボイスの再開
//=============================================================================
void ReStartSound(int label)
{
	g_apSourceVoice[label]->Start();
	return;
}

