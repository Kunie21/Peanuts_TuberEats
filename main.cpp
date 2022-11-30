//=============================================================================
//
// メイン処理 [main.cpp]
// Author : 國江 翔太
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

// title_start
#include "title.h"
#include "start.h"

// home
#include "home.h"

// stage_select

// game
#include "game.h"

// result
#include "result.h"
#include "result_event.h"
#include "tube.h"
#include "player.h"



//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define CLASS_NAME		"TuberEatsClass"	// ウインドウのクラス名
#define WINDOW_NAME		"TuberEats"			// ウインドウのキャプション名

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow);
void Uninit(void);
void Update(void);
void Draw(void);

//*****************************************************************************
// グローバル変数:
//*****************************************************************************
static long		g_MouseX = 0;
static long		g_MouseY = 0;
static long		g_WinWidth = SCREEN_WIDTH;		// ウィンドウの横幅
static long		g_WinHeight = SCREEN_HEIGHT;	// ウィンドウの縦幅
static HWND		g_hWnd;							// ウィンドウハンドル

#ifdef _DEBUG
int		g_CountFPS;							// FPSカウンタ
char	g_DebugStr[2048] = WINDOW_NAME;		// デバッグ文字表示用
#endif

MODE_LABEL	g_Mode = MODE_LOADING;	// 起動時の画面を設定

