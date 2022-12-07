//=============================================================================
//
// 入力処理 [input.h]
// Author : 國江 翔太
//
//=============================================================================
#pragma once

#include "main.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************

// プログラム分けするときに使う
#define	USE_KEYBOARD				// 宣言するとキーボードで操作可能になる
#define	USE_MOUSE					// 宣言するとマウスで操作可能になる
#define	USE_PAD						// 宣言するとパッドで操作可能になる

/* game pad情報 */
//#define BUTTON_UP		0x00000001l	// 方向キー上(.IY<0)
//#define BUTTON_DOWN		0x00000002l	// 方向キー下(.IY>0)
//#define BUTTON_LEFT		0x00000004l	// 方向キー左(.IX<0)
//#define BUTTON_RIGHT	0x00000008l	// 方向キー右(.IX>0)
//#define BUTTON_X		0x00000010l	// Ｘボタン(.rgbButtons[0]&0x80)
//#define BUTTON_A		0x00000020l	// Ａボタン(.rgbButtons[1]&0x80)
//#define BUTTON_B		0x00000040l	// Ｂボタン(.rgbButtons[2]&0x80)
//#define BUTTON_Y		0x00000080l	// Ｙボタン(.rgbButtons[3]&0x80)
//#define BUTTON_L1		0x00000100l	// Ｌボタン(.rgbButtons[4]&0x80)
//#define BUTTON_R1		0x00000200l	// Ｒボタン(.rgbButtons[5]&0x80)
//#define BUTTON_L2		0x00000400l	// Ｌボタン(.rgbButtons[6]&0x80)
//#define BUTTON_R2		0x00000800l	// Ｒボタン(.rgbButtons[7]&0x80)
//#define BUTTON_SELECT	0x00001000l	// Ｚボタン(.rgbButtons[8]&0x80)
//#define BUTTON_START	0x00002000l	// ＳＴＡＲＴボタン(.rgbButtons[9]&0x80)
//#define BUTTON_L3		0x00004000l	// L3ボタン(.rgbButtons[10]&0x80)
//#define BUTTON_R3		0x00008000l	// R3ボタン(.rgbButtons[11]&0x80)
//#define BUTTON_R_UP		0x00010000l	// 方向キー上(.IY<0)
//#define BUTTON_R_DOWN	0x00020000l	// 方向キー下(.IY>0)
//#define BUTTON_R_LEFT	0x00040000l	// 方向キー左(.IX<0)
//#define BUTTON_R_RIGHT	0x00080000l	// 方向キー右(.IX>0)
//#define BUTTON_L_UP		0x00100000l	// 上ボタン(.rgdwPOV[0]&0x80)
//#define BUTTON_L_DOWN	0x00200000l	// 下ボタン(.rgdwPOV[0]&0x80)
//#define BUTTON_L_LEFT	0x00400000l	// 左ボタン(.rgdwPOV[0]&0x80)
//#define BUTTON_L_RIGHT	0x00800000l	// 右ボタン(.rgdwPOV[0]&0x80)

#define GAMEPADMAX		4			// 同時に接続するジョイパッドの最大数をセット

#define BUTTON_UP		0x00000001l	// 方向キー上(.IY<0)
#define BUTTON_DOWN		0x00000002l	// 方向キー下(.IY>0)
#define BUTTON_LEFT		0x00000004l	// 方向キー左(.IX<0)
#define BUTTON_RIGHT	0x00000008l	// 方向キー右(.IX>0)

#define BUTTON_X		0x00000010l	// Ｘボタン(.rgbButtons[0]&0x80)
#define BUTTON_A		0x00000020l	// Ａボタン(.rgbButtons[1]&0x80)
#define BUTTON_B		0x00000040l	// Ｂボタン(.rgbButtons[2]&0x80)
#define BUTTON_Y		0x00000080l	// Ｙボタン(.rgbButtons[3]&0x80)

#define BUTTON_L1		0x00000100l	// Ｌボタン(.rgbButtons[4]&0x80)
#define BUTTON_R1		0x00000200l	// Ｒボタン(.rgbButtons[5]&0x80)

