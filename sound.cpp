//=============================================================================
//
// サウンド処理 [sound.cpp]
// Author : 武藤 颯太
//
//=============================================================================
#include "main.h"
#include "sound.h"
#include "debugproc.h"
#include "load.h"
#include "stage.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	AUDIO_FADEOUT_SPEED		(0.01f)			// オーディオフェードインスピード
#define	AUDIO_FADEIN_SPEED		(0.01f)			// オーディオフェードアウトスピード

#define MASTER_INITIAL_VOLUME	(0.5f)			// 初期音量
#define BGM_INITIAL_VOLUME		(0.9f)			// 初期音量
#define SE_INITIAL_VOLUME		(1.0f)			// 初期音量
#define VOICE_INITIAL_VOLUME	(0.6f)			// 初期音量

//#define NO_AUDIO	// サウンド無し(これ消せばロードはいる)

//*****************************************************************************
// パラメータ構造体定義
//*****************************************************************************
typedef struct
{
	char *pFilename;	// ファイル名
	int nCntLoop;		// ループカウント
	int type;			// サウンドの種類
	//BOOL UseFilter;		// エフェクト使うか使わないか？？

} SOUNDPARAM;

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT CheckChunk(HANDLE hFile, DWORD format, DWORD *pChunkSize, DWORD *pChunkDataPosition);
HRESULT ReadChunkData(HANDLE hFile, void *pBuffer, DWORD dwBuffersize, DWORD dwBufferoffset);
BOOL LoadSound(int loadpoint);

//*****************************************************************************
// グローバル変数
//*****************************************************************************
IXAudio2 *g_pXAudio2 = NULL;								// XAudio2オブジェクトへのインターフェイス
IXAudio2MasteringVoice *g_pMasteringVoice = NULL;			// マスターボイス

IXAudio2SubmixVoice *g_apSubmixVoice;				//サブミックスボイス


//// サブミックスボイスの作成
//IXAudio2SubmixVoice *g_apDrySubmixVoice;					//サブミックスボイス(エフェクトなし)
//IXAudio2SubmixVoice *g_apWetSubmixVoice;					//サブミックスボイス(エフェクトあり)

IXAudio2SourceVoice *g_apSourceVoice[SOUND_LABEL_MAX] = {};	// ソースボイス
BYTE *g_apDataAudio[SOUND_LABEL_MAX] = {};					// オーディオデータ
DWORD g_aSizeAudio[SOUND_LABEL_MAX] = {};					// オーディオデータサイズ

AUDIOFADE g_AudioFade = AUDIOFADE_IN;						// フェードの状態
float g_VolumeSound = 1.0f;									// 音量
int g_Label = 0;											// オーディオラベル番号の初期化

int g_NowBGMLabel = 0;											// オーディオラベル番号の初期化

//音量管理
float				g_VolParam[SOUND_TYPE_MAX];