//=============================================================================
// メイン関数
//=============================================================================
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);	// 無くても良いけど、警告が出る（未使用宣言）
	UNREFERENCED_PARAMETER(lpCmdLine);		// 無くても良いけど、警告が出る（未使用宣言）

	// 時間計測用
	DWORD dwExecLastTime;
	DWORD dwFPSLastTime;
	DWORD dwCurrentTime;
	DWORD dwFrameCount;

	HICON res_icon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));

	// 作成するウィンドウ情報の登録
	// EX版はタイトルバーにアイコンを設定することができる。
	WNDCLASSEX	wcex = {
		sizeof(WNDCLASSEX),			// 構造体のサイズ
		CS_CLASSDC,					// ウィンドウのクラススタイル（閉じるを無効にしたりできる）
		WndProc,					// ウィンドウプロシージャのアドレス（名前）
		0,							// 構造体側で確保される予備メモリ（基本的に0）
		0,							// ウィンドウ側で確保される予備メモリ（基本的に0）
		hInstance,					// インスタンスハンドル（Windows（OS）が設定してくれたこのアプリのハンドル）
		//LoadIcon(NULL, IDI_APPLICATION),	// アプリのショートカットなどで使用されるアイコンの指定（デフォルトはNULL）
		res_icon,	// アプリのショートカットなどで使用されるアイコンの指定（デフォルトはNULL）
		LoadCursor(NULL, IDC_ARROW),		// ウィンドウ上のマウスカーソルの指定（デフォルトはNULL）
		//LoadCursor(NULL, IDC_CROSS),		// ウィンドウ上のマウスカーソルの指定（デフォルトはNULL）
		(HBRUSH)(COLOR_WINDOW + 1),			// ウィンドウのクライアント領域の背景色
		NULL,						// ウィンドウ上段に設定するメニューの名前（メニューがなければNULL）
		CLASS_NAME,					// 登録時に使用するウィンドウクラスの名前
		//NULL						// タイトルバーで使用されるアイコンの指定（デフォルトはNULL）
		res_icon						// タイトルバーで使用されるアイコンの指定（デフォルトはNULL）
	};

	HWND		hWnd;	// このウィンドウのハンドルを受け取る場所の確保
	MSG			msg;	// Windowsからのメッセージを受け取る場所の確保

	// ウィンドウ情報（クラス）の登録
	RegisterClassEx(&wcex);

	// ウィンドウの作成
	hWnd = CreateWindow(CLASS_NAME,	// 設定を登録したウィンドウクラスの名前
		WINDOW_NAME,				// タイトルバーに表示されるウィンドウ名の文字列
		//WS_POPUP,
		WS_OVERLAPPEDWINDOW,		// ウィンドウの種類（スクロールバーがあるとか、サイズ固定とか）
		//(WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME),	// サイズの変更だけ排他的に禁止した
		CW_USEDEFAULT,				// ウィンドウの左X座標（CW_USEDEFAULTはシステムにお任せ）
		CW_USEDEFAULT,				// ウィンドウの上Y座標（CW_USEDEFAULTはシステムにお任せ）
		SCREEN_WIDTH + GetSystemMetrics(SM_CXDLGFRAME) * 2,										// ウィンドウ横幅
		SCREEN_HEIGHT + GetSystemMetrics(SM_CXDLGFRAME) * 2 + GetSystemMetrics(SM_CYCAPTION),	// ウィンドウ縦幅
		NULL,						// 親のウィンドウハンドル（なければNULL）
		NULL,						// このウィンドウに設定するメニューハンドル（なければNULL）
		hInstance,					// このウィンドウを保持するアプリのインスタンスハンドル（Windows（OS）が設定してくれたこのアプリのハンドル）
		NULL						// WM_CREATEメッセージでIpparamに渡したい値。CREATESTRUCT構造体のポインタ。（NULLでいい）
	);

	g_hWnd = hWnd;

	// 初期化処理(ウィンドウを作成してから行う)
	if (FAILED(Init(hInstance, hWnd, TRUE))) { return -1; }

	// ウィンドウのリサイズ
	RECT winRect, cliRect;
	GetWindowRect(hWnd, &winRect);	// ウィンドウ領域の取得
	GetClientRect(hWnd, &cliRect);	// クライアント領域の取得
	int frameWidth = (winRect.right - winRect.left) - (cliRect.right - cliRect.left);
	int frameHeight = (winRect.bottom - winRect.top) - (cliRect.bottom - cliRect.top);
	int resizeWidth = frameWidth + SCREEN_WIDTH;
	int resizeHeight = frameHeight + SCREEN_HEIGHT;
	SetWindowPos(
		hWnd,			// ウィンドウハンドル
		NULL,			// 配置順序のハンドル(NULLで良い)
		CW_USEDEFAULT,	// 表示座標X
		CW_USEDEFAULT,	// 表示座標Y
		resizeWidth,	// リサイズ横幅
		resizeHeight,	// リサイズ縦幅
		SWP_NOMOVE		// SWP_NOMOVE => 位置変更なし
	);

	// フレームカウント初期化
	timeBeginPeriod(1);	// 分解能を設定
	dwExecLastTime = dwFPSLastTime = timeGetTime();	// システム時刻をミリ秒単位で取得
	dwCurrentTime = dwFrameCount = 0;

