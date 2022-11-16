//=============================================================================
//
// ���͏��� [input.cpp]
// Author : ���] �đ�
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "input.h"
#include "debugproc.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	NUM_KEY_MAX		(256)

// game pad�p�ݒ�l
#define DEADZONE		(2500)		// �e����25%�𖳌��]�[���Ƃ���
#define RANGE_MAX		(1000)		// �L���͈͂̍ő�l
#define RANGE_MIN		(-1000)		// �L���͈͂̍ŏ��l
#define BUTTON_WAIT		(15)		// �{�^�����͎��̃E�F�C�g�^�C��


#define XINPUT_PAD_CHECK_COUNT	60			// �p�b�h�ڑ��󋵂�`�F�b�N����Ԋu�i�t���[�����j
#define XINPUT_PAD_STICK_MAX_VALUE	32767	// XInput�̃X�e�B�b�N���Ԃ��ő�l

#define XINPUT_BUTTON_REPEAT_START	30	// ���s�[�g��n�߂�t���[����
#define XINPUT_BUTTON_REPEAT_NEXT	15	// ���s�[�g����t���[����


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
// �L�[�{�[�h
HRESULT InitKeyboard(HINSTANCE hInst, HWND hWnd);
void UninitKeyboard(void);
HRESULT UpdateKeyboard(void);

// �}�E�X
HRESULT InitializeMouse(HINSTANCE hInst, HWND hWindow); // �}�E�X�̏�����
void UninitMouse();		// �}�E�X�̏I������
HRESULT UpdateMouse();	// �}�E�X�̍X�V����

// �Q�[���p�b�h
HRESULT InitializePad(void);			// �p�b�h������
//BOOL CALLBACK SearchPadCallback(LPDIDEVICEINSTANCE lpddi, LPVOID);	// �p�b�h�����R�[���o�b�N
void UpdatePad(void);
void UninitPad(void);

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
// �L�[�{�[�h
LPDIRECTINPUT8				g_pDInput = NULL;					// IDirectInput8�C���^�[�t�F�[�X�ւ̃|�C���^
LPDIRECTINPUTDEVICE8		g_pDIDevKeyboard = NULL;			// IDirectInputDevice8�C���^�[�t�F�[�X�ւ̃|�C���^(�L�[�{�[�h)
BYTE						g_keyState[NUM_KEY_MAX];			// �L�[�{�[�h�̏�Ԃ�󂯎�郏�[�N
BYTE						g_keyStateTrigger[NUM_KEY_MAX];		// �L�[�{�[�h�̏�Ԃ�󂯎�郏�[�N
BYTE						g_keyStateRepeat[NUM_KEY_MAX];		// �L�[�{�[�h�̏�Ԃ�󂯎�郏�[�N
BYTE						g_keyStateRelease[NUM_KEY_MAX];		// �L�[�{�[�h�̏�Ԃ�󂯎�郏�[�N
int							g_keyStateRepeatCnt[NUM_KEY_MAX];	// �L�[�{�[�h�̃��s�[�g�J�E���^

// �}�E�X
static LPDIRECTINPUTDEVICE8	pMouse = NULL;				// mouse
static DIMOUSESTATE2		mouseState;					// �}�E�X�̃_�C���N�g�ȏ��
static DIMOUSESTATE2		mouseTrigger;				// �����ꂽ�u�Ԃ���ON
static BOOL					g_MouseUse = TRUE;			// �}�E�X�̎g�p��

// �Q�[���p�b�h
static LPDIRECTINPUTDEVICE8	pGamePad[GAMEPADMAX] = { NULL, NULL, NULL, NULL };	// �p�b�h�f�o�C�X
static DWORD				padState[GAMEPADMAX];		// �p�b�h���i�����Ή��j
static DWORD				padTrigger[GAMEPADMAX];		// �p�b�h�g���K�[�i�����Ή��j
static int					padCount = 0;				// ���o�����p�b�h�̐�
static int					g_buttonTime = BUTTON_WAIT;	// �{�^�����͎��̃E�F�C�g�^�C����i�[

// ���_���
static ID3D11Buffer	*g_VertexBuffer = NULL;

