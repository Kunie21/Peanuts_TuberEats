//=============================================================================
//
// ���͏��� [input.h]
// Author : ���] �đ�
//
//=============================================================================
#pragma once

#include "main.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************

// �v���O������������Ƃ��Ɏg��
#define	USE_KEYBOARD				// �錾����ƃL�[�{�[�h�ő���\�ɂȂ�
#define	USE_MOUSE					// �錾����ƃ}�E�X�ő���\�ɂȂ�
#define	USE_PAD						// �錾����ƃp�b�h�ő���\�ɂȂ�

/* game pad��� */
//#define BUTTON_UP		0x00000001l	// �����L�[��(.IY<0)
//#define BUTTON_DOWN		0x00000002l	// �����L�[��(.IY>0)
//#define BUTTON_LEFT		0x00000004l	// �����L�[��(.IX<0)
//#define BUTTON_RIGHT	0x00000008l	// �����L�[�E(.IX>0)
//#define BUTTON_X		0x00000010l	// �w�{�^��(.rgbButtons[0]&0x80)
//#define BUTTON_A		0x00000020l	// �`�{�^��(.rgbButtons[1]&0x80)
//#define BUTTON_B		0x00000040l	// �a�{�^��(.rgbButtons[2]&0x80)
//#define BUTTON_Y		0x00000080l	// �x�{�^��(.rgbButtons[3]&0x80)
//#define BUTTON_L1		0x00000100l	// �k�{�^��(.rgbButtons[4]&0x80)
//#define BUTTON_R1		0x00000200l	// �q�{�^��(.rgbButtons[5]&0x80)
//#define BUTTON_L2		0x00000400l	// �k�{�^��(.rgbButtons[6]&0x80)
//#define BUTTON_R2		0x00000800l	// �q�{�^��(.rgbButtons[7]&0x80)
//#define BUTTON_SELECT	0x00001000l	// �y�{�^��(.rgbButtons[8]&0x80)
//#define BUTTON_START	0x00002000l	// �r�s�`�q�s�{�^��(.rgbButtons[9]&0x80)
//#define BUTTON_L3		0x00004000l	// L3�{�^��(.rgbButtons[10]&0x80)
//#define BUTTON_R3		0x00008000l	// R3�{�^��(.rgbButtons[11]&0x80)
//#define BUTTON_R_UP		0x00010000l	// �����L�[��(.IY<0)
//#define BUTTON_R_DOWN	0x00020000l	// �����L�[��(.IY>0)
//#define BUTTON_R_LEFT	0x00040000l	// �����L�[��(.IX<0)
//#define BUTTON_R_RIGHT	0x00080000l	// �����L�[�E(.IX>0)
//#define BUTTON_L_UP		0x00100000l	// ��{�^��(.rgdwPOV[0]&0x80)
//#define BUTTON_L_DOWN	0x00200000l	// ���{�^��(.rgdwPOV[0]&0x80)
//#define BUTTON_L_LEFT	0x00400000l	// ���{�^��(.rgdwPOV[0]&0x80)
//#define BUTTON_L_RIGHT	0x00800000l	// �E�{�^��(.rgdwPOV[0]&0x80)

#define GAMEPADMAX		4			// �����ɐڑ�����W���C�p�b�h�̍ő吔��Z�b�g

#define BUTTON_UP		0x00000001l	// �����L�[��(.IY<0)
#define BUTTON_DOWN		0x00000002l	// �����L�[��(.IY>0)
#define BUTTON_LEFT		0x00000004l	// �����L�[��(.IX<0)
#define BUTTON_RIGHT	0x00000008l	// �����L�[�E(.IX>0)

#define BUTTON_X		0x00000010l	// �w�{�^��(.rgbButtons[0]&0x80)
#define BUTTON_A		0x00000020l	// �`�{�^��(.rgbButtons[1]&0x80)
#define BUTTON_B		0x00000040l	// �a�{�^��(.rgbButtons[2]&0x80)
#define BUTTON_Y		0x00000080l	// �x�{�^��(.rgbButtons[3]&0x80)

#define BUTTON_L1		0x00000100l	// �k�{�^��(.rgbButtons[4]&0x80)
#define BUTTON_R1		0x00000200l	// �q�{�^��(.rgbButtons[5]&0x80)

#define BUTTON_L2		0x00000400l	// �k�{�^��(.rgbButtons[6]&0x80)
#define BUTTON_R2		0x00000800l	// �q�{�^��(.rgbButtons[7]&0x80)
#define BUTTON_SELECT	0x00001000l	// �y�{�^��(.rgbButtons[8]&0x80)
#define BUTTON_START	0x00002000l	// �r�s�`�q�s�{�^��(.rgbButtons[9]&0x80)

#define BUTTON_L3		0x00004000l	// L3�{�^��(.rgbButtons[10]&0x80)
#define BUTTON_R3		0x00008000l	// R3�{�^��(.rgbButtons[11]&0x80)


