//=============================================================================
//
// �`�[�����S��ʏ��� [character.cpp]
// Author : �t�u��
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "fade.h"
#include "input.h"
#include "texture2d.h"
#include "wallet.h"
#include "rocket_select.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH)	// �w�i�T�C�Y��
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)	// �w�i�T�C�Y�c

#define MONEY_MAX			(99999999999)			// �������̏��

#define SIZE_WALLET			{536.0f,183.0f}			// ���z�T�C�Y
#define POS_WALLET			{1300.0f,880.0f}		// ���z���W

#define SIZE_X_TOTAL_NUM	(347.0f/10.0f)	// �i���o�[�T�C�Y��
#define SIZE_Y_TOTAL_NUM	(45.0f)			// �i���o�[�T�C�Y�c
#define POS_X_TOTAL_NUM		(1790.0f)		// �i���o�[���W��
#define POS_Y_TOTAL_NUM		(945.0f)		// �i���o�[���W�c

#define SIZE_X_MINUS_NUM	(262.0f/10.0f)	// �L�����N�^�[�T�C�Y��
#define SIZE_Y_MINUS_NUM	(33.0f)			// �L�����N�^�[�T�C�Y��
#define POS_X_MINUS_NUM		(1793.0f)		// �L�����N�^�[�T�C�Y�c
#define POS_Y_MINUS_NUM		(1014.0f)		// �L�����N�^�[�T�C�Y�c

#define POS_X_SELECTION_NO		(1175.0f)		// �L�����N�^�[�T�C�Y�c
#define POS_X_SELECTION_BUY		(1506.0f)		// �L�����N�^�[�T�C�Y�c

#define COL_BLACK		{0.0f,0.0f,0.0f,1.0f}	// �����F
#define COL_ORIGINAL		{1.0f,1.0f,1.0f,1.0f}	// ���F


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static BOOL				g_Load = FALSE;
static BOOL				g_InTransaction = FALSE;
static BOOL				g_SuccessfulTransaction = FALSE;
static ULONG64			g_Money;
static ULONG64			g_Price;

// �e�N�X�`���Ǘ�
enum
{
	TEXTURE_WALLET = 0,
	TEXTURE_WALLET_TOTAL,
	TEXTURE_WALLET_DOT,
	TEXTURE_WALLET_COMMA,
	TEXTURE_YEN,
	TEXTURE_MINUS_TOTAL,
	TEXTURE_MINUS_DOT,
	TEXTURE_MINUS_COMMA,
	TEXTURE_MINUS,
	TEXTURE_SHOP_MENU_OPACITY,
	TEXTURE_BUY_MENU,
	TEXTURE_BUY_MENU_SELECTION,
	TEXTURE_NO,
	TEXTURE_BUY,
	TEXTURE_MAX,
};
static TEXTURE2D_DESC	g_td[TEXTURE_MAX];


