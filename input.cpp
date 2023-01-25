//=============================================================================
//
// 入力処理 [input.cpp]
// Author : 國江 翔太
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "input.h"
#include "debugproc.h"
#include "sound.h"

static BOOL g_bSkip = FALSE;
//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	NUM_KEY_MAX		(256)

// game pad用設定値
#define DEADZONE		(2500)		// 各軸の25%を無効ゾーンとする
#define RANGE_MAX		(1000)		// 有効範囲の最大値
#define RANGE_MIN		(-1000)		// 有効範囲の最小値
#define BUTTON_WAIT		(15)		// ボタン入力時のウェイトタイム


#define XINPUT_PAD_CHECK_COUNT	60			// パッド接続状況をチェックする間隔（フレーム数）
#define XINPUT_PAD_STICK_MAX_VALUE	32767	// XInputのスティックが返す最大値

#define XINPUT_BUTTON_REPEAT_START	30	// リピートを始めるフレーム数
#define XINPUT_BUTTON_REPEAT_NEXT	15	// リピートするフレーム数


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
// キーボード
HRESULT InitKeyboard(HINSTANCE hInst, HWND hWnd);
void UninitKeyboard(void);
HRESULT UpdateKeyboard(void);

// マウス
HRESULT InitializeMouse(HINSTANCE hInst, HWND hWindow); // マウスの初期化
void UninitMouse();		// マウスの終了処理
HRESULT UpdateMouse();	// マウスの更新処理

// ゲームパッド
HRESULT InitializePad(void);			// パッド初期化
//BOOL CALLBACK SearchPadCallback(LPDIDEVICEINSTANCE lpddi, LPVOID);	// パッド検査コールバック
void UpdatePad(void);
void UninitPad(void);

//*****************************************************************************
// グローバル変数
//*****************************************************************************
// キーボード
LPDIRECTINPUT8				g_pDInput = NULL;					// IDirectInput8インターフェースへのポインタ
LPDIRECTINPUTDEVICE8		g_pDIDevKeyboard = NULL;			// IDirectInputDevice8インターフェースへのポインタ(キーボード)
BYTE						g_keyState[NUM_KEY_MAX];			// キーボードの状態を受け取るワーク
BYTE						g_keyStateTrigger[NUM_KEY_MAX];		// キーボードの状態を受け取るワーク
BYTE						g_keyStateRepeat[NUM_KEY_MAX];		// キーボードの状態を受け取るワーク
BYTE						g_keyStateRelease[NUM_KEY_MAX];		// キーボードの状態を受け取るワーク
int							g_keyStateRepeatCnt[NUM_KEY_MAX];	// キーボードのリピートカウンタ

// マウス
static LPDIRECTINPUTDEVICE8	pMouse = NULL;				// mouse
static DIMOUSESTATE2		mouseState;					// マウスのダイレクトな状態
static DIMOUSESTATE2		mouseTrigger;				// 押された瞬間だけON
static BOOL					g_MouseUse = TRUE;			// マウスの使用状況

// ゲームパッド
static LPDIRECTINPUTDEVICE8	pGamePad[GAMEPADMAX] = { NULL, NULL, NULL, NULL };	// パッドデバイス
static DWORD				padState[GAMEPADMAX];		// パッド情報（複数対応）
static DWORD				padTrigger[GAMEPADMAX];		// パッドトリガー（複数対応）
static int					padCount = 0;				// 検出したパッドの数
static int					g_buttonTime = BUTTON_WAIT;	// ボタン入力時のウェイトタイムを格納

// 頂点情報
static ID3D11Buffer	*g_VertexBuffer = NULL;

// テクスチャ情報
static ID3D11ShaderResourceView	*g_Texture[INPUT_TEXTURE_LABEL_MAX] = { NULL };
static char *g_TextureName[INPUT_TEXTURE_LABEL_MAX] = {
	"data/TEXTURE/Button/mouse_icon.png",
};


//--------------------------------- Xinput game pad
static XINPUT_STATE xPadState[GAMEPADMAX];		// Xinput用パッド情報
static BOOL			xPadConnected[GAMEPADMAX];	// 接続状況
static XINPUT_STATE xPadStateTest;				// パッド接続チェック用の情報を格納する構造体
static int			g_xPadCheckCount;			// パッド接続チェック用カウント


static BOOL						g_XinputButtonOld[GAMEPADMAX][XINPUT_BUTTON_MAX];		// 
static XINPUT_BUTTON_STATUS		g_XinputButtonStatus[GAMEPADMAX][XINPUT_BUTTON_MAX];	// 
static int						g_XinputButtonRepeatCount[GAMEPADMAX][XINPUT_BUTTON_MAX];	// 
static BOOL						g_XinputButtonIsRepeat[GAMEPADMAX][XINPUT_BUTTON_MAX];	// 



//=============================================================================
// 入力処理の初期化
//=============================================================================
HRESULT InitInput(HINSTANCE hInst, HWND hWnd)
{
	HRESULT hr;

	if(!g_pDInput)
	{
		// DirectInputオブジェクトの作成
		hr = DirectInput8Create(hInst, DIRECTINPUT_VERSION,
									IID_IDirectInput8, (void**)&g_pDInput, NULL);
	}

	// キーボードの初期化
	InitKeyboard(hInst, hWnd);

 	// マウスの初期化
	InitializeMouse(hInst, hWnd);
	
	// パッドの初期化
	InitializePad();

	ID3D11Device *pDevice = GetDevice();

	//テクスチャ生成
	for (int i = 0; i < INPUT_TEXTURE_LABEL_MAX; i++)
	{
		g_Texture[i] = NULL;
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_TextureName[i],
			NULL,
			NULL,
			&g_Texture[i],
			NULL);
	}

	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);


	// ★★★　XInputパッドの初期化
	InitializeXinputPad();

	return S_OK;
}

