//=============================================================================
//
// �`�[�����S��ʏ��� [rocket_select.cpp]
// Author : �t�u��
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "fade.h"
#include "texture2d.h"
#include "input.h"
#include "model.h"
#include "rocket_select.h"
#include "wallet.h"
#include "home.h"
#include "player.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH)	// �w�i�T�C�Y��
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)	// �w�i�T�C�Y�c

#define SIZE_SHOP_MENU		{ 1001.0f, 1080.0f }		// �L�����N�^�[�T�C�Y�c
#define POS_SHOP_MENU		{ 919.0f, 0.0f }		// �L�����N�^�[�T�C�Y�c
#define POS_STATUSBAR		{ 160.0f, 50.0f }		// �L�����N�^�[�T�C�Y�c
#define POS_STATUSBARPOINT		{ 162.5f, 52.7f }		// �L�����N�^�[�T�C�Y�c
#define DISTANCE_STATUSBAR_Y			(76.0f)	// �L�����N�^�[�T�C�Y�c
#define DISTANCE_STATUSBARPOINT_X		(79.2f)	// �L�����N�^�[�T�C�Y�c

#define COL_BLACK			{0.0f,0.0f,0.0f,1.0f}	// �����F
#define COL_ORIGINAL		{1.0f,1.0f,1.0f,1.0f}	// ���̐F

#define SLIDE_X				(1040.0f)
#define ANIM_SLIDE			(70.0f)		// ���j���[���X���C�h���Ă���X�s�[�h
#define ANIM_ALPHA			(0.1f)		// ���j���[���F�Â��X�s�[�h
#define ANIM_SCALING		(0.1f)		// ���j���[���傫���Ȃ�{��
#define SHOP_SLIDE_Y		(-200.0f)
#define SHOP_SLIDE_SPD		(20.0f)
#define ROCKET_SLIDE_Y		(40.0f)
#define ROCKET_SLIDE_SPD	(5.0f)
#define ROCKET_STAGE_Y		(-95.0f)
#define ROCKET_DEF_Y		(-15.0f)

#define STATUS_SLIDE_Y	(-500.0f)
#define STATUS_MAX		(10)
#define STATUS_MAX_COLOR { 64.0f, 64.0f, 0.0f, 1.0f };

#define SHOP_X		(0)
#define SHOP_Y		(0)

#define TEXT_START_Y (255.0f)
#define TEXT_BETWN_Y (103.0f)
#define TEXT_START_X (330.0f)
#define TEXT_BETWN_X (-20.0f)
#define ICON_BETWN_X (-35.0f)

#define ROCKET_SCL (0.5f)

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
//void DrawTextureStatus(void);

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static BOOL			g_Load = FALSE;

static MODEL_DATA	g_ModelStage;	// �\�����f���̊Ǘ�
static SRT			g_RocketSRT;	// �\�����f���̊Ǘ�
static int			g_ModelNo_Rocket;  // �I�����Ă��郍�P�b�g�̃��f���ԍ�
static int			g_SelectBar;	   // �Z���N�g�o�[���̃e�L�X�`���[�ԍ�
static BOOL			g_IsSelectFinished; // ���P�b�g��I�񂾂�A���̃X�e�b�v�ɐi��

static BOOL			g_bStartOn = FALSE;
static BOOL			g_bStartFlg = FALSE;
static BOOL			g_bStartOffFlg = FALSE;

// �Z���N�g��ʐ�p�̃��P�b�g�\����
struct ROCKET_STATUS
{
	MODEL_LABEL model;
	ULONG64 price;
	int speed;
	int accelerate;
	int control;
	int fuel;
	LOCK_STATUS status;
};
static ROCKET_STATUS*	g_RS;	// �S���f���̊Ǘ��������P�b�g���
static ROCKET_LABEL		g_SR = ROCKET01;	// �I������Ă��郍�P�b�g
static ROCKET_LABEL		g_RSEquip = ROCKET01;  // �������̃��P�b�g�A�C�R���̃e�L�X�`���[�ԍ�

static float		g_AnimScl = 0.0f;				// �A�j���[�V�����Ǘ��p
static float		g_AnimRocketScl = 0.0f;				// �A�j���[�V�����Ǘ��p
static float		g_AnimRocketY = ROCKET_SLIDE_Y;				// �A�j���[�V�����Ǘ��p
static float		g_AnimAlpha = 0.0f;				// �A�j���[�V�����Ǘ��p
static float		g_AnimSlide = SLIDE_X;	// �A�j���[�V�����Ǘ��p
static float		g_AnimShopSlide = SHOP_SLIDE_Y;	// �A�j���[�V�����Ǘ��p
static float		g_AnimStatusSlide = STATUS_SLIDE_Y;	// �A�j���[�V�����Ǘ��p
static float		g_RocketOutline = FALSE;
static BOOL			g_bButton = FALSE;

// ���i�̎��
enum {
	TEXT_WHITE = 0,
	TEXT_BLACK,
	TEXT_PANNEL,
	TEXT_LOCK,

