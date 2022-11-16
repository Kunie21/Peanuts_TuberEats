//=============================================================================
//
// ���T���g��ʏ��� [result.cpp]
// Author : � ��m
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "fade.h"
#include "input.h"
#include "result.h"
#include "texture2d.h"
#include "ui_game.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH)	// �w�i�T�C�Y��
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)	// �w�i�T�C�Y�c

#define STAR_MAX					(5)				// ���̍ő吔
#define STAR_WIDTH					(118.0f)		// ���̃T�C�Y��
#define STAR_HEIGHT					(113.0f)		// ���̃T�C�Y�c

#define EXPRESSION_WIDTH			(190.0f)		// �\��̃T�C�Y��
#define EXPRESSION_HEIGHT			(190.0f)		// �\��̃T�C�Y�c

#define WORD_WIDTH					(205.0f)		// ���ڂ̃T�C�Y��
#define WORD_HEIGHT					(52.0f)			// ���ڂ̃T�C�Y�c

#define YEN_MAX						(4)				// ���i���ڂ̍ő吔
#define NUMBER_WIDTH				(389.0f)		// �����̉���
#define TOTAL_NUMBER_WIDTH			(517.0f)		// �����v�����̉���
#define YEN_DIGIT					(4)				// �����̌���
#define ZERO_DIGIT					(2)				// �����_�ȍ~�̌���
#define TOTAL_YEN_DIGIT				(5)				// �����v�����̌���
#define YEN_FINAL_POS				(570.0f)		// �����̍Ō�̌��̈ʒu

#define MOVE						(28.0f)			// �ړ���

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static int				g_Time = 0;
static BOOL				g_Load = FALSE;

// �e�N�X�`���Ǘ�
enum
{
	TEXTURE_RESULT_BAR = 0,
	TEXTURE_RESULT_BG,
	TEXTURE_RESULT_NO_STARS,
	TEXTURE_RESULT_GREEN_STARS,
	TEXTURE_RESULT_EXPRESSION_1,
	TEXTURE_RESULT_EXPRESSION_2,
	TEXTURE_RESULT_EXPRESSION_3,
	TEXTURE_RESULT_SCROLL_BAR,
	TEXTURE_RESULT_SCROLL,
	TEXTURE_RESULT_DELIVERY_DISTAMCE,
	TEXTURE_RESULT_DELIVERY_TIME,
	TEXTURE_RESULT_DELIVERY_FEE,
	TEXTURE_RESULT_TIP,
	TEXTURE_RESULT_DAMAGE,
	TEXTURE_RESULT_TOTAL_AMOUNT,
	TEXTURE_RESULT_YEN,
	TEXTURE_RESULT_MIUS_YEN,
	TEXTURE_RESULT_TOTAL_YEN,
	TEXTURE_RESULT_NUMBER,
	TEXTURE_RESULT_MIUS_NUMBER,
	TEXTURE_RESULT_TOTAL_NUMBER,
	TEXTURE_RESULT_POINT,
	TEXTURE_RESULT_MIUS_POINT,
	TEXTURE_RESULT_TOTAL_POINT,
	TEXTURE_RESULT_FIREWORK_1,
	TEXTURE_RESULT_FIREWORK_2,
	TEXTURE_RESULT_FIREWORK_3,
	TEXTURE_RESULT_FIREWORK_4,
	TEXTURE_RESULT_FIREWORK_5,
	TEXTURE_MAX,
};
static TEXTURE2D_DESC	g_td[TEXTURE_MAX];
static ID3D11ShaderResourceView*	g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����
static char*	g_TextureName[TEXTURE_MAX] = {
	"data/TEXTURE/result_gamen/result_banner.png",
	"data/TEXTURE/result_gamen/result_bg.png",
	"data/TEXTURE/result_gamen/no_star.png",
	"data/TEXTURE/result_gamen/green_star.png",
	"data/TEXTURE/result_gamen/expression_1.png",
	"data/TEXTURE/result_gamen/expression_2.png",
	"data/TEXTURE/result_gamen/expression_3.png",
	"data/TEXTURE/result_gamen/scrollbar.png",
	"data/TEXTURE/result_gamen/scroll.png",
	"data/TEXTURE/result_gamen/delivery_distance.png",
	"data/TEXTURE/result_gamen/delivery_time.png",
	"data/TEXTURE/result_gamen/delivery_fee.png",
	"data/TEXTURE/result_gamen/tip.png",
	"data/TEXTURE/result_gamen/damage.png",
	"data/TEXTURE/result_gamen/total_amount.png",
	"data/TEXTURE/result_gamen/yen.png",
	"data/TEXTURE/result_gamen/minus_yen.png",
	"data/TEXTURE/result_gamen/total_yen.png",
	"data/TEXTURE/result_gamen/result_number.png",
	"data/TEXTURE/result_gamen/minus_result_number.png",
	"data/TEXTURE/result_gamen/total_result_number.png",
	"data/TEXTURE/result_gamen/point.png",
	"data/TEXTURE/result_gamen/minus_point.png",
	"data/TEXTURE/result_gamen/total_point.png",
	"data/TEXTURE/result_gamen/firework1.png",
	"data/TEXTURE/result_gamen/firework2.png",
	"data/TEXTURE/result_gamen/firework3.png",
	"data/TEXTURE/result_gamen/firework4.png",
	"data/TEXTURE/result_gamen/firework5.png",

};