//=============================================================================
// 入力処理の終了処理
//=============================================================================
void UninitInput(void)
{
	// キーボードの終了処理
	UninitKeyboard();

	// マウスの終了処理
	UninitMouse();

	// パッドの終了処理
	UninitPad();

	if(g_pDInput)
	{
		g_pDInput->Release();
		g_pDInput = NULL;
	}

	if (g_VertexBuffer)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	for (int i = 0; i < INPUT_TEXTURE_LABEL_MAX; i++)
	{
		if (g_Texture[i])
		{
			g_Texture[i]->Release();
			g_Texture[i] = NULL;
		}
	}


	// ★★★　XInputパッドの終了処理
	UninitXinputPad();
}

//=============================================================================
// 入力処理の更新処理
//=============================================================================
void UpdateInput(void)
{
	// キーボードの更新
	UpdateKeyboard();

	// マウスの更新
	UpdateMouse();

	// パッドの更新
	UpdatePad();

	// Mキーかセレクトボタンでマウスモードの変更
	//if (GetKeyboardTrigger(DIK_M) || IsButtonTriggered(0, BUTTON_SELECT)) SetMouseUse();

	// ボタンのウェイトタイムを更新
	if (g_buttonTime < BUTTON_WAIT) g_buttonTime++;


	// ★★★　XInputパッドの更新
	//UpdateXinputPad(); // UpdateXinputPad()ではなくUpdateXinputAllButtonStatus(padNo)が呼ばれてしまう不具合のためコメントアウト
	
	// 60フレームに1度、パッド接続状況の更新を行う
	if (g_xPadCheckCount >= XINPUT_PAD_CHECK_COUNT)
	{
		for (int i = 0; i < GAMEPADMAX; i++)
		{
			DWORD dwResult = XInputGetState(i, &xPadStateTest);

			if (dwResult == ERROR_SUCCESS)
			{
				// コントローラーが接続されている
				xPadConnected[i] = TRUE;
			}
			else
			{
				// コントローラーが接続されていない
				xPadConnected[i] = FALSE;
			}
		}

		// カウントリセット
		g_xPadCheckCount = 0;
	}
	g_xPadCheckCount++;

	for (int padNo = 0; padNo < GAMEPADMAX; padNo++)
	{
		// 接続されているパッドのみ処理
		if (xPadConnected[padNo] == FALSE)
		{
			continue;
		}

		DWORD dwResult = XInputGetState(padNo, &xPadState[padNo]);

		//if(xPadState[i].Gamepad.wButtons & XINPUT_GAMEPAD_A)

		UpdateXinputAllButtonStatus(padNo);
	}

	g_bSkip = FALSE;
}

//=============================================================================
// 描画処理の更新処理
//=============================================================================
void DrawInput(void)
{
	if (GetMouseUse())	// マウス使用モードのときマウスアイコンを表示
	{
		//// 頂点バッファ設定
		//UINT stride = sizeof(VERTEX_3D);
		//UINT offset = 0;
		//GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

		//// マトリクス設定
		//SetWorldViewProjection2D();

		//// プリミティブトポロジ設定
		//GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

		//// マテリアル設定
		//MATERIAL material;
		//ZeroMemory(&material, sizeof(material));
		//material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		//SetMaterial(material);

		//// テクスチャ設定
		//GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[INPUT_TEXTURE_LABEL_MOUSE]);

		//// １枚のポリゴンの頂点とテクスチャ座標を設定
		//SetSpriteColor(g_VertexBuffer,
		//	50.0f, SCREEN_HEIGHT - 50.0f, 64.0f, 64.0f,
		//	0.0f, 0.0f, 1.0f, 1.0f, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		//// ポリゴン描画
		//GetDeviceContext()->Draw(4, 0);
	}
}

//=============================================================================
// キーボードの初期化
//=============================================================================
HRESULT InitKeyboard(HINSTANCE hInst, HWND hWnd)
{
	HRESULT hr;

	// デバイスオブジェクトを作成
	hr = g_pDInput->CreateDevice(GUID_SysKeyboard, &g_pDIDevKeyboard, NULL);
	if(FAILED(hr) || g_pDIDevKeyboard == NULL)
	{
		MessageBox(hWnd, "キーボードがありません。", "警告！", MB_ICONWARNING);
		return hr;
	}

	// データフォーマットを設定
	hr = g_pDIDevKeyboard->SetDataFormat(&c_dfDIKeyboard);
	if(FAILED(hr))
	{
		MessageBox(hWnd, "キーボードのデータフォーマットを設定できませんでした。", "警告！", MB_ICONWARNING);
		return hr;
	}

	// 協調モードを設定（フォアグラウンド＆非排他モード）
	hr = g_pDIDevKeyboard->SetCooperativeLevel(hWnd, (DISCL_FOREGROUND | DISCL_NONEXCLUSIVE));
	if(FAILED(hr))
	{
		MessageBox(hWnd, "キーボードの協調モードを設定できませんでした。", "警告！", MB_ICONWARNING);
		return hr;
	}

	// キーボードへのアクセス権を獲得(入力制御開始)
	g_pDIDevKeyboard->Acquire();

	return S_OK;
}

//=============================================================================
// キーボードの終了処理
//=============================================================================
void UninitKeyboard(void)
{
	if(g_pDIDevKeyboard)
	{
		g_pDIDevKeyboard->Release();
		g_pDIDevKeyboard = NULL;
	}
}