//*****************************************************************************
// 各音素材のパラメータ
//*****************************************************************************
SOUNDPARAM g_aParam[SOUND_LABEL_MAX] =
{
	{ (char*)"data/SOUND/BGM/title.wav", -1, BGM },			// タイトル
	//{ (char*)"data/SOUND/BGM/STAGESELECT.wav", -1, BGM },		// スタート
	{ (char*)"data/SOUND/BGM/home.wav", -1, BGM },				// ホーム
	{ (char*)"data/SOUND/BGM/stage_select.wav", -1, BGM },		// ステージセレクト
	{ (char*)"data/SOUND/BGM/stage_1_1.wav", -1, BGM },		// ステージ1-1
	//{ (char*)"data/BGM/stage_1_3.wav", -1, BGM },		// ステージ1-3
	{ (char*)"data/SOUND/BGM/stage_2_1.wav", -1, BGM },		// ステージ2-1
	//{ (char*)"data/BGM/stage_2_2.wav", -1, BGM },		// ステージ2-2
	//{ (char*)"data/BGM/stage_2_3.wav", -1, BGM },		// ステージ2-3
	//{ (char*)"data/BGM/stage_1_2.wav", -1, BGM },		// ステージ1-2
	{ (char*)"data/SOUND/BGM/stage_3_1.wav", -1, BGM },		// ステージ3-1
	//{ (char*)"data/BGM/stage_3_2.wav", -1, BGM },		// ステージ3-2
	//{ (char*)"data/BGM/stage_3_3.wav", -1, BGM },		// ステージ3-3
	{ (char*)"data/SOUND/BGM/stage_4_1.wav", -1, BGM },		// ステージ4-1
	//{ (char*)"data/BGM/stage_4_2.wav", -1, BGM },		// ステージ4-2
	//{ (char*)"data/BGM/stage_4_3.wav", -1, BGM },		// ステージ4-3
	{ (char*)"data/SOUND/BGM/result.wav", -1, BGM },			// リザルト
	{ (char*)"data/SOUND/BGM/ending.wav", -1, BGM },			// エンディング
	{ (char*)"data/SOUND/SE/engine_3.wav", -1, BGM },			// エンジン小
	{ (char*)"data/SOUND/SE/engine_2.wav", -1, BGM },			// エンジン中
	{ (char*)"data/SOUND/SE/engine_1.wav", -1, BGM },			// エンジン強


	{ (char*)"data/SOUND/SE/airleak.wav", 0, SE },			// 空気漏れ音
	{ (char*)"data/SOUND/SE/back.wav", 0, SE },				// 戻る
	{ (char*)"data/SOUND/SE/barrier.wav", 0, SE },			// 無敵バリア
	{ (char*)"data/SOUND/SE/break_ice.wav", 0, SE },		// 氷を破壊
	{ (char*)"data/SOUND/SE/change_rocket.wav", 0, SE },	// ロケットチェンジ
	{ (char*)"data/SOUND/SE/collision.wav", 0, SE },		// 氷に衝突
	{ (char*)"data/SOUND/SE/collision_blast.wav", 0, SE },	// 噴出孔に衝突
	{ (char*)"data/SOUND/SE/countdown.wav", 0, SE },		// カウントダウン
	{ (char*)"data/SOUND/SE/cursor.wav", 0, SE },			// メニューカーソル
	{ (char*)"data/SOUND/SE/decide.wav", 0, SE },			// 決定
	{ (char*)"data/SOUND/SE/equip.wav", 0, SE },			// ロケット装備
	{ (char*)"data/SOUND/SE/get_donut.wav", 0, SE },		// ドーナッツ取得
	{ (char*)"data/SOUND/SE/get_ramen.wav", 0, SE },		// ラーメン取得
	{ (char*)"data/SOUND/SE/get_sushi.wav", 0, SE },		// 寿司取得
	{ (char*)"data/SOUND/SE/goal.wav", 0, SE },				// ゴール
	{ (char*)"data/SOUND/SE/launch_sushi.wav", 0, SE },		// 寿司ミサイル発射
	{ (char*)"data/SOUND/SE/pin.wav", 0, SE },				// 地球のピン
	{ (char*)"data/SOUND/SE/refuse.wav", 0, SE },			// クリック拒否
	{ (char*)"data/SOUND/SE/spin_earth.wav", 0, SE },		// 地球回転
	{ (char*)"data/SOUND/SE/start_deliver.wav", 0, SE },	// 配達スタート
	{ (char*)"data/SOUND/SE/start_go.wav", 0, SE },			// 発進
	{ (char*)"data/SOUND/SE/through_ring.wav", 0, SE },		// リング通過

	//{ (char*)"data/SOUND/SE/select.wav", 0, SE },			// 選択音
	//{ (char*)"data/SOUND/SE/decide.wav", 0, SE },			// 決定音
	//{ (char*)"data/SOUND/SE/decide.wav", 0, SE },			// オープニングSE?
	//{ (char*)"data/SOUND/SE/engine.wav", 0, SE },			// エンジン音
	//{ (char*)"data/SOUND/SE/collision.wav", 0, SE },		// 衝突音
	////{ (char*)"data/SE/p_voice.wav", 0, SE },		// プレイヤーボイス
	//{ (char*)"data/SOUND/SE/airleak.wav", 0, SE },		// 空気漏れ音
	//{ (char*)"data/SOUND/SE/door_open.wav", 0, SE },		// ドアが開く音
	//{ (char*)"data/SOUND/SE/across_ring.wav", 0, SE },	// リング通過音
	//{ (char*)"data/SOUND/SE/star.wav", 0, SE },			// 星の音
	//{ (char*)"data/SOUND/SE/point_add.wav", 0, SE },		// ポイント加算
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

////LowPass
//XAUDIO2_FILTER_PARAMETERS FilterParams;

// マスタリングリミッターつくる




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

	//サブミックスボイスの作成
	hr = g_pXAudio2->CreateSubmixVoice(&g_apSubmixVoice, 2, 44800);
	if (FAILED(hr))
	{
		MessageBox(hWnd, "サブミックスの生成に失敗！", "警告！", MB_ICONWARNING);
		return FALSE;
	}


	////サブミックスボイス（エフェクトあり）の作成
	//hr = g_pXAudio2->CreateSubmixVoice(&g_apWetSubmixVoice, 2, 48000);
	//if (FAILED(hr))
	//{
	//	MessageBox(hWnd, "サブミックス（エフェクトあり）の生成に失敗！", "警告！", MB_ICONWARNING);
	//	return FALSE;
	//}

	////サブミックスボイス（エフェクトなし）の作成
	//hr = g_pXAudio2->CreateSubmixVoice(&g_apDrySubmixVoice, 2, 48000);
	//if (FAILED(hr))
	//{
	//	MessageBox(hWnd, "サブミックス（エフェクトなし）の生成に失敗！", "警告！", MB_ICONWARNING);
	//	return FALSE;
	//}

	return TRUE;
}