// �e�N�X�`�����
static ID3D11ShaderResourceView	*g_Texture[INPUT_TEXTURE_LABEL_MAX] = { NULL };
static char *g_TextureName[INPUT_TEXTURE_LABEL_MAX] = {
	"data/TEXTURE/Button/mouse_icon.png",
};


//--------------------------------- Xinput game pad
static XINPUT_STATE xPadState[GAMEPADMAX];		// Xinput�p�p�b�h���
static BOOL			xPadConnected[GAMEPADMAX];	// �ڑ���
static XINPUT_STATE xPadStateTest;				// �p�b�h�ڑ��`�F�b�N�p�̏���i�[����\����
static int			g_xPadCheckCount;			// �p�b�h�ڑ��`�F�b�N�p�J�E���g


static BOOL						g_XinputButtonOld[GAMEPADMAX][XINPUT_BUTTON_MAX];		// 
static XINPUT_BUTTON_STATUS		g_XinputButtonStatus[GAMEPADMAX][XINPUT_BUTTON_MAX];	// 
static int						g_XinputButtonRepeatCount[GAMEPADMAX][XINPUT_BUTTON_MAX];	// 
static BOOL						g_XinputButtonIsRepeat[GAMEPADMAX][XINPUT_BUTTON_MAX];	// 



//=============================================================================
// ���͏����̏�����
//=============================================================================
HRESULT InitInput(HINSTANCE hInst, HWND hWnd)
{
	HRESULT hr;

	if(!g_pDInput)
	{
		// DirectInput�I�u�W�F�N�g�̍쐬
		hr = DirectInput8Create(hInst, DIRECTINPUT_VERSION,
									IID_IDirectInput8, (void**)&g_pDInput, NULL);
	}

	// �L�[�{�[�h�̏�����
	InitKeyboard(hInst, hWnd);

 	// �}�E�X�̏�����
	InitializeMouse(hInst, hWnd);
	
	// �p�b�h�̏�����
	InitializePad();

	ID3D11Device *pDevice = GetDevice();

	//�e�N�X�`������
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

	// ���_�o�b�t�@����
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);


	// �������@XInput�p�b�h�̏�����
	InitializeXinputPad();

	return S_OK;
}

//=============================================================================
// ���͏����̏I������
//=============================================================================
void UninitInput(void)
{
	// �L�[�{�[�h�̏I������
	UninitKeyboard();

	// �}�E�X�̏I������
	UninitMouse();

	// �p�b�h�̏I������
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


	// �������@XInput�p�b�h�̏I������
	UninitXinputPad();
}

//=============================================================================
// ���͏����̍X�V����
//=============================================================================
void UpdateInput(void)
{
	// �L�[�{�[�h�̍X�V
	UpdateKeyboard();

	// �}�E�X�̍X�V
	UpdateMouse();

	// �p�b�h�̍X�V
	UpdatePad();

	// M�L�[���Z���N�g�{�^���Ń}�E�X���[�h�̕ύX
	//if (GetKeyboardTrigger(DIK_M) || IsButtonTriggered(0, BUTTON_SELECT)) SetMouseUse();

	// �{�^���̃E�F�C�g�^�C����X�V
	if (g_buttonTime < BUTTON_WAIT) g_buttonTime++;


	// �������@XInput�p�b�h�̍X�V
	//UpdateXinputPad(); // UpdateXinputPad()�ł͂Ȃ�UpdateXinputAllButtonStatus(padNo)���Ă΂�Ă��܂��s��̂��߃R�����g�A�E�g
	
	// 60�t���[����1�x�A�p�b�h�ڑ��󋵂̍X�V��s��
	if (g_xPadCheckCount >= XINPUT_PAD_CHECK_COUNT)
	{
		for (int i = 0; i < GAMEPADMAX; i++)
		{
			DWORD dwResult = XInputGetState(i, &xPadStateTest);

			if (dwResult == ERROR_SUCCESS)
			{
				// �R���g���[���[���ڑ�����Ă���
				xPadConnected[i] = TRUE;
			}
			else
			{
				// �R���g���[���[���ڑ�����Ă��Ȃ�
				xPadConnected[i] = FALSE;
			}
		}

		// �J�E���g���Z�b�g
		g_xPadCheckCount = 0;
	}
	g_xPadCheckCount++;

	for (int padNo = 0; padNo < GAMEPADMAX; padNo++)
	{
		// �ڑ�����Ă���p�b�h�̂ݏ���
		if (xPadConnected[padNo] == FALSE)
		{
			continue;
		}

		DWORD dwResult = XInputGetState(padNo, &xPadState[padNo]);

		//if(xPadState[i].Gamepad.wButtons & XINPUT_GAMEPAD_A)

		UpdateXinputAllButtonStatus(padNo);
	}
}