//=============================================================================
// ����������
//=============================================================================
HRESULT InitWallet(void)
{
	// �e�N�X�`������
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		g_td[i].tex = (TEXTURE_LABEL)(TEXTURE_LABEL_WALLET + i);
		g_td[i].ctrType = CENTER_LEFTTOP;
		g_td[i].posType = POSITION_ABSOLUTE;
	}

	// �ڍאݒ�
	g_td[TEXTURE_WALLET].size = SIZE_WALLET;
	g_td[TEXTURE_WALLET].pos = POS_WALLET;

	g_td[TEXTURE_WALLET_TOTAL].size = { SIZE_X_TOTAL_NUM ,SIZE_Y_TOTAL_NUM };
	g_td[TEXTURE_WALLET_TOTAL].pos = { POS_X_TOTAL_NUM ,POS_Y_TOTAL_NUM };
	g_td[TEXTURE_WALLET_TOTAL].uv_pos = { 0.1f, 0.0f, 0.1f, 1.0f };
	g_td[TEXTURE_WALLET_DOT].size = {17.0f,52.0f};
	g_td[TEXTURE_WALLET_DOT].pos = { POS_X_TOTAL_NUM ,POS_Y_TOTAL_NUM };
	g_td[TEXTURE_WALLET_COMMA].size = { 17.0f,52.0f };
	g_td[TEXTURE_WALLET_COMMA].pos = { POS_X_TOTAL_NUM ,POS_Y_TOTAL_NUM };
	g_td[TEXTURE_YEN].size = { 30.0f,37.0f };
	g_td[TEXTURE_YEN].pos = { POS_X_TOTAL_NUM ,POS_Y_TOTAL_NUM };
	g_td[TEXTURE_YEN].pos.y += 10.0f;

	g_td[TEXTURE_MINUS_TOTAL].size = { SIZE_X_MINUS_NUM ,SIZE_Y_MINUS_NUM };
	g_td[TEXTURE_MINUS_TOTAL].pos = { POS_X_MINUS_NUM ,POS_Y_MINUS_NUM };
	g_td[TEXTURE_MINUS_TOTAL].uv_pos = { 0.1f, 0.0f, 0.1f, 1.0f };
	g_td[TEXTURE_MINUS_DOT].size = { 13.0f,37.0f };
	g_td[TEXTURE_MINUS_DOT].pos = { POS_X_MINUS_NUM ,POS_Y_MINUS_NUM };
	g_td[TEXTURE_MINUS_COMMA].size = { 13.0f,37.0f };
	g_td[TEXTURE_MINUS_COMMA].pos = { POS_X_MINUS_NUM ,POS_Y_MINUS_NUM };
	g_td[TEXTURE_MINUS].size = { 19.0f,37.0f };
	g_td[TEXTURE_MINUS].pos = { POS_X_MINUS_NUM ,POS_Y_MINUS_NUM };

	g_td[TEXTURE_SHOP_MENU_OPACITY].size = { 1001.0f, 1080.0f };
	g_td[TEXTURE_SHOP_MENU_OPACITY].pos = { 919.0f,0.0f };
	g_td[TEXTURE_BUY_MENU].size = { 669.0f, 225.0f };
	g_td[TEXTURE_BUY_MENU].pos = { 1130.0f,600.0f };
	g_td[TEXTURE_BUY_MENU_SELECTION].size = { 239.0f, 62.0f };
	g_td[TEXTURE_BUY_MENU_SELECTION].pos = { POS_X_SELECTION_NO,700.0f };
	g_td[TEXTURE_NO].size = { 134.0f, 37.0f };
	g_td[TEXTURE_NO].pos = { 1224.0f,711.0f };
	g_td[TEXTURE_NO].col = { 0.0f,0.0f,0.0f,1.0f };
	g_td[TEXTURE_BUY].size = { 91.0f, 37.0f };
	g_td[TEXTURE_BUY].pos = { 1580.0f,711.0f };
	g_td[TEXTURE_BUY].col = { 0.0f,0.0f,0.0f,1.0f };

	g_InTransaction = FALSE;
	g_SuccessfulTransaction = FALSE;

	g_Money = MONEY_MAX; //SaveData����ǂݍ��ޗ\��

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitWallet(void)
{
	if (g_Load == FALSE) return;

	g_Load = FALSE;
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateWallet(void)
{
	if (!g_InTransaction)return;

	if (GetKeyboardTrigger(DIK_A)) 
	{// ����Ȃ��{�^����I������ꍇ
		g_td[TEXTURE_BUY_MENU_SELECTION].pos.x = POS_X_SELECTION_NO;
		g_td[TEXTURE_NO].col = COL_ORIGINAL;
		g_td[TEXTURE_BUY].col = COL_BLACK;
		
	}
	else if (GetKeyboardTrigger(DIK_D)) 
	{// �����{�^����I������ꍇ
		g_td[TEXTURE_BUY_MENU_SELECTION].pos.x = POS_X_SELECTION_BUY;
		g_td[TEXTURE_NO].col = COL_BLACK;
		g_td[TEXTURE_BUY].col = COL_ORIGINAL ;
	}
	else if (GetKeyboardTrigger(DIK_RETURN))
	{
		if ((g_Money >= g_Price) && ((int)g_td[TEXTURE_BUY_MENU_SELECTION].pos.x == (int)POS_X_SELECTION_BUY))
		{//�����������Ă��� + �����{�^����I������ꍇ
			g_Money -= g_Price;
			g_SuccessfulTransaction = TRUE;
		}
		else
		{
			g_InTransaction = FALSE;
		}
	}
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawWallet(void)
{	
	if (IsRocketSelectFinished())return;

	DrawTexture2D(&g_td[TEXTURE_WALLET]);

	// �v���C���[�̂����̕`�揈��
	ULONG64 money = g_Money;
	int count = 0;
	while (money > 0)
	{
		if (count == 2)
		{
			g_td[TEXTURE_WALLET_DOT].pos.x = POS_X_TOTAL_NUM - count * SIZE_X_TOTAL_NUM;
			DrawTexture2D(&g_td[TEXTURE_WALLET_DOT]);
			count++;
		}
		else if (count == 6)
		{
			g_td[TEXTURE_WALLET_COMMA].pos.x = POS_X_TOTAL_NUM - count * SIZE_X_TOTAL_NUM;
			DrawTexture2D(&g_td[TEXTURE_WALLET_COMMA]);
			count++;
		}
		else if (count == 10)
		{
			g_td[TEXTURE_WALLET_COMMA].pos.x = POS_X_TOTAL_NUM - count * SIZE_X_TOTAL_NUM;
			DrawTexture2D(&g_td[TEXTURE_WALLET_COMMA]);
			count++;
		}
		g_td[TEXTURE_WALLET_TOTAL].uv_pos.u = money % 10 * 0.1f;
		g_td[TEXTURE_WALLET_TOTAL].pos.x = POS_X_TOTAL_NUM - count * SIZE_X_TOTAL_NUM;
		DrawTexture2D(&g_td[TEXTURE_WALLET_TOTAL]);
		money /= 10;
		count++;
	}
	g_td[TEXTURE_YEN].pos.x = POS_X_TOTAL_NUM - count* SIZE_X_TOTAL_NUM;
	DrawTexture2D(&g_td[TEXTURE_YEN]);


	// ����̂����̕`�揈��
	money = g_Price;
	if (money == 0)return;

	count = 0;
	while (money > 0)
	{
		if (count == 2)
		{
			g_td[TEXTURE_MINUS_DOT].pos.x = POS_X_MINUS_NUM - count * SIZE_X_MINUS_NUM;
			DrawTexture2D(&g_td[TEXTURE_MINUS_DOT]);
			count++;
		}
		else if (count == 6)
		{
			g_td[TEXTURE_MINUS_COMMA].pos.x = POS_X_MINUS_NUM - count * SIZE_X_MINUS_NUM;
			DrawTexture2D(&g_td[TEXTURE_MINUS_COMMA]);
			count++;
		}
		else if (count == 10)
		{
			g_td[TEXTURE_MINUS_COMMA].pos.x = POS_X_MINUS_NUM - count * SIZE_X_MINUS_NUM;
			DrawTexture2D(&g_td[TEXTURE_MINUS_COMMA]);
			count++;
		}
		g_td[TEXTURE_MINUS_TOTAL].uv_pos.u = money % 10 * 0.1f;
		g_td[TEXTURE_MINUS_TOTAL].pos.x = POS_X_MINUS_NUM - count* SIZE_X_MINUS_NUM;
		DrawTexture2D(&g_td[TEXTURE_MINUS_TOTAL]);
		money /= 10;
		count++;
	}
	g_td[TEXTURE_MINUS].pos.x = POS_X_MINUS_NUM - count* SIZE_X_MINUS_NUM;
	DrawTexture2D(&g_td[TEXTURE_MINUS]);


	if (!g_InTransaction)return;

	DrawTexture2D(&g_td[TEXTURE_SHOP_MENU_OPACITY]);
	DrawTexture2D(&g_td[TEXTURE_BUY_MENU]);
	DrawTexture2D(&g_td[TEXTURE_BUY_MENU_SELECTION]);
	DrawTexture2D(&g_td[TEXTURE_NO]);
	DrawTexture2D(&g_td[TEXTURE_BUY]);
}

void SetPriceRocket(const ULONG64& price)
{
	g_Price = price;
}


void StartTransaction(void)
{
	g_InTransaction = TRUE;
	g_SuccessfulTransaction = FALSE;
	g_td[TEXTURE_BUY_MENU_SELECTION].pos.x = POS_X_SELECTION_NO;
	g_td[TEXTURE_NO].col = COL_ORIGINAL;
	g_td[TEXTURE_BUY].col = COL_BLACK;
}

BOOL InTransaction(void)
{
	return g_InTransaction;
}

BOOL SuccessfulTransaction(void)
{
	if (g_SuccessfulTransaction)
	{
		g_InTransaction = FALSE;
		return TRUE;
	}
	return FALSE;
}

void GainMoney(ULONG64& gain)
{
	g_Money += gain;
	if (g_Money > MONEY_MAX)
	{
		g_Money = MONEY_MAX;
	}
}

ULONG64& GetMoney(void)
{
	return g_Money;
}