#ifdef _DEBUG
	LARGE_INTEGER UpdateStart, UpdateEnd, DrawEnd, DrawStart;
	QueryPerformanceCounter(&UpdateStart);
	QueryPerformanceCounter(&UpdateEnd);
	QueryPerformanceCounter(&DrawEnd);
	QueryPerformanceCounter(&DrawStart);

	// メモリリークチェック用
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF | _CRTDBG_CHECK_ALWAYS_DF);
#endif

	// ウインドウの表示(初期化処理の後に呼ばないと駄目)
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	// メッセージループ
	while(1){
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)){
			if(msg.message == WM_QUIT){// PostQuitMessage()が呼ばれたらループ終了
				break;
			}
			else{	// メッセージの翻訳と送出
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else{

			dwCurrentTime = timeGetTime();

			if ((dwCurrentTime - dwFPSLastTime) >= 1000){	// 1秒ごとに実行
#ifdef _DEBUG
				g_CountFPS = dwFrameCount;
#endif
				dwFPSLastTime = dwCurrentTime;				// FPSを測定した時刻を保存
				dwFrameCount = 0;							// カウントをクリア
			}

			if ((dwCurrentTime - dwExecLastTime) >= (1000 / 60)){	// 1/60秒ごとに実行
				dwExecLastTime = dwCurrentTime;	// 処理した時刻を保存

#ifdef _DEBUG	// デバッグ版の時だけFPSを表示する
				wsprintf(g_DebugStr, WINDOW_NAME);
				wsprintf(&g_DebugStr[strlen(g_DebugStr)], " FPS:%d", g_CountFPS);

				PrintDebugProc("DrawTime:%d\n", DrawEnd.QuadPart - DrawStart.QuadPart);
				PrintDebugProc("UpdateTime:%d\n", UpdateEnd.QuadPart - UpdateStart.QuadPart);
				if (dwCurrentTime - dwFPSLastTime <= 20) QueryPerformanceCounter(&UpdateStart);
#endif
				Update();			// 更新処理
#ifdef _DEBUG
				if (dwCurrentTime - dwFPSLastTime <= 20){
					QueryPerformanceCounter(&UpdateEnd);
					QueryPerformanceCounter(&DrawStart);
				}
#endif
				Draw();				// 描画処理
#ifdef _DEBUG
				if (dwCurrentTime - dwFPSLastTime <= 20) { QueryPerformanceCounter(&DrawEnd); }

				wsprintf(&g_DebugStr[strlen(g_DebugStr)], " MX:%d MY:%d", GetMousePosX(), GetMousePosY());
				SetWindowText(hWnd, g_DebugStr);
				
#endif
				dwFrameCount++;
			}
		}
	}


	// 分解能を戻す
	timeEndPeriod(1);

	// ウィンドウクラスの登録を解除
	UnregisterClass(CLASS_NAME, wcex.hInstance);

	// 終了処理
	Uninit();

	return (int)msg.wParam;
}

//=============================================================================
// 初期化処理
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

	//InitModel();

	SetMode(g_Mode);	// 最初のモードをセット
	//SetFade(FADE_IN, MODE_OPENING);

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void Uninit(void)
{
	SetMode(MODE_END);	// 終了のモードをセット
	
	UninitModel();
	UninitTexture2D();
	UninitSound();
	UninitCamera();
	UninitInput();
	UninitRenderer();
}

//=============================================================================
// 更新処理
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

	case MODE_TITLE_START:
		UpdateStart();
		UpdateTitle();
		break;

	case MODE_HOME:
		UpdateHome();
		break;

	case MODE_STAGESELECT:
		break;

	case MODE_GAME:
	#ifdef _DEBUG
		static LARGE_INTEGER Shadow_S, Shadow_E;
		static int oldTime, nowTime;
		nowTime++;
		if (nowTime - oldTime >= 20) { QueryPerformanceCounter(&Shadow_S); }
	#endif
		UpdateGame();
	#ifdef _DEBUG
		if (nowTime - oldTime >= 20) { QueryPerformanceCounter(&Shadow_E); }
		if (nowTime - oldTime >= 20) oldTime = nowTime;
		PrintDebugProc("UpdateTimeA:%d\n", Shadow_E.QuadPart - Shadow_S.QuadPart);
	#endif
		break;

	case MODE_RESULT:
		UpdateResult();
		UpdateResultEvent();
		break;

	case MODE_END:
		break;

	default:
		break;
	}

	UpdateFade();	// フェード処理の更新

	// オーディオフェード更新処理
	//if (g_Mode > MODE_LOADING) { UpdateAudioFade(); }
}