	TEXT_NUM,
};

//*****************************************************************************
// UI��`
//*****************************************************************************
// UI���i�`�揇�j
enum UI_LABEL {
	UI_SHOP_MENU = 0,

	UI_ROCKET_ICON_LOCK,
	UI_ROCKET_ICON_NEW,
	UI_ROCKET_ICON,
	UI_ROCKET_ICON_EQUIP,

	UI_ROCKET_NAME_1,	// ��
	UI_ROCKET_NAME2_1,	// ��
	UI_ROCKET_PANNEL_1,	// �p�l��
	UI_ROCKET_LOCK_1,	// ���b�N��

	UI_ROCKET_NAME_2,
	UI_ROCKET_NAME2_2,
	UI_ROCKET_PANNEL_2,
	UI_ROCKET_LOCK_2,

	UI_ROCKET_NAME_3,
	UI_ROCKET_NAME2_3,
	UI_ROCKET_PANNEL_3,
	UI_ROCKET_LOCK_3,

	UI_ROCKET_NAME_4,
	UI_ROCKET_NAME2_4,
	UI_ROCKET_PANNEL_4,
	UI_ROCKET_LOCK_4,

	UI_SHOP_DETAIL,

	UI_SHOP_S,
	UI_SHOP_H,
	UI_SHOP_O,
	UI_SHOP_P,

	UI_STATUS_LIST,
	UI_STATUSBAR,
	UI_STATUSBAR_POINT,

	UI_NUM,
};
#define ALPHA_START (UI_ROCKET_ICON_LOCK)
#define ALPHA_END (UI_SHOP_DETAIL)
// �Q�ƃe�N�X�`�����iUI�����j
#define REF_TL {\
	TEXTURE_LABEL_SHOP_MENU,\
\
	TEXTURE_LABEL_ROCKET_ICON_LOCK,\
	TEXTURE_LABEL_ROCKET_ICON_NEW,\
	TEXTURE_LABEL_ROCKET_ICON,\
	TEXTURE_LABEL_ROCKET_ICON_EQUIP,\
\
	TEXTURE_LABEL_ROCKET_1,\
	TEXTURE_LABEL_ROCKET_1,\
	TEXTURE_LABEL_ROCKETSELECT_1,\
	TEXTURE_LABEL_ROCKET_ICON,\
\
	TEXTURE_LABEL_ROCKET_2,\
	TEXTURE_LABEL_ROCKET_2,\
	TEXTURE_LABEL_ROCKETSELECT_2,\
	TEXTURE_LABEL_ROCKET_ICON,\
\
	TEXTURE_LABEL_ROCKET_3,\
	TEXTURE_LABEL_ROCKET_3,\
	TEXTURE_LABEL_ROCKETSELECT_3,\
	TEXTURE_LABEL_ROCKET_ICON,\
\
	TEXTURE_LABEL_ROCKET_4,\
	TEXTURE_LABEL_ROCKET_4,\
	TEXTURE_LABEL_ROCKETSELECT_4,\
	TEXTURE_LABEL_ROCKET_ICON,\
\
	TEXTURE_LABEL_SHOP_DETAIL,\
	TEXTURE_LABEL_SHOP_S,\
	TEXTURE_LABEL_SHOP_H,\
	TEXTURE_LABEL_SHOP_O,\
	TEXTURE_LABEL_SHOP_P,\
\
	TEXTURE_LABEL_STATUS_LIST,\
	TEXTURE_LABEL_STATUSBAR,\
	TEXTURE_LABEL_STATUSBAR_POINT,\
\
}
// UI�ڍ׊Ǘ�
static TEXTURE2D_DESC* g_td = NULL;

//*****************************************************************************
// �{�^����`
//*****************************************************************************
// �{�^�����i�D�揇�j
enum BT_LABEL {
	BT_ROCKET1 = 0,
	BT_ROCKET2,
	BT_ROCKET3,
	BT_ROCKET4,

