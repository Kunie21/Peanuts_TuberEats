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

#include "main.h"

#pragma comment (lib, "xapofx.lib")


//*****************************************************************************
// サウンドファイル
//*****************************************************************************
enum 
{
	SOUND_LABEL_BGM_TITLE,			// タイトル
	SOUND_LABEL_BGM_HOME,			// ホーム
	SOUND_LABEL_BGM_STAGE_SELECT,	// ステージセレクト
	SOUND_LABEL_BGM_STAGE_1_1,		// ステージ1-1
	//SOUND_LABEL_BGM_STAGE_1_2,		// ステージ1-2
	//SOUND_LABEL_BGM_STAGE_1_3,		// ステージ1-3
	SOUND_LABEL_BGM_STAGE_2_1,		// ステージ2-1
	//SOUND_LABEL_BGM_STAGE_2_2,		// ステージ2-2
	//SOUND_LABEL_BGM_STAGE_2_3,		// ステージ2-3
	SOUND_LABEL_BGM_STAGE_3_1,		// ステージ3-1
	//SOUND_LABEL_BGM_STAGE_3_2,		// ステージ3-2
	//SOUND_LABEL_BGM_STAGE_3_3,		// ステージ3-3
	SOUND_LABEL_BGM_STAGE_4_1,		// ステージ4-1
	//SOUND_LABEL_BGM_STAGE_4_2,		// ステージ4-2
	//SOUND_LABEL_BGM_STAGE_4_3,		// ステージ4-3
	SOUND_LABEL_BGM_RESULT,			// リザルト
	SOUND_LABEL_BGM_ENDING,			// エンディング


	SOUND_LABEL_BGM_ENGINE1,			// エンジン小
	SOUND_LABEL_BGM_ENGINE2,			// エンジン中
	SOUND_LABEL_BGM_ENGINE3,			// エンジン強

	SOUND_LABEL_SE_AIRLEAK,			// 空気漏れ音
	SOUND_LABEL_SE_BACK,			// 戻る
	SOUND_LABEL_SE_BARRIER,			// 無敵バリア
	SOUND_LABEL_SE_BREAK_ICE,		// 氷を破壊
	SOUND_LABEL_SE_CHANGE_ROCKET,	// ロケットチェンジ
	SOUND_LABEL_SE_COLLISION,		// 氷に衝突
	SOUND_LABEL_SE_COLLISION_BLAST,	// 噴出孔に衝突
	SOUND_LABEL_SE_COUNTDOWN,		// カウントダウン
	SOUND_LABEL_SE_CURSOR,			// メニューカーソル
	SOUND_LABEL_SE_DECIDE,			// 決定
	SOUND_LABEL_SE_EQUIP,			// ロケット装備
	SOUND_LABEL_SE_GET_DONUT,		// ドーナッツ取得
	SOUND_LABEL_SE_GET_RAMEN,		// ラーメン取得
	SOUND_LABEL_SE_GET_SUSHI,		// 寿司取得
	SOUND_LABEL_SE_GOAL,			// ゴール
	SOUND_LABEL_SE_LAUNCH_SUSHI,	// 寿司ミサイル発射
	SOUND_LABEL_SE_PIN,				// 地球のピン
	SOUND_LABEL_SE_REFUSE,			// クリック拒否
	SOUND_LABEL_SE_SPIN_EARTH,		// 地球回転
	SOUND_LABEL_SE_START_DELIVER,	// 配達スタート
	SOUND_LABEL_SE_START_GO,		// 発進
	SOUND_LABEL_SE_THROUGH_RING,	// リング通過

	//SOUND_LABEL_SE_SELECT,			// 選択音
	//SOUND_LABEL_SE_DECIDE,			// 決定音
	//SOUND_LABEL_SE_OPENING,			// オープニングSE?
	//SOUND_LABEL_SE_ENGINE,			// エンジン音
	//SOUND_LABEL_SE_COLLISION,		// 衝突音
	////SOUND_LABEL_SE_PLAYERVOICE,		// プレイヤーボイス
	//SOUND_LABEL_SE_DOOR_OPEN,		// ドアが開く音
	//SOUND_LABEL_SE_ACROSS_RING,		// リング通過音
	//SOUND_LABEL_SE_STAR,			// 星の音
	//SOUND_LABEL_SE_POINT_ADD,		// ポイント加算

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

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
BOOL InitSound(HWND hWnd);
void UninitSound(void);
void PlaySound(int label);
void StopSound(int label);
void StopSound(void);

void UpdateAudioFade(void);

BOOL LoadSoundKernel(void);

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
// BGMの管理
void SetBGM(MODE_LABEL mode);