////=============================================================================
////
//// �`�[�����S��ʏ��� [start.cpp]
//// Author : ���] �đ�
////
////=============================================================================
#include "main.h"
#include "renderer.h"
#include "texture2d.h"
#include "start.h"
#include "fade.h"
#include "input.h"
#include "model.h"
//
////*****************************************************************************
//// �}�N����`
////*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH)	// �w�i�T�C�Y��
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)	// �w�i�T�C�Y�c

#define TEXTURE_WIDTH_MENU			(1340)			// ���S�T�C�Y��
#define TEXTURE_HEIGHT_MENU			(1080)			// ���S�T�C�Y�c

#define TEXTURE_WIDTH_MENU_LINE		(601)			// ���S�T�C�Y��
#define TEXTURE_HEIGHT_MENU_LINE	(1080)			// ���S�T�C�Y�c

#define TEXTURE_WIDTH_START			(282)			// ���S�T�C�Y��
#define TEXTURE_WIDTH_START_JP		(217)			// ���S�T�C�Y��

#define TEXTURE_HEIGHT_MENUALL		(111)			// ���S�T�C�Y�c
#define TEXTURE_HEIGHT_MENUALL_JP	( 61)			// ���S�T�C�Y�c

#define TEXTURE_WIDTH_OPTION		(368)			// ���S�T�C�Y��
#define TEXTURE_WIDTH_OPTION_JP		(293)			// ���S�T�C�Y��

#define TEXTURE_WIDTH_GALLERY		(416)			// ���S�T�C�Y��
#define TEXTURE_WIDTH_GALLERY_JP	(399)			// ���S�T�C�Y��

#define TEXTURE_WIDTH_CREDIT		(335)			// ���S�T�C�Y��
#define TEXTURE_WIDTH_CREDIT_JP		(335)			// ���S�T�C�Y��

#define TEXTURE_WIDTH_QUIT			(227)			// ���S�T�C�Y��
#define TEXTURE_WIDTH_QUIT_JP		(121)			// ���S�T�C�Y��

#define TEXTURE_WIDTH_MENUPANNEL	(1273)			// ���S�T�C�Y��
#define TEXTURE_HEIGHT_MENUPANNEL	(1080)			// ���S�T�C�Y��

#define MENU_Y						(150.0f)

#define TEAMLOGO_TIME				(100)			// ���S�\������
//
////*****************************************************************************
//// �O���[�o���ϐ�
////*****************************************************************************
static int				g_Time = 0;
static BOOL				g_Load = FALSE;
static BOOL				IsMenuMove = FALSE;
static int MenuPannelNum;
static int MenuNum;
static int MenuJp;