#define BUTTON_L2		0x00000400l	// Ｌボタン(.rgbButtons[6]&0x80)
#define BUTTON_R2		0x00000800l	// Ｒボタン(.rgbButtons[7]&0x80)
#define BUTTON_SELECT	0x00001000l	// Ｚボタン(.rgbButtons[8]&0x80)
#define BUTTON_START	0x00002000l	// ＳＴＡＲＴボタン(.rgbButtons[9]&0x80)

#define BUTTON_L3		0x00004000l	// L3ボタン(.rgbButtons[10]&0x80)
#define BUTTON_R3		0x00008000l	// R3ボタン(.rgbButtons[11]&0x80)


#define BUTTON_R_UP		0x00010000l	// 方向キー上(.IY<0)
#define BUTTON_R_DOWN	0x00020000l	// 方向キー下(.IY>0)
#define BUTTON_R_LEFT	0x00040000l	// 方向キー左(.IX<0)
#define BUTTON_R_RIGHT	0x00080000l	// 方向キー右(.IX>0)
#define BUTTON_L_UP		0x00100000l	// 上ボタン(.rgdwPOV[0]&0x80)
#define BUTTON_L_DOWN	0x00200000l	// 下ボタン(.rgdwPOV[0]&0x80)
#define BUTTON_L_LEFT	0x00400000l	// 左ボタン(.rgdwPOV[0]&0x80)
#define BUTTON_L_RIGHT	0x00800000l	// 右ボタン(.rgdwPOV[0]&0x80)



//enum {	// ボタンに対するrgbButtons[?]配列の配列番号(DirectInput上でのXYABボタン位置に合わせた)
//	rgbButtons_X,
//	rgbButtons_A,
//	rgbButtons_B,
//	rgbButtons_Y,
//	rgbButtons_L,
//	rgbButtons_R,
//	rgbButtons_L2,
//	rgbButtons_R2,
//	rgbButtons_SELECT,
//	rgbButtons_START,
//	rgbButtons_L3,
//	rgbButtons_R3,
//};

enum {	// ボタンに対するrgbButtons[?]配列の配列番号(DirectInput上でのXYABボタン位置に合わせた)
	rgbButtons_A,
	rgbButtons_B,
	rgbButtons_X,
	rgbButtons_Y,
	rgbButtons_L,
	rgbButtons_R,
	rgbButtons_L3,
	rgbButtons_R3,
	rgbButtons_L2,
	rgbButtons_R2,
	rgbButtons_SELECT,
	rgbButtons_START,
};

enum {
	INPUT_TEXTURE_LABEL_MOUSE,
	INPUT_TEXTURE_LABEL_MAX,
};


// XInput用
#define TRIGGER_LEFT	0		// 左トリガー
#define TRIGGER_RIGHT	1		// 右トリガー

enum XINPUT_STICK {

	XINPUT_LEFTSTICK_X_RIGHT,	// 左スティック　右
	XINPUT_LEFTSTICK_X_LEFT,	// 左スティック　左
	XINPUT_LEFTSTICK_Y_UP,		// 左スティック　上
	XINPUT_LEFTSTICK_Y_DOWN,	// 左スティック　下

	XINPUT_RIGHTSTICK_X_RIGHT,	// 右スティック　右
	XINPUT_RIGHTSTICK_X_LEFT,	// 右スティック　左
	XINPUT_RIGHTSTICK_Y_UP,		// 右スティック　上
	XINPUT_RIGHTSTICK_Y_DOWN,	// 右スティック　下

	XINPUT_LEFTSTICK,			// 左スティック
	XINPUT_RIGHTSTICK,			// 右スティック
};

enum XINPUT_BUTTON {

	XINPUT_BUTTON_UP,
	XINPUT_BUTTON_DOWN,
	XINPUT_BUTTON_LEFT,
	XINPUT_BUTTON_RIGHT,
	XINPUT_BUTTON_START,
	XINPUT_BUTTON_SELECT,
	XINPUT_BUTTON_L3,
	XINPUT_BUTTON_R3,
	XINPUT_BUTTON_L1,
	XINPUT_BUTTON_R1,
	XINPUT_BUTTON_A,
	XINPUT_BUTTON_B,
	XINPUT_BUTTON_X,
	XINPUT_BUTTON_Y,

	XINPUT_BUTTON_L2,
	XINPUT_BUTTON_R2,

	XINPUT_BUTTON_LSTICK_UP,
	XINPUT_BUTTON_LSTICK_DOWN,
	XINPUT_BUTTON_LSTICK_LEFT,
	XINPUT_BUTTON_LSTICK_RIGHT,