//=============================================================================
// キーボードの更新
//=============================================================================
HRESULT UpdateKeyboard(void)
{
	HRESULT hr;
	BYTE keyStateOld[256];

	// 前回のデータを保存
	memcpy(keyStateOld, g_keyState, NUM_KEY_MAX);

	// デバイスからデータを取得
	hr = g_pDIDevKeyboard->GetDeviceState(sizeof(g_keyState), g_keyState);
	if(SUCCEEDED(hr))
	{
		for(int cnt = 0; cnt < NUM_KEY_MAX; cnt++)
		{
			g_keyStateTrigger[cnt] = (keyStateOld[cnt] ^ g_keyState[cnt]) & g_keyState[cnt];
			g_keyStateRelease[cnt] = (keyStateOld[cnt] ^ g_keyState[cnt]) & ~g_keyState[cnt];
			g_keyStateRepeat[cnt] = g_keyStateTrigger[cnt];

			if(g_keyState[cnt])
			{
				g_keyStateRepeatCnt[cnt]++;
				if(g_keyStateRepeatCnt[cnt] >= 20)
				{
					g_keyStateRepeat[cnt] = g_keyState[cnt];
				}
			}
			else
			{
				g_keyStateRepeatCnt[cnt] = 0;
				g_keyStateRepeat[cnt] = 0;
			}
		}
	}
	else
	{
		// キーボードへのアクセス権を取得
		g_pDIDevKeyboard->Acquire();
	}
	return S_OK;
}

//=============================================================================
// キーボードのプレス状態を取得
//=============================================================================
bool GetKeyboardPress(int key)
{
	return (g_keyState[key] & 0x80) ? true : false;
}

//=============================================================================
// キーボードのトリガー状態を取得
//=============================================================================
bool GetKeyboardTrigger(int key)
{
	return (g_keyStateTrigger[key] & 0x80) ? true : false;
}

//=============================================================================
// キーボードのリピート状態を取得
//=============================================================================
bool GetKeyboardRepeat(int key)
{
	return (g_keyStateRepeat[key] & 0x80) ? true : false;
}

//=============================================================================
// キーボードのリリ－ス状態を取得
//=============================================================================
bool GetKeyboardRelease(int key)
{
	return (g_keyStateRelease[key] & 0x80) ? true : false;
}

//=============================================================================
// マウス関係の処理
//=============================================================================
// マウスの初期化
HRESULT InitializeMouse(HINSTANCE hInst,HWND hWindow)
{
	HRESULT result;
	// デバイス作成
	result = g_pDInput->CreateDevice(GUID_SysMouse,&pMouse,NULL);
	if(FAILED(result) || pMouse==NULL)
	{
		MessageBox(hWindow,"No mouse","Warning",MB_OK | MB_ICONWARNING);
		return result;
	}
	// データフォーマット設定
	result = pMouse->SetDataFormat(&c_dfDIMouse2);
	if(FAILED(result))
	{
		MessageBox(hWindow,"Can't setup mouse","Warning",MB_OK | MB_ICONWARNING);
		return result;
	}
	// 他のアプリと協調モードに設定
	result = pMouse->SetCooperativeLevel(hWindow, (DISCL_FOREGROUND | DISCL_NONEXCLUSIVE));
	if(FAILED(result))
	{
		MessageBox(hWindow,"Mouse mode error","Warning",MB_OK | MB_ICONWARNING);
		return result;
	}
	// デバイスの設定
	DIPROPDWORD prop;
	prop.diph.dwSize = sizeof(prop);
	prop.diph.dwHeaderSize = sizeof(prop.diph);
	prop.diph.dwObj = 0;
	prop.diph.dwHow = DIPH_DEVICE;
	prop.dwData = DIPROPAXISMODE_REL;		// マウスの移動値　相対値

	result = pMouse->SetProperty(DIPROP_AXISMODE,&prop.diph);
	if(FAILED(result))
	{
		MessageBox(hWindow,"Mouse property error","Warning",MB_OK | MB_ICONWARNING);
		return result;	
	}
	// アクセス権を得る
	pMouse->Acquire();
	return result;
}
//---------------------------------------------------------
void UninitMouse()
{
	if(pMouse)
	{
		pMouse->Unacquire();
		pMouse->Release();
		pMouse = NULL;
	}
}
//-----------------------------------------------------------
HRESULT UpdateMouse()
{
	HRESULT result;
	// 前回の値保存
	DIMOUSESTATE2 lastMouseState = mouseState;
	// データ取得
	result = pMouse->GetDeviceState(sizeof(mouseState),&mouseState);
	if(SUCCEEDED(result))
	{
		mouseTrigger.lX = mouseState.lX;
		mouseTrigger.lY = mouseState.lY;
		mouseTrigger.lZ = mouseState.lZ;
		// マウスのボタン状態
		for(int i=0;i<8;i++)
		{
			mouseTrigger.rgbButtons[i] = ((lastMouseState.rgbButtons[i] ^
				mouseState.rgbButtons[i]) & mouseState.rgbButtons[i]);
		}
	}
	else	// 取得失敗
	{
		// アクセス権を得てみる
		result = pMouse->Acquire();
	}
	return result;
}

//----------------------------------------------
BOOL IsMouseLeftPressed(void)
{
	return (BOOL)(mouseState.rgbButtons[0] & 0x80);	// 押されたときに立つビットを検査
}
BOOL IsMouseLeftTriggered(void)
{
	return (BOOL)(mouseTrigger.rgbButtons[0] & 0x80);
}
BOOL IsMouseRightPressed(void)
{
	return (BOOL)(mouseState.rgbButtons[1] & 0x80);
}
BOOL IsMouseRightTriggered(void)
{
	return (BOOL)(mouseTrigger.rgbButtons[1] & 0x80);
}
BOOL IsMouseCenterPressed(void)
{
	return (BOOL)(mouseState.rgbButtons[2] & 0x80);
}
BOOL IsMouseCenterTriggered(void)
{
	return (BOOL)(mouseTrigger.rgbButtons[2] & 0x80);
}
//------------------
long GetMouseX(void)
{
	//return mouseState.lX;
	return GetMousePosX();
}
long GetMouseY(void)
{
	//return mouseState.lY;
	return GetMousePosY();

}
long GetMouseZ(void)
{
	return mouseState.lZ;
}

