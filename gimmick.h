//=============================================================================
//
// �M�~�b�N���� [gimmick.h]
// Author : ���] �đ�
//
//=============================================================================
#pragma once

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitGimmick(void);
void UninitGimmick(void);
void UpdateGimmick(void);
void DrawGimmick(void);

bool CollisionGimmick(float oldZ, float newZ, float oldRot, float newRot);
