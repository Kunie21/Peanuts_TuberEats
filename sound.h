//=============================================================================
//
// サウンド処理 [sound.h]
// Author : 武藤 颯太
//
//=============================================================================
#pragma once

#include <windows.h>
#include "xaudio2.h"			// サウンド処理で必要
#include "xaudio2fx.h"			// サウンドのエフェクト処理で必要
#include <XAPOFX.h>				// サウンドのエフェクト処理で必要

#pragma comment (lib, "xapofx.lib")


//*****************************************************************************
// サウンドファイル
//*****************************************************************************
enum 
{
	SOUND_LABEL_BGM_TITLE,			// タイトル
	SOUND_LABEL_BGM_START,			// スタート
	SOUND_LABEL_BGM_HOME,			// ホーム
	SOUND_LABEL_BGM_STAGE_SELECT,	// ステージセレクト
	SOUND_LABEL_BGM_STAGE_1_1,		// ステージ1-1
	//SOUND_LABEL_BGM_STAGE_1_2,		// ステージ1-2
	//SOUND_LABEL_BGM_STAGE_1_3,		// ステージ1-3
	//SOUND_LABEL_BGM_STAGE_2_1,		// ステージ2-1
	//SOUND_LABEL_BGM_STAGE_2_2,		// ステージ2-2
	//SOUND_LABEL_BGM_STAGE_2_3,		// ステージ2-3
	//SOUND_LABEL_BGM_STAGE_3_1,		// ステージ3-1
	//SOUND_LABEL_BGM_STAGE_3_2,		// ステージ3-2
	//SOUND_LABEL_BGM_STAGE_3_3,		// ステージ3-3
	//SOUND_LABEL_BGM_STAGE_4_1,		// ステージ4-1
	//SOUND_LABEL_BGM_STAGE_4_2,		// ステージ4-2
	//SOUND_LABEL_BGM_STAGE_4_3,		// ステージ4-3
	SOUND_LABEL_BGM_RESULT,			// リザルト
	SOUND_LABEL_BGM_ENDING,			// エンディング
	SOUND_LABEL_SE_SELECT,			// 選択音
	SOUND_LABEL_SE_DECIDE,			// 決定音
	SOUND_LABEL_SE_OPENING,			// オープニングSE?
	SOUND_LABEL_SE_ENGINE,			// エンジン音
	SOUND_LABEL_SE_COLLISION,		// 衝突音
	//SOUND_LABEL_SE_PLAYERVOICE,		// プレイヤーボイス
	SOUND_LABEL_SE_AIRLEAK,			// 空気漏れ音
	SOUND_LABEL_SE_DOOR_OPEN,		// ドアが開く音
	SOUND_LABEL_SE_ACROSS_RING,		// リング通過音
	SOUND_LABEL_SE_STAR,			// 星の音
	SOUND_LABEL_SE_POINT_ADD,		// ポイント加算

	SOUND_LABEL_MAX,
};


// フェードの状態
typedef enum
{
	AUDIOFADE_NONE = 0,				// 何もない状態
	AUDIOFADE_IN,					// フェードイン処理
	AUDIOFADE_OUT,					// フェードアウト処理
	AUDIOFADE_MAX
} AUDIOFADE;


// 使用するサウンドの種類
enum {
	MASTER,
	BGM,
	SE,
	VOICE,
	SOUND_TYPE_MAX,				// サウンド種類のMAX数
};

// 使用するエフェクト
typedef enum
{
	REVERB = 0,		// リバーブ
	ECHO,			// エコー
	EQ,				// イコライザー
	SOUND_EFFECT_MAX,				// エフェクトのMAX数
} SOUND_EFFECT;


#define BGM_VOLUME (0.7f)			//BGMの音量
#define MAX_VOLUME (1.6f)			//BGMのMAX音量
#define MIN_VOLUME (0.0f)			//BGMのMIN音量

//*****************************************************************************
// プロトタイプ宣言
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
// 音量のフェードイン・アウト処理
// 第一引数:AUDIOFADE_OUT or AUDIOFADE_IN を指定
// 第二引数:フェードをかけたいラベルを指定する GetAudioLabel()でいいと思います。
//*****************************************************************************
void SetAudioFade(AUDIOFADE audiofade,int label);

// 現在鳴っているオーディオラベル番号を取得
int GetSoundLabel(void);
// ソースボイスの音量調整
void SetSourceVolume(int label, float volume);
// ソースボイスのボイスの再生ピッチ調整
void SetFrequencyRatio(int label, float pitch);
// ソースボイスの一時停止
void PauseSound(int label);
// ソースボイスの再開
void ReStartSound(int label);