//=============================================================================
// �`�揈���̍X�V����
//=============================================================================
void DrawInput(void)
{
	if (GetMouseUse())	// �}�E�X�g�p���[�h�̂Ƃ��}�E�X�A�C�R����\��
	{
		//// ���_�o�b�t�@�ݒ�
		//UINT stride = sizeof(VERTEX_3D);
		//UINT offset = 0;
		//GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

		//// �}�g���N�X�ݒ�
		//SetWorldViewProjection2D();

		//// �v���~�e�B�u�g�|���W�ݒ�
		//GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

		//// �}�e���A���ݒ�
		//MATERIAL material;
		//ZeroMemory(&material, sizeof(material));
		//material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		//SetMaterial(material);

		//// �e�N�X�`���ݒ�
		//GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[INPUT_TEXTURE_LABEL_MOUSE]);

		//// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		//SetSpriteColor(g_VertexBuffer,
		//	50.0f, SCREEN_HEIGHT - 50.0f, 64.0f, 64.0f,
		//	0.0f, 0.0f, 1.0f, 1.0f, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		//// �|���S���`��
		//GetDeviceContext()->Draw(4, 0);
	}
}

//=============================================================================
// �L�[�{�[�h�̏�����
//=============================================================================
HRESULT InitKeyboard(HINSTANCE hInst, HWND hWnd)
{
	HRESULT hr;

	// �f�o�C�X�I�u�W�F�N�g��쐬
	hr = g_pDInput->CreateDevice(GUID_SysKeyboard, &g_pDIDevKeyboard, NULL);
	if(FAILED(hr) || g_pDIDevKeyboard == NULL)
	{
		MessageBox(hWnd, "�L�[�{�[�h������܂���B", "�x���I", MB_ICONWARNING);
		return hr;
	}

	// �f�[�^�t�H�[�}�b�g��ݒ�
	hr = g_pDIDevKeyboard->SetDataFormat(&c_dfDIKeyboard);
	if(FAILED(hr))
	{
		MessageBox(hWnd, "�L�[�{�[�h�̃f�[�^�t�H�[�}�b�g��ݒ�ł��܂���ł����B", "�x���I", MB_ICONWARNING);
		return hr;
	}

	// �������[�h��ݒ�i�t�H�A�O���E���h����r�����[�h�j
	hr = g_pDIDevKeyboard->SetCooperativeLevel(hWnd, (DISCL_FOREGROUND | DISCL_NONEXCLUSIVE));
	if(FAILED(hr))
	{
		MessageBox(hWnd, "�L�[�{�[�h�̋������[�h��ݒ�ł��܂���ł����B", "�x���I", MB_ICONWARNING);
		return hr;
	}

	// �L�[�{�[�h�ւ̃A�N�Z�X����l��(���͐���J�n)
	g_pDIDevKeyboard->Acquire();

	return S_OK;
}

//=============================================================================
// �L�[�{�[�h�̏I������
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
// �L�[�{�[�h�̍X�V
//=============================================================================
HRESULT UpdateKeyboard(void)
{
	HRESULT hr;
	BYTE keyStateOld[256];

	// �O��̃f�[�^��ۑ�
	memcpy(keyStateOld, g_keyState, NUM_KEY_MAX);

	// �f�o�C�X����f�[�^��擾
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
		// �L�[�{�[�h�ւ̃A�N�Z�X����擾
		g_pDIDevKeyboard->Acquire();
	}
	return S_OK;
}

