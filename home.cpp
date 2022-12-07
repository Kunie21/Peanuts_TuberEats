//=============================================================================
//
// �`�[�����S��ʏ��� [home.cpp]
// Author : �t�u��
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "fade.h"
#include "home.h"
#include "texture2d.h"
#include "character_select.h"
#include "rocket_select.h"
#include "wallet.h"
#include "input.h"
#include "collision.h"

//*****************************************************************************
// UI��`
//*****************************************************************************
// UI��
enum UI_LABEL {
	UI_BG = 0,
	UI_ICON,
	UI_ROCKET_NAME,
	UI_PLAYER,
	UI_LEFT,
	UI_RIGHT,
	UI_START,
	UI_SHOP,
	UI_BACK,
	UI_NUM
};
// �Q�ƃe�N�X�`����
TEXTURE_LABEL g_tl[UI_NUM] = {
	TEXTURE_LABEL_BG,
	TEXTURE_LABEL_ICON,
	TEXTURE_LABEL_ROCKET_1,
	TEXTURE_LABEL_CHARACTER_01,
	TEXTURE_LABEL_START_BUTTON_1,
	TEXTURE_LABEL_LEFT,
	TEXTURE_LABEL_RIGHT,
	TEXTURE_LABEL_SHOP,
	TEXTURE_LABEL_ICON,
};
// UI�ڍ׊Ǘ�
static TEXTURE2D_DESC	g_td[UI_NUM];

//*****************************************************************************
// �{�^����`
//*****************************************************************************
// �{�^����
enum BT_LABEL {
	BT_START = 0,
	BT_LEFT,
	BT_RIGHT,
	BT_SHOP,
	BT_BACK,
	BT_NUM
};
// �{�^���\
#define BT_NUM_X 2
#define BT_NUM_Y 4
static BT_LABEL g_btTbl[BT_NUM_Y][BT_NUM_X] = {
	{BT_BACK, BT_BACK},
	{BT_LEFT, BT_RIGHT},
	{BT_SHOP, BT_SHOP},
	{BT_START, BT_START},
};
// �J�[�\���ʒu
XMINT2 g_cursor = { 3, 0 };	// BT_START����
// �{�^���ڍ׊Ǘ�
static BUTTON_DESC g_bd[BT_NUM];
// �Q��UI��
UI_LABEL g_ul[BT_NUM] = {
	UI_START,
	UI_LEFT,
	UI_RIGHT,
	UI_SHOP,
	UI_BACK
};
// �{�^����S�ăI�t�ɂ���
void SetButtonOffAll(void)
{
	for (int i = 0; i < BT_NUM; i++) {
		if (g_bd[i].b_on) {
			g_bd[i].b_on = FALSE;
			g_bd[i].p_td->col = g_bd[i].col_off;
			g_bd[i].p_td->scl = g_bd[i].scl_off;
		}
	}
}
// �{�^�����I���ɂ���
void SetButtonOn(int bt)
{
	if (!g_bd[bt].b_on) {
		g_bd[bt].b_on = TRUE;
		g_bd[bt].p_td->col = g_bd[bt].col_on;
		g_bd[bt].p_td->scl = g_bd[bt].scl_on;
	}
}
// �J�[�\���ʒu����{�^�������擾����
BT_LABEL GetButtonByCursor(void) {
	return g_btTbl[g_cursor.x][g_cursor.y];
}
// �J�[�\��������{�^�����I���ɂ���
void SetButtonOnByCursor(void) {
	int bt = GetButtonByCursor();
	g_bd[bt].b_on = TRUE;
	SetButtonOn(bt);
}
// �{�^��������J�[�\���ʒu��ύX����
void SetButtonPosition(int bt) {
	for (int x = 0; x < BT_NUM_X; x++) {
		for (int y = 0; y < BT_NUM_Y; y++) {
			if (g_btTbl[x][y] == (BT_LABEL)bt) {
				g_cursor = { x, y };
				return;
			}
		}
	}
}
// �{�^�����Ƃ̏���
void ButtonPressed(int bt)
{
	switch (bt)
	{
	case BT_START:
		break;

	case BT_LEFT:
		break;

	case BT_RIGHT:
		break;

	case BT_SHOP:
		break;

	case BT_BACK:
		break;

	case BT_NUM:
		break;

	default:
		break;
	}
}
// �}�E�X�E�J�[�\���̂���{�^�����I���ɂ���
void CheckButtonOn(void)
{
	// �J�[�\��������{�^��ON
	if (GetMouseUse())	// �}�E�X���g���Ă���Ƃ�
	{
		for (int i = 0; i < BT_NUM; i++)
		{
			if (CollisionMouse(g_bd[i].pos, g_bd[i].size))
			{
				// �}�E�X�J�[�\�����̂��Ă���{�^�����I���ɂ���
				g_bd[i].b_on = TRUE;
				SetButtonOn(i);

				// �L�[�{�[�h�p�̃{�^���ʒu���ύX
				SetButtonPosition(i);

				// �{�^���������ꂽ
				if (IsMouseLeftTriggered()) ButtonPressed(i);

				break;	// ON�ɂ���̂�1����
			}
		}
	}
	else	// �L�[�{�[�h���g���Ă���Ƃ�
	{
		// �J�[�\���𓮂���
		MoveCursor(g_cursor, BT_NUM_X, BT_NUM_Y);

		// �J�[�\�����̂��Ă���{�^�����I���ɂ���
		SetButtonOnByCursor();

		// �{�^���������ꂽ
		if (GetKeyboardTrigger(DIK_RETURN)) ButtonPressed(GetButtonByCursor());
	}
}