//=============================================================================
// マウスカーソルの現在位置を取得する
//=============================================================================
XMFLOAT2 GetMousePos(void)
{
	return XMFLOAT2((float)GetMousePosX() * SCREEN_WIDTH / (float)GetWinWidth(), (float)GetMousePosY() * SCREEN_HEIGHT / (float)GetWinHeight());
}

//=============================================================================
// マウスの使用状況を取得する
//=============================================================================
BOOL GetMouseUse(void)
{
	return g_MouseUse;
}

//=============================================================================
// マウスの使用状況を変更する
//=============================================================================
void SetMouseUse(void)
{
	g_MouseUse = (g_MouseUse + 1) % 2;	// 0か1（TUREかFALSE）を行ったり来たりする
}
void SetMouseUse(BOOL use)
{
	g_MouseUse = use;
}

//================================================= game pad
//---------------------------------------- コールバック関数
BOOL CALLBACK SearchGamePadCallback(LPDIDEVICEINSTANCE lpddi, LPVOID )
{
	HRESULT result;

	result = g_pDInput->CreateDevice(lpddi->guidInstance, &pGamePad[padCount++], NULL);
	return DIENUM_CONTINUE;	// 次のデバイスを列挙

}
//---------------------------------------- 初期化
HRESULT InitializePad(void)			// パッド初期化
{
	HRESULT		result;
	int			i;

	padCount = 0;
	// ジョイパッドを探す
	g_pDInput->EnumDevices(DI8DEVCLASS_GAMECTRL, (LPDIENUMDEVICESCALLBACK)SearchGamePadCallback, NULL, DIEDFL_ATTACHEDONLY);
	// セットしたコールバック関数が、パッドを発見した数だけ呼ばれる。

	for ( i=0 ; i<padCount ; i++ ) {
		// ジョイスティック用のデータ・フォーマットを設定
		result = pGamePad[i]->SetDataFormat(&c_dfDIJoystick);
		if ( FAILED(result) )
			return false; // データフォーマットの設定に失敗

		// モードを設定（フォアグラウンド＆非排他モード）
//		result = pGamePad[i]->SetCooperativeLevel(hWindow, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
//		if ( FAILED(result) )
//			return false; // モードの設定に失敗

		// 軸の値の範囲を設定
		// X軸、Y軸のそれぞれについて、オブジェクトが報告可能な値の範囲をセットする。
		// (max-min)は、最大10,000(?)。(max-min)/2が中央値になる。
		// 差を大きくすれば、アナログ値の細かな動きを捕らえられる。(パッドの性能による)
		DIPROPRANGE				diprg;
		ZeroMemory(&diprg, sizeof(diprg));
		diprg.diph.dwSize		= sizeof(diprg); 
		diprg.diph.dwHeaderSize	= sizeof(diprg.diph); 
		diprg.diph.dwHow		= DIPH_BYOFFSET; 
		diprg.lMin				= RANGE_MIN;
		diprg.lMax				= RANGE_MAX;
		// X軸の範囲を設定
		diprg.diph.dwObj = DIJOFS_X;
		pGamePad[i]->SetProperty(DIPROP_RANGE, &diprg.diph);
		// Y軸の範囲を設定
		diprg.diph.dwObj = DIJOFS_Y;
		pGamePad[i]->SetProperty(DIPROP_RANGE, &diprg.diph);
		// Z軸の範囲を設定（右スティックのX軸）
		diprg.diph.dwObj = DIJOFS_Z;
		pGamePad[i]->SetProperty(DIPROP_RANGE, &diprg.diph);
		// RZ軸の範囲を設定（右スティックのY軸）
		diprg.diph.dwObj = DIJOFS_RZ;
		pGamePad[i]->SetProperty(DIPROP_RANGE, &diprg.diph);

		// 各軸ごとに、無効のゾーン値を設定する。
		// 無効ゾーンとは、中央からの微少なジョイスティックの動きを無視する範囲のこと。
		// 指定する値は、10000に対する相対値(2000なら20パーセント)。
		DIPROPDWORD				dipdw;
		dipdw.diph.dwSize		= sizeof(DIPROPDWORD);
		dipdw.diph.dwHeaderSize	= sizeof(dipdw.diph);
		dipdw.diph.dwHow		= DIPH_BYOFFSET;
		dipdw.dwData			= DEADZONE;
		//X軸の無効ゾーンを設定
		dipdw.diph.dwObj = DIJOFS_X;
		pGamePad[i]->SetProperty(DIPROP_DEADZONE, &dipdw.diph);
		//Y軸の無効ゾーンを設定
		dipdw.diph.dwObj = DIJOFS_Y;
		pGamePad[i]->SetProperty(DIPROP_DEADZONE, &dipdw.diph);
		//Z軸の無効ゾーンを設定（右スティックのX軸）
		dipdw.diph.dwObj = DIJOFS_Z;
		pGamePad[i]->SetProperty(DIPROP_DEADZONE, &dipdw.diph);
		//RZ軸の無効ゾーンを設定（右スティックのY軸）
		dipdw.diph.dwObj = DIJOFS_RZ;
		pGamePad[i]->SetProperty(DIPROP_DEADZONE, &dipdw.diph);
			
		//ジョイスティック入力制御開始
		pGamePad[i]->Acquire();
	}
	return true;
}
//------------------------------------------- 終了処理
void UninitPad(void)
{
	for (int i=0 ; i<GAMEPADMAX ; i++) {
		if ( pGamePad[i] )
		{
			pGamePad[i]->Unacquire();
			pGamePad[i]->Release();
		}
	}

}

