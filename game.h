//=============================================================================
//
// �Q�[����ʏ��� [game.h]
// Author : ���] �đ�
//
//=============================================================================
#pragma once

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitGame(void);
void UninitGame(void);
void UpdateGame(void);
void DrawGame(void);

BOOL LoadGameTextureKernel(float* loadPalam, int* loadSum);
BOOL LoadGameTexture(void);