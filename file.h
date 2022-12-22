//=============================================================================
//
// �t�@�C������ [file.h]
// Author : ���] �đ�
//
//=============================================================================
#pragma once

#include "rocket_select.h"
#include "stage.h"

#define RECORD_NUM (3)
/*******************************************************************************
* �\���̒�`
*******************************************************************************/
struct	SAVEDATA	// �Z�[�u�f�[�^�̍\����
{
	ULONG64			have_pp;								// ������
	LOCK_STATUS		status_rocket[ROCKET_NUM];				// �������P�b�g
	ROCKET_LABEL	select_rocket;							// �I�����P�b�g
	int				select_char;							// �I���L����
	BOOL			stage_clear[STAGE_MAX];					// �X�e�[�W�N���A��
	int				stage_record[STAGE_MAX][RECORD_NUM];	// �X�e�[�W�ō��L�^

	int				sum;	// �`�F�b�N�T��
};

/*******************************************************************************
* �v���g�^�C�v�錾
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