//------------------------------------------ 更新
float GY, GX;

void UpdatePad(void)
{
	HRESULT			result;
	DIJOYSTATE2		dijs;
	int				i;

	for (i = 0; i < padCount; i++)
	{
		DWORD lastPadState;
		lastPadState = padState[i];
		padState[i] = 0x00000000l;	// 初期化

		result = pGamePad[i]->Poll();	// ジョイスティックにポールをかける
		if (FAILED(result)) {
			result = pGamePad[i]->Acquire();
			while (result == DIERR_INPUTLOST)
				result = pGamePad[i]->Acquire();
		}

		result = pGamePad[i]->GetDeviceState(sizeof(DIJOYSTATE), &dijs);	// デバイス状態を読み取る
		if (result == DIERR_INPUTLOST || result == DIERR_NOTACQUIRED) {
			result = pGamePad[i]->Acquire();
			while (result == DIERR_INPUTLOST)
				result = pGamePad[i]->Acquire();
		}
		
		// ３２の各ビットに意味を持たせ、ボタン押下に応じてビットをオンにする
		// 左スティック
		//* y-axis (forward)
		if (dijs.lY < 0)					padState[i] |= BUTTON_L_UP;
		//* y-axis (backward)
		if (dijs.lY > 0)					padState[i] |= BUTTON_L_DOWN;
		//* x-axis (left)
		if (dijs.lX < 0)					padState[i] |= BUTTON_L_LEFT;
		//* x-axis (right)
		if (dijs.lX > 0)					padState[i] |= BUTTON_L_RIGHT;
		// 右スティック
		//* y-axis (forward)
		if (dijs.lRz < 0)					padState[i] |= BUTTON_R_UP;
		//* y-axis (backward)
		if (dijs.lRz > 0)					padState[i] |= BUTTON_R_DOWN;
		//* x-axis (left)
		if (dijs.lZ < 0)					padState[i] |= BUTTON_R_LEFT;
		//* x-axis (right)
		if (dijs.lZ > 0)					padState[i] |= BUTTON_R_RIGHT;
		//* Ｘボタン
		if (dijs.rgbButtons[rgbButtons_X] & 0x80)	padState[i] |= BUTTON_X;
		//* Ａボタン
		if (dijs.rgbButtons[rgbButtons_A] & 0x80)	padState[i] |= BUTTON_A;
		//* Ｂボタン
		if (dijs.rgbButtons[rgbButtons_B] & 0x80)	padState[i] |= BUTTON_B;
		//* Ｙボタン
		if (dijs.rgbButtons[rgbButtons_Y] & 0x80)	padState[i] |= BUTTON_Y;
		//* Ｌボタン
		if (dijs.rgbButtons[rgbButtons_L] & 0x80)	padState[i] |= BUTTON_L1;
		//* Ｒボタン
		if (dijs.rgbButtons[rgbButtons_R] & 0x80)	padState[i] |= BUTTON_R1;
		//* Ｌ2ボタン
		if (dijs.rgbButtons[rgbButtons_L2] & 0x80)	padState[i] |= BUTTON_L2;
		//* Ｒ2ボタン
		if (dijs.rgbButtons[rgbButtons_R2] & 0x80)	padState[i] |= BUTTON_R2;
		//* Selectボタン
		if (dijs.rgbButtons[rgbButtons_SELECT] & 0x80)	padState[i] |= BUTTON_SELECT;
		//* ＳＴＡＲＴボタン
		if (dijs.rgbButtons[rgbButtons_START] & 0x80)	padState[i] |= BUTTON_START;
		//* L3ボタン
		if (dijs.rgbButtons[rgbButtons_L3] & 0x80)	padState[i] |= BUTTON_L3;
		//* R3ボタン
		if (dijs.rgbButtons[rgbButtons_R3] & 0x80)	padState[i] |= BUTTON_R3;

		//* 十字ボタン
		if (dijs.rgdwPOV[0] != 0xFFFFFFFF)
		{
			float rad = XMConvertToRadians((dijs.rgdwPOV[0] / 100.0f));
			float x = sinf(rad);	// 時計回りなのでcosとsinを逆にする
			float y = cosf(rad);

			if (x < -0.01f)
			{
				padState[i] |= BUTTON_LEFT;
			}
			else if (x > 0.01f)
			{
				padState[i] |= BUTTON_RIGHT;
			}

			if (y > 0.01f)
			{
				padState[i] |= BUTTON_UP;
			}
			else if (y < -0.01f)
			{
				padState[i] |= BUTTON_DOWN;
			}
		}

		// Trigger設定
		padTrigger[i] = ((lastPadState ^ padState[i])	// 前回と違っていて
			& padState[i]);					// しかも今ONのやつ
	}
}
//----------------------------------------------- 検査
BOOL IsButtonPressed(int padNo,DWORD button)
{
	return (button & padState[padNo]);
}

BOOL IsButtonTriggered(int padNo, DWORD button)
{
	return (button & padTrigger[padNo]);
}

// ボタン入力が可能かどうか
BOOL CheckButtonWait(void)
{
	BOOL answer = FALSE;
	if (g_buttonTime >= BUTTON_WAIT) answer = TRUE;
	return answer;
}

// ボタン入力のウェイトタイムのリセット
void SetButtonWait(void)
{
	g_buttonTime = 0;
}

// Xinput用=======================================================