static float ResultTime = 0.0f;
static int GreenStarNum = 0;
static int DeliveryDistance = 0;
static int DeliveryTime = 0;
static int DeliveryFee = 0;
static int Tip = 0;
static int Damage = 0;
static int TotalAmount = 0;


//=============================================================================
// ����������
//=============================================================================
HRESULT InitResult(void)
{
	// �e�N�X�`������
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_TextureName[i],
			NULL,
			NULL,
			&g_Texture[i],
			NULL);
	}

	// �ڍאݒ�
	//��ԏ�̃o�[
	g_td[TEXTURE_RESULT_BAR].size = { TEXTURE_WIDTH, 154.0f };
	g_td[TEXTURE_RESULT_BAR].tex = &g_Texture[TEXTURE_RESULT_BAR];
	g_td[TEXTURE_RESULT_BAR].pos = { 0.0f, (-SCREEN_HEIGHT/2 + (g_td[TEXTURE_RESULT_BAR].size.y / 2))};
	
	//�w�i
	g_td[TEXTURE_RESULT_BG].size = { TEXTURE_WIDTH, 1978.0f };
	g_td[TEXTURE_RESULT_BG].tex = &g_Texture[TEXTURE_RESULT_BG];
	g_td[TEXTURE_RESULT_BG].pos = { 0.0f, (0.0f + g_td[TEXTURE_RESULT_BAR].size.y) };
	g_td[TEXTURE_RESULT_BG].uv_pos = { 0.0f, 0.0f, 1.0f, 0.7f };

	//��
	g_td[TEXTURE_RESULT_NO_STARS].size = { STAR_WIDTH, STAR_HEIGHT };
	g_td[TEXTURE_RESULT_NO_STARS].tex = &g_Texture[TEXTURE_RESULT_NO_STARS];
	g_td[TEXTURE_RESULT_NO_STARS].pos = { 0.0f, -180.0f };

	g_td[TEXTURE_RESULT_GREEN_STARS].size = { STAR_WIDTH, STAR_HEIGHT };
	g_td[TEXTURE_RESULT_GREEN_STARS].tex = &g_Texture[TEXTURE_RESULT_GREEN_STARS];
	g_td[TEXTURE_RESULT_GREEN_STARS].pos = { -(g_td[TEXTURE_RESULT_GREEN_STARS].size.x + 50.0f) * 2, -180.0f };

	//�\��
	g_td[TEXTURE_RESULT_EXPRESSION_1].size = { EXPRESSION_WIDTH, EXPRESSION_HEIGHT };
	g_td[TEXTURE_RESULT_EXPRESSION_1].tex = &g_Texture[TEXTURE_RESULT_EXPRESSION_1];
	g_td[TEXTURE_RESULT_EXPRESSION_1].pos = { 0.0f, 90.0f };

	g_td[TEXTURE_RESULT_EXPRESSION_2].size = { EXPRESSION_WIDTH, EXPRESSION_HEIGHT };
	g_td[TEXTURE_RESULT_EXPRESSION_2].tex = &g_Texture[TEXTURE_RESULT_EXPRESSION_2];
	g_td[TEXTURE_RESULT_EXPRESSION_2].pos = { 0.0f, 90.0f };

	g_td[TEXTURE_RESULT_EXPRESSION_3].size = { EXPRESSION_WIDTH, EXPRESSION_HEIGHT };
	g_td[TEXTURE_RESULT_EXPRESSION_3].tex = &g_Texture[TEXTURE_RESULT_EXPRESSION_3];
	g_td[TEXTURE_RESULT_EXPRESSION_3].pos = { 0.0f, 90.0f };

	//�X�N���[��
	g_td[TEXTURE_RESULT_SCROLL_BAR].size = { 35.0f, 772.0f };
	g_td[TEXTURE_RESULT_SCROLL_BAR].tex = &g_Texture[TEXTURE_RESULT_SCROLL_BAR];
	g_td[TEXTURE_RESULT_SCROLL_BAR].pos = { (SCREEN_WIDTH /2 - g_td[TEXTURE_RESULT_SCROLL_BAR].size.x - 50.0f), 20.0f };
	
	g_td[TEXTURE_RESULT_SCROLL].size = { 35.0f, 386.0f };
	g_td[TEXTURE_RESULT_SCROLL].tex = &g_Texture[TEXTURE_RESULT_SCROLL];
	g_td[TEXTURE_RESULT_SCROLL].pos = { (SCREEN_WIDTH / 2 - g_td[TEXTURE_RESULT_SCROLL_BAR].size.x - 50.0f), (0.0f - g_td[TEXTURE_RESULT_SCROLL].size.y / 2 + g_td[TEXTURE_RESULT_SCROLL_BAR].pos.y)  };

	//����
	g_td[TEXTURE_RESULT_DELIVERY_DISTAMCE].size = { WORD_WIDTH, WORD_HEIGHT };
	g_td[TEXTURE_RESULT_DELIVERY_DISTAMCE].tex = &g_Texture[TEXTURE_RESULT_DELIVERY_DISTAMCE];
	g_td[TEXTURE_RESULT_DELIVERY_DISTAMCE].pos = { -700.0f, 610.0f };

	g_td[TEXTURE_RESULT_DELIVERY_TIME].size = { WORD_WIDTH, WORD_HEIGHT };
	g_td[TEXTURE_RESULT_DELIVERY_TIME].tex = &g_Texture[TEXTURE_RESULT_DELIVERY_TIME];
	g_td[TEXTURE_RESULT_DELIVERY_TIME].pos = { -700.0f, 810.0f };

	g_td[TEXTURE_RESULT_DELIVERY_FEE].size = { WORD_WIDTH, WORD_HEIGHT };
	g_td[TEXTURE_RESULT_DELIVERY_FEE].tex = &g_Texture[TEXTURE_RESULT_DELIVERY_FEE];
	g_td[TEXTURE_RESULT_DELIVERY_FEE].pos = { -700.0f, 1010.0f };

	g_td[TEXTURE_RESULT_TIP].size = { 143.0f,WORD_HEIGHT };	// �O�����Ȃ̂ŉ�������
	g_td[TEXTURE_RESULT_TIP].tex = &g_Texture[TEXTURE_RESULT_TIP];
	g_td[TEXTURE_RESULT_TIP].pos = { -700.0f, 1210.0f };

	g_td[TEXTURE_RESULT_DAMAGE].size = { 195.0f, WORD_HEIGHT };
	g_td[TEXTURE_RESULT_DAMAGE].tex = &g_Texture[TEXTURE_RESULT_DAMAGE];
	g_td[TEXTURE_RESULT_DAMAGE].pos = { -700.0f , 1410.0f };

	g_td[TEXTURE_RESULT_TOTAL_AMOUNT].size = { 217.0f, 73.0f };
	g_td[TEXTURE_RESULT_TOTAL_AMOUNT].tex = &g_Texture[TEXTURE_RESULT_TOTAL_AMOUNT];
	g_td[TEXTURE_RESULT_TOTAL_AMOUNT].pos = { -700.0f , 1700.0f };

	//�l�i�`��
	g_td[TEXTURE_RESULT_YEN].size = { 42.0f, 52.0f };
	g_td[TEXTURE_RESULT_YEN].tex = &g_Texture[TEXTURE_RESULT_YEN];
	g_td[TEXTURE_RESULT_YEN].pos = { 400.0f, 608.0f };

	g_td[TEXTURE_RESULT_MIUS_YEN].size = { 66.0f, 52.0f };
	g_td[TEXTURE_RESULT_MIUS_YEN].tex = &g_Texture[TEXTURE_RESULT_MIUS_YEN];
	g_td[TEXTURE_RESULT_MIUS_YEN].pos = { 380.0f , 1410.0f };

	g_td[TEXTURE_RESULT_TOTAL_YEN].size = { 58.0f, 73.0f };
	g_td[TEXTURE_RESULT_TOTAL_YEN].tex = &g_Texture[TEXTURE_RESULT_TOTAL_YEN];
	g_td[TEXTURE_RESULT_TOTAL_YEN].pos = { 250.0f , 1700.0f };

	g_td[TEXTURE_RESULT_NUMBER].size = { 389.0f, 52.0f };
	g_td[TEXTURE_RESULT_NUMBER].tex = &g_Texture[TEXTURE_RESULT_NUMBER];
	g_td[TEXTURE_RESULT_NUMBER].pos = { YEN_FINAL_POS, 608.0f };
	g_td[TEXTURE_RESULT_NUMBER].scl = { 0.1f, 1.0f };
	g_td[TEXTURE_RESULT_NUMBER].uv_pos = { 0.1f, 0.0f, 0.1f, 1.0f };

	g_td[TEXTURE_RESULT_MIUS_NUMBER].size = { NUMBER_WIDTH, 52.0f };
	g_td[TEXTURE_RESULT_MIUS_NUMBER].tex = &g_Texture[TEXTURE_RESULT_MIUS_NUMBER];
	g_td[TEXTURE_RESULT_MIUS_NUMBER].pos = { YEN_FINAL_POS , 1410.0f };
	g_td[TEXTURE_RESULT_MIUS_NUMBER].scl = { 0.1f, 1.0f };
	g_td[TEXTURE_RESULT_MIUS_NUMBER].uv_pos = { 0.1f, 0.0f, 0.1f, 1.0f };

	g_td[TEXTURE_RESULT_TOTAL_NUMBER].size = { TOTAL_NUMBER_WIDTH, 73.0f };
	g_td[TEXTURE_RESULT_TOTAL_NUMBER].tex = &g_Texture[TEXTURE_RESULT_TOTAL_NUMBER];
	g_td[TEXTURE_RESULT_TOTAL_NUMBER].pos = { YEN_FINAL_POS , 1700.0f };
	g_td[TEXTURE_RESULT_TOTAL_NUMBER].scl = { 0.1f, 1.0f };
	g_td[TEXTURE_RESULT_TOTAL_NUMBER].uv_pos = { 0.1f, 0.0f, 0.1f, 1.0f };

	g_td[TEXTURE_RESULT_POINT].size = { 8.0f, 8.0f };
	g_td[TEXTURE_RESULT_POINT].tex = &g_Texture[TEXTURE_RESULT_POINT];
	g_td[TEXTURE_RESULT_POINT].pos = { (YEN_FINAL_POS + 25.0f) , 608.0f };

	g_td[TEXTURE_RESULT_MIUS_POINT].size = { 8.0f, 8.0f };
	g_td[TEXTURE_RESULT_MIUS_POINT].tex = &g_Texture[TEXTURE_RESULT_MIUS_POINT];
	g_td[TEXTURE_RESULT_MIUS_POINT].pos = { (YEN_FINAL_POS + 25.0f) , 1428.0f };

	g_td[TEXTURE_RESULT_TOTAL_POINT].size = { 13.0f, 12.0f };
	g_td[TEXTURE_RESULT_TOTAL_POINT].tex = &g_Texture[TEXTURE_RESULT_TOTAL_POINT];
	g_td[TEXTURE_RESULT_TOTAL_POINT].pos = { (YEN_FINAL_POS + 32.0f) , 1725.0f };

	//�ԉ�
	g_td[TEXTURE_RESULT_FIREWORK_1].size = { 664.0f, 664.0f };
	g_td[TEXTURE_RESULT_FIREWORK_1].tex = &g_Texture[TEXTURE_RESULT_FIREWORK_1];
	g_td[TEXTURE_RESULT_FIREWORK_1].pos = { -600.0f , 200.0f };
	g_td[TEXTURE_RESULT_FIREWORK_1].scl = { 0.0f, 0.0f };

	g_td[TEXTURE_RESULT_FIREWORK_2].size = { 679.0f, 632.0f };
	g_td[TEXTURE_RESULT_FIREWORK_2].tex = &g_Texture[TEXTURE_RESULT_FIREWORK_2];
	g_td[TEXTURE_RESULT_FIREWORK_2].pos = { 500.0f , -250.0f };
	g_td[TEXTURE_RESULT_FIREWORK_2].scl = { 0.0f, 0.0f };

	g_td[TEXTURE_RESULT_FIREWORK_3].size = { 483.0f, 471.0f };
	g_td[TEXTURE_RESULT_FIREWORK_3].tex = &g_Texture[TEXTURE_RESULT_FIREWORK_3];
	g_td[TEXTURE_RESULT_FIREWORK_3].pos = {-300.0f , -100.0f };
	g_td[TEXTURE_RESULT_FIREWORK_3].scl = { 0.0f, 0.0f };

	g_td[TEXTURE_RESULT_FIREWORK_4].size = { 610.0f, 613.0f };
	g_td[TEXTURE_RESULT_FIREWORK_4].tex = &g_Texture[TEXTURE_RESULT_FIREWORK_4];
	g_td[TEXTURE_RESULT_FIREWORK_4].pos = { 400.0f , 300.0f };
	g_td[TEXTURE_RESULT_FIREWORK_4].scl = { 0.0f, 0.0f };

	g_td[TEXTURE_RESULT_FIREWORK_5].size = { 368.0f, 349.0f };
	g_td[TEXTURE_RESULT_FIREWORK_5].tex = &g_Texture[TEXTURE_RESULT_FIREWORK_5];
	g_td[TEXTURE_RESULT_FIREWORK_5].pos = { -600.0f , -250.0f };
	g_td[TEXTURE_RESULT_FIREWORK_5].scl = { 0.0f, 0.0f };


	//ResultTime = GetTimer();
	ResultTime = 10.0f;

	DeliveryDistance = 3333;
	DeliveryTime = 222;
	DeliveryFee = 11;
	Tip = 10;
	Damage = 1000;

	TotalAmount = (DeliveryDistance + DeliveryTime + DeliveryFee + Tip - Damage);

	g_Time = 0;

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitResult(void)
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

