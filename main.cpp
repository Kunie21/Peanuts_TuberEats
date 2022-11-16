//=============================================================================
//
// ���C������ [main.cpp]
// Author : ���] �đ�
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "input.h"
#include "camera.h"
#include "debugproc.h"
#include "model.h"
#include "texture2d.h"
#include "light.h"
#include "collision.h"
#include "sound.h"
#include "fade.h"
#include "resource.h"

// opening
#include "teamlogo.h"

// loading
#include "load.h"

// title
#include "title.h"

// start
#include "start.h"
// home

// stage_select
#include "stage_select.h"

// game
#include "game.h"

// result
#include "result.h"



//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define CLASS_NAME		"TuberEatsClass"	// �E�C���h�E�̃N���X��
#define WINDOW_NAME		"TuberEats"			// �E�C���h�E�̃L���v�V������

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow);
void Uninit(void);
void Update(void);
void Draw(void);

//*****************************************************************************
// �O���[�o���ϐ�:
//*****************************************************************************
static long		g_MouseX = 0;
static long		g_MouseY = 0;
static long		g_WinWidth = SCREEN_WIDTH;		// �E�B���h�E�̉���
static long		g_WinHeight = SCREEN_HEIGHT;	// �E�B���h�E�̏c��
static HWND		g_hWnd;							// �E�B���h�E�n���h��

#ifdef _DEBUG
int		g_CountFPS;							// FPS�J�E���^
char	g_DebugStr[2048] = WINDOW_NAME;		// �f�o�b�O�����\���p
#endif

MODE_LABEL	g_Mode = MODE_GAME;	// �N�����̉�ʂ�ݒ�

//=============================================================================
// ���C���֐�
//=============================================================================
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);	// �����Ă�ǂ����ǁA�x�����o��i���g�p�錾�j
	UNREFERENCED_PARAMETER(lpCmdLine);		// �����Ă�ǂ����ǁA�x�����o��i���g�p�錾�j

	// ���Ԍv���p
	DWORD dwExecLastTime;
	DWORD dwFPSLastTime;
	DWORD dwCurrentTime;
	DWORD dwFrameCount;

	HICON res_icon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));

	// �쐬����E�B���h�E���̓o�^
	// EX�ł̓^�C�g���o�[�ɃA�C�R����ݒ肷�邱�Ƃ��ł���B
	WNDCLASSEX	wcex = {
		sizeof(WNDCLASSEX),			// �\���̂̃T�C�Y
		CS_CLASSDC,					// �E�B���h�E�̃N���X�X�^�C���i����𖳌��ɂ�����ł���j
		WndProc,					// �E�B���h�E�v���V�[�W���̃A�h���X�i���O�j
		0,							// �\���̑��Ŋm�ۂ����\���������i��{�I��0�j
		0,							// �E�B���h�E���Ŋm�ۂ����\���������i��{�I��0�j
		hInstance,					// �C���X�^���X�n���h���iWindows�iOS�j���ݒ肵�Ă��ꂽ���̃A�v���̃n���h���j
		//LoadIcon(NULL, IDI_APPLICATION),	// �A�v���̃V���[�g�J�b�g�ȂǂŎg�p�����A�C�R���̎w��i�f�t�H���g��NULL�j
		res_icon,	// �A�v���̃V���[�g�J�b�g�ȂǂŎg�p�����A�C�R���̎w��i�f�t�H���g��NULL�j
		LoadCursor(NULL, IDC_ARROW),		// �E�B���h�E��̃}�E�X�J�[�\���̎w��i�f�t�H���g��NULL�j
		//LoadCursor(NULL, IDC_CROSS),		// �E�B���h�E��̃}�E�X�J�[�\���̎w��i�f�t�H���g��NULL�j
		(HBRUSH)(COLOR_WINDOW + 1),			// �E�B���h�E�̃N���C�A���g�̈�̔w�i�F
		NULL,						// �E�B���h�E��i�ɐݒ肷�郁�j���[�̖��O�i���j���[���Ȃ����NULL�j
		CLASS_NAME,					// �o�^���Ɏg�p����E�B���h�E�N���X�̖��O
		//NULL						// �^�C�g���o�[�Ŏg�p�����A�C�R���̎w��i�f�t�H���g��NULL�j
		res_icon						// �^�C�g���o�[�Ŏg�p�����A�C�R���̎w��i�f�t�H���g��NULL�j
	};

	HWND		hWnd;	// ���̃E�B���h�E�̃n���h����󂯎��ꏊ�̊m��
	MSG			msg;	// Windows����̃��b�Z�[�W��󂯎��ꏊ�̊m��

	// �E�B���h�E���i�N���X�j�̓o�^
	RegisterClassEx(&wcex);

	// �E�B���h�E�̍쐬
	hWnd = CreateWindow(CLASS_NAME,	// �ݒ��o�^�����E�B���h�E�N���X�̖��O
		WINDOW_NAME,				// �^�C�g���o�[�ɕ\�������E�B���h�E���̕�����
		//WS_POPUP,
		WS_OVERLAPPEDWINDOW,		// �E�B���h�E�̎�ށi�X�N���[���o�[������Ƃ��A�T�C�Y�Œ�Ƃ��j
		//(WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME),	// �T�C�Y�̕ύX�����r���I�ɋ֎~����
		CW_USEDEFAULT,				// �E�B���h�E�̍�X���W�iCW_USEDEFAULT�̓V�X�e���ɂ��C���j
		CW_USEDEFAULT,				// �E�B���h�E�̏�Y���W�iCW_USEDEFAULT�̓V�X�e���ɂ��C���j
		SCREEN_WIDTH + GetSystemMetrics(SM_CXDLGFRAME) * 2,										// �E�B���h�E����
		SCREEN_HEIGHT + GetSystemMetrics(SM_CXDLGFRAME) * 2 + GetSystemMetrics(SM_CYCAPTION),	// �E�B���h�E�c��
		NULL,						// �e�̃E�B���h�E�n���h���i�Ȃ����NULL�j
		NULL,						// ���̃E�B���h�E�ɐݒ肷�郁�j���[�n���h���i�Ȃ����NULL�j
		hInstance,					// ���̃E�B���h�E��ێ�����A�v���̃C���X�^���X�n���h���iWindows�iOS�j���ݒ肵�Ă��ꂽ���̃A�v���̃n���h���j
		NULL						// WM_CREATE���b�Z�[�W��Ipparam�ɓn�������l�BCREATESTRUCT�\���̂̃|�C���^�B�iNULL�ł����j
	);

	g_hWnd = hWnd;

	// ����������(�E�B���h�E��쐬���Ă���s��)
	if (FAILED(Init(hInstance, hWnd, TRUE))) { return -1; }

	// �E�B���h�E�̃��T�C�Y
	RECT winRect, cliRect;
	GetWindowRect(hWnd, &winRect);	// �E�B���h�E�̈�̎擾
	GetClientRect(hWnd, &cliRect);	// �N���C�A���g�̈�̎擾
	int frameWidth = (winRect.right - winRect.left) - (cliRect.right - cliRect.left);
	int frameHeight = (winRect.bottom - winRect.top) - (cliRect.bottom - cliRect.top);
	int resizeWidth = frameWidth + SCREEN_WIDTH;
	int resizeHeight = frameHeight + SCREEN_HEIGHT;
	SetWindowPos(
		hWnd,			// �E�B���h�E�n���h��
		NULL,			// �z�u�����̃n���h��(NULL�ŗǂ�)
		CW_USEDEFAULT,	// �\�����WX
		CW_USEDEFAULT,	// �\�����WY
		resizeWidth,	// ���T�C�Y����
		resizeHeight,	// ���T�C�Y�c��
		SWP_NOMOVE		// SWP_NOMOVE => �ʒu�ύX�Ȃ�
	);

	// �t���[���J�E���g������
	timeBeginPeriod(1);	// ����\��ݒ�
	dwExecLastTime = dwFPSLastTime = timeGetTime();	// �V�X�e��������~���b�P�ʂŎ擾
	dwCurrentTime = dwFrameCount = 0;