void InitializeXinputPad(void)
{
	// コントローラーの初期化・検出
	// Xinputではコントローラー番号が0~3まで割り当てられる
	for (int i = 0; i < GAMEPADMAX; i++)
	{
		ZeroMemory(&xPadState[i], sizeof(XINPUT_STATE));
		ZeroMemory(&xPadConnected[i], sizeof(BOOL));


		DWORD dwResult = XInputGetState(i, &xPadStateTest);

		if (dwResult == ERROR_SUCCESS)
		{
			// コントローラーが接続されている
			xPadConnected[i] = TRUE;
		}
		else
		{
			// コントローラーが接続されていない
			xPadConnected[i] = FALSE;
		}
	}

	// 接続状態の定期チェック用カウントの初期化
	g_xPadCheckCount = 0;


	for (int padNo = 0; padNo < GAMEPADMAX; padNo++)
	{
		// 接続されているパッドのみ処理
		if (xPadConnected[padNo] == FALSE)
		{
			continue;
		}

		DWORD dwResult = XInputGetState(padNo, &xPadState[padNo]);

		// 全てFALSEで初期化
		for (int button = 0; button < XINPUT_BUTTON_MAX; button++)
		{
			g_XinputButtonOld[padNo][button] = FALSE;
			g_XinputButtonStatus[padNo][button] = XINPUT_BUTTON_STATUS_NONE;
		}

		UpdateXinputAllButtonStatus(padNo);
	}
}

void UninitXinputPad(void)
{
	// XInputを停止　振動を止める
	XInputEnable(false);
}

void UpdateXinputPad(void)
{
	// 60フレームに1度、パッド接続状況の更新を行う
	if (g_xPadCheckCount >= XINPUT_PAD_CHECK_COUNT)
	{
		for (int i = 0; i < GAMEPADMAX; i++)
		{
			DWORD dwResult = XInputGetState(i, &xPadStateTest);

			if (dwResult == ERROR_SUCCESS)
			{
				// コントローラーが接続されている
				xPadConnected[i] = TRUE;
			}
			else
			{
				// コントローラーが接続されていない
				xPadConnected[i] = FALSE;
			}
		}

		// カウントリセット
		g_xPadCheckCount = 0;
	}
	g_xPadCheckCount++;

	for (int padNo = 0; padNo < GAMEPADMAX; padNo++)
	{
		// 接続されているパッドのみ処理
		if (xPadConnected[padNo] == FALSE)
		{
			continue;
		}

		DWORD dwResult = XInputGetState(padNo, &xPadState[padNo]);

		//if(xPadState[i].Gamepad.wButtons & XINPUT_GAMEPAD_A)

		UpdateXinputAllButtonStatus(padNo);
	}
}

void UpdateXinputAllButtonStatus(int padNo)
{
	UpdateXinputButtonStatus(padNo, XINPUT_BUTTON_UP, IsButtonPressedXInput(padNo, XINPUT_GAMEPAD_DPAD_UP));
	UpdateXinputButtonStatus(padNo, XINPUT_BUTTON_DOWN, IsButtonPressedXInput(padNo, XINPUT_GAMEPAD_DPAD_DOWN));
	UpdateXinputButtonStatus(padNo, XINPUT_BUTTON_LEFT, IsButtonPressedXInput(padNo, XINPUT_GAMEPAD_DPAD_LEFT));
	UpdateXinputButtonStatus(padNo, XINPUT_BUTTON_RIGHT, IsButtonPressedXInput(padNo, XINPUT_GAMEPAD_DPAD_RIGHT));
	UpdateXinputButtonStatus(padNo, XINPUT_BUTTON_START, IsButtonPressedXInput(padNo, XINPUT_GAMEPAD_START));
	UpdateXinputButtonStatus(padNo, XINPUT_BUTTON_SELECT, IsButtonPressedXInput(padNo, XINPUT_GAMEPAD_BACK));
	UpdateXinputButtonStatus(padNo, XINPUT_BUTTON_L3, IsButtonPressedXInput(padNo, XINPUT_GAMEPAD_LEFT_THUMB));
	UpdateXinputButtonStatus(padNo, XINPUT_BUTTON_R3, IsButtonPressedXInput(padNo, XINPUT_GAMEPAD_RIGHT_THUMB));
	UpdateXinputButtonStatus(padNo, XINPUT_BUTTON_L1, IsButtonPressedXInput(padNo, XINPUT_GAMEPAD_LEFT_SHOULDER));
	UpdateXinputButtonStatus(padNo, XINPUT_BUTTON_R1, IsButtonPressedXInput(padNo, XINPUT_GAMEPAD_RIGHT_SHOULDER));
	UpdateXinputButtonStatus(padNo, XINPUT_BUTTON_A, IsButtonPressedXInput(padNo, XINPUT_GAMEPAD_A));
	UpdateXinputButtonStatus(padNo, XINPUT_BUTTON_B, IsButtonPressedXInput(padNo, XINPUT_GAMEPAD_B));
	UpdateXinputButtonStatus(padNo, XINPUT_BUTTON_X, IsButtonPressedXInput(padNo, XINPUT_GAMEPAD_X));
	UpdateXinputButtonStatus(padNo, XINPUT_BUTTON_Y, IsButtonPressedXInput(padNo, XINPUT_GAMEPAD_Y));

	UpdateXinputButtonStatus(padNo, XINPUT_BUTTON_L2, IsButtonTriggerXInput(padNo, TRIGGER_LEFT));
	UpdateXinputButtonStatus(padNo, XINPUT_BUTTON_R2, IsButtonTriggerXInput(padNo, TRIGGER_RIGHT));

	UpdateXinputButtonStatus(padNo, XINPUT_BUTTON_LSTICK_UP, IsStickPressedXInput(padNo, XINPUT_LEFTSTICK_Y_UP));
	UpdateXinputButtonStatus(padNo, XINPUT_BUTTON_LSTICK_DOWN, IsStickPressedXInput(padNo, XINPUT_LEFTSTICK_Y_DOWN));
	UpdateXinputButtonStatus(padNo, XINPUT_BUTTON_LSTICK_LEFT, IsStickPressedXInput(padNo, XINPUT_LEFTSTICK_X_LEFT));
	UpdateXinputButtonStatus(padNo, XINPUT_BUTTON_LSTICK_RIGHT, IsStickPressedXInput(padNo, XINPUT_LEFTSTICK_X_RIGHT));

	UpdateXinputButtonStatus(padNo, XINPUT_BUTTON_RSTICK_UP, IsStickPressedXInput(padNo, XINPUT_RIGHTSTICK_Y_UP));
	UpdateXinputButtonStatus(padNo, XINPUT_BUTTON_RSTICK_DOWN, IsStickPressedXInput(padNo, XINPUT_RIGHTSTICK_Y_DOWN));
	UpdateXinputButtonStatus(padNo, XINPUT_BUTTON_RSTICK_LEFT, IsStickPressedXInput(padNo, XINPUT_RIGHTSTICK_X_LEFT));
	UpdateXinputButtonStatus(padNo, XINPUT_BUTTON_RSTICK_RIGHT, IsStickPressedXInput(padNo, XINPUT_RIGHTSTICK_X_RIGHT));
}