//=============================================================================
// �L�[�{�[�h�̃v���X��Ԃ�擾
//=============================================================================
bool GetKeyboardPress(int key)
{
	return (g_keyState[key] & 0x80) ? true : false;
}

//=============================================================================
// �L�[�{�[�h�̃g���K�[��Ԃ�擾
//=============================================================================
bool GetKeyboardTrigger(int key)
{
	return (g_keyStateTrigger[key] & 0x80) ? true : false;
}

//=============================================================================
// �L�[�{�[�h�̃��s�[�g��Ԃ�擾
//=============================================================================
bool GetKeyboardRepeat(int key)
{
	return (g_keyStateRepeat[key] & 0x80) ? true : false;
}

//=============================================================================
// �L�[�{�[�h�̃����|�X��Ԃ�擾
//=============================================================================
bool GetKeyboardRelease(int key)
{
	return (g_keyStateRelease[key] & 0x80) ? true : false;
}

//=============================================================================
// �}�E�X�֌W�̏���
//=============================================================================
// �}�E�X�̏�����
HRESULT InitializeMouse(HINSTANCE hInst,HWND hWindow)
{
	HRESULT result;
	// �f�o�C�X�쐬
	result = g_pDInput->CreateDevice(GUID_SysMouse,&pMouse,NULL);
	if(FAILED(result) || pMouse==NULL)
	{
		MessageBox(hWindow,"No mouse","Warning",MB_OK | MB_ICONWARNING);
		return result;
	}
	// �f�[�^�t�H�[�}�b�g�ݒ�
	result = pMouse->SetDataFormat(&c_dfDIMouse2);
	if(FAILED(result))
	{
		MessageBox(hWindow,"Can't setup mouse","Warning",MB_OK | MB_ICONWARNING);
		return result;
	}
	// ���̃A�v���Ƌ������[�h�ɐݒ�
	result = pMouse->SetCooperativeLevel(hWindow, (DISCL_FOREGROUND | DISCL_NONEXCLUSIVE));
	if(FAILED(result))
	{
		MessageBox(hWindow,"Mouse mode error","Warning",MB_OK | MB_ICONWARNING);
		return result;
	}
	// �f�o�C�X�̐ݒ�
	DIPROPDWORD prop;
	prop.diph.dwSize = sizeof(prop);
	prop.diph.dwHeaderSize = sizeof(prop.diph);
	prop.diph.dwObj = 0;
	prop.diph.dwHow = DIPH_DEVICE;
	prop.dwData = DIPROPAXISMODE_REL;		// �}�E�X�̈ړ��l�@���Βl

	result = pMouse->SetProperty(DIPROP_AXISMODE,&prop.diph);
	if(FAILED(result))
	{
		MessageBox(hWindow,"Mouse property error","Warning",MB_OK | MB_ICONWARNING);
		return result;	
	}
	// �A�N�Z�X���𓾂�
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
	// �O��̒l�ۑ�
	DIMOUSESTATE2 lastMouseState = mouseState;
	// �f�[�^�擾
	result = pMouse->GetDeviceState(sizeof(mouseState),&mouseState);
	if(SUCCEEDED(result))
	{
		mouseTrigger.lX = mouseState.lX;
		mouseTrigger.lY = mouseState.lY;
		mouseTrigger.lZ = mouseState.lZ;
		// �}�E�X�̃{�^�����
		for(int i=0;i<8;i++)
		{
			mouseTrigger.rgbButtons[i] = ((lastMouseState.rgbButtons[i] ^
				mouseState.rgbButtons[i]) & mouseState.rgbButtons[i]);
		}
	}
	else	// �擾���s
	{
		// �A�N�Z�X���𓾂Ă݂�
		result = pMouse->Acquire();
	}
	return result;
}

