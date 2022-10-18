////=============================================================================
////
//// ファイル処理 [file.cpp]
//// Author : 國江 翔太
////
////=============================================================================
//#include "main.h"
//#include "game.h"
//#include "file.h"
//
///*******************************************************************************
//* グローバル変数
//*******************************************************************************/
//static SAVEDATA		g_SaveData;		// セーブデータ作成場所
//
///*******************************************************************************
//関数名:	void SaveData( void )
//引数:	void
//戻り値:	void
//説明:	セーブデータを作成し、ファイルへ出力する
//*******************************************************************************/
//BOOL SaveData(void)
//{
//	// スコア記録データ
//	{
//		int *record = GetRecord();
//
//		// セーブ
//		for (int i = 0; i < STAGE_MAX; i++)
//		{
//			for (int j = 0; j < RECORD_MAX; j++)
//			{
//				g_SaveData.record[i][j] = record[i * RECORD_MAX + j];
//			}
//		}
//	}
//	// クリア記録データ
//	{
//		BOOL *clear = GetStageClear();
//
//		// セーブ
//		for (int i = 0; i < STAGE_MAX; i++)
//		{
//			g_SaveData.clear[i] = clear[i];
//		}
//	}
//
//	// セーブデータのチェックサムを求める
//	{
//		char *adr = (char *)&g_SaveData;	// セーブデータの先頭アドレスをadrに入れる
//		int sum = 0;				// 計算するチェックサム
//
//		g_SaveData.sum = 0;				// チェックサムを０でクリアしていく
//
//		for (int i = 0; i < sizeof(SAVEDATA); i++)
//		{
//			sum += adr[i];	// char型の１バイト単位のデータを足してく
//		}
//
//		g_SaveData.sum = sum;
//	}
//
//	// SAVEDATA構造体ごと全部をファイルに出力する
//	FILE *fp = NULL;
//
//	printf("\nセーブ開始・・・");		// デバッグ画面のみ
//	fp = fopen("savedata.bin", "wb");	// ファイルをバイナリ書き込みモードでOpenする
//
//	if (fp != NULL)								// ファイルがあれば書き込み、無ければ無視
//	{	
//		fwrite(&g_SaveData, 1, sizeof(SAVEDATA), fp);	// 指定したアドレスから指定したバイト数分ファイルへ書き込む
//		fclose(fp);								// Openしていたファイルを閉じる
//		printf("終了！\n");
//	}
//	else
//	{
//		printf("ファイルエラー！\n");			// ハードがいっぱい、書き込み禁止の時
//		return FALSE;	// セーブ失敗
//	}
//
//	return TRUE;	// セーブ成功
//}
//
///*******************************************************************************
//関数名:	void LoadData( void )
//引数:	void
//戻り値:	void
//説明:	セーブデータをファイルから読み込む
//*******************************************************************************/
//BOOL LoadData(void)
//{
//	// ファイルからセーブデータを読み込む
//	FILE *fp = NULL;
//
//	printf("\nロード開始・・・");		// デバッグ画面のみ
//	fp = fopen("savedata.bin", "rb");	// ファイルをバイナリ書き込みモードでOpenする
//
//	if (fp != NULL)						// ファイルがあれば書き込み、無ければ無視
//	{
//		fread(&g_SaveData, 1, sizeof(SAVEDATA), fp);	// 指定したアドレスへ指定したバイト数分ファイルから読み込む
//		fclose(fp);	// Openしていたファイルを閉じる
//		printf("終了！\n");
//	}
//	else
//	{
//		printf("ファイルエラー！\n");
//		return FALSE;	// ロード失敗
//	}
//
//	// セーブデータのチェックサムがあっているか調べる
//	{
//		char *adr = (char *)&g_SaveData;	// セーブデータの先頭アドレスをadrに入れる
//		int sum = 0;				// 計算するチェックサム
//		int org = g_SaveData.sum;			// セーブデータ内のチェックサム
//
//		g_SaveData.sum = 0;				// チェックサムを０でクリアしていく
//
//		for (int i = 0; i < sizeof(SAVEDATA); i++)
//		{
//			sum += adr[i];	// char型の１バイト単位のデータを足してく
//		}
//
//		// もともとのチェックサムと再計算したチェックサムが同じか調べている
//		if (sum != org)
//		{
//			//データが改ざんされているのでデータをロードせずに戻る
//			return FALSE;	// ロード失敗
//		}
//	}
//
//	// スコア記録データ
//	{
//		int *record = GetRecord();
//
//		// セーブ
//		for (int i = 0; i < STAGE_MAX; i++)
//		{
//			for (int j = 0; j < RECORD_MAX; j++)
//			{
//				record[i * RECORD_MAX + j] = g_SaveData.record[i][j];
//			}
//		}
//	}
//	// クリア記録データ
//	{
//		BOOL *clear = GetStageClear();
//
//		// セーブ
//		for (int i = 0; i < STAGE_MAX; i++)
//		{
//			clear[i] = g_SaveData.clear[i];
//		}
//	}
//
//	return TRUE;	// ロード成功
//}
//
//void DeleteSaveData(void)
//{
//	remove("savedata.bin");
//	ResetRecord();
//}
//void log(int param)
//{
//	FILE *fp = NULL;
//
//	fp = fopen("log.txt", "a");
//
//	if (fp != NULL)
//	{
//		fprintf(fp, "%d\n", param);	// 指定したアドレスから指定したバイト数分ファイルへ書き込む
//		fclose(fp);					// Openしていたファイルを閉じる
//		printf("終了！\n");
//	}
//	else
//	{
//		printf("ファイルエラー！\n");	// ハードがいっぱい、書き込み禁止の時
//	}
//}