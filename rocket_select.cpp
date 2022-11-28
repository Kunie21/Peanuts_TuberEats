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


//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH)	// �w�i�T�C�Y��
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)	// �w�i�T�C�Y�c

#define SIZE_SHOP_MENU		{ 1001.0f, 1080.0f }		// �L�����N�^�[�T�C�Y�c
#define POS_SHOP_MENU		{ 919.0f,0.0f }		// �L�����N�^�[�T�C�Y�c
#define POS_STATUSBAR		{ 210.0f, 70.0f }		// �L�����N�^�[�T�C�Y�c
#define POS_STATUSBARPOINT		{ 212.5f, 72.7f }		// �L�����N�^�[�T�C�Y�c
#define DISTANCE_STATUSBAR_Y			(76.0f)	// �L�����N�^�[�T�C�Y�c
#define DISTANCE_STATUSBARPOINT_X		(79.2f)	// �L�����N�^�[�T�C�Y�c

#define COL_BLACK		{0.0f,0.0f,0.0f,1.0f}	// �����F
#define COL_ORIGINAL	{1.0f,1.0f,1.0f,1.0f}	// ���̐F

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
void DrawTextureStatus(void);

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static BOOL				g_Load = FALSE;


// �e�N�X�`���Ǘ�
enum
{
	TEXTURE_BG = 0,
	TEXTURE_SHOP_MENU,
	TEXTURE_STATUSBAR,
	TEXTURE_STATUSBAR_POINT,
	TEXTURE_ROCKET_ICON_LOCK,
	TEXTURE_ROCKET_ICON_NEW,
	TEXTURE_ROCKET_ICON,
	TEXTURE_ROCKET_ICON_EQUIP,
	TEXTURE_ROCKET_1,
	TEXTURE_ROCKET_2,
	TEXTURE_ROCKET_3,
	TEXTURE_ROCKET_4,
	TEXTURE_ROCKETSELECT_1,
	TEXTURE_ROCKETSELECT_2,
	TEXTURE_ROCKETSELECT_3,
	TEXTURE_ROCKETSELECT_4,
	TEXTURE_DISPLAY_ICON_01, //�e�L�X�`���[���Ȃ��A
	TEXTURE_DISPLAY_ICON_02, //�P�Ƀ��P�b�g�̃A�C�R���̕\���Ŏg��
	TEXTURE_DISPLAY_ICON_03,
	TEXTURE_DISPLAY_ICON_04,
	TEXTURE_MAX,
};

static TEXTURE2D_DESC	g_td[TEXTURE_MAX];
//static ID3D11ShaderResourceView*	g_Texture[TEXTURE_DISPLAY_ICON_01] = { NULL };	// �e�N�X�`�����
//static char*	g_TextureName[TEXTURE_DISPLAY_ICON_01] = {
//	"data/TEXTURE/home_menu_gamen/customize_bg.png",
//	"data/TEXTURE/home_menu_gamen/shop_menu.png",
//	"data/TEXTURE/home_menu_gamen/statusbar_0.png",
//	"data/TEXTURE/home_menu_gamen/statusbar_point.png",
//	"data/TEXTURE/home_menu_gamen/rocket_icon_lock.png",
//	"data/TEXTURE/home_menu_gamen/rocket_icon_new.png",
//	"data/TEXTURE/home_menu_gamen/rocket_icon.png",
//	"data/TEXTURE/home_menu_gamen/rocket_icon_equip.png",
//	"data/TEXTURE/home_menu_gamen/rocket_1.png",
//	"data/TEXTURE/home_menu_gamen/rocket_2.png",
//	"data/TEXTURE/home_menu_gamen/rocket_3.png",
//	"data/TEXTURE/home_menu_gamen/rocket_4.png",
//	"data/TEXTURE/home_menu_gamen/rocket_select_1.png",
//	"data/TEXTURE/home_menu_gamen/rocket_select_2.png",
//	"data/TEXTURE/home_menu_gamen/rocket_select_3.png",
//	"data/TEXTURE/home_menu_gamen/rocket_select_4.png",
//};

enum {
	MODEL_SELECT_ROCKET_01,
	MODEL_SELECT_ROCKET_02,
	MODEL_SELECT_ROCKET_03,
	MODEL_SELECT_ROCKET_04,
	MODEL_SELECT_MAX,
};

