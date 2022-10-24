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


// フェードの状態
typedef enum
{
	AUDIOFADE_NONE = 0,				// 何もない状態
	AUDIOFADE_IN,					// フェードイン処理
	AUDIOFADE_OUT,					// フェードアウト処理
	AUDIOFADE_MAX
} AUDIOFADE;




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
int GetAudioLabel(void);

