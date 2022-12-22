//=============================================================================
//
// ファイル処理 [file.cpp]
// Author : 國江 翔太
//
//=============================================================================
#include "main.h"
#include "game.h"
#include "file.h"

/*******************************************************************************
* グローバル変数
*******************************************************************************/
static SAVEDATA		g_SaveData;			// セーブデータ
static BOOL			g_Load = FALSE;

/*******************************************************************************
関数名:	void SaveData( void )
引数:	void
戻り値:	void
説明:	セーブデータを作成し、ファイルへ出力する
*******************************************************************************/
BOOL SaveData(void)
{
	// セーブデータのチェックサムを求める
	{
		char *adr = (char *)&g_SaveData;	// セーブデータの先頭アドレスをadrに入れる
		int sum = 0;				// 計算するチェックサム

		g_SaveData.sum = 0;				// チェックサムを０でクリアしていく

		for (int i = 0; i < sizeof(SAVEDATA); i++)
		{
			sum += adr[i];	// char型の１バイト単位のデータを足してく
		}

		g_SaveData.sum = sum;
	}

	// SAVEDATA構造体ごと全部をファイルに出力する
	FILE *fp = NULL;

	printf("\nセーブ開始・・・");		// デバッグ画面のみ
	fp = fopen("savedata.bin", "wb");	// ファイルをバイナリ書き込みモードでOpenする

	if (fp != NULL)								// ファイルがあれば書き込み、無ければ無視
	{	
		fwrite(&g_SaveData, 1, sizeof(SAVEDATA), fp);	// 指定したアドレスから指定したバイト数分ファイルへ書き込む
		fclose(fp);								// Openしていたファイルを閉じる
		printf("終了！\n");
	}
	else
	{
		printf("ファイルエラー！\n");			// ハードがいっぱい、書き込み禁止の時
		return FALSE;	// セーブ失敗
	}

	return TRUE;	// セーブ成功
}

/*******************************************************************************
関数名:	void LoadData( void )
引数:	void
戻り値:	void
説明:	セーブデータをファイルから読み込む
*******************************************************************************/
BOOL LoadData(void)
{
	// ファイルからセーブデータを読み込む
	FILE *fp = NULL;

	printf("\nロード開始・・・");		// デバッグ画面のみ
	fp = fopen("savedata.bin", "rb");	// ファイルをバイナリ書き込みモードでOpenする

	if (fp != NULL)						// ファイルがあれば書き込み、無ければ無視
	{
		fread(&g_SaveData, 1, sizeof(SAVEDATA), fp);	// 指定したアドレスへ指定したバイト数分ファイルから読み込む
		fclose(fp);	// Openしていたファイルを閉じる
		printf("終了！\n");
	}
	else
	{
		printf("ファイルエラー！\n");
		return FALSE;	// ロード失敗
	}

	// セーブデータのチェックサムがあっているか調べる
	{
		char *adr = (char *)&g_SaveData;	// セーブデータの先頭アドレスをadrに入れる
		int sum = 0;				// 計算するチェックサム
		int org = g_SaveData.sum;			// セーブデータ内のチェックサム

		g_SaveData.sum = 0;				// チェックサムを０でクリアしていく

		for (int i = 0; i < sizeof(SAVEDATA); i++)
		{
			sum += adr[i];	// char型の１バイト単位のデータを足してく
		}

		// もともとのチェックサムと再計算したチェックサムが同じか調べている
		if (sum != org)
		{
			//データが改ざんされているのでデータをロードせずに戻る
			return FALSE;	// ロード失敗
		}
	}

	g_Load = TRUE;

	return TRUE;	// ロード成功
}

void DeleteSaveData(void)
{
	remove("savedata.bin");
	g_Load = FALSE;
	//ResetRecord();
}
void log(int param)
{
	FILE *fp = NULL;

	fp = fopen("log.txt", "a");

	if (fp != NULL)
	{
		fprintf(fp, "%d\n", param);	// 指定したアドレスから指定したバイト数分ファイルへ書き込む
		fclose(fp);					// Openしていたファイルを閉じる
		printf("終了！\n");
	}
	else
	{
		printf("ファイルエラー！\n");	// ハードがいっぱい、書き込み禁止の時
	}
}

SAVEDATA* GetSaveData(void) {
	return &g_SaveData;
}
ULONG64 GetHavePP(void) {
	return g_SaveData.have_pp;
}

void InitSaveData(void) {
	g_SaveData.have_pp = 0;
	g_SaveData.select_char = 0;
	g_SaveData.select_rocket = ROCKET01;
	for (int i = 0; i < STAGE_MAX; i++) {
		g_SaveData.stage_clear[i] = FALSE;
	}
	for (int i = 0; i < STAGE_MAX; i++) {
		for (int j = 0; j < RECORD_NUM; j++) {
			g_SaveData.stage_record[i][j] = 0;
		}
	}
	g_SaveData.status_rocket[ROCKET01] = STATUS_EQUIP;
	g_SaveData.status_rocket[ROCKET02] = STATUS_LOCK;
	g_SaveData.status_rocket[ROCKET03] = STATUS_LOCK;
	g_SaveData.status_rocket[ROCKET04] = STATUS_LOCK;
}

void SaveHavePP(ULONG64 pp) {
	g_SaveData.have_pp = pp;
}
void SaveSelectCharactor(int charactor) {
	g_SaveData.select_char = charactor;
}
void SaveSelectRocket(ROCKET_LABEL rocket) {
	g_SaveData.select_rocket = rocket;
}
void SaveStageClear(int stage) {
	g_SaveData.stage_clear[stage] = TRUE;
}
void SaveStageRecord(int stage, int record) {
	for (int j = 0; j < RECORD_NUM; j++) {
		if (g_SaveData.stage_record[stage][j] > record) {
			continue;
		}
		else {
			for (int i = RECORD_NUM - 1; i > j; i--) {
				g_SaveData.stage_record[stage][i] = g_SaveData.stage_record[stage][i - 1];
			}
			g_SaveData.stage_record[stage][j] = record;
			return;
		}
	}
}
void SaveStatusRocket(ROCKET_LABEL rocket, LOCK_STATUS status) {
	g_SaveData.status_rocket[rocket] = status;
}