#ifdef _DEBUG
	LARGE_INTEGER UpdateStart, UpdateEnd, DrawEnd, DrawStart;
	QueryPerformanceCounter(&UpdateStart);
	QueryPerformanceCounter(&UpdateEnd);
	QueryPerformanceCounter(&DrawEnd);
	QueryPerformanceCounter(&DrawStart);

	// ���������[�N�`�F�b�N�p
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF | _CRTDBG_CHECK_ALWAYS_DF);
#endif

	// �E�C���h�E�̕\��(�����������̌�ɌĂ΂Ȃ��Ƒʖ�)
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	// ���b�Z�[�W���[�v
	while(1){
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)){
			if(msg.message == WM_QUIT){// PostQuitMessage()���Ă΂ꂽ�烋�[�v�I��
				break;
			}
			else{	// ���b�Z�[�W�̖|��Ƒ��o
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else{

			dwCurrentTime = timeGetTime();

			if ((dwCurrentTime - dwFPSLastTime) >= 1000){	// 1�b���ƂɎ��s
#ifdef _DEBUG
				g_CountFPS = dwFrameCount;
#endif
				dwFPSLastTime = dwCurrentTime;				// FPS�𑪒肵��������ۑ�
				dwFrameCount = 0;							// �J�E���g��N���A
			}

			if ((dwCurrentTime - dwExecLastTime) >= (1000 / 60)){	// 1/60�b���ƂɎ��s
				dwExecLastTime = dwCurrentTime;	// ��������������ۑ�

#ifdef _DEBUG	// �f�o�b�O�ł̎�����FPS��\������
				wsprintf(g_DebugStr, WINDOW_NAME);
				wsprintf(&g_DebugStr[strlen(g_DebugStr)], " FPS:%d", g_CountFPS);

				PrintDebugProc("DrawTime:%d\n", DrawEnd.QuadPart - DrawStart.QuadPart);
				PrintDebugProc("UpdateTime:%d\n", UpdateEnd.QuadPart - UpdateStart.QuadPart);
				if (dwCurrentTime - dwFPSLastTime <= 20) QueryPerformanceCounter(&UpdateStart);
#endif
				Update();			// �X�V����
#ifdef _DEBUG
				if (dwCurrentTime - dwFPSLastTime <= 20){
					QueryPerformanceCounter(&UpdateEnd);
					QueryPerformanceCounter(&DrawStart);
				}
#endif
				Draw();				// �`�揈��
#ifdef _DEBUG
				if (dwCurrentTime - dwFPSLastTime <= 20) { QueryPerformanceCounter(&DrawEnd); }

				wsprintf(&g_DebugStr[strlen(g_DebugStr)], " MX:%d MY:%d", GetMousePosX(), GetMousePosY());
				SetWindowText(hWnd, g_DebugStr);
				
				// ���������[�N�`�F�b�N�p �����t���[���ĂԂƏd��
				//_CrtDumpMemoryLeaks();
#endif
				dwFrameCount++;
			}
		}
	}


	// ����\��߂�
	timeEndPeriod(1);

	// �E�B���h�E�N���X�̓o�^����
	UnregisterClass(CLASS_NAME, wcex.hInstance);

	// �I������
	Uninit();

	return (int)msg.wParam;
}