//// �e�N�X�`���Ǘ�
enum
{
	TEXTURE_BG = 0,
	TEXTURE_MENUBOARD,
	TEXTURE_MENU_LINE,
	TEXTURE_STAR,
	TEXTURE_START,
	TEXTURE_OPTION,
	TEXTURE_GALLERY,
	TEXTURE_CREDIT,
	TEXTURE_QUIT,
	TEXTURE_START_01,
	TEXTURE_OPTION_01,
	TEXTURE_GALLERY_01,
	TEXTURE_CREDIT_01,
	TEXTURE_QUIT_01,
	TEXTURE_START_JP,
	TEXTURE_OPTION_JP,
	TEXTURE_GALLERY_JP,
	TEXTURE_CREDIT_JP,
	TEXTURE_QUIT_JP,
	TEXTURE_MENU_PANNEL_01,
	TEXTURE_MENU_PANNEL_02,
	TEXTURE_MENU_PANNEL_03,
	TEXTURE_MENU_PANNEL_04,
	TEXTURE_MENU_PANNEL_05,
	TEXTURE_MENU_DISPLAY,
	TEXTURE_MENU_JP,
	TEXTURE_MAX,

};
static TEXTURE2D_DESC	g_td[TEXTURE_MAX];
static ID3D11ShaderResourceView*	g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����
static char*	g_TextureName[TEXTURE_MAX] = {
	"data/TEXTURE/white.png",
	"data/TEXTURE/title_menu_gamen/menu_board.png",
	"data/TEXTURE/title_menu_gamen/menu_line.png",
	"data/MODEL/star1.jpg",
	"data/TEXTURE/title_menu_gamen/start_1.png",
	"data/TEXTURE/title_menu_gamen/option_1.png",
	"data/TEXTURE/title_menu_gamen/gallery_1.png",
	"data/TEXTURE/title_menu_gamen/credit_1.png",
	"data/TEXTURE/title_menu_gamen/quit_1.png",
	"data/TEXTURE/title_menu_gamen/start_2.png",
	"data/TEXTURE/title_menu_gamen/option_2.png",
	"data/TEXTURE/title_menu_gamen/gallery_2.png",
	"data/TEXTURE/title_menu_gamen/credit_2.png",
	"data/TEXTURE/title_menu_gamen/quit_2.png",
	"data/TEXTURE/title_menu_gamen/start_setsumei.png",
	"data/TEXTURE/title_menu_gamen/option_setsumei.png",
	"data/TEXTURE/title_menu_gamen/gallery_setsumei.png",
	"data/TEXTURE/title_menu_gamen/credit_setsumei.png",
	"data/TEXTURE/title_menu_gamen/quit_setsumei.png",
	"data/TEXTURE/title_menu_gamen/menupannel_1.png",
	"data/TEXTURE/title_menu_gamen/menupannel_2.png",
	"data/TEXTURE/title_menu_gamen/menupannel_3.png",
	"data/TEXTURE/title_menu_gamen/menupannel_4.png",
	"data/TEXTURE/title_menu_gamen/menupannel_5.png",
};

enum {
	MODEL_STAR = 0,
	MODEL_EARTH,
	MODEL_ROCKET,
	MODEL_MAX,
};

