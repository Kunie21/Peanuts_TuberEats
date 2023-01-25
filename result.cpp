//=============================================================================
//
// リサルト画面処理 [result.cpp]
// Author : 譚 倩盈
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "fade.h"
#include "input.h"
#include "result.h"
#include "texture2d.h"
#include "ui_game.h"
#include "stage.h"
#include "player.h"
#include "result_event.h"
#include "wallet.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH)	// 背景サイズ横
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)	// 背景サイズ縦

#define STAR_MAX					(5)				// 星の最大数
#define STAR_WIDTH					(118.0f)		// 星のサイズ横
#define STAR_HEIGHT					(113.0f)		// 星のサイズ縦

#define EXPRESSION_WIDTH			(190.0f)		// 表情のサイズ横
#define EXPRESSION_HEIGHT			(190.0f)		// 表情のサイズ縦

#define WORD_WIDTH					(205.0f)		// 項目のサイズ横
#define WORD_HEIGHT					(52.0f)			// 項目のサイズ縦

#define YEN_MAX						(4)				// 価格項目の最大数
#define NUMBER_WIDTH				(389.0f)		// 数字の横幅
#define TOTAL_NUMBER_WIDTH			(517.0f)		// 総収益数字の横幅
#define YEN_DIGIT					(4)				// 数字の桁数
#define ZERO_DIGIT					(2)				// 小数点以降の桁数
#define TOTAL_YEN_DIGIT				(5)				// 総収益数字の桁数
#define YEN_FINAL_POS				(570.0f)		// 数字の最後の桁の位置

#define MOVE						(28.0f)			// 移動量

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static int				g_Time = 0;
static BOOL				g_Load = FALSE;

// テクスチャ管理
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
	TEXTURE_RESULT_MAX,
};
static TEXTURE2D_DESC	g_td[TEXTURE_RESULT_MAX];

static float ResultTime = 0.0f;
static int GreenStarNum = 0;
static int DeliveryDistance = 0;
static int DeliveryTime = 0;
static int DeliveryFee = 0;
static int Tip = 0;
static int Damage = 0;
static int TotalAmount = 0;

static int DeliveryDistanceTbl[STAGE_MAX]
{ 401 ,1644 ,9844 ,10144 };