//=============================================================================
// �X�V����
//=============================================================================
void UpdateResult(void)
{
	//�X�N���[������
	if (GetKeyboardPress(DIK_DOWN))
	{
		if (g_td[TEXTURE_RESULT_BG].uv_pos.v <= 0.45f)
		{
			g_td[TEXTURE_RESULT_BG].uv_pos.v += 0.01f;
			g_td[TEXTURE_RESULT_NO_STARS].pos.y -= MOVE;
			g_td[TEXTURE_RESULT_GREEN_STARS].pos.y -= MOVE;
			g_td[TEXTURE_RESULT_EXPRESSION_1].pos.y -= MOVE;
			g_td[TEXTURE_RESULT_EXPRESSION_2].pos.y -= MOVE;
			g_td[TEXTURE_RESULT_EXPRESSION_3].pos.y -= MOVE;
			g_td[TEXTURE_RESULT_SCROLL].pos.y += 8.4f;

			g_td[TEXTURE_RESULT_DELIVERY_DISTAMCE].pos.y -= MOVE;
			g_td[TEXTURE_RESULT_DELIVERY_TIME].pos.y -= MOVE;
			g_td[TEXTURE_RESULT_DELIVERY_FEE].pos.y -= MOVE;
			g_td[TEXTURE_RESULT_TIP].pos.y -= MOVE;
			g_td[TEXTURE_RESULT_DAMAGE].pos.y -= MOVE;
			g_td[TEXTURE_RESULT_TOTAL_AMOUNT].pos.y -= MOVE;

			g_td[TEXTURE_RESULT_YEN].pos.y -= MOVE;
			g_td[TEXTURE_RESULT_MIUS_YEN].pos.y -= MOVE;
			g_td[TEXTURE_RESULT_TOTAL_YEN].pos.y -= MOVE;
			g_td[TEXTURE_RESULT_NUMBER].pos.y -= MOVE;
			g_td[TEXTURE_RESULT_MIUS_NUMBER].pos.y -= MOVE;
			g_td[TEXTURE_RESULT_TOTAL_NUMBER].pos.y -= MOVE;
			g_td[TEXTURE_RESULT_POINT].pos.y -= MOVE;
			g_td[TEXTURE_RESULT_MIUS_POINT].pos.y -= MOVE;
			g_td[TEXTURE_RESULT_TOTAL_POINT].pos.y -= MOVE;

			g_td[TEXTURE_RESULT_FIREWORK_1].pos.y -= MOVE;
			g_td[TEXTURE_RESULT_FIREWORK_2].pos.y -= MOVE;
			g_td[TEXTURE_RESULT_FIREWORK_3].pos.y -= MOVE;
			g_td[TEXTURE_RESULT_FIREWORK_4].pos.y -= MOVE;
			g_td[TEXTURE_RESULT_FIREWORK_5].pos.y -= MOVE;
		}
	}
	if (GetKeyboardPress(DIK_UP))
	{
		if (g_td[TEXTURE_RESULT_BG].uv_pos.v > 0.0f)
		{
			g_td[TEXTURE_RESULT_BG].uv_pos.v -= 0.01f;
			g_td[TEXTURE_RESULT_NO_STARS].pos.y += MOVE;
			g_td[TEXTURE_RESULT_GREEN_STARS].pos.y += MOVE;
			g_td[TEXTURE_RESULT_EXPRESSION_1].pos.y += MOVE;
			g_td[TEXTURE_RESULT_EXPRESSION_2].pos.y += MOVE;
			g_td[TEXTURE_RESULT_EXPRESSION_3].pos.y += MOVE;
			g_td[TEXTURE_RESULT_SCROLL].pos.y -= 8.4f;

			g_td[TEXTURE_RESULT_DELIVERY_DISTAMCE].pos.y += MOVE;
			g_td[TEXTURE_RESULT_DELIVERY_TIME].pos.y += MOVE;
			g_td[TEXTURE_RESULT_DELIVERY_FEE].pos.y += MOVE;
			g_td[TEXTURE_RESULT_TIP].pos.y += MOVE;
			g_td[TEXTURE_RESULT_DAMAGE].pos.y += MOVE;
			g_td[TEXTURE_RESULT_TOTAL_AMOUNT].pos.y += MOVE;

			g_td[TEXTURE_RESULT_YEN].pos.y += MOVE;
			g_td[TEXTURE_RESULT_MIUS_YEN].pos.y += MOVE;
			g_td[TEXTURE_RESULT_TOTAL_YEN].pos.y += MOVE;
			g_td[TEXTURE_RESULT_NUMBER].pos.y += MOVE;
			g_td[TEXTURE_RESULT_MIUS_NUMBER].pos.y += MOVE;
			g_td[TEXTURE_RESULT_TOTAL_NUMBER].pos.y += MOVE;
			g_td[TEXTURE_RESULT_POINT].pos.y += MOVE;
			g_td[TEXTURE_RESULT_MIUS_POINT].pos.y += MOVE;
			g_td[TEXTURE_RESULT_TOTAL_POINT].pos.y += MOVE;

			g_td[TEXTURE_RESULT_FIREWORK_1].pos.y += MOVE;
			g_td[TEXTURE_RESULT_FIREWORK_2].pos.y += MOVE;
			g_td[TEXTURE_RESULT_FIREWORK_3].pos.y += MOVE;
			g_td[TEXTURE_RESULT_FIREWORK_4].pos.y += MOVE;
			g_td[TEXTURE_RESULT_FIREWORK_5].pos.y += MOVE;

		}
	}

	//���ԂŐ��������
	if (ResultTime <= 20.0f)
	{
		GreenStarNum = 5;
	}
	else if (ResultTime <= 30.0f)
	{
		GreenStarNum = 4;
	}
	else if (ResultTime <= 40.0f)
	{
		GreenStarNum = 3;
	}
	else if (ResultTime <= 50.0f)
	{
		GreenStarNum = 2;
	}
	else if (ResultTime <= 60.0f)
	{
		GreenStarNum = 1;
	}
	else
	{
		GreenStarNum = 0;
	}

	//�ԉ�
	if (g_Time > 60)
	{
		if (g_td[TEXTURE_RESULT_FIREWORK_1].scl.x <= 1.0f)
		{
			g_td[TEXTURE_RESULT_FIREWORK_1].scl.x += 0.01f;
			g_td[TEXTURE_RESULT_FIREWORK_1].scl.y += 0.01f;
			g_td[TEXTURE_RESULT_FIREWORK_1].col.w -= 0.01f;

		}
	}	
	if (g_Time > 80)
	{
		if (g_td[TEXTURE_RESULT_FIREWORK_2].scl.x <= 0.8f)
		{
			g_td[TEXTURE_RESULT_FIREWORK_2].scl.x += 0.02f;
			g_td[TEXTURE_RESULT_FIREWORK_2].scl.y += 0.02f;
		}
		g_td[TEXTURE_RESULT_FIREWORK_2].col.w -= 0.02f;
	}

	if (g_Time > 90)
	{
		if (g_td[TEXTURE_RESULT_FIREWORK_3].scl.x <= 1.0f)
		{
			g_td[TEXTURE_RESULT_FIREWORK_3].scl.x += 0.02f;
			g_td[TEXTURE_RESULT_FIREWORK_3].scl.y += 0.02f;
		}
		g_td[TEXTURE_RESULT_FIREWORK_3].col.w -= 0.02f;
	}
	if (g_Time > 110)
	{
		if (g_td[TEXTURE_RESULT_FIREWORK_4].scl.x <= 0.7f)
		{
			g_td[TEXTURE_RESULT_FIREWORK_4].scl.x += 0.01f;
			g_td[TEXTURE_RESULT_FIREWORK_4].scl.y += 0.01f;
		}
		g_td[TEXTURE_RESULT_FIREWORK_4].col.w -= 0.02f;
	}
	if (g_Time > 130)
	{
		if (g_td[TEXTURE_RESULT_FIREWORK_5].scl.x <= 1.0f)
		{
			g_td[TEXTURE_RESULT_FIREWORK_5].scl.x += 0.02f;
			g_td[TEXTURE_RESULT_FIREWORK_5].scl.y += 0.02f;
		}
		g_td[TEXTURE_RESULT_FIREWORK_5].col.w -= 0.02f;
	}

	


	g_Time++;
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawResult(void)
{
	SetDraw2DTexture();

	//�w�i�`��
	DrawTexture2D(&g_td[TEXTURE_RESULT_BG], FALSE, TRUE);
	
	//�ő启���`��
	for (int i = 0; i < STAR_MAX; i++)
	{
		DrawTexture2D(&g_td[TEXTURE_RESULT_NO_STARS]);
		g_td[TEXTURE_RESULT_NO_STARS].pos.x = (-(g_td[TEXTURE_RESULT_NO_STARS].size.x + 50.0f) * 2) + ((g_td[TEXTURE_RESULT_NO_STARS].size.x + 50.0f) * i);
		//																				�����̊Ԋu����
	}

	//�l���������`��
	for (int i = 0; i < GreenStarNum; i++)
	{
		if (g_Time >= (i + 1) * 40)
		{
			DrawTexture2D(&g_td[TEXTURE_RESULT_GREEN_STARS]);
			g_td[TEXTURE_RESULT_GREEN_STARS].pos.x = (-(g_td[TEXTURE_RESULT_GREEN_STARS].size.x + 50.0f) * 2) + ((g_td[TEXTURE_RESULT_GREEN_STARS].size.x + 50.0f) * i);
			//																				�����̊Ԋu����
		}
	}

	//�\��`��
	if (ResultTime <= 30.0f)
	{
		DrawTexture2D(&g_td[TEXTURE_RESULT_EXPRESSION_1]);
	}
	else if(ResultTime <= 60.0f)
	{
		DrawTexture2D(&g_td[TEXTURE_RESULT_EXPRESSION_2]);
	}
	else
	{
		DrawTexture2D(&g_td[TEXTURE_RESULT_EXPRESSION_3]);
	}

	//�X�N���[���`��
	DrawTexture2D(&g_td[TEXTURE_RESULT_SCROLL_BAR]);
	DrawTexture2D(&g_td[TEXTURE_RESULT_SCROLL]);

	//���ڕ`��
	DrawTexture2D(&g_td[TEXTURE_RESULT_DELIVERY_DISTAMCE]);
	DrawTexture2D(&g_td[TEXTURE_RESULT_DELIVERY_TIME]);
	DrawTexture2D(&g_td[TEXTURE_RESULT_DELIVERY_FEE]);
	DrawTexture2D(&g_td[TEXTURE_RESULT_TIP]);
	DrawTexture2D(&g_td[TEXTURE_RESULT_DAMAGE]);
	DrawTexture2D(&g_td[TEXTURE_RESULT_TOTAL_AMOUNT]);

	//�~�}�[�N�`��
	for (int i = 0; i < YEN_MAX; i++)
	{
		g_td[TEXTURE_RESULT_YEN].pos.y = (g_td[TEXTURE_RESULT_DELIVERY_DISTAMCE + i].pos.y);
		DrawTexture2D(&g_td[TEXTURE_RESULT_YEN]);
	}
	DrawTexture2D(&g_td[TEXTURE_RESULT_MIUS_YEN]);
	DrawTexture2D(&g_td[TEXTURE_RESULT_TOTAL_YEN]);

	//�l�i�`��
	for (int i = 0; i < YEN_MAX; i++)
	{
		int number[YEN_MAX] = { DeliveryDistance,DeliveryTime,DeliveryFee,Tip };
	
		g_td[TEXTURE_RESULT_NUMBER].pos.y = (g_td[TEXTURE_RESULT_DELIVERY_DISTAMCE + i].pos.y);

		for (int j = 0; j < YEN_DIGIT; j++)
		{
			g_td[TEXTURE_RESULT_NUMBER].uv_pos.u = (number[i] % 10 * 0.1f);
			g_td[TEXTURE_RESULT_NUMBER].pos.x = (YEN_FINAL_POS - ((g_td[TEXTURE_RESULT_NUMBER].size.x / 10) * j));
			DrawTexture2D(&g_td[TEXTURE_RESULT_NUMBER], FALSE, TRUE);
			number[i] /= 10;
		}
	}
	int DamageNumber = Damage;
	for (int i = 0; i < YEN_DIGIT; i++)
	{
		g_td[TEXTURE_RESULT_MIUS_NUMBER].uv_pos.u = (DamageNumber % 10 * 0.1f);
		g_td[TEXTURE_RESULT_MIUS_NUMBER].pos.x = (YEN_FINAL_POS - ((g_td[TEXTURE_RESULT_MIUS_NUMBER].size.x / 10) * i));
		DrawTexture2D(&g_td[TEXTURE_RESULT_MIUS_NUMBER], FALSE, TRUE);
		DamageNumber /= 10;
	}
	int TotalNumber = TotalAmount;
	for (int i = 0; i < TOTAL_YEN_DIGIT; i++)
	{
		g_td[TEXTURE_RESULT_TOTAL_NUMBER].uv_pos.u = (TotalNumber % 10 * 0.1f);
		g_td[TEXTURE_RESULT_TOTAL_NUMBER].pos.x = (YEN_FINAL_POS - ((g_td[TEXTURE_RESULT_TOTAL_NUMBER].size.x / 10) * i) - 5.0f);
		DrawTexture2D(&g_td[TEXTURE_RESULT_TOTAL_NUMBER], FALSE, TRUE);
		TotalNumber /= 10;
	}
	for (int i = 0; i < YEN_MAX; i++)
	{
		g_td[TEXTURE_RESULT_POINT].pos.y = (g_td[TEXTURE_RESULT_DELIVERY_DISTAMCE + i].pos.y + 18.0f);

		DrawTexture2D(&g_td[TEXTURE_RESULT_POINT]);
	}
	DrawTexture2D(&g_td[TEXTURE_RESULT_MIUS_POINT]);
	DrawTexture2D(&g_td[TEXTURE_RESULT_TOTAL_POINT]);

	for (int i = 0; i < ZERO_DIGIT; i++)
	{
		for (int j = 0; j < YEN_MAX; j++)
		{
			g_td[TEXTURE_RESULT_NUMBER].pos.x = (YEN_FINAL_POS + g_td[TEXTURE_RESULT_POINT].size.x + ((g_td[TEXTURE_RESULT_NUMBER].size.x / 10) * (i + 1)));
			g_td[TEXTURE_RESULT_NUMBER].pos.y = (g_td[TEXTURE_RESULT_DELIVERY_DISTAMCE + j].pos.y);
			g_td[TEXTURE_RESULT_NUMBER].uv_pos.u = 0.0f;
			DrawTexture2D(&g_td[TEXTURE_RESULT_NUMBER], FALSE, TRUE);
		}
		g_td[TEXTURE_RESULT_MIUS_NUMBER].pos.x = (YEN_FINAL_POS + g_td[TEXTURE_RESULT_MIUS_POINT].size.x + ((g_td[TEXTURE_RESULT_MIUS_NUMBER].size.x / 10) * (i + 1)));
		g_td[TEXTURE_RESULT_MIUS_NUMBER].uv_pos.u = 0.0f;
		DrawTexture2D(&g_td[TEXTURE_RESULT_MIUS_NUMBER], FALSE, TRUE);

		g_td[TEXTURE_RESULT_TOTAL_NUMBER].pos.x = (YEN_FINAL_POS + g_td[TEXTURE_RESULT_TOTAL_POINT].size.x + ((g_td[TEXTURE_RESULT_TOTAL_NUMBER].size.x / 10) * (i + 1)));
		g_td[TEXTURE_RESULT_TOTAL_NUMBER].uv_pos.u = 0.0f;
		DrawTexture2D(&g_td[TEXTURE_RESULT_TOTAL_NUMBER], FALSE, TRUE);

	}

	//�ԉΕ`��
	DrawTexture2D(&g_td[TEXTURE_RESULT_FIREWORK_1]);
	DrawTexture2D(&g_td[TEXTURE_RESULT_FIREWORK_2]);
	DrawTexture2D(&g_td[TEXTURE_RESULT_FIREWORK_3]);
	DrawTexture2D(&g_td[TEXTURE_RESULT_FIREWORK_4]);
	DrawTexture2D(&g_td[TEXTURE_RESULT_FIREWORK_5]);


	//�Ō�ɏ�̃o�[�`��
	DrawTexture2D(&g_td[TEXTURE_RESULT_BAR]);


}