static MODEL_DATA	g_Model[MODEL_MAX];	// �v���C���[�̃��f���Ǘ�
////=============================================================================
//// ����������
////=============================================================================
HRESULT InitStart(void)
{
	MenuPannelNum = TEXTURE_MENU_PANNEL_01;
	MenuNum = TEXTURE_START_01;
	MenuJp = TEXTURE_START_JP;

	// �e�N�X�`������
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		D3DX11CreateShaderResourceViewFromFile(GetDevice(), g_TextureName[i], NULL, NULL, &g_Texture[i], NULL);
		g_td[i].tex = &g_Texture[i];
	}


	LoadModel("data/MODEL/earth01.obj", &g_Model[MODEL_STAR].model);
	LoadModel("data/MODEL/earth01.obj", &g_Model[MODEL_EARTH].model);
	LoadModel("data/MODEL/titleRocket01.obj", &g_Model[MODEL_ROCKET].model);

	g_td[TEXTURE_MENUBOARD].size = { TEXTURE_WIDTH_MENU, TEXTURE_HEIGHT_MENU };
	g_td[TEXTURE_MENUBOARD].scl = { 1.0f, 1.0f };
	g_td[TEXTURE_MENUBOARD].pos = { -TEXTURE_WIDTH_MENU, 0.0f };
	g_td[TEXTURE_MENUBOARD].col = { 1.0f, 1.0f, 1.0f, 1.0f };
	g_td[TEXTURE_MENUBOARD].ctrType = CENTER_LEFTTOP;
	g_td[TEXTURE_MENUBOARD].posType = POSITION_ABSOLUTE;

	g_td[TEXTURE_MENU_LINE].size = { TEXTURE_WIDTH_MENU_LINE, TEXTURE_HEIGHT_MENU_LINE };
	g_td[TEXTURE_MENU_LINE].scl = { 1.0f, 1.0f };
	g_td[TEXTURE_MENU_LINE].pos = { 680.0f, 0.0f };
	g_td[TEXTURE_MENU_LINE].col = { 1.0f, 1.0f, 1.0f, 0.0f };
	g_td[TEXTURE_MENU_LINE].ctrType = CENTER_LEFTTOP;
	g_td[TEXTURE_MENU_LINE].posType = POSITION_ABSOLUTE;

	g_td[TEXTURE_START].size = { TEXTURE_WIDTH_START, TEXTURE_HEIGHT_MENUALL };
	g_td[TEXTURE_START].scl = { 1.0f, 1.0f };
	g_td[TEXTURE_START].pos = { 200.0f, 160.0f };
	g_td[TEXTURE_START].col = { 1.0f, 1.0f, 1.0f, 0.0f };
	g_td[TEXTURE_START].ctrType = CENTER_LEFTTOP;
	g_td[TEXTURE_START].posType = POSITION_ABSOLUTE;

	g_td[TEXTURE_START_01].size = { TEXTURE_WIDTH_START, TEXTURE_HEIGHT_MENUALL };
	g_td[TEXTURE_START_01].scl = { 1.0f, 1.0f };
	g_td[TEXTURE_START_01].pos = { 200.0f, 160.0f };
	g_td[TEXTURE_START_01].col = { 1.0f, 1.0f, 1.0f, 1.0f };
	g_td[TEXTURE_START_01].ctrType = CENTER_LEFTTOP;
	g_td[TEXTURE_START_01].posType = POSITION_ABSOLUTE;


	g_td[TEXTURE_OPTION].size = { TEXTURE_WIDTH_OPTION, TEXTURE_HEIGHT_MENUALL };
	g_td[TEXTURE_OPTION].scl = { 1.0f, 1.0f };
	g_td[TEXTURE_OPTION].pos = { 200.0f, 310.0f };
	g_td[TEXTURE_OPTION].col = { 1.0f, 1.0f, 1.0f, 0.0f };
	g_td[TEXTURE_OPTION].ctrType = CENTER_LEFTTOP;
	g_td[TEXTURE_OPTION].posType = POSITION_ABSOLUTE;



	g_td[TEXTURE_GALLERY].size = { TEXTURE_WIDTH_GALLERY, TEXTURE_HEIGHT_MENUALL };
	g_td[TEXTURE_GALLERY].scl = { 1.0f, 1.0f };
	g_td[TEXTURE_GALLERY].pos = { 200.0f, 460.0f };
	g_td[TEXTURE_GALLERY].col = { 1.0f, 1.0f, 1.0f, 0.0f };
	g_td[TEXTURE_GALLERY].ctrType = CENTER_LEFTTOP;
	g_td[TEXTURE_GALLERY].posType = POSITION_ABSOLUTE;

	g_td[TEXTURE_CREDIT].size = { TEXTURE_WIDTH_CREDIT, TEXTURE_HEIGHT_MENUALL };
	g_td[TEXTURE_CREDIT].scl = { 1.0f, 1.0f };
	g_td[TEXTURE_CREDIT].pos = { 200.0f, 610.0f };
	g_td[TEXTURE_CREDIT].col = { 1.0f, 1.0f, 1.0f, 0.0f };
	g_td[TEXTURE_CREDIT].ctrType = CENTER_LEFTTOP;
	g_td[TEXTURE_CREDIT].posType = POSITION_ABSOLUTE;

	g_td[TEXTURE_QUIT].size = { TEXTURE_WIDTH_QUIT, TEXTURE_HEIGHT_MENUALL };
	g_td[TEXTURE_QUIT].scl = { 1.0f, 1.0f };
	g_td[TEXTURE_QUIT].pos = { 200.0f, 760.0f };
	g_td[TEXTURE_QUIT].col = { 1.0f, 1.0f, 1.0f, 0.0f };
	g_td[TEXTURE_QUIT].ctrType = CENTER_LEFTTOP;
	g_td[TEXTURE_QUIT].posType = POSITION_ABSOLUTE;

	g_td[TEXTURE_OPTION_01].size = { TEXTURE_WIDTH_OPTION, TEXTURE_HEIGHT_MENUALL };
	g_td[TEXTURE_OPTION_01].scl = { 1.0f, 1.0f };
	g_td[TEXTURE_OPTION_01].pos = { 200.0f, 310.0f };

	g_td[TEXTURE_GALLERY_01].size = { TEXTURE_WIDTH_GALLERY, TEXTURE_HEIGHT_MENUALL };
	g_td[TEXTURE_GALLERY_01].scl = { 1.0f, 1.0f };
	g_td[TEXTURE_GALLERY_01].pos = { 200.0f, 460.0f };

	g_td[TEXTURE_CREDIT_01].size = { TEXTURE_WIDTH_CREDIT, TEXTURE_HEIGHT_MENUALL };
	g_td[TEXTURE_CREDIT_01].scl = { 1.0f, 1.0f };
	g_td[TEXTURE_CREDIT_01].pos = { 200.0f, 610.0f };

	g_td[TEXTURE_QUIT_01].size = { TEXTURE_WIDTH_QUIT, TEXTURE_HEIGHT_MENUALL };
	g_td[TEXTURE_QUIT_01].scl = { 1.0f, 1.0f };
	g_td[TEXTURE_QUIT_01].pos = { 200.0f, 760.0f };

	g_td[TEXTURE_START_JP].size = { TEXTURE_WIDTH_START_JP, TEXTURE_HEIGHT_MENUALL_JP };
	g_td[TEXTURE_START_JP].scl = { 1.0f, 1.0f };
	g_td[TEXTURE_START_JP].pos = { 200.0f, 1000.0f };
	g_td[TEXTURE_START_JP].col = { 1.0f, 1.0f, 1.0f, 1.0f };
	g_td[TEXTURE_START_JP].ctrType = CENTER_LEFTTOP;
	g_td[TEXTURE_START_JP].posType = POSITION_ABSOLUTE;

	g_td[TEXTURE_OPTION_JP].size = { TEXTURE_WIDTH_OPTION_JP, TEXTURE_HEIGHT_MENUALL_JP };
	g_td[TEXTURE_OPTION_JP].scl = { 1.0f, 1.0f };
	g_td[TEXTURE_OPTION_JP].pos = { 200.0f, 1000.0f };


	g_td[TEXTURE_GALLERY_JP].size = { TEXTURE_WIDTH_GALLERY_JP, TEXTURE_HEIGHT_MENUALL_JP };
	g_td[TEXTURE_GALLERY_JP].scl = { 1.0f, 1.0f };
	g_td[TEXTURE_GALLERY_JP].pos = { 200.0f, 1000.0f };


	g_td[TEXTURE_CREDIT_JP].size = { TEXTURE_WIDTH_CREDIT_JP, TEXTURE_HEIGHT_MENUALL_JP };
	g_td[TEXTURE_CREDIT_JP].scl = { 1.0f, 1.0f };
	g_td[TEXTURE_CREDIT_JP].pos = { 200.0f, 1000.0f };


	g_td[TEXTURE_QUIT_JP].size = { TEXTURE_WIDTH_QUIT_JP, TEXTURE_HEIGHT_MENUALL_JP };
	g_td[TEXTURE_QUIT_JP].scl = { 1.0f, 1.0f };
	g_td[TEXTURE_QUIT_JP].pos = { 200.0f, 1000.0f };



	g_td[TEXTURE_MENU_PANNEL_01].size = { TEXTURE_WIDTH_MENUPANNEL, TEXTURE_HEIGHT_MENUPANNEL };
	g_td[TEXTURE_MENU_PANNEL_01].scl = { 1.0f, 1.0f };
	g_td[TEXTURE_MENU_PANNEL_01].pos = { 0.0f, 0.0f };
	g_td[TEXTURE_MENU_PANNEL_01].tex = &g_Texture[19];
	g_td[TEXTURE_MENU_PANNEL_01].col = { 1.0f, 1.0f, 1.0f, 0.0f };
	g_td[TEXTURE_MENU_PANNEL_01].ctrType = CENTER_LEFTTOP;
	g_td[TEXTURE_MENU_PANNEL_01].posType = POSITION_ABSOLUTE;

	g_td[TEXTURE_MENU_PANNEL_02].size = { TEXTURE_WIDTH_MENUPANNEL, TEXTURE_HEIGHT_MENUPANNEL };
	g_td[TEXTURE_MENU_PANNEL_02].scl = { 1.0f, 1.0f };
	g_td[TEXTURE_MENU_PANNEL_02].pos = { 0.0f, 0.0f };
	g_td[TEXTURE_MENU_PANNEL_02].col = { 1.0f, 1.0f, 1.0f, 1.0f };
	g_td[TEXTURE_MENU_PANNEL_02].ctrType = CENTER_LEFTTOP;
	g_td[TEXTURE_MENU_PANNEL_02].posType = POSITION_ABSOLUTE;

	g_td[TEXTURE_MENU_PANNEL_03].size = { TEXTURE_WIDTH_MENUPANNEL, TEXTURE_HEIGHT_MENUPANNEL };
	g_td[TEXTURE_MENU_PANNEL_03].scl = { 1.0f, 1.0f };
	g_td[TEXTURE_MENU_PANNEL_03].pos = { 0.0f, 0.0f };
	g_td[TEXTURE_MENU_PANNEL_03].col = { 1.0f, 1.0f, 1.0f, 1.0f };
	g_td[TEXTURE_MENU_PANNEL_03].ctrType = CENTER_LEFTTOP;
	g_td[TEXTURE_MENU_PANNEL_03].posType = POSITION_ABSOLUTE;

	g_td[TEXTURE_MENU_PANNEL_04].size = { TEXTURE_WIDTH_MENUPANNEL, TEXTURE_HEIGHT_MENUPANNEL };
	g_td[TEXTURE_MENU_PANNEL_04].scl = { 1.0f, 1.0f };
	g_td[TEXTURE_MENU_PANNEL_04].pos = { 0.0f, 0.0f };
	g_td[TEXTURE_MENU_PANNEL_04].col = { 1.0f, 1.0f, 1.0f, 1.0f };
	g_td[TEXTURE_MENU_PANNEL_04].ctrType = CENTER_LEFTTOP;
	g_td[TEXTURE_MENU_PANNEL_04].posType = POSITION_ABSOLUTE;

	g_td[TEXTURE_MENU_PANNEL_05].size = { TEXTURE_WIDTH_MENUPANNEL, TEXTURE_HEIGHT_MENUPANNEL };
	g_td[TEXTURE_MENU_PANNEL_05].scl = { 1.0f, 1.0f };
	g_td[TEXTURE_MENU_PANNEL_05].pos = { 0.0f, 0.0f };
	g_td[TEXTURE_MENU_PANNEL_05].col = { 1.0f, 1.0f, 1.0f, 1.0f };
	g_td[TEXTURE_MENU_PANNEL_05].ctrType = CENTER_LEFTTOP;
	g_td[TEXTURE_MENU_PANNEL_05].posType = POSITION_ABSOLUTE;


	g_td[TEXTURE_MENU_DISPLAY].tex = g_td[MenuNum].tex;
	g_td[TEXTURE_MENU_DISPLAY].size = { TEXTURE_WIDTH_START, TEXTURE_HEIGHT_MENUALL };
	g_td[TEXTURE_MENU_DISPLAY].scl = { 1.0f, 1.0f };
	g_td[TEXTURE_MENU_DISPLAY].pos = { 200.0f, 150.0f };
	g_td[TEXTURE_MENU_DISPLAY].col = { 1.0f, 1.0f, 1.0f, 0.0f };
	g_td[TEXTURE_MENU_DISPLAY].ctrType = CENTER_LEFTTOP;
	g_td[TEXTURE_MENU_DISPLAY].posType = POSITION_ABSOLUTE;


	g_td[TEXTURE_MENU_JP].tex = g_td[MenuJp].tex;
	g_td[TEXTURE_MENU_JP].size = { TEXTURE_WIDTH_START_JP, TEXTURE_HEIGHT_MENUALL_JP };
	g_td[TEXTURE_MENU_JP].scl = { 1.0f, 1.0f };
	g_td[TEXTURE_MENU_JP].pos = { 200.0f, 1000.0f };
	g_td[TEXTURE_MENU_JP].col = { 1.0f, 1.0f, 1.0f, 1.0f };
	g_td[TEXTURE_MENU_JP].ctrType = CENTER_LEFTTOP;
	g_td[TEXTURE_MENU_JP].posType = POSITION_ABSOLUTE;


	g_Model[MODEL_STAR].srt.pos = { 0.0f, 0.0f, 0.0f };
	g_Model[MODEL_EARTH].srt.pos = { 0.0f, 0.0f, 100.0f };
	g_Model[MODEL_STAR].srt.scl = { 100.0f, 100.0f, 100.0f };
	g_Model[MODEL_EARTH].srt.scl = { 20.3f, 20.3f, 20.3f };
	g_Model[MODEL_ROCKET].srt.pos = { 0.0f, 0.0f, 0.0f };
	g_Model[MODEL_ROCKET].srt.scl = { 1.1f, 1.1f, 1.1f };


	g_Time = 0;

	g_Load = TRUE;
	return S_OK;
}

