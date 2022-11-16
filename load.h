//=============================================================================
//
// ���[�h���� [load.h]
// Author : ���] �đ�
//
//=============================================================================
#pragma once

enum LOAD_LABEL
{
	LOAD_GAMETEXTURE = 0,
	LOAD_SOUND,
	LOAD_OBJECT,
	LOAD_SKYDOME,
	LOAD_WALL,
	LOAD_HOLD,
	LOAD_EXHOLD,
	LOAD_PARTICLE,
	LOAD_STARPARTICLE,
	LOAD_EFFECT,
	LOAD_ARROW,
	LOAD_PLAYER,
	LOAD_MAX,
};

/*******************************************************************************
* �v���g�^�C�v�錾
*******************************************************************************/
void InitLoad(void);
void UpdateLoad(void);
BOOL LoadingNow(void);
void DrawLoad(void);
DWORD WINAPI ThreadFuncLoad(LPVOID pParam);