	BT_NUM
};
// �Q��UI���i�{�^�������j
#define REF_UL {\
	UI_ROCKET_PANNEL_1,\
	UI_ROCKET_PANNEL_2,\
	UI_ROCKET_PANNEL_3,\
	UI_ROCKET_PANNEL_4,\
}
// �{�^���ڍ׊Ǘ�
static BUTTON_DESC* g_bd = NULL;
// �{�^���\
#define BT_NUM_X 1
#define BT_NUM_Y 4
static int g_btTbl[BT_NUM_Y][BT_NUM_X] = {
	{BT_ROCKET1},
	{BT_ROCKET2},
	{BT_ROCKET3},
	{BT_ROCKET4},
};
static BUTTON_TABLE* g_bt = NULL;
// �J�[�\���ʒu
static XMINT2 g_cursor = { 0, 0 };	// BT_ROCKET1����
// �{�^�����Ƃ̏���
static void ButtonPressed(int b)
{
	switch (b)
	{
	case BT_ROCKET1:
	case BT_ROCKET2:
	case BT_ROCKET3:
	case BT_ROCKET4:
		switch (g_RS[b].status)
		{
		case STATUS_LOCK:
			break;

		case STATUS_NEW:
		case STATUS_NORMAL:
			g_RS[g_RSEquip].status = STATUS_NORMAL;
			g_td[UI_ROCKET_LOCK_1 + TEXT_NUM * g_RSEquip].tex = g_td[UI_ROCKET_ICON_LOCK + g_RS[g_RSEquip].status].tex;
			g_td[UI_ROCKET_LOCK_1 + TEXT_NUM * g_RSEquip].size = g_td[UI_ROCKET_ICON_LOCK + g_RS[g_RSEquip].status].size;

			g_RSEquip = (ROCKET_LABEL)b;
			g_RS[g_RSEquip].status = STATUS_EQUIP;
			g_td[UI_ROCKET_LOCK_1 + TEXT_NUM * g_RSEquip].tex = g_td[UI_ROCKET_ICON_LOCK + g_RS[g_RSEquip].status].tex;
			g_td[UI_ROCKET_LOCK_1 + TEXT_NUM * g_RSEquip].size = g_td[UI_ROCKET_ICON_LOCK + g_RS[g_RSEquip].status].size;
			break;

		case STATUS_EQUIP:
			break;
		}
		break;

	default:
		SetHomeMode(HOME_HOME);
		if(g_RSEquip != g_cursor.y) g_AnimRocketScl = 0.0f;
		break;
	}
	g_bButton = FALSE;
}
// ������
static void InitUI(void)
{
	// �������m��
	g_td = new TEXTURE2D_DESC[UI_NUM];
	g_bd = new BUTTON_DESC[BT_NUM];

	// UI�ڍאݒ�
	TEXTURE_LABEL tl[UI_NUM] = REF_TL;
	for (int i = 0; i < UI_NUM; i++)
	{
		// �e�N�X�`���̑Ή��t��
		g_td[i].tex = tl[i];

		// �e�N�X�`���T�C�Y�̎擾
		g_td[i].size = GetTextureSize(g_td[i].tex);
	}

	g_td[UI_SHOP_MENU].ctrType = CENTER_RIGHTTOP;
	g_td[UI_SHOP_MENU].posType = POSITION_RIGHTTOP;

	g_td[UI_SHOP_P].ctrType = CENTER_RIGHTTOP;
	g_td[UI_SHOP_P].posType = POSITION_RIGHTTOP;
	g_td[UI_SHOP_P].pos = { -SHOP_X, SHOP_Y };
	g_td[UI_SHOP_P].sd_pos = { 10.0f, 10.0f };

	g_td[UI_SHOP_O].ctrType = CENTER_RIGHTTOP;
	g_td[UI_SHOP_O].posType = POSITION_RIGHTTOP;
	g_td[UI_SHOP_O].pos = { g_td[UI_SHOP_P].pos.x - g_td[UI_SHOP_P].size.x , g_td[UI_SHOP_P].pos.y };
	g_td[UI_SHOP_O].sd_pos = { 10.0f, 10.0f };

	g_td[UI_SHOP_H].ctrType = CENTER_RIGHTTOP;
	g_td[UI_SHOP_H].posType = POSITION_RIGHTTOP;
	g_td[UI_SHOP_H].pos = { g_td[UI_SHOP_O].pos.x - g_td[UI_SHOP_P].size.x , g_td[UI_SHOP_P].pos.y };
	g_td[UI_SHOP_H].sd_pos = { 10.0f, 10.0f };

	g_td[UI_SHOP_S].ctrType = CENTER_RIGHTTOP;
	g_td[UI_SHOP_S].posType = POSITION_RIGHTTOP;
	g_td[UI_SHOP_S].pos = { g_td[UI_SHOP_H].pos.x - g_td[UI_SHOP_P].size.x , g_td[UI_SHOP_P].pos.y };
	g_td[UI_SHOP_S].sd_pos = { 10.0f, 10.0f };


	g_td[UI_SHOP_DETAIL].ctrType = CENTER_RIGHTBOTTOM;
	g_td[UI_SHOP_DETAIL].posType = POSITION_RIGHTBOTTOM;
	g_td[UI_SHOP_DETAIL].sd_pos = { 2.5f, 2.5f };

	g_td[UI_STATUS_LIST].ctrType = CENTER_RIGHTTOP;
	g_td[UI_STATUS_LIST].posType = POSITION_LEFTTOP;
	g_td[UI_STATUS_LIST].sd_pos = { 2.0f, 2.0f };
	g_td[UI_STATUSBAR].ctrType = CENTER_LEFTTOP;
	g_td[UI_STATUSBAR].posType = POSITION_LEFTTOP;
	g_td[UI_STATUSBAR].sd_pos = { 2.0f, 2.0f };
	g_td[UI_STATUSBAR_POINT].ctrType = CENTER_LEFTTOP;
	g_td[UI_STATUSBAR_POINT].posType = POSITION_LEFTTOP;

	for (int i = UI_ROCKET_ICON_LOCK; i <= UI_ROCKET_ICON_EQUIP; i++)
	{
		g_td[i].ctrType = CENTER_RIGHT;
		g_td[i].posType = POSITION_TOP;
	}
	for (int i = UI_ROCKET_NAME_1; i <= UI_ROCKET_LOCK_4; i++)
	{
		g_td[i].ctrType = CENTER_LEFT;
		g_td[i].posType = POSITION_TOP;
	}
	
	g_td[UI_ROCKET_NAME2_1].col = { 0.0f, 0.2f, 0.0f, 1.0f };
	g_td[UI_ROCKET_NAME2_2].col = { 0.0f, 0.2f, 0.0f, 1.0f };
	g_td[UI_ROCKET_NAME2_3].col = { 0.0f, 0.2f, 0.0f, 1.0f };
	g_td[UI_ROCKET_NAME2_4].col = { 0.0f, 0.2f, 0.0f, 1.0f };

	g_td[UI_ROCKET_NAME2_1].sd_pos = { 3.5f, 3.5f };
	g_td[UI_ROCKET_NAME2_2].sd_pos = { 3.5f, 3.5f };
	g_td[UI_ROCKET_NAME2_3].sd_pos = { 3.5f, 3.5f };
	g_td[UI_ROCKET_NAME2_4].sd_pos = { 3.5f, 3.5f };

	g_td[UI_ROCKET_LOCK_1] = g_td[UI_ROCKET_ICON_EQUIP];
	g_td[UI_ROCKET_LOCK_2] = g_td[UI_ROCKET_ICON];
	g_td[UI_ROCKET_LOCK_3] = g_td[UI_ROCKET_ICON_NEW];
	g_td[UI_ROCKET_LOCK_4] = g_td[UI_ROCKET_ICON_LOCK];

	XMFLOAT2 pos = { 0.0f, 0.0f };
	for (int i = 0; i < BT_NUM_Y; i++) {
		pos.x = TEXT_START_X + TEXT_BETWN_X * (float)i;
		pos.y = TEXT_START_Y + TEXT_BETWN_Y * (float)i;
		g_td[i * TEXT_NUM + UI_ROCKET_NAME_1].pos = pos;
		g_td[i * TEXT_NUM + UI_ROCKET_NAME2_1].pos = pos;
		g_td[i * TEXT_NUM + UI_ROCKET_PANNEL_1].pos.y = pos.y;
		g_td[i * TEXT_NUM + UI_ROCKET_LOCK_1].pos = pos;
	}

	g_td[UI_ROCKET_LOCK_1].pos.x += ICON_BETWN_X;
	g_td[UI_ROCKET_LOCK_2].pos.x += ICON_BETWN_X;
	g_td[UI_ROCKET_LOCK_3].pos.x += ICON_BETWN_X;
	g_td[UI_ROCKET_LOCK_4].pos.x += ICON_BETWN_X;

	g_td[UI_ROCKET_PANNEL_1].ctrType = CENTER_RIGHT;
	g_td[UI_ROCKET_PANNEL_2].ctrType = CENTER_RIGHT;
	g_td[UI_ROCKET_PANNEL_3].ctrType = CENTER_RIGHT;
	g_td[UI_ROCKET_PANNEL_4].ctrType = CENTER_RIGHT;

	g_td[UI_ROCKET_PANNEL_1].posType = POSITION_RIGHTTOP;
	g_td[UI_ROCKET_PANNEL_2].posType = POSITION_RIGHTTOP;
	g_td[UI_ROCKET_PANNEL_3].posType = POSITION_RIGHTTOP;
	g_td[UI_ROCKET_PANNEL_4].posType = POSITION_RIGHTTOP;


	// �{�^���ڍאݒ�
	UI_LABEL ul[BT_NUM] = REF_UL;
	for (int i = 0; i < BT_NUM; i++)
	{
		g_bd[i].col_off = g_bd[i].col_on;
		g_bd[i].scl_off = g_bd[i].scl_on;
		SetUIButton(&g_bd[i], &g_td[ul[i]]);
	}

	// �{�^���e�[�u���ւ̓o�^
	g_bt = new BUTTON_TABLE;
	*g_bt = { &g_btTbl[0][0], BT_NUM_X, BT_NUM_Y, &g_bd[0], BT_NUM, &g_cursor };
}
// �X�V
static void UpdateUI(void)
{
	if(g_bButton) UpdateButton(g_bt, ButtonPressed);
	else g_bButton = TRUE;
}
// �`��
static void DrawUI(void)
{
	for (int i = 0; i < UI_NUM; i++)
	{
		DrawTexture2D(&g_td[i], TRUE);
	}
}
// �I��
static void UninitUI(void)
{
	delete[] g_td, g_bd;
	delete g_bt;
}