//*****************************************************************************
// HOME_UI�����ݒ�
//*****************************************************************************
void InitHomeUI(void)
{
	// UI�ڍאݒ�
	for (int i = 0; i < UI_NUM; i++)
	{
		// �e�N�X�`���̑Ή��t��
		g_td[i].tex = g_tl[i];

		// �e�N�X�`���T�C�Y�̎擾
		g_td[i].size = GetTextureSize(g_tl[i]);

		// ���̑��ݒ�
		switch (i)
		{
		case UI_BG:
			break;

		case UI_ICON:
			g_td[i].posType = POSITION_LEFTTOP;
			g_td[i].ctrType = CENTER_LEFTTOP;
			g_td[i].pos = { 0.0f, 0.0f };
			break;

		case UI_ROCKET_NAME:
			g_td[i].posType = POSITION_LEFTTOP;
			g_td[i].ctrType = CENTER_LEFT;
			g_td[i].pos = { g_td[UI_ICON].size.x, g_td[UI_ICON].size.y * 0.5f };
			break;

		case UI_PLAYER:
			g_td[i].posType = POSITION_RIGHT;
			g_td[i].ctrType = CENTER_RIGHT;
			g_td[i].pos = { -50.0f, 0.0f };
			break;

		case UI_LEFT:
			g_td[i].posType = POSITION_RIGHT;
			g_td[i].ctrType = CENTER_CENTER;
			g_td[i].pos = {
				g_td[UI_PLAYER].pos.x - g_td[UI_PLAYER].size.x,
				g_td[UI_PLAYER].pos.y
			};
			break;

		case UI_RIGHT:
			g_td[i].posType = POSITION_RIGHT;
			g_td[i].ctrType = CENTER_CENTER;
			g_td[i].pos = {
				g_td[UI_PLAYER].pos.x,
				g_td[UI_PLAYER].pos.y
			};
			break;

		case UI_START:
			g_td[i].posType = POSITION_RIGHTBOTTOM;
			g_td[i].ctrType = CENTER_RIGHTBOTTOM;
			g_td[i].pos = { -5.0f, -5.0f };
			break;

		case UI_SHOP:
			g_td[i].posType = POSITION_RIGHTBOTTOM;
			g_td[i].ctrType = CENTER_RIGHTBOTTOM;
			g_td[i].pos = {
				g_td[UI_START].pos.x,
				g_td[UI_START].pos.y - g_td[UI_START].size.y
			};
			break;

		case UI_BACK:
			g_td[i].posType = POSITION_RIGHTTOP;
			g_td[i].ctrType = CENTER_RIGHTTOP;
			g_td[i].pos = { 0.0f, 0.0f };
			break;

		default:
			break;
		}
	}

	// �{�^���ڍאݒ�
	for (int i = 0; i < BT_NUM; i++)
	{
		SetUIButton(&g_bd[i], &g_td[g_ul[i]]);
	}
}
void UpdateHomeUI(void)
{
	// �S�{�^��OFF
	SetButtonOffAll();

	// �{�^��ON�`�F�b�N
	CheckButtonOn();

}
void DrawHomeUI(void)
{
	for (int i = 0; i < UI_NUM; i++)
	{
		DrawTexture2D(&g_td[i]);
	}
}

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static BOOL				g_Load = FALSE;
static HOME_MODE		g_Home = HOME_MODE_HOME;

//=============================================================================
// ����������
//=============================================================================
HRESULT InitHome(void)
{
	InitHomeUI();
	//InitRocketSelect();
	//InitCharacterSelect();
	//InitWallet();

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitHome(void)
{
	if (g_Load == FALSE) return;

	UninitRocketSelect();
	UninitCharacterSelect();
	UninitWallet();

	g_Load = FALSE;
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateHome(void)
{
	UpdateHomeUI();

	//UpdateRocketSelect();
	//UpdateCharacterSelect();
	//UpdateWallet();


	if (GetKeyboardTrigger(DIK_1))
	{
		g_Home = HOME_MODE_HOME;
	}
	if (GetKeyboardTrigger(DIK_2))
	{
		g_Home = HOME_MODE_SHOP;
	}
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawHome(void)
{
	DrawHomeUI();

	//DrawRocketSelect();
	//DrawCharacterSelect();
	//DrawWallet();
}

HOME_MODE GetHomeMode(void) {
	return g_Home;
}