//----------------------------------------------
BOOL IsMouseLeftPressed(void)
{
	return (BOOL)(mouseState.rgbButtons[0] & 0x80);	// �����ꂽ�Ƃ��ɗ��r�b�g�����
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
// �}�E�X�̎g�p�󋵂�擾����
//=============================================================================
BOOL GetMouseUse(void)
{
	return g_MouseUse;
}

//=============================================================================
// �}�E�X�̎g�p�󋵂�ύX����
//=============================================================================
void SetMouseUse(void)
{
	g_MouseUse = (g_MouseUse + 1) % 2;	// 0��1�iTURE��FALSE�j��s�����藈���肷��
}

//================================================= game pad
//---------------------------------------- �R�[���o�b�N�֐�
BOOL CALLBACK SearchGamePadCallback(LPDIDEVICEINSTANCE lpddi, LPVOID )
{
	HRESULT result;

	result = g_pDInput->CreateDevice(lpddi->guidInstance, &pGamePad[padCount++], NULL);
	return DIENUM_CONTINUE;	// ���̃f�o�C�X���

}
//---------------------------------------- ������
HRESULT InitializePad(void)			// �p�b�h������
{
	HRESULT		result;
	int			i;

	padCount = 0;
	// �W���C�p�b�h��T��
	g_pDInput->EnumDevices(DI8DEVCLASS_GAMECTRL, (LPDIENUMDEVICESCALLBACK)SearchGamePadCallback, NULL, DIEDFL_ATTACHEDONLY);
	// �Z�b�g�����R�[���o�b�N�֐����A�p�b�h�𔭌������������Ă΂��B

	for ( i=0 ; i<padCount ; i++ ) {
		// �W���C�X�e�B�b�N�p�̃f�[�^�E�t�H�[�}�b�g��ݒ�
		result = pGamePad[i]->SetDataFormat(&c_dfDIJoystick);
		if ( FAILED(result) )
			return false; // �f�[�^�t�H�[�}�b�g�̐ݒ�Ɏ��s

		// ���[�h��ݒ�i�t�H�A�O���E���h����r�����[�h�j
//		result = pGamePad[i]->SetCooperativeLevel(hWindow, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
//		if ( FAILED(result) )
//			return false; // ���[�h�̐ݒ�Ɏ��s

		// ���̒l�͈̔͂�ݒ�
		// X���AY���̂��ꂼ��ɂ��āA�I�u�W�F�N�g���񍐉\�Ȓl�͈̔͂�Z�b�g����B
		// (max-min)�́A�ő�10,000(?)�B(max-min)/2�������l�ɂȂ�B
		// ����傫������΁A�A�i���O�l�ׂ̍��ȓ�����߂炦����B(�p�b�h�̐��\�ɂ��)
		DIPROPRANGE				diprg;
		ZeroMemory(&diprg, sizeof(diprg));
		diprg.diph.dwSize		= sizeof(diprg); 
		diprg.diph.dwHeaderSize	= sizeof(diprg.diph); 
		diprg.diph.dwHow		= DIPH_BYOFFSET; 
		diprg.lMin				= RANGE_MIN;
		diprg.lMax				= RANGE_MAX;
		// X���͈̔͂�ݒ�
		diprg.diph.dwObj = DIJOFS_X;
		pGamePad[i]->SetProperty(DIPROP_RANGE, &diprg.diph);
		// Y���͈̔͂�ݒ�
		diprg.diph.dwObj = DIJOFS_Y;
		pGamePad[i]->SetProperty(DIPROP_RANGE, &diprg.diph);
		// Z���͈̔͂�ݒ�i�E�X�e�B�b�N��X���j
		diprg.diph.dwObj = DIJOFS_Z;
		pGamePad[i]->SetProperty(DIPROP_RANGE, &diprg.diph);
		// RZ���͈̔͂�ݒ�i�E�X�e�B�b�N��Y���j
		diprg.diph.dwObj = DIJOFS_RZ;
		pGamePad[i]->SetProperty(DIPROP_RANGE, &diprg.diph);

		// �e�����ƂɁA�����̃]�[���l��ݒ肷��B
		// �����]�[���Ƃ́A��������̔����ȃW���C�X�e�B�b�N�̓����𖳎�����͈͂̂��ƁB
		// �w�肷��l�́A10000�ɑ΂��鑊�Βl(2000�Ȃ�20�p�[�Z���g)�B
		DIPROPDWORD				dipdw;
		dipdw.diph.dwSize		= sizeof(DIPROPDWORD);
		dipdw.diph.dwHeaderSize	= sizeof(dipdw.diph);
		dipdw.diph.dwHow		= DIPH_BYOFFSET;
		dipdw.dwData			= DEADZONE;
		//X���̖����]�[����ݒ�
		dipdw.diph.dwObj = DIJOFS_X;
		pGamePad[i]->SetProperty(DIPROP_DEADZONE, &dipdw.diph);
		//Y���̖����]�[����ݒ�
		dipdw.diph.dwObj = DIJOFS_Y;
		pGamePad[i]->SetProperty(DIPROP_DEADZONE, &dipdw.diph);
		//Z���̖����]�[����ݒ�i�E�X�e�B�b�N��X���j
		dipdw.diph.dwObj = DIJOFS_Z;
		pGamePad[i]->SetProperty(DIPROP_DEADZONE, &dipdw.diph);
		//RZ���̖����]�[����ݒ�i�E�X�e�B�b�N��Y���j
		dipdw.diph.dwObj = DIJOFS_RZ;
		pGamePad[i]->SetProperty(DIPROP_DEADZONE, &dipdw.diph);
			
		//�W���C�X�e�B�b�N���͐���J�n
		pGamePad[i]->Acquire();
	}
	return true;
}
//------------------------------------------- �I������
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

//------------------------------------------ �X�V
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
		padState[i] = 0x00000000l;	// ������

		result = pGamePad[i]->Poll();	// �W���C�X�e�B�b�N�Ƀ|�[���������
		if (FAILED(result)) {
			result = pGamePad[i]->Acquire();
			while (result == DIERR_INPUTLOST)
				result = pGamePad[i]->Acquire();
		}

		result = pGamePad[i]->GetDeviceState(sizeof(DIJOYSTATE), &dijs);	// �f�o�C�X��Ԃ�ǂݎ��
		if (result == DIERR_INPUTLOST || result == DIERR_NOTACQUIRED) {
			result = pGamePad[i]->Acquire();
			while (result == DIERR_INPUTLOST)
				result = pGamePad[i]->Acquire();
		}
		
		// �R�Q�̊e�r�b�g�ɈӖ���������A�{�^�������ɉ����ăr�b�g��I���ɂ���
		// ���X�e�B�b�N
		//* y-axis (forward)
		if (dijs.lY < 0)					padState[i] |= BUTTON_L_UP;
		//* y-axis (backward)
		if (dijs.lY > 0)					padState[i] |= BUTTON_L_DOWN;
		//* x-axis (left)
		if (dijs.lX < 0)					padState[i] |= BUTTON_L_LEFT;
		//* x-axis (right)
		if (dijs.lX > 0)					padState[i] |= BUTTON_L_RIGHT;
		// �E�X�e�B�b�N
		//* y-axis (forward)
		if (dijs.lRz < 0)					padState[i] |= BUTTON_R_UP;
		//* y-axis (backward)
		if (dijs.lRz > 0)					padState[i] |= BUTTON_R_DOWN;
		//* x-axis (left)
		if (dijs.lZ < 0)					padState[i] |= BUTTON_R_LEFT;
		//* x-axis (right)
		if (dijs.lZ > 0)					padState[i] |= BUTTON_R_RIGHT;
		//* �w�{�^��
		if (dijs.rgbButtons[rgbButtons_X] & 0x80)	padState[i] |= BUTTON_X;
		//* �`�{�^��
		if (dijs.rgbButtons[rgbButtons_A] & 0x80)	padState[i] |= BUTTON_A;
		//* �a�{�^��
		if (dijs.rgbButtons[rgbButtons_B] & 0x80)	padState[i] |= BUTTON_B;
		//* �x�{�^��
		if (dijs.rgbButtons[rgbButtons_Y] & 0x80)	padState[i] |= BUTTON_Y;
		//* �k�{�^��
		if (dijs.rgbButtons[rgbButtons_L] & 0x80)	padState[i] |= BUTTON_L1;
		//* �q�{�^��
		if (dijs.rgbButtons[rgbButtons_R] & 0x80)	padState[i] |= BUTTON_R1;
		//* �k2�{�^��
		if (dijs.rgbButtons[rgbButtons_L2] & 0x80)	padState[i] |= BUTTON_L2;
		//* �q2�{�^��
		if (dijs.rgbButtons[rgbButtons_R2] & 0x80)	padState[i] |= BUTTON_R2;
		//* Select�{�^��
		if (dijs.rgbButtons[rgbButtons_SELECT] & 0x80)	padState[i] |= BUTTON_SELECT;
		//* �r�s�`�q�s�{�^��
		if (dijs.rgbButtons[rgbButtons_START] & 0x80)	padState[i] |= BUTTON_START;
		//* L3�{�^��
		if (dijs.rgbButtons[rgbButtons_L3] & 0x80)	padState[i] |= BUTTON_L3;
		//* R3�{�^��
		if (dijs.rgbButtons[rgbButtons_R3] & 0x80)	padState[i] |= BUTTON_R3;

		//* �\���{�^��
		if (dijs.rgdwPOV[0] != 0xFFFFFFFF)
		{
			float rad = XMConvertToRadians((dijs.rgdwPOV[0] / 100.0f));
			float x = sinf(rad);	// ���v���Ȃ̂�cos��sin��t�ɂ���
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

		// Trigger�ݒ�
		padTrigger[i] = ((lastPadState ^ padState[i])	// �O��ƈ���Ă���
			& padState[i]);					// �������ON�̂��
	}
}
//----------------------------------------------- ����
BOOL IsButtonPressed(int padNo,DWORD button)
{
	return (button & padState[padNo]);
}

