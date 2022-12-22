//=============================================================================
//
// ファイル処理 [file.h]
// Author : 國江 翔太
//
//=============================================================================
#pragma once

#include "rocket_select.h"
#include "stage.h"

#define RECORD_NUM (3)
/*******************************************************************************
* 構造体定義
*******************************************************************************/
struct	SAVEDATA	// セーブデータの構造体
{
	ULONG64			have_pp;								// 所持金
	LOCK_STATUS		status_rocket[ROCKET_NUM];				// 所持ロケット
	ROCKET_LABEL	select_rocket;							// 選択ロケット
	int				select_char;							// 選択キャラ
	BOOL			stage_clear[STAGE_MAX];					// ステージクリア状況
	int				stage_record[STAGE_MAX][RECORD_NUM];	// ステージ最高記録

	int				sum;	// チェックサム
};

/*******************************************************************************
* プロトタイプ宣言
*******************************************************************************/
BOOL SaveData(void);
BOOL LoadData(void);
void DeleteSaveData(void);

void log(int param);

SAVEDATA* GetSaveData(void);
ULONG64 GetHavePP(void);

void InitSaveData(void);

void SaveHavePP(ULONG64 pp);
void SaveSelectCharactor(int charactor);
void SaveSelectRocket(ROCKET_LABEL rocket);
void SaveStageClear(int stage);
void SaveStageRecord(int stage, int record);
void SaveStatusRocket(ROCKET_LABEL rocket, LOCK_STATUS status);