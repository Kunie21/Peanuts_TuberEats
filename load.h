//=============================================================================
//
// ���[�h���� [load.h]
// Author : ���] �đ�
//
//=============================================================================
#pragma once

enum LOAD_LABEL
{
	LOAD_TEXTURE = 0,
	LOAD_MODEL,
	LOAD_SOUND,
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
void AddLoadSum(void);