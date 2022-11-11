//=============================================================================
//
// �X�e�[�W���� [stage.h]
// Author : ���] �đ�
//
//=============================================================================
#pragma once

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
	int corner = 7;			// �p�C�v�f�ʂ̒��_��
};

enum STAGE_LABEL
{
	STAGE_OSAKA =0,
	STAGE_SHANGHAI,
	STAGE_PARIS,
	STAGE_NEWYORK,
	STAGE_MAX
};

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitStage(void);
void UninitStage(void);
void UpdateStage(void);
void DrawStage(void);

STAGE* GetStage(int stageNo);
void SetStageCurve(int stageNo, float zPos);