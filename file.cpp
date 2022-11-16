////=============================================================================
////
//// �t�@�C������ [file.cpp]
//// Author : ���] �đ�
////
////=============================================================================
//#include "main.h"
//#include "game.h"
//#include "file.h"
//
///*******************************************************************************
//* �O���[�o���ϐ�
//*******************************************************************************/
//static SAVEDATA		g_SaveData;		// �Z�[�u�f�[�^�쐬�ꏊ
//
///*******************************************************************************
//�֐���:	void SaveData( void )
//����:	void
//�߂�l:	void
//���:	�Z�[�u�f�[�^��쐬���A�t�@�C���֏o�͂���
//*******************************************************************************/
//BOOL SaveData(void)
//{
//	// �X�R�A�L�^�f�[�^
//	{
//		int *record = GetRecord();
//
//		// �Z�[�u
//		for (int i = 0; i < STAGE_MAX; i++)
//		{
//			for (int j = 0; j < RECORD_MAX; j++)
//			{
//				g_SaveData.record[i][j] = record[i * RECORD_MAX + j];
//			}
//		}
//	}
//	// �N���A�L�^�f�[�^
//	{
//		BOOL *clear = GetStageClear();
//
//		// �Z�[�u
//		for (int i = 0; i < STAGE_MAX; i++)
//		{
//			g_SaveData.clear[i] = clear[i];
//		}
//	}
//
//	// �Z�[�u�f�[�^�̃`�F�b�N�T������߂�
//	{
//		char *adr = (char *)&g_SaveData;	// �Z�[�u�f�[�^�̐擪�A�h���X��adr�ɓ����
//		int sum = 0;				// �v�Z����`�F�b�N�T��
//
//		g_SaveData.sum = 0;				// �`�F�b�N�T����O�ŃN���A���Ă���
//
//		for (int i = 0; i < sizeof(SAVEDATA); i++)
//		{
//			sum += adr[i];	// char�^�̂P�o�C�g�P�ʂ̃f�[�^�𑫂��Ă�
//		}
//
//		g_SaveData.sum = sum;
//	}
//
//	// SAVEDATA�\���̂��ƑS����t�@�C���ɏo�͂���
//	FILE *fp = NULL;
//
//	printf("\n�Z�[�u�J�n�E�E�E");		// �f�o�b�O��ʂ̂�
//	fp = fopen("savedata.bin", "wb");	// �t�@�C����o�C�i���������݃��[�h��Open����
//
//	if (fp != NULL)								// �t�@�C��������Ώ������݁A������Ζ���
//	{	
//		fwrite(&g_SaveData, 1, sizeof(SAVEDATA), fp);	// �w�肵���A�h���X����w�肵���o�C�g�����t�@�C���֏�������
//		fclose(fp);								// Open���Ă����t�@�C�������
//		printf("�I���I\n");
//	}
//	else
//	{
//		printf("�t�@�C���G���[�I\n");			// �n�[�h�������ς��A�������݋֎~�̎�
//		return FALSE;	// �Z�[�u���s
//	}
//
//	return TRUE;	// �Z�[�u����
//}
//
///*******************************************************************************
//�֐���:	void LoadData( void )
//����:	void
//�߂�l:	void
//���:	�Z�[�u�f�[�^��t�@�C������ǂݍ���
//*******************************************************************************/
//BOOL LoadData(void)
//{
//	// �t�@�C������Z�[�u�f�[�^��ǂݍ���
//	FILE *fp = NULL;
//
//	printf("\n���[�h�J�n�E�E�E");		// �f�o�b�O��ʂ̂�
//	fp = fopen("savedata.bin", "rb");	// �t�@�C����o�C�i���������݃��[�h��Open����
//
//	if (fp != NULL)						// �t�@�C��������Ώ������݁A������Ζ���
//	{
//		fread(&g_SaveData, 1, sizeof(SAVEDATA), fp);	// �w�肵���A�h���X�֎w�肵���o�C�g�����t�@�C������ǂݍ���
//		fclose(fp);	// Open���Ă����t�@�C�������
//		printf("�I���I\n");
//	}
//	else
//	{
//		printf("�t�@�C���G���[�I\n");
//		return FALSE;	// ���[�h���s
//	}
//
//	// �Z�[�u�f�[�^�̃`�F�b�N�T���������Ă��邩���ׂ�
//	{
//		char *adr = (char *)&g_SaveData;	// �Z�[�u�f�[�^�̐擪�A�h���X��adr�ɓ����
//		int sum = 0;				// �v�Z����`�F�b�N�T��
//		int org = g_SaveData.sum;			// �Z�[�u�f�[�^��̃`�F�b�N�T��
//
//		g_SaveData.sum = 0;				// �`�F�b�N�T����O�ŃN���A���Ă���
//
//		for (int i = 0; i < sizeof(SAVEDATA); i++)
//		{
//			sum += adr[i];	// char�^�̂P�o�C�g�P�ʂ̃f�[�^�𑫂��Ă�
//		}
//
//		// ��Ƃ�Ƃ̃`�F�b�N�T���ƍČv�Z�����`�F�b�N�T�������������ׂĂ���
//		if (sum != org)
//		{
//			//�f�[�^�������񂳂�Ă���̂Ńf�[�^����[�h�����ɖ߂�
//			return FALSE;	// ���[�h���s
//		}
//	}
//
//	// �X�R�A�L�^�f�[�^
//	{
//		int *record = GetRecord();
//
//		// �Z�[�u
//		for (int i = 0; i < STAGE_MAX; i++)
//		{
//			for (int j = 0; j < RECORD_MAX; j++)
//			{
//				record[i * RECORD_MAX + j] = g_SaveData.record[i][j];
//			}
//		}
//	}
//	// �N���A�L�^�f�[�^
//	{
//		BOOL *clear = GetStageClear();
//
//		// �Z�[�u
//		for (int i = 0; i < STAGE_MAX; i++)
//		{
//			clear[i] = g_SaveData.clear[i];
//		}
//	}
//
//	return TRUE;	// ���[�h����
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
//		fprintf(fp, "%d\n", param);	// �w�肵���A�h���X����w�肵���o�C�g�����t�@�C���֏�������
//		fclose(fp);					// Open���Ă����t�@�C�������
//		printf("�I���I\n");
//	}
//	else
//	{
//		printf("�t�@�C���G���[�I\n");	// �n�[�h�������ς��A�������݋֎~�̎�
//	}
//}