//=============================================================================
// データ処理
//=============================================================================
BOOL LoadSoundKernel(void)
{
	static int	loadpoint = 0;
	if (loadpoint < SOUND_LABEL_MAX)
	{
		if(LoadSound(loadpoint))
		{
			loadpoint++;
			AddLoadSum();
		}
		if (loadpoint == SOUND_LABEL_MAX)
		{
			return TRUE;
		}
	}
	return FALSE;
}

//=============================================================================
// サウンドデータ処理
//=============================================================================
BOOL LoadSound(int loadpoint)
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
	hFile = CreateFile(g_aParam[loadpoint].pFilename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
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
	hr = CheckChunk(hFile, 'atad', &g_aSizeAudio[loadpoint], &dwChunkPosition);
	if (FAILED(hr))
	{
		MessageBox(hWnd, "オーディオデータ読み込みに失敗！(1)", "警告！", MB_ICONWARNING);
		return FALSE;
	}
	g_apDataAudio[loadpoint] = (BYTE*)malloc(g_aSizeAudio[loadpoint]);
	hr = ReadChunkData(hFile, g_apDataAudio[loadpoint], g_aSizeAudio[loadpoint], dwChunkPosition);
	if (FAILED(hr))
	{
		MessageBox(hWnd, "オーディオデータ読み込みに失敗！(2)", "警告！", MB_ICONWARNING);
		return FALSE;
	}

	// ソースボイスの生成
	hr = g_pXAudio2->CreateSourceVoice(&g_apSourceVoice[loadpoint], &(wfx.Format), XAUDIO2_VOICE_USEFILTER, 16.0f);
	if (FAILED(hr))
	{
		MessageBox(hWnd, "ソースボイスの生成に失敗！", "警告！", MB_ICONWARNING);
		return FALSE;
	}

	// バッファの値設定
	memset(&buffer, 0, sizeof(XAUDIO2_BUFFER));
	buffer.AudioBytes = g_aSizeAudio[loadpoint];
	buffer.pAudioData = g_apDataAudio[loadpoint];
	buffer.Flags = XAUDIO2_END_OF_STREAM;
	buffer.LoopCount = g_aParam[loadpoint].nCntLoop;

	// オーディオバッファの登録
	g_apSourceVoice[loadpoint]->SubmitSourceBuffer(&buffer);

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


	//音量の初期化
	{
		g_VolParam[MASTER]	= MASTER_INITIAL_VOLUME;
		g_VolParam[BGM]		= BGM_INITIAL_VOLUME;
		g_VolParam[SE]		= SE_INITIAL_VOLUME;
		g_VolParam[VOICE]	= VOICE_INITIAL_VOLUME;
	}



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

	//// サブミックスボイスの破棄

	g_apSubmixVoice->DestroyVoice();
	//g_apWetSubmixVoice->DestroyVoice();
	//g_apDrySubmixVoice->DestroyVoice();

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
void PlaySound(int label)
{
#ifdef NO_AUDIO
	return;
#endif

	if (g_aParam[label].type == BGM)
	{
		g_NowBGMLabel = label;					// 現在鳴らしているBGMラベル番号を保存
	}

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

	////if (g_aParam[label].UseFilter == TRUE)
	//{
	//	//サブミックスボイスに送信
	//	XAUDIO2_SEND_DESCRIPTOR Send[2] = { 0, g_apWetSubmixVoice,
	//										0, g_apDrySubmixVoice };
	//	XAUDIO2_VOICE_SENDS SendList = { 2, Send };
	//	g_apSourceVoice[label]->SetOutputVoices(&SendList);
	//}

	////ソースボイスの出力をサブミックスボイスに切り替え
	//XAUDIO2_SEND_DESCRIPTOR send = { 0, g_apSubmixVoice };
	//XAUDIO2_VOICE_SENDS sendlist = { 1, &send };
	//g_apSourceVoice[label]->SetOutputVoices(&sendlist);

	// オーディオバッファの登録
	g_apSourceVoice[label]->SubmitSourceBuffer(&buffer);

	// 音量調整
	g_apSourceVoice[label]->SetVolume(g_VolParam[g_aParam[label].type]);

	// 再生
	g_apSourceVoice[label]->Start(0);
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
	// 音量が1.0f以下の時は2乗カーブ
	if (volume <= 1.0f)
	{
		volume *= volume;
	}
	g_apSourceVoice[label]->SetVolume(volume);
	return;
}

//=============================================================================
// ソースボイスのボイスの再生ピッチ調整
//=============================================================================
void SetFrequencyRatio(int label, float pitch)
{
	g_apSourceVoice[label]->SetFrequencyRatio(pitch);
	return;
}

//=============================================================================
// ソースボイスの一時停止
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
// ソースボイスの再開
//=============================================================================
void ReStartSound(int label)
{
	g_apSourceVoice[label]->Start();
	return;
}


//=============================================================================
// BGMの管理
//=============================================================================
void SetBGM(MODE_LABEL mode)
{
	//return;
	if (GetMode() > MODE_LOADING)
	{
		StopSound(g_NowBGMLabel);
		switch (mode)
		{

		case MODE_OPENING:
		case MODE_LOADING:
			break;

		case MODE_TITLE_START:
			//SOUND_LABEL_BGM_TITLE,			// タイトル
			PlaySound(SOUND_LABEL_BGM_TITLE);
			break;

		case MODE_HOME:
			PlaySound(SOUND_LABEL_BGM_HOME);
			break;

		case MODE_STAGESELECT:
			PlaySound(SOUND_LABEL_BGM_STAGE_SELECT);
			break;

		case MODE_GAME:
			switch (GetStageNo())
			{
			case STAGE_OSAKA:
				PlaySound(SOUND_LABEL_BGM_STAGE_1_1);
				break;

			case STAGE_SHANGHAI:
				PlaySound(SOUND_LABEL_BGM_STAGE_2_1);
				break;

			case STAGE_PARIS:
				PlaySound(SOUND_LABEL_BGM_STAGE_3_1);
				break;

			case STAGE_NEWYORK:
				PlaySound(SOUND_LABEL_BGM_STAGE_4_1);
				break;

			default:
				break;
			}
			break;

		case MODE_RESULT:
			PlaySound(SOUND_LABEL_BGM_RESULT);
			break;

		case MODE_END:
			//PlaySound(SOUND_LABEL_BGM_ENDING);
			break;


		default:
			break;
		}
	}
	return;
}