	XINPUT_BUTTON_RSTICK_UP,
	XINPUT_BUTTON_RSTICK_DOWN,
	XINPUT_BUTTON_RSTICK_LEFT,
	XINPUT_BUTTON_RSTICK_RIGHT,

	XINPUT_BUTTON_MAX,
};

enum XINPUT_BUTTON_STATUS {
	XINPUT_BUTTON_STATUS_NONE,
	XINPUT_BUTTON_STATUS_TRIGGERED,
	XINPUT_BUTTON_STATUS_PRESSED,
	XINPUT_BUTTON_STATUS_REPEAT,
	XINPUT_BUTTON_STATUS_RELEASED,
	XINPUT_BUTTON_STATUS_MAX,
};


//*****************************************************************************
// 構造体定義
//*****************************************************************************

// XInput　スティック情報構造体
struct XINPUT_STICK_STATUS
{
	float	radian;		// 角度
	float	magnitude;	// 距離
};


//*****************************************************************************
// 構造体定義
//*****************************************************************************
struct BUTTON
{
	XMFLOAT4		color_off;		// 色
	XMFLOAT4		color_on;		// 色
	XMFLOAT3		pos;			// 座標
	XMFLOAT3		rot_off;		// 回転量
	XMFLOAT3		rot_on;			// 回転量
	float			scale_off;		// 大きさ
	float			scale_on;		// 大きさ
	float			w, h;			// 幅と高さ
	int				texNo;			// テクスチャ番号
	int				texPatternX;	// テクスチャ内分割数（X)
	int				texPatternY;	// テクスチャ内分割数（Y)
	int				animPatternMax;	// アニメーションパターン数
	int				animWait;		// アニメーションパターンの切り替えフレーム数
	int				animCount;		// アニメーション用のフレーム数をカウントする
	int				animPattern;	// アニメーション番号
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitInput(HINSTANCE hInst, HWND hWnd);
void UninitInput(void);
void UpdateInput(void);
void DrawInput(void);

//---------------------------- keyboard
bool GetKeyboardPress(int nKey);
bool GetKeyboardTrigger(int nKey);
bool GetKeyboardRepeat(int nKey);
bool GetKeyboardRelease(int nKey);

//---------------------------- mouse
BOOL IsMouseLeftPressed(void);      // 左クリックした状態
BOOL IsMouseLeftTriggered(void);    // 左クリックした瞬間
BOOL IsMouseRightPressed(void);     // 右クリックした状態
BOOL IsMouseRightTriggered(void);   // 右クリックした瞬間
BOOL IsMouseCenterPressed(void);    // 中クリックした状態
BOOL IsMouseCenterTriggered(void);  // 中クリックした瞬間
long GetMouseX(void);               // マウスがX方向に動いた相対値
long GetMouseY(void);               // マウスがY方向に動いた相対値
long GetMouseZ(void);               // マウスホイールが動いた相対値
BOOL GetMouseUse(void);
void SetMouseUse(void);
void SetMouseUse(BOOL use);
XMFLOAT2 GetMousePos(void);

//---------------------------- game pad
BOOL IsButtonPressed(int padNo,DWORD button);
BOOL IsButtonTriggered(int padNo,DWORD button);
BOOL CheckButtonWait(void);
void SetButtonWait(void);


//---------------------------- XInput用
void	InitializeXinputPad(void);		// XInputパッドの初期化
void	UninitXinputPad(void);			// XInputパッドの終了処理
void	UpdateXinputPad(void);			// XInputパッドの更新処理

BOOL	IsButtonPressedXInput(int padNo, WORD button);
int		IsButtonTriggerXInput(int padNo, int RorL);
BOOL	IsStickPressedXInput(int padNo, int stick);
BOOL	CheckStickPressedXInput(int padNo, int stick);
void	GetPadStickXInput(int padNo, int stick, XINPUT_STICK_STATUS *p);

void	UpdateXinputAllButtonStatus(int padNo);
void	UpdateXinputButtonStatus(int padNo, XINPUT_BUTTON button, BOOL press);
BOOL	IsXinputButtonStatus(int padNo, XINPUT_BUTTON button, XINPUT_BUTTON_STATUS status);

void MoveCursor(XMINT2& cursor, const int num_x, const int num_y);