void UpdateXinputButtonStatus(int padNo, XINPUT_BUTTON button, BOOL press)
{
	if (g_XinputButtonOld[padNo][button])
	{
		if (press)
		{
			g_XinputButtonStatus[padNo][button] = XINPUT_BUTTON_STATUS_PRESSED;

			// リピートチェック
			if (g_XinputButtonRepeatCount[padNo][button] == 0 ||
				g_XinputButtonRepeatCount[padNo][button] == XINPUT_BUTTON_REPEAT_START)
			{
				g_XinputButtonIsRepeat[padNo][button] = TRUE;
			}
			else if (g_XinputButtonRepeatCount[padNo][button] > XINPUT_BUTTON_REPEAT_START + XINPUT_BUTTON_REPEAT_NEXT)
			{
				g_XinputButtonIsRepeat[padNo][button] = TRUE;
				g_XinputButtonRepeatCount[padNo][button] -= XINPUT_BUTTON_REPEAT_NEXT;
			}
			else
			{
				g_XinputButtonIsRepeat[padNo][button] = FALSE;
			}
			g_XinputButtonRepeatCount[padNo][button]++;
		}
		else
		{
			g_XinputButtonStatus[padNo][button] = XINPUT_BUTTON_STATUS_RELEASED;
			g_XinputButtonIsRepeat[padNo][button] = FALSE;
			g_XinputButtonRepeatCount[padNo][button] = 0;
		}
	}
	else
	{
		if (press)
		{
			g_XinputButtonStatus[padNo][button] = XINPUT_BUTTON_STATUS_TRIGGERED;
			g_XinputButtonIsRepeat[padNo][button] = FALSE;
			g_XinputButtonRepeatCount[padNo][button] = 0;
		}
		else
		{
			g_XinputButtonStatus[padNo][button] = XINPUT_BUTTON_STATUS_NONE;
			g_XinputButtonIsRepeat[padNo][button] = FALSE;
			g_XinputButtonRepeatCount[padNo][button] = 0;
		}
	}
	g_XinputButtonOld[padNo][button] = press;
}

//----------------------------------------------- 検査

/*===============================================
説明	：	ボタン押下チェック関数
制作者	：	吉永健人
メモ	：	対応するボタンのマクロ名は下記参照のこと。
			https://docs.microsoft.com/ja-jp/windows/win32/api/XInput/ns-xinput-xinput_gamepad
===============================================*/
BOOL IsButtonPressedXInput(int padNo, WORD button)
{
	BOOL pressed = FALSE;
	if (button & xPadState[padNo].Gamepad.wButtons)
	{
		pressed = TRUE;
	}
	return pressed;
}

/*===============================================
説明	：	トリガー押し込みチェック関数
制作者	：	吉永健人
メモ	：	戻り値は0~255。
===============================================*/
int IsButtonTriggerXInput(int padNo, int RorL)
{
	switch (RorL)
	{
	case TRIGGER_LEFT:
		// 閾値を越えているかチェック
		if (xPadState[padNo].Gamepad.bLeftTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD)
		{
			return (int)xPadState[padNo].Gamepad.bLeftTrigger;
		}
		else
		{
			return 0;
		}
		break;

	case TRIGGER_RIGHT:
		// 閾値を越えているかチェック
		if (xPadState[padNo].Gamepad.bRightTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD)
		{
			return (int)xPadState[padNo].Gamepad.bRightTrigger;
		}
		else
		{
			return 0;
		}
		break;
	}

	return 0;
}