//*****************************************************************************
// ���[�J���֐�
//*****************************************************************************
// ���j���[�̎�ނƕ��i������e�N�X�`�������擾
static int GetTexNo(int menu_tex) {
	return UI_ROCKET_NAME_1 + TEXT_NUM * g_cursor.y + menu_tex;
}
// �p�l���̃A�j���[�V����
static void PannelAnim(void) {
	g_td[GetTexNo(TEXT_PANNEL)].scl.y = g_AnimScl;
	g_td[GetTexNo(TEXT_PANNEL)].uv_pos.v = 0.5f - g_AnimScl * 0.5f;
	g_td[GetTexNo(TEXT_PANNEL)].uv_pos.vh = g_AnimScl;

	g_td[GetTexNo(TEXT_BLACK)].scl.y = g_AnimScl;
	g_td[GetTexNo(TEXT_BLACK)].uv_pos.v = 0.5f - g_AnimScl * 0.5f;
	g_td[GetTexNo(TEXT_BLACK)].uv_pos.vh = g_AnimScl;
}

//=============================================================================
// ����������
//=============================================================================
HRESULT InitRocketSelect(void)
{
	g_AnimRocketScl = 0.0f;
	g_RocketSRT.scl.x = ROCKET_SCL * g_AnimRocketScl;
	g_RocketSRT.scl.y = ROCKET_SCL * g_AnimRocketScl;
	g_RocketSRT.scl.z = ROCKET_SCL * g_AnimRocketScl;

	if (g_Load) return S_OK;

	InitUI();

	// �X�e�[�W���f���̐ݒ�
	g_ModelStage.model = MODEL_STAGE;
	g_ModelStage.srt.rot = { 0.0f, 0.0f, 0.0f };
	g_ModelStage.srt.scl = { 9.0f, 9.0f, 9.0f };

	// �\�����f���̐ݒ�
	g_RocketSRT.rot = { XM_PIDIV2 * 0.3f, 0.0f, 0.0f };
	g_RocketSRT.scl = { ROCKET_SCL, ROCKET_SCL, ROCKET_SCL };

	g_RocketSRT.pos.y = ROCKET_DEF_Y;
	g_ModelStage.srt.pos.y = ROCKET_STAGE_Y;
	g_RocketSRT.pos.z = g_AnimRocketY;
	g_ModelStage.srt.pos.z = g_AnimRocketY;

	// ���P�b�g���̐ݒ�
	g_RS = new ROCKET_STATUS[ROCKET_NUM];

	g_RS[ROCKET01].model = MODEL_ROCKET1;
	g_RS[ROCKET01].speed = 3;
	g_RS[ROCKET01].accelerate = 4;
	g_RS[ROCKET01].control = 10;
	g_RS[ROCKET01].fuel = 5;
	g_RS[ROCKET01].price = 100000;
	g_RS[ROCKET01].status = STATUS_EQUIP;// SavaData�Őݒ肷��\��

	g_RS[ROCKET02].model = MODEL_ROCKET3;
	g_RS[ROCKET02].speed = 5;
	g_RS[ROCKET02].accelerate = 5;
	g_RS[ROCKET02].control = 9;
	g_RS[ROCKET02].fuel = 7;
	g_RS[ROCKET02].price = 50;
	g_RS[ROCKET02].status = STATUS_NORMAL;

	g_RS[ROCKET03].model = MODEL_ROCKET4;
	g_RS[ROCKET03].speed = 9;
	g_RS[ROCKET03].accelerate = 6;
	g_RS[ROCKET03].control = 7;
	g_RS[ROCKET03].fuel = 8;
	g_RS[ROCKET03].price = 99999;
	g_RS[ROCKET03].status = STATUS_NEW;

	g_RS[ROCKET04].model = MODEL_ROCKET5;
	g_RS[ROCKET04].speed = 10;
	g_RS[ROCKET04].accelerate = 10;
	g_RS[ROCKET04].control = 5;
	g_RS[ROCKET04].fuel = 10;
	g_RS[ROCKET04].price = 20221119;
	g_RS[ROCKET04].status = STATUS_LOCK;

	// �A�j���[�V���������ݒ�
	g_AnimSlide = SLIDE_X;
	g_AnimShopSlide = SHOP_SLIDE_Y;
	g_AnimScl = 0.0f;
	g_AnimAlpha = 0.0f;
	for (int i = ALPHA_START; i <= ALPHA_END; i++) {
		g_td[i].col.w = g_AnimAlpha;
	}
	for (int i = 0; i <= ALPHA_END; i++) {
		g_td[i].posAdd.x = g_AnimSlide;
	}
	g_td[UI_SHOP_S].posAdd.y = g_AnimShopSlide;
	g_td[UI_SHOP_H].posAdd.y = g_AnimShopSlide;
	g_td[UI_SHOP_O].posAdd.y = g_AnimShopSlide;
	g_td[UI_SHOP_P].posAdd.y = g_AnimShopSlide;
	PannelAnim();

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitRocketSelect(void)
{
	if (g_Load == FALSE) return;

	UninitUI();

	delete[] g_RS;

	g_Load = FALSE;
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateRocketSelect(void)
{
	//if (!g_bStartOn)
	//{
	//	if (g_bStartFlg)
	//	{
	//		g_bStartFlg = FALSE;
	//		g_bStartOn = TRUE;
	//	}
	//	return;
	//}

	if (g_AnimRocketScl < 1.0f)
	{
		g_AnimRocketScl += ANIM_SCALING;
		g_RocketSRT.scl.x = ROCKET_SCL * g_AnimRocketScl;
		g_RocketSRT.scl.y = ROCKET_SCL * g_AnimRocketScl;
		g_RocketSRT.scl.z = ROCKET_SCL * g_AnimRocketScl;
	}
	else
	{
		g_AnimRocketScl = 1.0f;
	}

	if (GetHomeMode() == HOME_SHOP)
	{
		if (g_AnimRocketY > 0.0f)
		{
			g_AnimRocketY = max(g_AnimRocketY - ROCKET_SLIDE_SPD, 0.0f);

			//g_RocketSRT.pos.y = g_AnimRocketY + ROCKET_DEF_Y;
			//g_ModelStage.srt.pos.y = ROCKET_STAGE_Y + g_AnimRocketY + ROCKET_DEF_Y;

			g_RocketSRT.pos.z = g_AnimRocketY;
			g_ModelStage.srt.pos.z = g_AnimRocketY;
		}
	}
	else
	{
		if (g_AnimRocketY < ROCKET_SLIDE_Y)
		{
			g_AnimRocketY = min(g_AnimRocketY + ROCKET_SLIDE_SPD, ROCKET_SLIDE_Y);

			//g_RocketSRT.pos.y = g_AnimRocketY + ROCKET_DEF_Y;
			//g_ModelStage.srt.pos.y = ROCKET_STAGE_Y + g_AnimRocketY + ROCKET_DEF_Y;

			g_RocketSRT.pos.z = g_AnimRocketY;
			g_ModelStage.srt.pos.z = g_AnimRocketY;
		}
	}

	g_RocketSRT.rot.y -= 0.02f;
	if (g_RocketSRT.rot.y < -XM_2PI) g_RocketSRT.rot.y += XM_2PI;

	if (g_AnimAlpha > 0.0f) UpdateUI();

	static int old_cur_y = g_cursor.y;
	if (old_cur_y != g_cursor.y) {
		g_AnimScl = 0.0f;
		PannelAnim();
		g_AnimRocketScl = 0.0f;
		old_cur_y = g_cursor.y;
	}

	if (GetHomeMode() == HOME_SHOP)
	{	// �o�Ă���Ƃ�
		if (g_AnimSlide > 0.0f) {
			g_AnimSlide = max(g_AnimSlide - ANIM_SLIDE, 0.0f);
			for (int i = 0; i <= ALPHA_END; i++) {
				g_td[i].posAdd.x = g_AnimSlide;
			}
		}
		else if(g_AnimShopSlide < 0.0f) {
			g_AnimShopSlide = min(g_AnimShopSlide + SHOP_SLIDE_SPD, 0.0f);
			g_td[UI_SHOP_S].posAdd.y = g_AnimShopSlide;
			g_td[UI_SHOP_H].posAdd.y = g_AnimShopSlide;
			g_td[UI_SHOP_O].posAdd.y = g_AnimShopSlide;
			g_td[UI_SHOP_P].posAdd.y = g_AnimShopSlide;
		}
		else if (g_AnimAlpha < 1.0f)
		{
			g_AnimAlpha += ANIM_ALPHA;
			for (int i = ALPHA_START; i <= ALPHA_END; i++) {
				g_td[i].col.w = g_AnimAlpha;
			}
		}
		else if (g_AnimStatusSlide < 0.0f) {
			g_AnimStatusSlide = min(g_AnimStatusSlide + ANIM_SLIDE, 0.0f);
		}
		else if (g_AnimScl < 1.0f)
		{
			g_AnimScl += ANIM_SCALING;
			PannelAnim();
		}
		g_bButton = TRUE;

		// SHOP�̃A�j���[�V����
		static int shop = 0;
		static float time = 0.0f;
		if (shop % 8 < 4) g_td[UI_SHOP_S + shop % 8].posAdd.y = g_AnimShopSlide - 20.0f * sinf(time);
		time += 0.15f; if (time >= XM_PI) { shop++; time = 0.0f; }
	}
	else
	{	// ���܂��Ƃ�
		if (g_AnimScl > 0.0f)
		{
			g_AnimScl -= ANIM_SCALING;
			PannelAnim();
		}
		else if (g_AnimStatusSlide > STATUS_SLIDE_Y) {
			g_AnimStatusSlide = max(g_AnimStatusSlide - ANIM_SLIDE, STATUS_SLIDE_Y);
		}
		else if (g_AnimAlpha > 0.0f)
		{
			g_AnimAlpha -= ANIM_ALPHA;
			for (int i = ALPHA_START; i <= ALPHA_END; i++) {
				g_td[i].col.w = g_AnimAlpha;
			}
		}
		else if (g_AnimShopSlide > SHOP_SLIDE_Y) {
			g_AnimShopSlide = max(g_AnimShopSlide - SHOP_SLIDE_SPD, SHOP_SLIDE_Y);
			g_td[UI_SHOP_S].posAdd.y = g_AnimShopSlide;
			g_td[UI_SHOP_H].posAdd.y = g_AnimShopSlide;
			g_td[UI_SHOP_O].posAdd.y = g_AnimShopSlide;
			g_td[UI_SHOP_P].posAdd.y = g_AnimShopSlide;
		}
		else if (g_AnimSlide < SLIDE_X) {
			g_AnimSlide = min(g_AnimSlide + ANIM_SLIDE, SLIDE_X);
			for (int i = 0; i <= ALPHA_END; i++) {
				g_td[i].posAdd.x = g_AnimSlide;
			}
		}
		return;
	}

	// �I�𒆂̃��P�b�g�̓��b�N���Ă���ꍇ�A�l�i��\������
	//if (g_RS[g_ModelNo_Rocket].status == STATUS_LOCK)
	//{
	//	SetPriceRocket(g_RS[g_ModelNo_Rocket].price);
	//}
	//else
	//{
	//	SetPriceRocket(0);
	//}
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawRocketSelect(void)
{
	DrawTextureStatus(g_cursor.y, g_AnimStatusSlide);

	DrawTexture2D(&g_td[UI_SHOP_MENU]);
	DrawTexture2D(&g_td[UI_SHOP_S], TRUE);
	DrawTexture2D(&g_td[UI_SHOP_H], TRUE);
	DrawTexture2D(&g_td[UI_SHOP_O], TRUE);
	DrawTexture2D(&g_td[UI_SHOP_P], TRUE);
	DrawTexture2D(&g_td[UI_SHOP_DETAIL], TRUE);

	DrawTexture2D(&g_td[UI_ROCKET_NAME_1]);
	DrawTexture2D(&g_td[UI_ROCKET_NAME_2]);
	DrawTexture2D(&g_td[UI_ROCKET_NAME_3]);
	DrawTexture2D(&g_td[UI_ROCKET_NAME_4]);

	if (g_AnimScl > 0.0f)
	{
		static float time = 0.0f;
		time += 0.05f; if (time > XM_2PI) time -= XM_2PI;
		if (g_AnimScl >= 1.0f) {
			g_td[GetTexNo(TEXT_BLACK)].scl.x = 1.0f + 0.025f * sinf(time);
			g_td[GetTexNo(TEXT_BLACK)].scl.y = 1.0f + 0.025f * sinf(time);
		}

		DrawTexture2D(&g_td[GetTexNo(TEXT_PANNEL)], FALSE, TRUE);	// ���j���[�p�l��
		DrawTexture2D(&g_td[GetTexNo(TEXT_BLACK)], TRUE, TRUE);		// ���j���[��
	}

	DrawTexture2D(&g_td[UI_ROCKET_LOCK_1], TRUE);
	DrawTexture2D(&g_td[UI_ROCKET_LOCK_2], TRUE);
	DrawTexture2D(&g_td[UI_ROCKET_LOCK_3], TRUE);
	DrawTexture2D(&g_td[UI_ROCKET_LOCK_4], TRUE);
}
#define ON_RATE (0.05f)
void DrawHomeRocket(void)
{
	SetViewPortHome();
	SetCullingMode(CULL_MODE_NONE);

	int rocket;
	if (GetHomeMode() == HOME_SHOP) {
		rocket = g_cursor.y;
	}
	else {
		rocket = g_RSEquip;
	}

	if (g_RocketOutline) {
		g_RocketSRT.scl.x += ON_RATE;
		g_RocketSRT.scl.y += ON_RATE;
		g_RocketSRT.scl.z += ON_RATE;
		SetDrawOutline(3.0f, { 1.0f, 1.0f, 0.0f, 1.0f }, TRUE);
		DrawModel(&g_RS[rocket].model, &g_RocketSRT);
		SetCullingMode(CULL_MODE_BACK);
	}

	// ���h��
	SetDrawFillBlackPlayer();
	DrawModel(&g_ModelStage.model, &g_ModelStage.srt);
	DrawModel(&g_RS[rocket].model, &g_RocketSRT);

	// �V���h�E�{�����[��
	SetStencilWritePL();
	DrawModel(&g_ModelStage.model, &g_ModelStage.srt);
	DrawModel(&g_RS[rocket].model, &g_RocketSRT);

	SetBlendState(BLEND_MODE_ADD);

	// �|�C���g���C�g
	SetLightNo(0);
	SetStencilReadPL();
	DrawModel(&g_ModelStage.model, &g_ModelStage.srt);
	DrawModel(&g_RS[rocket].model, &g_RocketSRT);

	// �A���r�G���g���C�g
	SetStencilNoneAL();
	DrawModel(&g_ModelStage.model, &g_ModelStage.srt);
	DrawModel(&g_RS[rocket].model, &g_RocketSRT);

	SetBlendState(BLEND_MODE_ALPHABLEND);

	SetCullingMode(CULL_MODE_BACK);
	ResetViewPort();

	if (g_RocketOutline) {
		g_RocketSRT.scl.x -= ON_RATE;
		g_RocketSRT.scl.y -= ON_RATE;
		g_RocketSRT.scl.z -= ON_RATE;
		g_RocketOutline = FALSE;
	}

}

BOOL IsRocketSelectFinished(void)
{
	return g_IsSelectFinished;
}

int GetRocketSelected(void)
{
	return g_cursor.y;
}

void SetRocketOutline(void)
{
	g_RocketOutline = TRUE;
}

void DrawTextureStatus(int rocket, float slide)
{
	if (rocket < 0) rocket = g_RSEquip;

	g_td[UI_STATUS_LIST].pos = POS_STATUSBAR;
	g_td[UI_STATUS_LIST].pos.x -= 25.0f;
	g_td[UI_STATUS_LIST].pos.y -= 10.0f;
	g_td[UI_STATUS_LIST].pos.y += slide;
	DrawTexture2D(&g_td[UI_STATUS_LIST], TRUE);

	// �X�s�[�h�̕`�揈��
	g_td[UI_STATUSBAR].pos = POS_STATUSBAR;
	g_td[UI_STATUSBAR].pos.y += slide;
	DrawTexture2D(&g_td[UI_STATUSBAR], TRUE);
	g_td[UI_STATUSBAR_POINT].pos = POS_STATUSBARPOINT;
	g_td[UI_STATUSBAR_POINT].pos.y += slide;
	g_td[UI_STATUSBAR_POINT].col = { 1.0f, 1.0f, 1.0f, 1.0f };
	if (g_RS[rocket].speed == STATUS_MAX) g_td[UI_STATUSBAR_POINT].col = STATUS_MAX_COLOR;
	for (int i = 0; i < g_RS[rocket].speed; i++)
	{
		DrawTexture2D(&g_td[UI_STATUSBAR_POINT]);
		g_td[UI_STATUSBAR_POINT].pos.x += DISTANCE_STATUSBARPOINT_X;
	}

	// �����̕`�揈��
	g_td[UI_STATUSBAR].pos.y += DISTANCE_STATUSBAR_Y;
	DrawTexture2D(&g_td[UI_STATUSBAR], TRUE);
	g_td[UI_STATUSBAR_POINT].pos = { 162.5f, g_td[UI_STATUSBAR_POINT].pos.y + DISTANCE_STATUSBAR_Y };
	g_td[UI_STATUSBAR_POINT].col = { 1.0f, 1.0f, 1.0f, 1.0f };
	if (g_RS[rocket].accelerate == STATUS_MAX) g_td[UI_STATUSBAR_POINT].col = STATUS_MAX_COLOR;
	for (int i = 0; i < g_RS[rocket].accelerate; i++)
	{
		DrawTexture2D(&g_td[UI_STATUSBAR_POINT]);
		g_td[UI_STATUSBAR_POINT].pos.x += DISTANCE_STATUSBARPOINT_X;
	}

	// ����̕`�揈��
	g_td[UI_STATUSBAR].pos.y += DISTANCE_STATUSBAR_Y;
	DrawTexture2D(&g_td[UI_STATUSBAR], TRUE);
	g_td[UI_STATUSBAR_POINT].pos = { 162.5f, g_td[UI_STATUSBAR_POINT].pos.y + DISTANCE_STATUSBAR_Y };
	g_td[UI_STATUSBAR_POINT].col = { 1.0f, 1.0f, 1.0f, 1.0f };
	if (g_RS[rocket].control == STATUS_MAX) g_td[UI_STATUSBAR_POINT].col = STATUS_MAX_COLOR;
	for (int i = 0; i < g_RS[rocket].control; i++)
	{
		DrawTexture2D(&g_td[UI_STATUSBAR_POINT]);
		g_td[UI_STATUSBAR_POINT].pos.x += DISTANCE_STATUSBARPOINT_X;
	}

	// �R���̕`�揈��
	g_td[UI_STATUSBAR].pos.y += DISTANCE_STATUSBAR_Y;
	DrawTexture2D(&g_td[UI_STATUSBAR], TRUE);
	g_td[UI_STATUSBAR_POINT].pos = { 162.5f, g_td[UI_STATUSBAR_POINT].pos.y + DISTANCE_STATUSBAR_Y };
	g_td[UI_STATUSBAR_POINT].col = { 1.0f, 1.0f, 1.0f, 1.0f };
	if (g_RS[rocket].fuel == STATUS_MAX) g_td[UI_STATUSBAR_POINT].col = STATUS_MAX_COLOR;
	for (int i = 0; i < g_RS[rocket].fuel; i++)
	{
		DrawTexture2D(&g_td[UI_STATUSBAR_POINT]);
		g_td[UI_STATUSBAR_POINT].pos.x += DISTANCE_STATUSBARPOINT_X;
	}
}
