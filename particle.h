//=============================================================================
//
// �p�[�e�B�N������ [particle.h]
// Author : ���]�đ�
//
//=============================================================================
#pragma once

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitParticle(void);
void UninitParticle(void);
void UpdateParticle(void);
void DrawParticle(void);

BOOL SetEmitter(int zPosNo, int rotPosNo);
BOOL SetEmitterExp(int zPosNo, int rotPosNo);