////=============================================================================
//// �I������
////=============================================================================
void UninitStart(void)
{
	if (g_Load == FALSE) return;

	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		if (g_Texture[i])
		{
			g_Texture[i]->Release();
			g_Texture[i] = NULL;
		}
	}

	g_Load = FALSE;
}
//
////=============================================================================
//// �X�V����
////=============================================================================
void UpdateStart(void)
{
	static int Timer = TEXTURE_MENU_LINE;
	static float EndTime = 0.0f;

	if (!IsMenuMove)
	{
		g_td[TEXTURE_MENUBOARD].pos.x += 40;

		if (g_td[TEXTURE_MENUBOARD].pos.x >= 0)
		{
			EndTime += 0.2f;

			g_td[TEXTURE_MENUBOARD].pos.x = 0.0f;
			g_td[TEXTURE_MENU_LINE].col.w = 1.0f;


			if (EndTime >= 1.0f)
			{
				if (g_td[Timer].col.w == 1.0f)
				{
					Timer++;
				}
				else
				{
					g_td[Timer].col.w = 1.0f;
					if (Timer == TEXTURE_QUIT)
					{
						g_td[TEXTURE_MENU_PANNEL_01].col.w = 1.0f;
						g_td[TEXTURE_MENU_DISPLAY].col.w = 1.0f;

						IsMenuMove = TRUE;
					}
				}

				EndTime = 0.0f;

			}
		}
	}

	if (GetKeyboardTrigger(DIK_UPARROW))
	{
		if (MenuPannelNum > TEXTURE_MENU_PANNEL_01)
		{
			g_td[TEXTURE_MENU_PANNEL_01].tex = &g_Texture[--MenuPannelNum];
			g_td[TEXTURE_MENU_DISPLAY].tex = &g_Texture[--MenuNum];
			g_td[TEXTURE_MENU_DISPLAY].size = g_td[MenuNum].size;
			g_td[TEXTURE_MENU_DISPLAY].pos = g_td[MenuNum].pos;
			g_td[TEXTURE_MENU_JP].tex = &g_Texture[--MenuJp];
			g_td[TEXTURE_MENU_JP].size = g_td[MenuJp].size;
			g_td[TEXTURE_MENU_JP].pos = g_td[MenuJp].pos;


		}
		else
		{
			MenuPannelNum = TEXTURE_MENU_PANNEL_05;
			MenuNum = TEXTURE_QUIT_01;
			MenuJp = TEXTURE_QUIT_JP;
			g_td[TEXTURE_MENU_PANNEL_01].tex = &g_Texture[MenuPannelNum];
			g_td[TEXTURE_MENU_DISPLAY].tex = &g_Texture[MenuNum];
			g_td[TEXTURE_MENU_DISPLAY].size = g_td[MenuNum].size;
			g_td[TEXTURE_MENU_DISPLAY].pos = g_td[MenuNum].pos;
			g_td[TEXTURE_MENU_JP].tex = &g_Texture[MenuJp];
			g_td[TEXTURE_MENU_JP].size = g_td[MenuJp].size;
			g_td[TEXTURE_MENU_JP].pos = g_td[MenuJp].pos;


		}


	}
	else if (GetKeyboardTrigger(DIK_DOWNARROW))
	{
		if (MenuPannelNum < TEXTURE_MENU_PANNEL_05)
		{
			g_td[TEXTURE_MENU_PANNEL_01].tex = &g_Texture[++MenuPannelNum];
			g_td[TEXTURE_MENU_DISPLAY].tex = &g_Texture[++MenuNum];
			g_td[TEXTURE_MENU_DISPLAY].size = g_td[MenuNum].size;
			g_td[TEXTURE_MENU_DISPLAY].pos = g_td[MenuNum].pos;
			g_td[TEXTURE_MENU_JP].tex = &g_Texture[++MenuJp];
			g_td[TEXTURE_MENU_JP].size = g_td[MenuJp].size;
			g_td[TEXTURE_MENU_JP].pos = g_td[MenuJp].pos;

		}
		else
		{
			MenuPannelNum = TEXTURE_MENU_PANNEL_01;
			MenuNum = TEXTURE_START_01;
			MenuJp = TEXTURE_START_JP;
			g_td[TEXTURE_MENU_PANNEL_01].tex = &g_Texture[MenuPannelNum];
			g_td[TEXTURE_MENU_DISPLAY].tex = &g_Texture[MenuNum];
			g_td[TEXTURE_MENU_DISPLAY].size = g_td[MenuNum].size;
			g_td[TEXTURE_MENU_DISPLAY].pos = g_td[MenuNum].pos;
			g_td[TEXTURE_MENU_JP].tex = &g_Texture[MenuJp];
			g_td[TEXTURE_MENU_JP].size = g_td[MenuJp].size;
			g_td[TEXTURE_MENU_JP].pos = g_td[MenuJp].pos;

		}

	}

	if (GetKeyboardTrigger(DIK_RETURN))
	{
		switch (MenuPannelNum)
		{
		case TEXTURE_MENU_PANNEL_01:
			SetFade(FADE_OUT, MODE_GAME); // game
			break;
		case TEXTURE_MENU_PANNEL_02:
			//SetMode(); //option
			break;
		case TEXTURE_MENU_PANNEL_03:
			//SetMode(); //gallary
			break;
		case TEXTURE_MENU_PANNEL_04:
			//SetMode(); //credit
			break;
		case TEXTURE_MENU_PANNEL_05:
			exit(-1); //quit
			break;
		}
	}


	g_Model[MODEL_EARTH].srt.rot.y -= 0.01f;
	g_Model[MODEL_ROCKET].srt.rot.y += 0.03f;

}
////=============================================================================
//// �`�揈��
////=============================================================================
void DrawStart(void)
{
	SetDrawNoLighting();

	SetCullingMode(CULL_MODE_NONE);

	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	for (int testNo = 0; testNo < MODEL_MAX; testNo++)
	{

		MATERIAL material;

		// ���f���`��
		if (testNo != MODEL_STAR)
		{
			//DrawModel(&g_Model[testNo].model, NULL, &material);
			DrawModel(&g_Model[testNo].model, &g_Model[testNo].srt, NULL, &material);	// ���f���`��
		}
		else
		{
			//DrawModel(&g_Model[testNo].model, &g_Texture[TEXTURE_STAR], &material);
			DrawModel(&g_Model[testNo].model, &g_Model[testNo].srt, &g_Texture[TEXTURE_STAR], &material);	// ���f���`��
		}
	}

	SetCullingMode(CULL_MODE_BACK);
	//SetDraw2DTexture();
	DrawTexture2D(&g_td[TEXTURE_MENUBOARD], FALSE, TRUE);
	DrawTexture2D(&g_td[TEXTURE_MENU_LINE], FALSE, TRUE);
	DrawTexture2D(&g_td[TEXTURE_START], FALSE, TRUE);
	DrawTexture2D(&g_td[TEXTURE_OPTION], FALSE, TRUE);
	DrawTexture2D(&g_td[TEXTURE_GALLERY], FALSE, TRUE);
	DrawTexture2D(&g_td[TEXTURE_CREDIT], FALSE, TRUE);
	DrawTexture2D(&g_td[TEXTURE_QUIT], FALSE, TRUE);
	DrawTexture2D(&g_td[TEXTURE_MENU_PANNEL_01], FALSE, TRUE);
	DrawTexture2D(&g_td[TEXTURE_MENU_DISPLAY], FALSE, TRUE);
	DrawTexture2D(&g_td[TEXTURE_MENU_JP], FALSE, TRUE);


}
