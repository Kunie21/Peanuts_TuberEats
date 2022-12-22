//=============================================================================
//
// �t�@�C������ [file.cpp]
// Author : ���] �đ�
//
//=============================================================================
#include "main.h"
#include "game.h"
#include "file.h"

/*******************************************************************************
* �O���[�o���ϐ�
*******************************************************************************/
static SAVEDATA		g_SaveData;			// �Z�[�u�f�[�^
static BOOL			g_Load = FALSE;

/*******************************************************************************
�֐���:	void SaveData( void )
����:	void
�߂�l:	void
����:	�Z�[�u�f�[�^���쐬���A�t�@�C���֏o�͂���
*******************************************************************************/
BOOL SaveData(void)
{
	// �Z�[�u�f�[�^�̃`�F�b�N�T�������߂�
	{
		char *adr = (char *)&g_SaveData;	// �Z�[�u�f�[�^�̐擪�A�h���X��adr�ɓ����
		int sum = 0;				// �v�Z����`�F�b�N�T��

		g_SaveData.sum = 0;				// �`�F�b�N�T�����O�ŃN���A���Ă���

		for (int i = 0; i < sizeof(SAVEDATA); i++)
		{
			sum += adr[i];	// char�^�̂P�o�C�g�P�ʂ̃f�[�^�𑫂��Ă�
		}

		g_SaveData.sum = sum;
	}

	// SAVEDATA�\���̂��ƑS�����t�@�C���ɏo�͂���
	FILE *fp = NULL;

	printf("\n�Z�[�u�J�n�E�E�E");		// �f�o�b�O��ʂ̂�
	fp = fopen("savedata.bin", "wb");	// �t�@�C�����o�C�i���������݃��[�h��Open����

	if (fp != NULL)								// �t�@�C��������Ώ������݁A������Ζ���
	{	
		fwrite(&g_SaveData, 1, sizeof(SAVEDATA), fp);	// �w�肵���A�h���X����w�肵���o�C�g�����t�@�C���֏�������
		fclose(fp);								// Open���Ă����t�@�C�������
		printf("�I���I\n");
	}
	else
	{
		printf("�t�@�C���G���[�I\n");			// �n�[�h�������ς��A�������݋֎~�̎�
		return FALSE;	// �Z�[�u���s
	}

	return TRUE;	// �Z�[�u����
}

/*******************************************************************************
�֐���:	void LoadData( void )
����:	void
�߂�l:	void
����:	�Z�[�u�f�[�^���t�@�C������ǂݍ���
*******************************************************************************/
BOOL LoadData(void)
{
	// �t�@�C������Z�[�u�f�[�^��ǂݍ���
	FILE *fp = NULL;

	printf("\n���[�h�J�n�E�E�E");		// �f�o�b�O��ʂ̂�
	fp = fopen("savedata.bin", "rb");	// �t�@�C�����o�C�i���������݃��[�h��Open����

	if (fp != NULL)						// �t�@�C��������Ώ������݁A������Ζ���
	{
		fread(&g_SaveData, 1, sizeof(SAVEDATA), fp);	// �w�肵���A�h���X�֎w�肵���o�C�g�����t�@�C������ǂݍ���
		fclose(fp);	// Open���Ă����t�@�C�������
		printf("�I���I\n");
	}
	else
	{
		printf("�t�@�C���G���[�I\n");
		return FALSE;	// ���[�h���s
	}

	// �Z�[�u�f�[�^�̃`�F�b�N�T���������Ă��邩���ׂ�
	{
		char *adr = (char *)&g_SaveData;	// �Z�[�u�f�[�^�̐擪�A�h���X��adr�ɓ����
		int sum = 0;				// �v�Z����`�F�b�N�T��
		int org = g_SaveData.sum;			// �Z�[�u�f�[�^���̃`�F�b�N�T��

		g_SaveData.sum = 0;				// �`�F�b�N�T�����O�ŃN���A���Ă���

		for (int i = 0; i < sizeof(SAVEDATA); i++)
		{
			sum += adr[i];	// char�^�̂P�o�C�g�P�ʂ̃f�[�^�𑫂��Ă�
		}

		// ���Ƃ��Ƃ̃`�F�b�N�T���ƍČv�Z�����`�F�b�N�T�������������ׂĂ���
		if (sum != org)
		{
			//�f�[�^�������񂳂�Ă���̂Ńf�[�^�����[�h�����ɖ߂�
			return FALSE;	// ���[�h���s
		}
	}

	g_Load = TRUE;

	return TRUE;	// ���[�h����
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
		fprintf(fp, "%d\n", param);	// �w�肵���A�h���X����w�肵���o�C�g�����t�@�C���֏�������
		fclose(fp);					// Open���Ă����t�@�C�������
		printf("�I���I\n");
	}
	else
	{
		printf("�t�@�C���G���[�I\n");	// �n�[�h�������ς��A�������݋֎~�̎�
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