static MODEL_DATA	g_ModelStage;	// �\�����f���̊Ǘ�
static MODEL_DATA	g_ModelDisplay;	// �\�����f���̊Ǘ�
static MODEL_DATA	g_ModelShader;	// �\�����f���̊Ǘ�
static int			g_RocketEquip;  // �������̃��P�b�g�A�C�R���̃e�L�X�`���[�ԍ�
static int			g_ModelNo_Rocket;  // �I�����Ă��郍�P�b�g�̃��f���ԍ�
static int			g_SelectBar;	   // �Z���N�g�o�[���̃e�L�X�`���[�ԍ�
static BOOL			g_IsSelectFinished; // ���P�b�g��I�񂾂�A���̃X�e�b�v�ɐi��

enum {
	STATUS_LOCK = 0,
	STATUS_NEW,
	STATUS_NORMAL,
	STATUS_EQUIP
};

// �Z���N�g��ʐ�p�̃��P�b�g�\����
struct SELECTROCKET
{
	//DX11_MODEL model;
	MODEL_LABEL model;
	int speed;
	int accelerate;
	int control;
	int fuel;
	ULONG64 price;
	int status;

};

static SELECTROCKET   g_Rocket[MODEL_MAX];	// �S���f���̊Ǘ��������P�b�g���

//=============================================================================
// ����������
//=============================================================================
HRESULT InitRocketSelect(void)
{
	// �e�N�X�`������
	for (int i = 0; i < TEXTURE_DISPLAY_ICON_01; i++)
	{
		//D3DX11CreateShaderResourceViewFromFile(GetDevice(), g_TextureName[i], NULL, NULL, &g_Texture[i], NULL);
		g_td[i].tex = (TEXTURE_LABEL)(TEXTURE_LABEL_BG + i);
	}

	// �e�N�X�`���ڍאݒ�
	for (int i = TEXTURE_SHOP_MENU; i < TEXTURE_MAX; i++)
	{
		g_td[i].ctrType = CENTER_LEFTTOP;
		g_td[i].posType = POSITION_ABSOLUTE;
	}

	g_td[TEXTURE_BG].col = { 1.0f, 1.0f, 1.0f,1.0f };
	g_td[TEXTURE_SHOP_MENU].size = SIZE_SHOP_MENU;
	g_td[TEXTURE_SHOP_MENU].pos = POS_SHOP_MENU;
	g_td[TEXTURE_STATUSBAR].size = { 795.0f, 33.0f };
	g_td[TEXTURE_STATUSBAR].pos = POS_STATUSBAR;
	g_td[TEXTURE_STATUSBAR_POINT].size = { 77.0f, 28.0f };
	g_td[TEXTURE_STATUSBAR_POINT].pos = POS_STATUSBARPOINT;
	g_td[TEXTURE_ROCKET_ICON_LOCK].size = { 72.0f, 90.0f };
	g_td[TEXTURE_ROCKET_ICON_NEW].size = { 78.0f, 90.0f };
	g_td[TEXTURE_ROCKET_ICON].size = { 72.0f, 87.0f };
	g_td[TEXTURE_ROCKET_ICON_EQUIP].size = { 72.0f, 92.0f };
	g_td[TEXTURE_ROCKET_1].size = { 563.0f, 67.0f };
	g_td[TEXTURE_ROCKET_1].pos = { 1300.0f, 215.0f };
	g_td[TEXTURE_ROCKET_2].size = { 509.0f, 56.0f };
	g_td[TEXTURE_ROCKET_2].pos = { 1280.0f, 323.0f };
	g_td[TEXTURE_ROCKET_3].size = { 461.0f, 67.0f };
	g_td[TEXTURE_ROCKET_3].pos = { 1260.0f, 415.0f };
	g_td[TEXTURE_ROCKET_4].size = { 528.0f, 67.0f };
	g_td[TEXTURE_ROCKET_4].pos = { 1240.0f, 515.0f };
	g_td[TEXTURE_ROCKETSELECT_1].size = SIZE_SHOP_MENU;
	g_td[TEXTURE_ROCKETSELECT_1].pos = POS_SHOP_MENU;
	g_td[TEXTURE_ROCKETSELECT_2].size = SIZE_SHOP_MENU;
	g_td[TEXTURE_ROCKETSELECT_2].pos = POS_SHOP_MENU;
	g_td[TEXTURE_ROCKETSELECT_3].size = SIZE_SHOP_MENU;
	g_td[TEXTURE_ROCKETSELECT_3].pos = POS_SHOP_MENU;
	g_td[TEXTURE_ROCKETSELECT_4].size = SIZE_SHOP_MENU;
	g_td[TEXTURE_ROCKETSELECT_4].pos = POS_SHOP_MENU;

	g_td[TEXTURE_DISPLAY_ICON_01].pos = { 1200.0f,200.0f };
	g_td[TEXTURE_DISPLAY_ICON_02].pos = { 1180.0f,300.0f };
	g_td[TEXTURE_DISPLAY_ICON_03].pos = { 1160.0f,400.0f };
	g_td[TEXTURE_DISPLAY_ICON_04].pos = { 1140.0f,500.0f };


	// ���f������
	//LoadModel("data/MODEL/stage.obj", &g_ModelStage.model);
	g_ModelStage.model = MODEL_STAGE;
	//LoadModel("data/MODEL/rocket01.obj", &g_Rocket[MODEL_ROCKET_01].model);
	//LoadModel("data/MODEL/rocket03.obj", &g_Rocket[MODEL_ROCKET_02].model);
	//LoadModel("data/MODEL/rocket04.obj", &g_Rocket[MODEL_ROCKET_03].model);
	//LoadModel("data/MODEL/rocket05.obj", &g_Rocket[MODEL_ROCKET_04].model);
	g_Rocket[MODEL_SELECT_ROCKET_01].model = MODEL_ROCKET1;
	g_Rocket[MODEL_SELECT_ROCKET_02].model = MODEL_ROCKET2;
	g_Rocket[MODEL_SELECT_ROCKET_03].model = MODEL_ROCKET3;
	g_Rocket[MODEL_SELECT_ROCKET_04].model = MODEL_ROCKET4;

	// �\�����f���̐ݒ�
	g_ModelDisplay.srt.pos = { -100.0f, -20.0f, 0.0f };
	g_ModelDisplay.srt.rot = { 0.45f, -1.6f, 0.03f };
	g_ModelDisplay.srt.scl = { 0.5f, 0.5f, 0.5f };

	// �X�e�[�W���f���̐ݒ�
	g_ModelStage.srt.pos = { -100.0f, -100.0f, 0.0f };
	g_ModelStage.srt.rot = { 0.0f, -1.5f, -0.22f };
	g_ModelStage.srt.scl = { 9.0f, 6.9f, 9.0f };

	// ���P�b�g���̐ݒ�
	g_Rocket[MODEL_SELECT_ROCKET_01].speed = 3;
	g_Rocket[MODEL_SELECT_ROCKET_01].accelerate = 4;
	g_Rocket[MODEL_SELECT_ROCKET_01].control = 10;
	g_Rocket[MODEL_SELECT_ROCKET_01].fuel = 5;
	g_Rocket[MODEL_SELECT_ROCKET_01].price = 100000;
	g_Rocket[MODEL_SELECT_ROCKET_01].status = STATUS_EQUIP;// SavaData�Őݒ肷��\��
		
	g_Rocket[MODEL_SELECT_ROCKET_02].speed = 5;
	g_Rocket[MODEL_SELECT_ROCKET_02].accelerate = 5;
	g_Rocket[MODEL_SELECT_ROCKET_02].control = 9;
	g_Rocket[MODEL_SELECT_ROCKET_02].fuel = 7;
	g_Rocket[MODEL_SELECT_ROCKET_02].price = 50;
	g_Rocket[MODEL_SELECT_ROCKET_02].status = STATUS_LOCK;
			
	g_Rocket[MODEL_SELECT_ROCKET_03].speed = 9;
	g_Rocket[MODEL_SELECT_ROCKET_03].accelerate = 6;
	g_Rocket[MODEL_SELECT_ROCKET_03].control = 7;
	g_Rocket[MODEL_SELECT_ROCKET_03].fuel = 8;
	g_Rocket[MODEL_SELECT_ROCKET_03].price = 99999;
	g_Rocket[MODEL_SELECT_ROCKET_03].status = STATUS_LOCK;
				
	g_Rocket[MODEL_SELECT_ROCKET_04].speed = 10;
	g_Rocket[MODEL_SELECT_ROCKET_04].accelerate = 10;
	g_Rocket[MODEL_SELECT_ROCKET_04].control = 5;
	g_Rocket[MODEL_SELECT_ROCKET_04].fuel = 10;
	g_Rocket[MODEL_SELECT_ROCKET_04].price = 20221119;
	g_Rocket[MODEL_SELECT_ROCKET_04].status = STATUS_LOCK;

	// ���P�b�g�A�C�R���̐ݒ�
	for (int i = 0; i < MODEL_MAX; i++)
	{
		switch(g_Rocket[i].status)
		{
		case STATUS_LOCK:
			g_td[i+ TEXTURE_DISPLAY_ICON_01].tex = g_td[TEXTURE_ROCKET_ICON_LOCK].tex;
			g_td[i+ TEXTURE_DISPLAY_ICON_01].size = g_td[TEXTURE_ROCKET_ICON_LOCK].size = { 72.0f, 90.0f };
			break;
		case STATUS_NEW:
			g_td[i + TEXTURE_DISPLAY_ICON_01].tex = g_td[TEXTURE_ROCKET_ICON_NEW].tex;
			g_td[i + TEXTURE_DISPLAY_ICON_01].size = g_td[TEXTURE_ROCKET_ICON_NEW].size = { 72.0f, 90.0f };
			break;
		case STATUS_NORMAL:
			g_td[i + TEXTURE_DISPLAY_ICON_01].tex = g_td[TEXTURE_ROCKET_ICON].tex;
			g_td[i + TEXTURE_DISPLAY_ICON_01].size = g_td[TEXTURE_ROCKET_ICON].size = { 72.0f, 90.0f };
			break;
		case STATUS_EQUIP:
			g_RocketEquip = i + TEXTURE_DISPLAY_ICON_01;
			g_td[g_RocketEquip].tex = g_td[TEXTURE_ROCKET_ICON_EQUIP].tex;
			g_td[g_RocketEquip].size = g_td[TEXTURE_ROCKET_ICON_EQUIP].size = { 72.0f, 90.0f };
			break;
		}
	}

	// �ŏ��̎��A�I�����郍�P�b�g�̐ݒ�
	g_ModelNo_Rocket = g_RocketEquip - TEXTURE_DISPLAY_ICON_01;
	g_SelectBar = g_ModelNo_Rocket + TEXTURE_ROCKETSELECT_1;
	g_td[g_SelectBar -4].col = COL_BLACK;

	g_IsSelectFinished = FALSE;

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitRocketSelect(void)
{
	if (g_Load == FALSE) return;

	//for (int i = 0; i < TEXTURE_DISPLAY_ICON_01; i++)
	//{
	//	if (g_Texture[i])
	//	{
	//		g_Texture[i]->Release();
	//		g_Texture[i] = NULL;
	//	}
	//}

	//for (int i = 0; i < MODEL_MAX; i++)
	//{
	//	UnloadModel(&g_Rocket[i].model);
	//}
	//UnloadModel(&g_ModelStage.model);
	//UnloadModel(&g_ModelDisplay.model);

	g_Load = FALSE;
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateRocketSelect(void)
{
	// ���P�b�g�I����ʂɖ߂�
	if (GetKeyboardTrigger(DIK_6))
	{
		g_IsSelectFinished = FALSE;
	}

	// ��������P�b�g�̍X�V����߂�
	if (InTransaction())
	{
		if (SuccessfulTransaction())
		{
			g_Rocket[g_ModelNo_Rocket].status = STATUS_NEW;
			g_td[g_ModelNo_Rocket + TEXTURE_DISPLAY_ICON_01].tex = g_td[TEXTURE_ROCKET_ICON_NEW].tex;
			g_td[g_ModelNo_Rocket + TEXTURE_DISPLAY_ICON_01].size = g_td[TEXTURE_ROCKET_ICON_NEW].size;
		}
		return;
	}

	g_ModelDisplay.srt.rot.y -= 0.02f;

	// ���P�b�g�̐ݒ芮��
	if (g_IsSelectFinished)return;


	if (GetKeyboardTrigger(DIK_W)) 
	{
		//g_ModelNo_Rocket = g_ModelNo_Rocket-- > MODEL_ROCKET_01 ?
		//	g_ModelNo_Rocket : MODEL_ROCKET_01;
		//g_SelectBar = TEXTURE_ROCKETSELECT_1 + g_ModelNo_Rocket;
		//g_td[g_SelectBar - 3].col = COL_ORIGINAL;// ���̔��F�̎��ɖ߂�
		//g_td[g_SelectBar - 4].col = COL_BLACK;// �o�[���Ɠ����ꏊ�̃��P�b�g�̖��O�̎�����������

		g_ModelNo_Rocket--;
		g_SelectBar = TEXTURE_ROCKETSELECT_1 + g_ModelNo_Rocket;
		g_td[g_SelectBar - 3].col = COL_ORIGINAL;// ���̔��F�̎��ɖ߂�
		g_td[g_SelectBar - 4].col = COL_BLACK;// �o�[���Ɠ����ꏊ�̃��P�b�g�̖��O�̎�����������
		if (g_ModelNo_Rocket < MODEL_SELECT_ROCKET_01)
		{
			g_ModelNo_Rocket = MODEL_SELECT_ROCKET_04;
			g_SelectBar = TEXTURE_ROCKETSELECT_4;
			g_td[TEXTURE_ROCKET_4].col = COL_BLACK;// �o�[���Ɠ����ꏊ�̃��P�b�g�̖��O�̎�����������
			g_td[TEXTURE_ROCKET_ICON_EQUIP].col = COL_ORIGINAL;// �o�[���Ɠ����ꏊ�̃��P�b�g�̖��O�̎�����������
		}	
	}
	else if (GetKeyboardTrigger(DIK_S)) 
	{
		//g_ModelNo_Rocket = g_ModelNo_Rocket++ < MODEL_ROCKET_04 ?
		//	g_ModelNo_Rocket : MODEL_ROCKET_04;
		//g_SelectBar = TEXTURE_ROCKETSELECT_1 + g_ModelNo_Rocket;
		//g_td[g_SelectBar - 5].col = COL_ORIGINAL;
		//g_td[g_SelectBar - 4].col = COL_BLACK;
		g_ModelNo_Rocket++;
		g_SelectBar = TEXTURE_ROCKETSELECT_1 + g_ModelNo_Rocket;
		g_td[g_SelectBar - 5].col = COL_ORIGINAL;
		g_td[g_SelectBar - 4].col = COL_BLACK;
		if (g_ModelNo_Rocket > MODEL_SELECT_ROCKET_04)
		{
			g_ModelNo_Rocket = MODEL_SELECT_ROCKET_01;
			g_SelectBar = TEXTURE_ROCKETSELECT_1;
			g_td[TEXTURE_ROCKET_1].col = COL_BLACK;// �o�[���Ɠ����ꏊ�̃��P�b�g�̖��O�̎�����������
			g_td[TEXTURE_ROCKETSELECT_1].col = COL_ORIGINAL;// �o�[���Ɠ����ꏊ�̃��P�b�g�̖��O�̎�����������
		}
	}
	else if (GetKeyboardTrigger(DIK_SPACE))
	{
		if (g_Rocket[g_ModelNo_Rocket].status != STATUS_LOCK)
		{
			g_Rocket[g_RocketEquip - TEXTURE_DISPLAY_ICON_01].status = STATUS_NORMAL;
			g_td[g_RocketEquip].tex = g_td[TEXTURE_ROCKET_ICON].tex;
			g_td[g_RocketEquip].size = g_td[TEXTURE_ROCKET_ICON].size;

			g_Rocket[g_ModelNo_Rocket].status = STATUS_EQUIP;
			g_RocketEquip = g_ModelNo_Rocket + TEXTURE_DISPLAY_ICON_01;
			g_td[g_RocketEquip].tex = g_td[TEXTURE_ROCKET_ICON_EQUIP].tex;
			g_td[g_RocketEquip].size = g_td[TEXTURE_ROCKET_ICON_EQUIP].size;
			g_IsSelectFinished = TRUE;
		}
		else
		{
			StartTransaction();
			return;
		}

	}

	// �I�𒆂̃��P�b�g�̓��b�N���Ă���ꍇ�A�l�i��\������
	if (g_Rocket[g_ModelNo_Rocket].status == STATUS_LOCK)
	{
		SetPriceRocket(g_Rocket[g_ModelNo_Rocket].price);
	}
	else
	{
		SetPriceRocket(0);
	}

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawRocketSelect(void)
{
	DrawTexture2D(&g_td[TEXTURE_BG]);
	DrawTexture2DAll();

	DrawTextureStatus();
	if (!g_IsSelectFinished)
	{
		DrawTexture2D(&g_td[TEXTURE_SHOP_MENU]);
		DrawTexture2D(&g_td[g_SelectBar]);
		DrawTexture2D(&g_td[TEXTURE_DISPLAY_ICON_01]);
		DrawTexture2D(&g_td[TEXTURE_DISPLAY_ICON_02]);
		DrawTexture2D(&g_td[TEXTURE_DISPLAY_ICON_03]);
		DrawTexture2D(&g_td[TEXTURE_DISPLAY_ICON_04]);
		DrawTexture2D(&g_td[TEXTURE_ROCKET_1]);
		DrawTexture2D(&g_td[TEXTURE_ROCKET_2]);
		DrawTexture2D(&g_td[TEXTURE_ROCKET_3]);
		DrawTexture2D(&g_td[TEXTURE_ROCKET_4]);
	}

	// ���f���`��
	SetDrawNoLighting();
	SetCullingMode(CULL_MODE_NONE);
	DrawModel(&g_ModelStage.model, &g_ModelStage.srt);
	DrawModel(&g_Rocket[g_ModelNo_Rocket].model, &g_ModelDisplay.srt);
	SetCullingMode(CULL_MODE_BACK);
}

BOOL IsRocketSelectFinished(void)
{
	return g_IsSelectFinished;
}

int GetRocketSelected(void)
{
	return g_ModelNo_Rocket;
}

void DrawTextureStatus(void)
{
	//return;
	// �X�s�[�h�̕`�揈��
	g_td[TEXTURE_STATUSBAR].pos = POS_STATUSBAR;
	DrawTexture2D(&g_td[TEXTURE_STATUSBAR]);
	g_td[TEXTURE_STATUSBAR_POINT].pos = POS_STATUSBARPOINT;
	for (int i = 0; i < g_Rocket[g_ModelNo_Rocket].speed; i++)
	{
		DrawTexture2D(&g_td[TEXTURE_STATUSBAR_POINT]);
		g_td[TEXTURE_STATUSBAR_POINT].pos.x += DISTANCE_STATUSBARPOINT_X;
	}

	// �����̕`�揈��
	g_td[TEXTURE_STATUSBAR].pos.y += DISTANCE_STATUSBAR_Y;
	DrawTexture2D(&g_td[TEXTURE_STATUSBAR]);
	g_td[TEXTURE_STATUSBAR_POINT].pos = { 212.5f, g_td[TEXTURE_STATUSBAR_POINT].pos.y + DISTANCE_STATUSBAR_Y };
	for (int i = 0; i < g_Rocket[g_ModelNo_Rocket].accelerate; i++)
	{
		DrawTexture2D(&g_td[TEXTURE_STATUSBAR_POINT]);
		g_td[TEXTURE_STATUSBAR_POINT].pos.x += DISTANCE_STATUSBARPOINT_X;
	}

	// ����̕`�揈��
	g_td[TEXTURE_STATUSBAR].pos.y += DISTANCE_STATUSBAR_Y;
	DrawTexture2D(&g_td[TEXTURE_STATUSBAR]);
	g_td[TEXTURE_STATUSBAR_POINT].pos = { 212.5f, g_td[TEXTURE_STATUSBAR_POINT].pos.y + DISTANCE_STATUSBAR_Y };
	for (int i = 0; i < g_Rocket[g_ModelNo_Rocket].control; i++)
	{
		DrawTexture2D(&g_td[TEXTURE_STATUSBAR_POINT]);
		g_td[TEXTURE_STATUSBAR_POINT].pos.x += DISTANCE_STATUSBARPOINT_X;
	}

	// �R���̕`�揈��
	g_td[TEXTURE_STATUSBAR].pos.y += DISTANCE_STATUSBAR_Y;
	DrawTexture2D(&g_td[TEXTURE_STATUSBAR]);
	g_td[TEXTURE_STATUSBAR_POINT].pos = { 212.5f, g_td[TEXTURE_STATUSBAR_POINT].pos.y + DISTANCE_STATUSBAR_Y };
	for (int i = 0; i < g_Rocket[g_ModelNo_Rocket].fuel; i++)
	{
		DrawTexture2D(&g_td[TEXTURE_STATUSBAR_POINT]);
		g_td[TEXTURE_STATUSBAR_POINT].pos.x += DISTANCE_STATUSBARPOINT_X;
	}
}
