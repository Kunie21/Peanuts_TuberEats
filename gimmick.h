//=============================================================================
//
// �M�~�b�N���� [gimmick.h]
// Author : ���] �đ�
//
//=============================================================================
#pragma once

//*****************************************************************************
// �\���̒�`
//*****************************************************************************
enum GIMMICK_TYPE
{
	GIMMICK_ICE = 0,
	GIMMICK_RING,

	GIMMICK_MAX
};

struct GIMMICK
{
	int rotPosNo = 0;
	int zPosNo = 0;
	float rotSizeHalf = XM_PIDIV4;
	GIMMICK_TYPE type = GIMMICK_ICE;
};

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitGimmick(void);
void UninitGimmick(void);
void UpdateGimmick(void);
void DrawGimmick(void);

bool CollisionGimmick(int stageNo, float oldZ, float newZ, float oldRot, float newRot);