//=============================================================================
// ����������
//=============================================================================
HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow)
{
	InitRenderer(hInstance, hWnd, bWindow);
	InitLight();
	InitCamera();
	InitInput(hInstance, hWnd);
	InitSound(hWnd);
	InitTexture2D();
	InitFade();

	SetMode(g_Mode);	// �ŏ��̃��[�h��Z�b�g
	//SetFade(FADE_IN, MODE_OPENING);

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void Uninit(void)
{
	SetMode(MODE_END);	// �I���̃��[�h��Z�b�g

	UninitTexture2D();
	UninitSound();
	UninitCamera();
	UninitInput();
	UninitRenderer();
}

//=============================================================================
// �X�V����
//=============================================================================
void Update(void)
{
	UpdateInput();
	UpdateLight();
	UpdateCamera();
	
	switch (g_Mode)
	{
	case MODE_OPENING:
		UpdateTeamLogo();
		break;
	case MODE_LOADING:
		break;
	case MODE_TITLE:
		UpdateTitle();
		break;
	case MODE_START:
		UpdateStart();
		break;
	case MODE_HOME:
		break;
	case MODE_STAGESELECT:
		UpdateStageSelect();
		break;
	case MODE_GAME:
		UpdateGame();
		break;
	case MODE_RESULT:
		UpdateResult();
		break;
	case MODE_END:
		break;
	default:
		break;
	}

	UpdateFade();	// �t�F�[�h�����̍X�V

	// �I�[�f�B�I�t�F�[�h�X�V����
	//if (g_Mode > MODE_LOADING) { UpdateAudioFade(); }
}

//=============================================================================
// �`�揈��
//=============================================================================
void Draw(void)
{
	Clear();
	SetCamera();

	switch (g_Mode)
	{
	case MODE_OPENING:
		DrawTeamLogo();
		break;
	case MODE_LOADING:
		break;
	case MODE_TITLE:
		DrawTitle();
		break;
	case MODE_START:
		DrawStart();
		break;
	case MODE_HOME:
		break;
	case MODE_STAGESELECT:
		DrawStageSelect();
		break;
	case MODE_GAME:
		DrawGame();
		break;
	case MODE_RESULT:
		DrawResult();
		break;
	case MODE_END:
		break;
	default:
		break;
	}

	DrawTarget();	// �o�b�N�o�b�t�@��^�[�Q�b�g�ɂ��č��`�悳��Ă����̂�`��

	DrawFade();	// �t�F�[�h�`��

	DrawTexture2DAll();	// 2D�̉摜��܂Ƃ߂ĕ`��

#ifdef _DEBUG
	static LARGE_INTEGER s, e;
	static int oldTime, nowTime;
	nowTime++;
	PrintDebugProc("PresentTime:%d\n", e.QuadPart - s.QuadPart);
	if (nowTime - oldTime >= 20) { QueryPerformanceCounter(&s); }
	DrawDebugProc();	// �f�o�b�O�\��
#endif

	Present();	// �o�b�N�o�b�t�@�A�t�����g�o�b�t�@����ւ�

#ifdef _DEBUG
	if (nowTime - oldTime >= 20) { QueryPerformanceCounter(&e); oldTime = nowTime; }
#endif

}

//=============================================================================
// ���[�h�̐ݒ�
//=============================================================================
void SetMode(MODE_LABEL mode)
{
	// ���[�h��ς���O�ɑS�����������������Ⴄ
	UninitTeamLogo();
	UninitResult();
	
	g_Mode = mode;	// ���̃��[�h��Z�b�g

	switch (g_Mode)
	{
	case MODE_OPENING:
		InitTeamLogo();
		break;
	case MODE_LOADING:
		break;
	case MODE_TITLE:
		InitTitle();
		break;
	case MODE_START:
		InitStart();
		break;
	case MODE_HOME:
		break;
	case MODE_STAGESELECT:
		InitStageSelect();
		break;
	case MODE_GAME:
		InitGame();
		break;
	case MODE_RESULT:
		InitResult();
		break;
	case MODE_END:
		break;
	default:
		break;
	}
}

// �V�X�e�����̎擾�֐�
long GetMousePosX(void) { return g_MouseX; }
long GetMousePosY(void) { return g_MouseY; }
long GetWinWidth(void) { return g_WinWidth; }
long GetWinHeight(void) { return g_WinHeight; }
HWND GetWindowHandle(void) { return g_hWnd; }
int GetMode(void) { return g_Mode; }

#ifdef _DEBUG
char* GetDebugStr(void) { return g_DebugStr; }
#endif

//=============================================================================
// �v���V�[�W��
//=============================================================================
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:	// �E�B���h�E������ꂽ
		PostQuitMessage(0);
		break;

	case WM_KEYDOWN:	// �L�[�������ꂽ
		switch (wParam)
		{
		case VK_ESCAPE:	// �G�X�P�[�v�L�[
			DestroyWindow(hWnd);
			break;
		}
		break;

	case WM_SIZE:		// �E�B���h�E�̃T�C�Y���ς����
		g_WinWidth = LOWORD(lParam);
		g_WinHeight = HIWORD(lParam);
		//if (g_WinWidth > 0) Resize(g_WinWidth, g_WinHeight);
		break;

	case WM_MOUSEMOVE:	// �}�E�X��������
		g_MouseX = LOWORD(lParam);
		g_MouseY = HIWORD(lParam);
		break;

	case WM_TIMER:	// �^�C�}�[���荞��
		//OnTimer(TIMER_ID_LOAD);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}