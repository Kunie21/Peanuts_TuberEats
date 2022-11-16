//=============================================================================
//
// ���C������ [main.h]
// Author : 
//
//=============================================================================
#pragma once


#pragma warning(push)
#pragma warning(disable:4005)

#define _CRT_SECURE_NO_WARNINGS			// scanf ��warning�h�~
#include <stdio.h>

#include <d3d11.h>
#include <d3dx11.h>
#include <d3dcompiler.h>

#include <DirectXMath.h>

//#include <pthread.h>

//#include <windows.h>


// �{���̓w�b�_�ɏ����Ȃ������ǂ�
using namespace DirectX;


#define DIRECTINPUT_VERSION 0x0800		// �x���Ώ�
#include "dinput.h"
#include "mmsystem.h"

#include "Xinput.h"        // XInput�p

#pragma warning(pop)

#ifdef _DEBUG
// ���������[�N�`�F�b�N�p
#include < cstdio >
#include < cstdlib >
#include < crtdbg.h >
#define _CRTDBG_MAP_ALLOC
#define new ::new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define malloc(X) _malloc_dbg(X,_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

//*****************************************************************************
// ���C�u�����̃����N
//*****************************************************************************
#pragma comment (lib, "d3d11.lib")		
#pragma comment (lib, "d3dcompiler.lib")
#pragma comment (lib, "d3dx11.lib")	
#pragma comment (lib, "d3dx9.lib")	
#pragma comment (lib, "winmm.lib")
#pragma comment (lib, "dxerr.lib")
#pragma comment (lib, "dxguid.lib")
#pragma comment (lib, "dinput8.lib")

#pragma comment (lib, "xinput.lib")        // XInput�p


//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define SCREEN_WIDTH	(1920)			// �E�C���h�E�̕�
#define SCREEN_HEIGHT	(1080)			// �E�C���h�E�̍���
//#define SCREEN_WIDTH	(1536)		// �E�C���h�E�̕�
//#define SCREEN_HEIGHT	(864)		// �E�C���h�E�̍���
//#define SCREEN_WIDTH	(960)			// �E�C���h�E�̕�
//#define SCREEN_HEIGHT	(540)			// �E�C���h�E�̍���
#define SCREEN_CENTER_X	(SCREEN_WIDTH / 2)	// �E�C���h�E�̒��S�w���W
#define SCREEN_CENTER_Y	(SCREEN_HEIGHT / 2)	// �E�C���h�E�̒��S�x���W

#define	MAP_W			(1280.0f)
#define	MAP_H			(1280.0f)
#define	MAP_TOP			(MAP_H/2)
#define	MAP_DOWN		(-MAP_H/2)
#define	MAP_LEFT		(-MAP_W/2)
#define	MAP_RIGHT		(MAP_W/2)

// ��ԗp�̃f�[�^�\���̂��`
struct INTERPOLATION_DATA
{
	XMFLOAT3	pos;		// ���_���W
	XMFLOAT3	rot;		// ��]
	XMFLOAT3	scl;		// �g��k��
	float		frame;		// ���s�t���[���� ( dt = 1.0f/frame )
};

struct INTERPOLATION_COLOR_DATA
{
	XMFLOAT4	color;		// 
	float		frame;		// ���s�t���[���� ( dt = 1.0f/frame )
};

enum MODE_LABEL
{
	MODE_OPENING = 0,
	MODE_LOADING,
	MODE_TITLE,
	MODE_START,
	MODE_HOME,
	MODE_STAGESELECT,
	MODE_GAME,
	MODE_RESULT,
	MODE_END,
};


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
long GetMousePosX(void);
long GetMousePosY(void);
char* GetDebugStr(void);

void SetMode(MODE_LABEL mode);
int GetMode(void);

long GetWinWidth(void);
long GetWinHeight(void);

HWND GetWindowHandle(void);