/*===============================================
説明	：	スティック押下チェック関数
制作者	：	吉永健人
メモ	：	指定したスティックが指定した方向（上下左右）に傾いているか、TRUE/FALSEで返す。
			第2引数には　列挙型XINPUT_STICKで宣言された8つの定数（XINPUT_LEFTSTICK_X_RIGHT～XINPUT_RIGHTSTICK_Y_DOWN）を使用してください。
===============================================*/
BOOL IsStickPressedXInput(int padNo, int stick)
{
	BOOL pressed = FALSE;

	switch (stick)
	{
		// 左スティック ==========
	case XINPUT_LEFTSTICK_X_RIGHT:
		if (xPadState[padNo].Gamepad.sThumbLX > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
		{
			pressed = TRUE;
		}
		break;

	case XINPUT_LEFTSTICK_X_LEFT:
		if (xPadState[padNo].Gamepad.sThumbLX < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
		{
			pressed = TRUE;
		}
		break;

	case XINPUT_LEFTSTICK_Y_UP:
		if (xPadState[padNo].Gamepad.sThumbLY > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
		{
			pressed = TRUE;
		}
		break;

	case XINPUT_LEFTSTICK_Y_DOWN:
		if (xPadState[padNo].Gamepad.sThumbLY < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
		{
			pressed = TRUE;
		}
		break;


		// 右スティック ==========
	case XINPUT_RIGHTSTICK_X_RIGHT:
		if (xPadState[padNo].Gamepad.sThumbRX > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)
		{
			pressed = TRUE;
		}
		break;

	case XINPUT_RIGHTSTICK_X_LEFT:
		if (xPadState[padNo].Gamepad.sThumbRX < -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)
		{
			pressed = TRUE;
		}
		break;

	case XINPUT_RIGHTSTICK_Y_UP:
		if (xPadState[padNo].Gamepad.sThumbRY > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)
		{
			pressed = TRUE;
		}
		break;

	case XINPUT_RIGHTSTICK_Y_DOWN:
		if (xPadState[padNo].Gamepad.sThumbRY < -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)
		{
			pressed = TRUE;
		}
		break;
	}


	return pressed;
}

/*===============================================
説明	：	スティック入力チェック関数
制作者	：	吉永健人
メモ	：	指定したスティックに何かしら入力があるかチェックする。
===============================================*/
BOOL CheckStickPressedXInput(int padNo, int stick)
{
	BOOL pressed = FALSE;

	switch (stick)
	{
	case XINPUT_LEFTSTICK:
		if ((xPadState[padNo].Gamepad.sThumbLX > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
			|| (xPadState[padNo].Gamepad.sThumbLX < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
			|| (xPadState[padNo].Gamepad.sThumbLY > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
			|| (xPadState[padNo].Gamepad.sThumbLY < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE))
		{
			pressed = TRUE;
		}
		break;

	case XINPUT_RIGHTSTICK:
		if ((xPadState[padNo].Gamepad.sThumbRX > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)
			|| (xPadState[padNo].Gamepad.sThumbRX < -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)
			|| (xPadState[padNo].Gamepad.sThumbRY > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)
			|| (xPadState[padNo].Gamepad.sThumbRY < -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE))
		{
			pressed = TRUE;
		}
		break;
	}

	return pressed;
}

/*===============================================
説明	：	スティック情報取得関数
制作者	：	吉永健人
メモ	：	指定したスティックのラジアンと正規化された距離を求める。
			予め用意した構造体のポインタを用い、そのアドレス先に情報を格納する。
===============================================*/
void GetPadStickXInput(int padNo, int stick, XINPUT_STICK_STATUS *p)
{
	XINPUT_STICK_STATUS status;
	status.radian = 0.0f;
	status.magnitude = 0.0f;

	float x = 0.0f;
	float y = 0.0f;
	float mag = 0.0f;

	switch (stick)
	{
	case XINPUT_LEFTSTICK:
		// X軸の処理
		if ((xPadState[padNo].Gamepad.sThumbLX > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
			|| (xPadState[padNo].Gamepad.sThumbLX < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE))
		{
			x = (float)xPadState[padNo].Gamepad.sThumbLX;
		}

		// Y軸の処理
		if ((xPadState[padNo].Gamepad.sThumbLY > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
			|| (xPadState[padNo].Gamepad.sThumbLY < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE))
		{
			y = (float)xPadState[padNo].Gamepad.sThumbLY;
		}
		break;

	case XINPUT_RIGHTSTICK:
		// X軸の処理
		if ((xPadState[padNo].Gamepad.sThumbRX > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)
			|| (xPadState[padNo].Gamepad.sThumbRX < -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE))
		{
			x = (float)xPadState[padNo].Gamepad.sThumbRX;
		}

		// Y軸の処理
		if ((xPadState[padNo].Gamepad.sThumbRY > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)
			|| (xPadState[padNo].Gamepad.sThumbRY < -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE))
		{
			y = (float)xPadState[padNo].Gamepad.sThumbRY;
		}
		break;
	}

	// 方向の処理
	status.radian = atan2f(y, x);

	// 距離の処理
	mag = (float)sqrt(x * x + y * y);
	if (mag > XINPUT_PAD_STICK_MAX_VALUE)
	{
		mag = XINPUT_PAD_STICK_MAX_VALUE;
	}
	status.magnitude = mag / (float)XINPUT_PAD_STICK_MAX_VALUE;

	// 受け取ったポインタで構造体を返す
	*p = status;
}


BOOL IsXinputButtonStatus(int padNo, XINPUT_BUTTON button, XINPUT_BUTTON_STATUS status)
{
	if (status == XINPUT_BUTTON_STATUS_REPEAT)
	{
		return g_XinputButtonIsRepeat[padNo][button];
	}
	else
	{
		if (g_XinputButtonStatus[padNo][button] == status)
		{
			return TRUE;
		}
		return FALSE;
	}
	return FALSE;
}


void MoveCursor(XMINT2& cursor, const int num_x, const int num_y)
{
	if (GetKeyboardRepeat(DIK_UP) || GetKeyboardRepeat(DIK_W)) {
		cursor.y = (cursor.y - 1 + num_y) % num_y;
	}
	else if (GetKeyboardRepeat(DIK_DOWN) || GetKeyboardRepeat(DIK_S)) {
		cursor.y = (cursor.y + 1) % num_y;
	}
	else if (GetKeyboardRepeat(DIK_LEFT) || GetKeyboardRepeat(DIK_A)) {
		cursor.x = (cursor.x - 1 + num_x) % num_x;
	}
	else if (GetKeyboardRepeat(DIK_RIGHT) || GetKeyboardRepeat(DIK_D)) {
		cursor.x = (cursor.x - 1 + num_x) % num_x;
	}
}

BOOL CheckButtonSkip(void)
{
	if (!g_bSkip && (IsMouseLeftTriggered() || GetKeyboardTrigger(DIK_RETURN)))
	{
		g_bSkip = TRUE;
		return TRUE;
	}
	return FALSE;
}
