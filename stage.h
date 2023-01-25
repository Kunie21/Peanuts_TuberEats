//=============================================================================
//
// �X�e�[�W���� [stage.h]
// Author : ���] �đ�
//
//=============================================================================
#pragma once

#include "gimmick.h"
#include "renderer.h"

//*****************************************************************************
// �\���̒�`
//*****************************************************************************
struct CURVE
{
	int zPosNo;		// �ʒu
	XMFLOAT2 angle;	// �p�x
};

struct STAGE
{
	GIMMICK* arrGmk = NULL;	// �M�~�b�N�̔z��
	CURVE* arrCrv = NULL;	// �J�[�u�̔z��
	int gmkNum;				// �M�~�b�N��
	int crvNum;				// �J�[�u��
	int length;				// �p�C�v�̒���
	//int corner = 7;			// �p�C�v�f�ʂ̒��_��
};

struct STAGE2
{
	GIMMICK* arrGmk = NULL;	// �M�~�b�N�̔z��
	int gmkNum;				// �M�~�b�N��
	float goal;				// �S�[���̈ʒu
};

enum STAGE_LABEL
{
	STAGE_OSAKA =0,
	STAGE_HOKKAIDO,
	STAGE_OKINAWA,

	STAGE_CHINA,
	STAGE_KOREA,
	STAGE_SINGAPORE,

	STAGE_FRANCE,
	STAGE_ENGLAND,
	STAGE_ITALY,

	STAGE_AMERICA,
	STAGE_CANADA,
	STAGE_MEXICO,

	STAGE_MAX
};

#define TUBE_ROT_NUM	(8)		// �M�~�b�N��z�u����p�x�̐�
#define TUBE_ZPS_SCL	(10)	// zPosNo�̔{���i10�����ɐݒu���Ă���j
#define TUBE_ZPS_NUM	(100)	// 10 * 100 �Ŏ��ۂ�zPosNo1000������
struct STAGE_TABLE {
	int			gimmick[TUBE_ROT_NUM];
	XMFLOAT2	curve;
};

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitStage(void);
void UninitStage(void);
void UpdateStage(void);
void DrawStage(void);

STAGE* GetStage(int stageNo);
STAGE2* GetStage2(int stageNo);
STAGE2* GetStage2(void);
STAGE_TABLE* GetStageTable(int stageNo);
void SetStageCurve(float zPos, float spd);
CURVE_BUFFER* GetCurveBuffer(void);

float GetZPos(int zPosNo);
float GetRotPos(int rotPosNo);

void SetStage(int stage);
int GetStageNo(void);
float GetBaseDeliveryTime(void);
BOOL CheckGoal(float oldZ, float newZ);