BOOL IsButtonTriggered(int padNo, DWORD button)
{
	return (button & padTrigger[padNo]);
}

// �{�^�����͂��\���ǂ���
BOOL CheckButtonWait(void)
{
	BOOL answer = FALSE;
	if (g_buttonTime >= BUTTON_WAIT) answer = TRUE;
	return answer;
}

// �{�^�����͂̃E�F�C�g�^�C���̃��Z�b�g
void SetButtonWait(void)
{
	g_buttonTime = 0;
}

// Xinput�p=======================================================

void InitializeXinputPad(void)
{
	// �R���g���[���[�̏������E���o
	// Xinput�ł̓R���g���[���[�ԍ���0~3�܂Ŋ��蓖�Ă���
	for (int i = 0; i < GAMEPADMAX; i++)
	{
		ZeroMemory(&xPadState[i], sizeof(XINPUT_STATE));
		ZeroMemory(&xPadConnected[i], sizeof(BOOL));


		DWORD dwResult = XInputGetState(i, &xPadStateTest);

		if (dwResult == ERROR_SUCCESS)
		{
			// �R���g���[���[���ڑ�����Ă���
			xPadConnected[i] = TRUE;
		}
		else
		{
			// �R���g���[���[���ڑ�����Ă��Ȃ�
			xPadConnected[i] = FALSE;
		}
	}

	// �ڑ���Ԃ̒���`�F�b�N�p�J�E���g�̏�����
	g_xPadCheckCount = 0;


	for (int padNo = 0; padNo < GAMEPADMAX; padNo++)
	{
		// �ڑ�����Ă���p�b�h�̂ݏ���
		if (xPadConnected[padNo] == FALSE)
		{
			continue;
		}

		DWORD dwResult = XInputGetState(padNo, &xPadState[padNo]);

		// �S��FALSE�ŏ�����
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
	// XInput���~�@�U����~�߂�
	XInputEnable(false);
}

void UpdateXinputPad(void)
{
	// 60�t���[����1�x�A�p�b�h�ڑ��󋵂̍X�V��s��
	if (g_xPadCheckCount >= XINPUT_PAD_CHECK_COUNT)
	{
		for (int i = 0; i < GAMEPADMAX; i++)
		{
			DWORD dwResult = XInputGetState(i, &xPadStateTest);

			if (dwResult == ERROR_SUCCESS)
			{
				// �R���g���[���[���ڑ�����Ă���
				xPadConnected[i] = TRUE;
			}
			else
			{
				// �R���g���[���[���ڑ�����Ă��Ȃ�
				xPadConnected[i] = FALSE;
			}
		}

		// �J�E���g���Z�b�g
		g_xPadCheckCount = 0;
	}
	g_xPadCheckCount++;

	for (int padNo = 0; padNo < GAMEPADMAX; padNo++)
	{
		// �ڑ�����Ă���p�b�h�̂ݏ���
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

			// ���s�[�g�`�F�b�N
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

//----------------------------------------------- ����

/*===============================================
���	�F	�{�^�������`�F�b�N�֐�
�����	�F	�g�i���l
����	�F	�Ή�����{�^���̃}�N�����͉��L�Q�Ƃ̂��ƁB
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
���	�F	�g���K�[�������݃`�F�b�N�֐�
�����	�F	�g�i���l
����	�F	�߂�l��0~255�B
===============================================*/
int IsButtonTriggerXInput(int padNo, int RorL)
{
	switch (RorL)
	{
	case TRIGGER_LEFT:
		// 臒l��z���Ă��邩�`�F�b�N
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
		// 臒l��z���Ă��邩�`�F�b�N
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
���	�F	�X�e�B�b�N�����`�F�b�N�֐�
�����	�F	�g�i���l
����	�F	�w�肵���X�e�B�b�N���w�肵�������i�㉺���E�j�ɌX���Ă��邩�ATRUE/FALSE�ŕԂ��B
			��2�����ɂ́@�񋓌^XINPUT_STICK�Ő錾���ꂽ8�̒萔�iXINPUT_LEFTSTICK_X_RIGHT�`XINPUT_RIGHTSTICK_Y_DOWN�j��g�p���Ă��������B
===============================================*/
BOOL IsStickPressedXInput(int padNo, int stick)
{
	BOOL pressed = FALSE;

	switch (stick)
	{
		// ���X�e�B�b�N ==========
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


		// �E�X�e�B�b�N ==========
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
���	�F	�X�e�B�b�N���̓`�F�b�N�֐�
�����	�F	�g�i���l
����	�F	�w�肵���X�e�B�b�N�ɉ���������͂����邩�`�F�b�N����B
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
���	�F	�X�e�B�b�N���擾�֐�
�����	�F	�g�i���l
����	�F	�w�肵���X�e�B�b�N�̃��W�A���Ɛ��K�����ꂽ��������߂�B
			�\�ߗp�ӂ����\���̂̃|�C���^��p���A���̃A�h���X��ɏ���i�[����B
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
		// X���̏���
		if ((xPadState[padNo].Gamepad.sThumbLX > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
			|| (xPadState[padNo].Gamepad.sThumbLX < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE))
		{
			x = (float)xPadState[padNo].Gamepad.sThumbLX;
		}

		// Y���̏���
		if ((xPadState[padNo].Gamepad.sThumbLY > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
			|| (xPadState[padNo].Gamepad.sThumbLY < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE))
		{
			y = (float)xPadState[padNo].Gamepad.sThumbLY;
		}
		break;

	case XINPUT_RIGHTSTICK:
		// X���̏���
		if ((xPadState[padNo].Gamepad.sThumbRX > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)
			|| (xPadState[padNo].Gamepad.sThumbRX < -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE))
		{
			x = (float)xPadState[padNo].Gamepad.sThumbRX;
		}

		// Y���̏���
		if ((xPadState[padNo].Gamepad.sThumbRY > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)
			|| (xPadState[padNo].Gamepad.sThumbRY < -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE))
		{
			y = (float)xPadState[padNo].Gamepad.sThumbRY;
		}
		break;
	}

	// �����̏���
	status.radian = atan2f(y, x);

	// �����̏���
	mag = (float)sqrt(x * x + y * y);
	if (mag > XINPUT_PAD_STICK_MAX_VALUE)
	{
		mag = XINPUT_PAD_STICK_MAX_VALUE;
	}
	status.magnitude = mag / (float)XINPUT_PAD_STICK_MAX_VALUE;

	// �󂯎�����|�C���^�ō\���̂�Ԃ�
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