static BOOL EventFinish = FALSE;

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitResult(void)
{
	// テクスチャ生成
	for (int i = 0; i < TEXTURE_RESULT_MAX; i++) {
		g_td[i].tex = (TEXTURE_LABEL)(TEXTURE_LABEL_RESULT_BAR + i);
	}

	// 詳細設定
	//一番上のバー
	g_td[TEXTURE_RESULT_BAR].size = { TEXTURE_WIDTH, 154.0f };
	g_td[TEXTURE_RESULT_BAR].pos = { 0.0f, (-SCREEN_HEIGHT/2 + (g_td[TEXTURE_RESULT_BAR].size.y / 2))};
	
	//背景
	g_td[TEXTURE_RESULT_BG].size = { TEXTURE_WIDTH, 1978.0f };
	g_td[TEXTURE_RESULT_BG].pos = { 0.0f, (0.0f + g_td[TEXTURE_RESULT_BAR].size.y) };
	g_td[TEXTURE_RESULT_BG].uv_pos = { 0.0f, 0.0f, 1.0f, 0.7f };

	//星
	g_td[TEXTURE_RESULT_NO_STARS].size = { STAR_WIDTH, STAR_HEIGHT };
	g_td[TEXTURE_RESULT_NO_STARS].pos = { 0.0f, -180.0f };

	g_td[TEXTURE_RESULT_GREEN_STARS].size = { STAR_WIDTH, STAR_HEIGHT };
	g_td[TEXTURE_RESULT_GREEN_STARS].pos = { -(g_td[TEXTURE_RESULT_GREEN_STARS].size.x + 50.0f) * 2, -180.0f };

	//表情
	g_td[TEXTURE_RESULT_EXPRESSION_1].size = { EXPRESSION_WIDTH, EXPRESSION_HEIGHT };
	g_td[TEXTURE_RESULT_EXPRESSION_1].pos = { 0.0f, 90.0f };

	g_td[TEXTURE_RESULT_EXPRESSION_2].size = { EXPRESSION_WIDTH, EXPRESSION_HEIGHT };
	g_td[TEXTURE_RESULT_EXPRESSION_2].pos = { 0.0f, 90.0f };

	g_td[TEXTURE_RESULT_EXPRESSION_3].size = { EXPRESSION_WIDTH, EXPRESSION_HEIGHT };
	g_td[TEXTURE_RESULT_EXPRESSION_3].pos = { 0.0f, 90.0f };

	//スクロール
	g_td[TEXTURE_RESULT_SCROLL_BAR].size = { 35.0f, 772.0f };
	g_td[TEXTURE_RESULT_SCROLL_BAR].pos = { (SCREEN_WIDTH /2 - g_td[TEXTURE_RESULT_SCROLL_BAR].size.x - 50.0f), 20.0f };
	
	g_td[TEXTURE_RESULT_SCROLL].size = { 35.0f, 386.0f };
	g_td[TEXTURE_RESULT_SCROLL].pos = { (SCREEN_WIDTH / 2 - g_td[TEXTURE_RESULT_SCROLL_BAR].size.x - 50.0f), (0.0f - g_td[TEXTURE_RESULT_SCROLL].size.y / 2 + g_td[TEXTURE_RESULT_SCROLL_BAR].pos.y)  };

	//項目
	g_td[TEXTURE_RESULT_DELIVERY_DISTAMCE].size = { WORD_WIDTH, WORD_HEIGHT };
	g_td[TEXTURE_RESULT_DELIVERY_DISTAMCE].pos = { -700.0f, 610.0f };

	g_td[TEXTURE_RESULT_DELIVERY_TIME].size = { WORD_WIDTH, WORD_HEIGHT };
	g_td[TEXTURE_RESULT_DELIVERY_TIME].pos = { -700.0f, 810.0f };

	g_td[TEXTURE_RESULT_DELIVERY_FEE].size = { WORD_WIDTH, WORD_HEIGHT };
	g_td[TEXTURE_RESULT_DELIVERY_FEE].pos = { -700.0f, 1010.0f };

	g_td[TEXTURE_RESULT_TIP].size = { 143.0f,WORD_HEIGHT };	// 三文字なので横幅調整
	g_td[TEXTURE_RESULT_TIP].pos = { -700.0f, 1210.0f };

	g_td[TEXTURE_RESULT_DAMAGE].size = { 195.0f, WORD_HEIGHT };
	g_td[TEXTURE_RESULT_DAMAGE].pos = { -700.0f , 1410.0f };

	g_td[TEXTURE_RESULT_TOTAL_AMOUNT].size = { 217.0f, 73.0f };
	g_td[TEXTURE_RESULT_TOTAL_AMOUNT].pos = { -700.0f , 1700.0f };

	//値段描画
	g_td[TEXTURE_RESULT_YEN].size = { 42.0f, 52.0f };
	g_td[TEXTURE_RESULT_YEN].pos = { 400.0f, 608.0f };

	g_td[TEXTURE_RESULT_MIUS_YEN].size = { 66.0f, 52.0f };
	g_td[TEXTURE_RESULT_MIUS_YEN].pos = { 380.0f , 1410.0f };

	g_td[TEXTURE_RESULT_TOTAL_YEN].size = { 58.0f, 73.0f };
	g_td[TEXTURE_RESULT_TOTAL_YEN].pos = { 250.0f , 1700.0f };

	g_td[TEXTURE_RESULT_NUMBER].size = { 389.0f, 52.0f };
	g_td[TEXTURE_RESULT_NUMBER].pos = { YEN_FINAL_POS, 608.0f };
	//g_td[TEXTURE_RESULT_NUMBER].scl = { 0.1f, 1.0f };
	//g_td[TEXTURE_RESULT_NUMBER].uv_pos = { 0.1f, 0.0f, 0.1f, 1.0f };
	g_td[TEXTURE_RESULT_NUMBER].size.x *= 0.1f;
	g_td[TEXTURE_RESULT_NUMBER].uv_pos.uw = 0.1f;

	g_td[TEXTURE_RESULT_MIUS_NUMBER].size = { NUMBER_WIDTH, 52.0f };
	g_td[TEXTURE_RESULT_MIUS_NUMBER].pos = { YEN_FINAL_POS , 1410.0f };
	//g_td[TEXTURE_RESULT_MIUS_NUMBER].scl = { 0.1f, 1.0f };
	//g_td[TEXTURE_RESULT_MIUS_NUMBER].uv_pos = { 0.1f, 0.0f, 0.1f, 1.0f };
	g_td[TEXTURE_RESULT_MIUS_NUMBER].size.x *= 0.1f;
	g_td[TEXTURE_RESULT_MIUS_NUMBER].uv_pos.uw = 0.1f;

	g_td[TEXTURE_RESULT_TOTAL_NUMBER].size = { TOTAL_NUMBER_WIDTH, 73.0f };
	g_td[TEXTURE_RESULT_TOTAL_NUMBER].pos = { YEN_FINAL_POS , 1700.0f };
	//g_td[TEXTURE_RESULT_TOTAL_NUMBER].scl = { 0.1f, 1.0f };
	//g_td[TEXTURE_RESULT_TOTAL_NUMBER].uv_pos = { 0.1f, 0.0f, 0.1f, 1.0f };
	g_td[TEXTURE_RESULT_TOTAL_NUMBER].size.x *= 0.1f;
	g_td[TEXTURE_RESULT_TOTAL_NUMBER].uv_pos.uw = 0.1f;

	g_td[TEXTURE_RESULT_POINT].size = { 8.0f, 8.0f };
	g_td[TEXTURE_RESULT_POINT].pos = { (YEN_FINAL_POS + 25.0f) , 608.0f };

	g_td[TEXTURE_RESULT_MIUS_POINT].size = { 8.0f, 8.0f };
	g_td[TEXTURE_RESULT_MIUS_POINT].pos = { (YEN_FINAL_POS + 25.0f) , 1428.0f };

	g_td[TEXTURE_RESULT_TOTAL_POINT].size = { 13.0f, 12.0f };
	g_td[TEXTURE_RESULT_TOTAL_POINT].pos = { (YEN_FINAL_POS + 32.0f) , 1725.0f };

	//花火
	g_td[TEXTURE_RESULT_FIREWORK_1].size = { 664.0f, 664.0f };
	g_td[TEXTURE_RESULT_FIREWORK_1].pos = { -600.0f , 200.0f };
	g_td[TEXTURE_RESULT_FIREWORK_1].scl = { 0.0f, 0.0f };

	g_td[TEXTURE_RESULT_FIREWORK_2].size = { 679.0f, 632.0f };
	g_td[TEXTURE_RESULT_FIREWORK_2].pos = { 500.0f , -250.0f };
	g_td[TEXTURE_RESULT_FIREWORK_2].scl = { 0.0f, 0.0f };

	g_td[TEXTURE_RESULT_FIREWORK_3].size = { 483.0f, 471.0f };
	g_td[TEXTURE_RESULT_FIREWORK_3].pos = {-300.0f , -100.0f };
	g_td[TEXTURE_RESULT_FIREWORK_3].scl = { 0.0f, 0.0f };

	g_td[TEXTURE_RESULT_FIREWORK_4].size = { 610.0f, 613.0f };
	g_td[TEXTURE_RESULT_FIREWORK_4].pos = { 400.0f , 300.0f };
	g_td[TEXTURE_RESULT_FIREWORK_4].scl = { 0.0f, 0.0f };

	g_td[TEXTURE_RESULT_FIREWORK_5].size = { 368.0f, 349.0f };
	g_td[TEXTURE_RESULT_FIREWORK_5].pos = { -600.0f , -250.0f };
	g_td[TEXTURE_RESULT_FIREWORK_5].scl = { 0.0f, 0.0f };


	//ResultTime = GetTime();
	ResultTime = 15.0f;

	DeliveryDistance = DeliveryDistanceTbl[GetStageNo()];
	DeliveryTime = (int)(10000.0f / ResultTime);
	DeliveryFee = 3000;	
	if (ResultTime <= 30) {Tip = 500;};
	Damage = (5000 - (int)GetFuel());
	if (Damage > 3000)
	{
		Damage = 3000;
	}

	TotalAmount = (DeliveryDistance + DeliveryTime + DeliveryFee + Tip - Damage);

	g_Time = 0;

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitResult(void)
{
	if (g_Load == FALSE) return;

	g_Load = FALSE;
}

void SetPosAddY(float addY)
{
	g_td[TEXTURE_RESULT_NO_STARS].posAdd.y = addY;
	g_td[TEXTURE_RESULT_GREEN_STARS].posAdd.y = addY;
	g_td[TEXTURE_RESULT_EXPRESSION_1].posAdd.y = addY;
	g_td[TEXTURE_RESULT_EXPRESSION_2].posAdd.y = addY;
	g_td[TEXTURE_RESULT_EXPRESSION_3].posAdd.y = addY;

	g_td[TEXTURE_RESULT_DELIVERY_DISTAMCE].posAdd.y = addY;
	g_td[TEXTURE_RESULT_DELIVERY_TIME].posAdd.y = addY;
	g_td[TEXTURE_RESULT_DELIVERY_FEE].posAdd.y = addY;
	g_td[TEXTURE_RESULT_TIP].posAdd.y = addY;
	g_td[TEXTURE_RESULT_DAMAGE].posAdd.y = addY;
	g_td[TEXTURE_RESULT_TOTAL_AMOUNT].pos.y = addY;

	g_td[TEXTURE_RESULT_YEN].posAdd.y = addY;
	g_td[TEXTURE_RESULT_MIUS_YEN].posAdd.y = addY;
	g_td[TEXTURE_RESULT_TOTAL_YEN].posAdd.y = addY;
	g_td[TEXTURE_RESULT_NUMBER].posAdd.y = addY;
	g_td[TEXTURE_RESULT_MIUS_NUMBER].posAdd.y = addY;
	g_td[TEXTURE_RESULT_TOTAL_NUMBER].posAdd.y = addY;
	g_td[TEXTURE_RESULT_POINT].posAdd.y = addY;
	g_td[TEXTURE_RESULT_MIUS_POINT].posAdd.y = addY;
	g_td[TEXTURE_RESULT_TOTAL_POINT].posAdd.y = addY;

	g_td[TEXTURE_RESULT_FIREWORK_1].posAdd.y = addY;
	g_td[TEXTURE_RESULT_FIREWORK_2].posAdd.y = addY;
	g_td[TEXTURE_RESULT_FIREWORK_3].posAdd.y = addY;
	g_td[TEXTURE_RESULT_FIREWORK_4].posAdd.y = addY;
	g_td[TEXTURE_RESULT_FIREWORK_5].posAdd.y = addY;
}
//=============================================================================
// 更新処理
//=============================================================================
void UpdateResult(void)
{
	static float posAddY = 0.0f;
	if (EventFinish)
	{//スクロールする
		if (GetKeyboardPress(DIK_DOWN))
		{
			if (g_td[TEXTURE_RESULT_BG].uv_pos.v <= 0.45f)
			{
				g_td[TEXTURE_RESULT_BG].uv_pos.v += 0.01f;
				g_td[TEXTURE_RESULT_SCROLL].posAdd.y += 8.4f;
				posAddY -= MOVE;
				SetPosAddY(posAddY);
			}
		}
		if (GetKeyboardPress(DIK_UP))
		{
			if (g_td[TEXTURE_RESULT_BG].uv_pos.v > 0.0f)
			{
				g_td[TEXTURE_RESULT_BG].uv_pos.v -= 0.01f;
				g_td[TEXTURE_RESULT_SCROLL].posAdd.y -= 8.4f;
				posAddY += MOVE;
				SetPosAddY(posAddY);
			}
		}

		//時間で星数を決定
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

		//花火
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


		if (CheckButtonSkip()) {
			SetFade(FADE_OUT, MODE_HOME, TRUE);
		}
	}
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawNumber(int number, int dotTex, int numberTex, int yenTex)//, int commaTex)
{
	// プレイヤーのお金の描画処理
	int money = number;
	int count = 0;
	int comma = 0;
	while (money > 0)
	{
		if (count == 2)
		{
			g_td[dotTex].pos.x = g_td[numberTex].pos.x - (count - comma) * g_td[numberTex].size.x - comma * g_td[numberTex].size.x * 0.5f + g_td[numberTex].size.x * 0.25f;
			DrawTexture2D(&g_td[dotTex]);
			count++;
			comma++;
		}
		//else if (count == 6 || count == 10 || count == 14)
		//{
		//	g_td[UI_WALLET_COMMA].pos.x = g_td[UI_WALLET_NUMBER].pos.x - (count - comma) * g_td[UI_WALLET_NUMBER].size.x - comma * g_td[UI_WALLET_COMMA].size.x;
		//	DrawTexture2D(&g_td[UI_WALLET_COMMA]);
		//	count++;
		//	comma++;
		//}
		g_td[numberTex].uv_pos.u = (float)(money % 10) * 0.1f;
		g_td[numberTex].posAdd.x = -(count - comma) * g_td[numberTex].size.x - comma * g_td[numberTex].size.x * 0.5f;
		DrawTexture2D(&g_td[numberTex]);
		money /= 10;
		count++;
	}
	while (count < 3)
	{
		if (count == 2)
		{
			g_td[dotTex].pos.x = g_td[numberTex].pos.x - (count - comma) * g_td[numberTex].size.x - comma * g_td[numberTex].size.x * 0.5f + g_td[numberTex].size.x * 0.25f;
			DrawTexture2D(&g_td[dotTex]);
			count++;
			comma++;
		}

		g_td[numberTex].uv_pos.u = 0.0f;
		g_td[numberTex].posAdd.x = -(count - comma) * g_td[numberTex].size.x - comma * g_td[numberTex].size.x * 0.5f;
		DrawTexture2D(&g_td[numberTex]);
		money /= 10;
		count++;
	}
	g_td[yenTex].pos.x = g_td[numberTex].pos.x - (count - comma - 1) * g_td[numberTex].size.x - comma * g_td[numberTex].size.x * 0.5f - g_td[yenTex].size.x * 0.5f - g_td[numberTex].size.x * 0.5f;
	DrawTexture2D(&g_td[yenTex]);
}
void DrawResult(void)
{
	if (EventFinish)
	{
		SetDraw2DTexture();

		// 背景描画
		DrawTexture2D(&g_td[TEXTURE_RESULT_BG], FALSE, TRUE);

		// 最大星数描画
		for (int i = 0; i < STAR_MAX; i++)
		{
			DrawTexture2D(&g_td[TEXTURE_RESULT_NO_STARS]);
			g_td[TEXTURE_RESULT_NO_STARS].pos.x = (-(g_td[TEXTURE_RESULT_NO_STARS].size.x + 50.0f) * 2) + ((g_td[TEXTURE_RESULT_NO_STARS].size.x + 50.0f) * i);
			//																				↑星の間隔調整
		}

		// 獲得した星描画
		for (int i = 0; i < GreenStarNum; i++)
		{
			if (g_Time >= (i + 1) * 40)
			{
				DrawTexture2D(&g_td[TEXTURE_RESULT_GREEN_STARS]);
				g_td[TEXTURE_RESULT_GREEN_STARS].pos.x = (-(g_td[TEXTURE_RESULT_GREEN_STARS].size.x + 50.0f) * 2) + ((g_td[TEXTURE_RESULT_GREEN_STARS].size.x + 50.0f) * i);
				//																				↑星の間隔調整
			}
		}

		//表情描画
		if (ResultTime <= 30.0f)
		{
			DrawTexture2D(&g_td[TEXTURE_RESULT_EXPRESSION_1]);
		}
		else if (ResultTime <= 60.0f)
		{
			DrawTexture2D(&g_td[TEXTURE_RESULT_EXPRESSION_2]);
		}
		else
		{
			DrawTexture2D(&g_td[TEXTURE_RESULT_EXPRESSION_3]);
		}

		//スクロール描画
		DrawTexture2D(&g_td[TEXTURE_RESULT_SCROLL_BAR]);
		DrawTexture2D(&g_td[TEXTURE_RESULT_SCROLL]);

		//項目描画
		DrawTexture2D(&g_td[TEXTURE_RESULT_DELIVERY_DISTAMCE]);
		DrawTexture2D(&g_td[TEXTURE_RESULT_DELIVERY_TIME]);
		DrawTexture2D(&g_td[TEXTURE_RESULT_DELIVERY_FEE]);
		DrawTexture2D(&g_td[TEXTURE_RESULT_TIP]);
		DrawTexture2D(&g_td[TEXTURE_RESULT_DAMAGE]);
		DrawTexture2D(&g_td[TEXTURE_RESULT_TOTAL_AMOUNT]);

		//円マーク描画
		//for (int i = 0; i < YEN_MAX; i++)
		//{
		//	g_td[TEXTURE_RESULT_YEN].pos.y = (g_td[TEXTURE_RESULT_DELIVERY_DISTAMCE + i].pos.y);
		//	DrawTexture2D(&g_td[TEXTURE_RESULT_YEN]);
		//}
		//DrawTexture2D(&g_td[TEXTURE_RESULT_MIUS_YEN]);
		//DrawTexture2D(&g_td[TEXTURE_RESULT_TOTAL_YEN]);

		//値段描画
		for (int i = 0; i < YEN_MAX; i++)
		{
			int number[YEN_MAX] = { DeliveryDistance, DeliveryTime, DeliveryFee, Tip };
			int numberlen[YEN_MAX] = { Len(DeliveryDistance), Len(DeliveryTime), Len(DeliveryFee), Len(Tip) };

			g_td[TEXTURE_RESULT_NUMBER].pos.y = (g_td[TEXTURE_RESULT_DELIVERY_DISTAMCE + i].pos.y);

			g_td[TEXTURE_RESULT_YEN].pos.y = (g_td[TEXTURE_RESULT_DELIVERY_DISTAMCE + i].pos.y);
			g_td[TEXTURE_RESULT_POINT].pos.y = (g_td[TEXTURE_RESULT_DELIVERY_DISTAMCE + i].pos.y + 18.0f);

			DrawNumber(number[i], TEXTURE_RESULT_POINT, TEXTURE_RESULT_NUMBER, TEXTURE_RESULT_YEN);
			//for (int j = 0; j < numberlen[i]; j++)
			//{
			//	g_td[TEXTURE_RESULT_NUMBER].uv_pos.u = (number[i] % 10 * 0.1f);
			//	g_td[TEXTURE_RESULT_NUMBER].pos.x = (YEN_FINAL_POS - (g_td[TEXTURE_RESULT_NUMBER].size.x * j));
			//	DrawTexture2D(&g_td[TEXTURE_RESULT_NUMBER], FALSE, TRUE);
			//	number[i] /= 10;
			//}

			//g_td[TEXTURE_RESULT_YEN].pos.y = (g_td[TEXTURE_RESULT_DELIVERY_DISTAMCE + i].pos.y);
			//g_td[TEXTURE_RESULT_YEN].pos.x = (YEN_FINAL_POS - (g_td[TEXTURE_RESULT_NUMBER].size.x * numberlen[i]));
			//DrawTexture2D(&g_td[TEXTURE_RESULT_YEN]);
		}

		// ダメージペナルティ
		DrawNumber(Damage, TEXTURE_RESULT_MIUS_POINT, TEXTURE_RESULT_MIUS_NUMBER, TEXTURE_RESULT_MIUS_YEN);
		//int DamageNumber = Damage;
		//for (int i = 0; i < Len(Damage); i++)
		//{
		//	g_td[TEXTURE_RESULT_MIUS_NUMBER].uv_pos.u = (DamageNumber % 10 * 0.1f);
		//	g_td[TEXTURE_RESULT_MIUS_NUMBER].pos.x = (YEN_FINAL_POS - (g_td[TEXTURE_RESULT_MIUS_NUMBER].size.x * i));
		//	DrawTexture2D(&g_td[TEXTURE_RESULT_MIUS_NUMBER], FALSE, TRUE);
		//	DamageNumber /= 10;
		//}
		//g_td[TEXTURE_RESULT_MIUS_YEN].pos.x = (YEN_FINAL_POS - (g_td[TEXTURE_RESULT_MIUS_NUMBER].size.x * Len(Damage)));
		//DrawTexture2D(&g_td[TEXTURE_RESULT_MIUS_YEN]);

		// 総収入
		DrawNumber(TotalAmount, TEXTURE_RESULT_TOTAL_POINT, TEXTURE_RESULT_TOTAL_NUMBER, TEXTURE_RESULT_TOTAL_YEN);
		//int TotalNumber = TotalAmount;
		//for (int i = 0; i < Len(TotalAmount); i++)
		//{
		//	g_td[TEXTURE_RESULT_TOTAL_NUMBER].uv_pos.u = (TotalNumber % 10 * 0.1f);
		//	g_td[TEXTURE_RESULT_TOTAL_NUMBER].pos.x = (YEN_FINAL_POS - (g_td[TEXTURE_RESULT_TOTAL_NUMBER].size.x * i) - 5.0f);
		//	DrawTexture2D(&g_td[TEXTURE_RESULT_TOTAL_NUMBER], FALSE, TRUE);
		//	TotalNumber /= 10;
		//}
		//g_td[TEXTURE_RESULT_TOTAL_YEN].pos.x = -g_td[TEXTURE_RESULT_TOTAL_YEN].size.x * 0.5f - g_td[TEXTURE_RESULT_MIUS_NUMBER].size.x * 0.05f + (YEN_FINAL_POS - ((g_td[TEXTURE_RESULT_MIUS_NUMBER].size.x * 0.1f) * (Len(TotalAmount) - 1)));
		//DrawTexture2D(&g_td[TEXTURE_RESULT_TOTAL_YEN]);

		// 小数点
		//for (int i = 0; i < YEN_MAX; i++)
		//{
		//	g_td[TEXTURE_RESULT_POINT].pos.y = (g_td[TEXTURE_RESULT_DELIVERY_DISTAMCE + i].pos.y + 18.0f);

		//	DrawTexture2D(&g_td[TEXTURE_RESULT_POINT]);
		//}
		//DrawTexture2D(&g_td[TEXTURE_RESULT_MIUS_POINT]);
		//DrawTexture2D(&g_td[TEXTURE_RESULT_TOTAL_POINT]);

		//for (int i = 0; i < ZERO_DIGIT; i++)
		//{
		//	for (int j = 0; j < YEN_MAX; j++)
		//	{
		//		g_td[TEXTURE_RESULT_NUMBER].pos.x = (YEN_FINAL_POS + g_td[TEXTURE_RESULT_POINT].size.x + (g_td[TEXTURE_RESULT_NUMBER].size.x * (i + 1)));
		//		g_td[TEXTURE_RESULT_NUMBER].pos.y = (g_td[TEXTURE_RESULT_DELIVERY_DISTAMCE + j].pos.y);
		//		g_td[TEXTURE_RESULT_NUMBER].uv_pos.u = 0.0f;
		//		DrawTexture2D(&g_td[TEXTURE_RESULT_NUMBER], FALSE, TRUE);
		//	}
		//	g_td[TEXTURE_RESULT_MIUS_NUMBER].pos.x = (YEN_FINAL_POS + g_td[TEXTURE_RESULT_MIUS_POINT].size.x + (g_td[TEXTURE_RESULT_MIUS_NUMBER].size.x * (i + 1)));
		//	g_td[TEXTURE_RESULT_MIUS_NUMBER].uv_pos.u = 0.0f;
		//	DrawTexture2D(&g_td[TEXTURE_RESULT_MIUS_NUMBER], FALSE, TRUE);

		//	g_td[TEXTURE_RESULT_TOTAL_NUMBER].pos.x = (YEN_FINAL_POS + g_td[TEXTURE_RESULT_TOTAL_POINT].size.x + (g_td[TEXTURE_RESULT_TOTAL_NUMBER].size.x * (i + 1)));
		//	g_td[TEXTURE_RESULT_TOTAL_NUMBER].uv_pos.u = 0.0f;
		//	DrawTexture2D(&g_td[TEXTURE_RESULT_TOTAL_NUMBER], FALSE, TRUE);
		//}

		//花火描画
		DrawTexture2D(&g_td[TEXTURE_RESULT_FIREWORK_1]);
		DrawTexture2D(&g_td[TEXTURE_RESULT_FIREWORK_2]);
		DrawTexture2D(&g_td[TEXTURE_RESULT_FIREWORK_3]);
		DrawTexture2D(&g_td[TEXTURE_RESULT_FIREWORK_4]);
		DrawTexture2D(&g_td[TEXTURE_RESULT_FIREWORK_5]);


		//最後に上のバー描画
		DrawTexture2D(&g_td[TEXTURE_RESULT_BAR]);
	}

}

void SetEventFinish(void)
{
	EventFinish = true;
}


//桁数チェック
int Len(int num)
{
	int ans = 0;
	while (num != 0) {
		num /= 10;
		ans++;
	}
	//値がゼロの場合は1桁
	if (ans == 0)
	{
		ans = 1;
	}
	return ans;
}

void ResetResult(void)
{
	ResultTime = 0.0f;
	GreenStarNum = 0;
	DeliveryDistance = 0;
	DeliveryTime = 0;
	DeliveryFee = 0;
	Tip = 0;
	Damage = 0;
	TotalAmount = 0;
	EventFinish = FALSE;
	g_Time = 0;

	g_td[TEXTURE_RESULT_BG].uv_pos.v = 0.0f;
	g_td[TEXTURE_RESULT_SCROLL].posAdd.y = 0.0f;
	SetPosAddY(0.0f);

	ResetResultEvent();
}

void SetDeliveryTime(float time)
{
	DeliveryTime = (int)((1.0f - (time / GetBaseDeliveryTime())) * DeliveryFee);
	if (DeliveryTime < 0) DeliveryTime = 0;
}
void SetResultDistance(int distance)
{
	DeliveryDistance = distance * 100;
}
void SetDeliveryFee(int fee)
{
	DeliveryFee = fee;
}
void SetDeliveryTip(int tip)
{
	Tip = tip * DeliveryFee / 5;
}
void SetDeliveryDamage(int damage)
{
	Damage = damage * DeliveryFee / 5;
}
void SetDeliveryResult(void)
{
	TotalAmount = (DeliveryDistance + DeliveryTime + DeliveryFee + Tip - Damage);
	GainMoney((ULONG)TotalAmount);
}