//=============================================================================
// 描画処理
//=============================================================================
void Draw(void)
{
#ifdef _DEBUG
	static LARGE_INTEGER s1, e1;
	static int oldTime1, nowTime1;
	nowTime1++;
	PrintDebugProc("DrawTimeClear:%d\n", e1.QuadPart - s1.QuadPart);
	if (nowTime1 - oldTime1 >= 20) { QueryPerformanceCounter(&s1); }
#endif

	Clear();
	SetCamera();
	
#ifdef _DEBUG
	if (nowTime1 - oldTime1 >= 20) { QueryPerformanceCounter(&e1); oldTime1 = nowTime1; }
#endif

	switch (g_Mode)
	{
	case MODE_OPENING:
		DrawTeamLogo();
		break;

	case MODE_LOADING:
		DrawLoad();
		break;

	case MODE_TITLE_START:
		DrawTitle();
		DrawStart();
		break;

	case MODE_HOME:
		DrawHome();
		break;

	case MODE_STAGESELECT:
		break;

	case MODE_GAME:
		DrawGame();
		break;

	case MODE_RESULT:
		DrawResultEvent();
		DrawResult();	
		break;

	case MODE_END:
		break;

	default:
		break;
	}

#ifdef _DEBUG
	static LARGE_INTEGER s, e;
	static int oldTime, nowTime;
	nowTime++;
	PrintDebugProc("DrawTimeElse:%d\n", e.QuadPart - s.QuadPart);
	if (nowTime - oldTime >= 20) { QueryPerformanceCounter(&s); }
#endif

	DrawTarget();	// バックバッファをターゲットにして今描画されているものを描画

	DrawFade();	// フェード描画

	DrawTexture2DAll();	// 2Dの画像をまとめて描画

#ifdef _DEBUG
	DrawDebugProc();	// デバッグ表示
#endif

	Present();	// バックバッファ、フロントバッファ入れ替え

#ifdef _DEBUG
	if (nowTime - oldTime >= 20) { QueryPerformanceCounter(&e); oldTime = nowTime; }
#endif

}

//=============================================================================
// モードの設定
//=============================================================================
void SetMode(MODE_LABEL mode)
{
	// モードを変える前に全部メモリを解放しちゃう
	UninitTeamLogo();
	UninitResult();
	UninitResultEvent();
	UninitTube();
	UninitPlayer();
	UninitHome();
	UninitTitle();
	UninitStart();
	UninitHome();
	UninitGame();


	g_Mode = mode;	// 次のモードをセット

	switch (g_Mode)
	{
	case MODE_OPENING:
		InitTeamLogo();
		break;

	case MODE_LOADING:
		InitLoad();
		break;

	case MODE_TITLE_START:
		InitTitle();
		InitStart();
		break;

	case MODE_HOME:
		InitHome();
		break;

	case MODE_STAGESELECT:
		break;

	case MODE_GAME:
		InitGame();
		break;

	case MODE_RESULT:
		InitLight();
		InitTube();
		InitPlayer();
		InitResult();
		InitResultEvent();
		break;

	case MODE_END:
		break;

	default:
		break;
	}

	// BGMの管理（再生）
	SetBGM(g_Mode);		// Load系通るまでコメントアウトしたままで！エラー出ます！

}

// システム情報の取得関数
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
// プロシージャ
//=============================================================================
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:	// ウィンドウが閉じられた
		PostQuitMessage(0);
		break;

	case WM_KEYDOWN:	// キーが押された
		switch (wParam)
		{
		case VK_ESCAPE:	// エスケープキー
			DestroyWindow(hWnd);

			// メモリリークチェック用
			_CrtDumpMemoryLeaks();
			break;

		case VK_BACK:

			break;

		default:
			if (g_Mode == MODE_TITLE_START) PressedAnyButton();
			break;
		}
		break;

	case WM_SIZE:		// ウィンドウのサイズが変わった
		g_WinWidth = LOWORD(lParam);
		g_WinHeight = HIWORD(lParam);
		//if (g_WinWidth > 0) Resize(g_WinWidth, g_WinHeight);
		break;

	case WM_MOUSEMOVE:	// マウスが動いた
		g_MouseX = LOWORD(lParam);
		g_MouseY = HIWORD(lParam);
		break;

	case WM_TIMER:	// タイマー割り込み
		//OnTimer(TIMER_ID_LOAD);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}