#define BUTTON_R_UP		0x00010000l	// �����L�[��(.IY<0)
#define BUTTON_R_DOWN	0x00020000l	// �����L�[��(.IY>0)
#define BUTTON_R_LEFT	0x00040000l	// �����L�[��(.IX<0)
#define BUTTON_R_RIGHT	0x00080000l	// �����L�[�E(.IX>0)
#define BUTTON_L_UP		0x00100000l	// ��{�^��(.rgdwPOV[0]&0x80)
#define BUTTON_L_DOWN	0x00200000l	// ���{�^��(.rgdwPOV[0]&0x80)
#define BUTTON_L_LEFT	0x00400000l	// ���{�^��(.rgdwPOV[0]&0x80)
#define BUTTON_L_RIGHT	0x00800000l	// �E�{�^��(.rgdwPOV[0]&0x80)



//enum {	// �{�^���ɑ΂���rgbButtons[?]�z��̔z��ԍ�(DirectInput��ł�XYAB�{�^���ʒu�ɍ��킹��)
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

enum {	// �{�^���ɑ΂���rgbButtons[?]�z��̔z��ԍ�(DirectInput��ł�XYAB�{�^���ʒu�ɍ��킹��)
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


// XInput�p
#define TRIGGER_LEFT	0		// ���g���K�[
#define TRIGGER_RIGHT	1		// �E�g���K�[

enum XINPUT_STICK {

	XINPUT_LEFTSTICK_X_RIGHT,	// ���X�e�B�b�N�@�E
	XINPUT_LEFTSTICK_X_LEFT,	// ���X�e�B�b�N�@��
	XINPUT_LEFTSTICK_Y_UP,		// ���X�e�B�b�N�@��
	XINPUT_LEFTSTICK_Y_DOWN,	// ���X�e�B�b�N�@��

	XINPUT_RIGHTSTICK_X_RIGHT,	// �E�X�e�B�b�N�@�E
	XINPUT_RIGHTSTICK_X_LEFT,	// �E�X�e�B�b�N�@��
	XINPUT_RIGHTSTICK_Y_UP,		// �E�X�e�B�b�N�@��
	XINPUT_RIGHTSTICK_Y_DOWN,	// �E�X�e�B�b�N�@��

	XINPUT_LEFTSTICK,			// ���X�e�B�b�N
	XINPUT_RIGHTSTICK,			// �E�X�e�B�b�N
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
// �\���̒�`
//*****************************************************************************

// XInput�@�X�e�B�b�N���\����
struct XINPUT_STICK_STATUS
{
	float	radian;		// �p�x
	float	magnitude;	// ����
};


//*****************************************************************************
// �\���̒�`
//*****************************************************************************
struct BUTTON
{
	XMFLOAT4		color_off;		// �F
	XMFLOAT4		color_on;		// �F
	XMFLOAT3		pos;			// ���W
	XMFLOAT3		rot_off;		// ��]��
	XMFLOAT3		rot_on;			// ��]��
	float			scale_off;		// �傫��
	float			scale_on;		// �傫��
	float			w, h;			// ���ƍ���
	int				texNo;			// �e�N�X�`���ԍ�
	int				texPatternX;	// �e�N�X�`����������iX)
	int				texPatternY;	// �e�N�X�`����������iY)
	int				animPatternMax;	// �A�j���[�V�����p�^�[����
	int				animWait;		// �A�j���[�V�����p�^�[���̐؂�ւ��t���[����
	int				animCount;		// �A�j���[�V�����p�̃t���[������J�E���g����
	int				animPattern;	// �A�j���[�V�����ԍ�
};

//*****************************************************************************
// �v���g�^�C�v�錾
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
BOOL IsMouseLeftPressed(void);      // ���N���b�N�������
BOOL IsMouseLeftTriggered(void);    // ���N���b�N�����u��
BOOL IsMouseRightPressed(void);     // �E�N���b�N�������
BOOL IsMouseRightTriggered(void);   // �E�N���b�N�����u��
BOOL IsMouseCenterPressed(void);    // ���N���b�N�������
BOOL IsMouseCenterTriggered(void);  // ���N���b�N�����u��
long GetMouseX(void);               // �}�E�X��X�����ɓ��������Βl
long GetMouseY(void);               // �}�E�X��Y�����ɓ��������Βl
long GetMouseZ(void);               // �}�E�X�z�C�[�������������Βl
BOOL GetMouseUse(void);
void SetMouseUse(void);

//---------------------------- game pad
BOOL IsButtonPressed(int padNo,DWORD button);
BOOL IsButtonTriggered(int padNo,DWORD button);
BOOL CheckButtonWait(void);
void SetButtonWait(void);


//---------------------------- XInput�p
void	InitializeXinputPad(void);		// XInput�p�b�h�̏�����
void	UninitXinputPad(void);			// XInput�p�b�h�̏I������
void	UpdateXinputPad(void);			// XInput�p�b�h�̍X�V����

BOOL	IsButtonPressedXInput(int padNo, WORD button);
int		IsButtonTriggerXInput(int padNo, int RorL);
BOOL	IsStickPressedXInput(int padNo, int stick);
BOOL	CheckStickPressedXInput(int padNo, int stick);
void	GetPadStickXInput(int padNo, int stick, XINPUT_STICK_STATUS *p);

void	UpdateXinputAllButtonStatus(int padNo);
void	UpdateXinputButtonStatus(int padNo, XINPUT_BUTTON button, BOOL press);
BOOL	IsXinputButtonStatus(int padNo, XINPUT_BUTTON button, XINPUT_BUTTON_STATUS status);