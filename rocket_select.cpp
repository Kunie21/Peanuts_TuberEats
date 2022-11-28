//=============================================================================
//
// チームロゴ画面処理 [rocket_select.cpp]
// Author : 葉志成
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
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH)	// 背景サイズ横
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)	// 背景サイズ縦

#define SIZE_SHOP_MENU		{ 1001.0f, 1080.0f }		// キャラクターサイズ縦
#define POS_SHOP_MENU		{ 919.0f,0.0f }		// キャラクターサイズ縦
#define POS_STATUSBAR		{ 210.0f, 70.0f }		// キャラクターサイズ縦
#define POS_STATUSBARPOINT		{ 212.5f, 72.7f }		// キャラクターサイズ縦
#define DISTANCE_STATUSBAR_Y			(76.0f)	// キャラクターサイズ縦
#define DISTANCE_STATUSBARPOINT_X		(79.2f)	// キャラクターサイズ縦

#define COL_BLACK		{0.0f,0.0f,0.0f,1.0f}	// 黒い色
#define COL_ORIGINAL	{1.0f,1.0f,1.0f,1.0f}	// 元の色

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void DrawTextureStatus(void);

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static BOOL				g_Load = FALSE;


// テクスチャ管理
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
	TEXTURE_DISPLAY_ICON_01, //テキスチャーがない、
	TEXTURE_DISPLAY_ICON_02, //単にロケットのアイコンの表示で使う
	TEXTURE_DISPLAY_ICON_03,
	TEXTURE_DISPLAY_ICON_04,
	TEXTURE_MAX,
};

static TEXTURE2D_DESC	g_td[TEXTURE_MAX];
//static ID3D11ShaderResourceView*	g_Texture[TEXTURE_DISPLAY_ICON_01] = { NULL };	// テクスチャ情報
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

static MODEL_DATA	g_ModelStage;	// 表示モデルの管理
static MODEL_DATA	g_ModelDisplay;	// 表示モデルの管理
static MODEL_DATA	g_ModelShader;	// 表示モデルの管理
static int			g_RocketEquip;  // 装備中のロケットアイコンのテキスチャー番号
static int			g_ModelNo_Rocket;  // 選択しているロケットのモデル番号
static int			g_SelectBar;	   // セレクトバールのテキスチャー番号
static BOOL			g_IsSelectFinished; // ロケットを選んだら、次のステップに進む

enum {
	STATUS_LOCK = 0,
	STATUS_NEW,
	STATUS_NORMAL,
	STATUS_EQUIP
};

// セレクト画面専用のロケット構造体
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

static SELECTROCKET   g_Rocket[MODEL_MAX];	// 全モデルの管理足すロケット情報

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitRocketSelect(void)
{
	// テクスチャ生成
	for (int i = 0; i < TEXTURE_DISPLAY_ICON_01; i++)
	{
		//D3DX11CreateShaderResourceViewFromFile(GetDevice(), g_TextureName[i], NULL, NULL, &g_Texture[i], NULL);
		g_td[i].tex = (TEXTURE_LABEL)(TEXTURE_LABEL_BG + i);
	}

	// テクスチャ詳細設定
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


	// モデル生成
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

	// 表示モデルの設定
	g_ModelDisplay.srt.pos = { -100.0f, -20.0f, 0.0f };
	g_ModelDisplay.srt.rot = { 0.45f, -1.6f, 0.03f };
	g_ModelDisplay.srt.scl = { 0.5f, 0.5f, 0.5f };

	// ステージモデルの設定
	g_ModelStage.srt.pos = { -100.0f, -100.0f, 0.0f };
	g_ModelStage.srt.rot = { 0.0f, -1.5f, -0.22f };
	g_ModelStage.srt.scl = { 9.0f, 6.9f, 9.0f };

	// ロケット情報の設定
	g_Rocket[MODEL_SELECT_ROCKET_01].speed = 3;
	g_Rocket[MODEL_SELECT_ROCKET_01].accelerate = 4;
	g_Rocket[MODEL_SELECT_ROCKET_01].control = 10;
	g_Rocket[MODEL_SELECT_ROCKET_01].fuel = 5;
	g_Rocket[MODEL_SELECT_ROCKET_01].price = 100000;
	g_Rocket[MODEL_SELECT_ROCKET_01].status = STATUS_EQUIP;// SavaDataで設定する予定
		
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

	// ロケットアイコンの設定
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

	// 最初の時、選択するロケットの設定
	g_ModelNo_Rocket = g_RocketEquip - TEXTURE_DISPLAY_ICON_01;
	g_SelectBar = g_ModelNo_Rocket + TEXTURE_ROCKETSELECT_1;
	g_td[g_SelectBar -4].col = COL_BLACK;

	g_IsSelectFinished = FALSE;

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
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
// 更新処理
//=============================================================================
void UpdateRocketSelect(void)
{
	// ロケット選択画面に戻る
	if (GetKeyboardTrigger(DIK_6))
	{
		g_IsSelectFinished = FALSE;
	}

	// 取引中ロケットの更新をやめる
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

	// ロケットの設定完了
	if (g_IsSelectFinished)return;


	if (GetKeyboardTrigger(DIK_W)) 
	{
		//g_ModelNo_Rocket = g_ModelNo_Rocket-- > MODEL_ROCKET_01 ?
		//	g_ModelNo_Rocket : MODEL_ROCKET_01;
		//g_SelectBar = TEXTURE_ROCKETSELECT_1 + g_ModelNo_Rocket;
		//g_td[g_SelectBar - 3].col = COL_ORIGINAL;// 元の白色の字に戻る
		//g_td[g_SelectBar - 4].col = COL_BLACK;// バールと同じ場所のロケットの名前の字を黒くする

		g_ModelNo_Rocket--;
		g_SelectBar = TEXTURE_ROCKETSELECT_1 + g_ModelNo_Rocket;
		g_td[g_SelectBar - 3].col = COL_ORIGINAL;// 元の白色の字に戻る
		g_td[g_SelectBar - 4].col = COL_BLACK;// バールと同じ場所のロケットの名前の字を黒くする
		if (g_ModelNo_Rocket < MODEL_SELECT_ROCKET_01)
		{
			g_ModelNo_Rocket = MODEL_SELECT_ROCKET_04;
			g_SelectBar = TEXTURE_ROCKETSELECT_4;
			g_td[TEXTURE_ROCKET_4].col = COL_BLACK;// バールと同じ場所のロケットの名前の字を黒くする
			g_td[TEXTURE_ROCKET_ICON_EQUIP].col = COL_ORIGINAL;// バールと同じ場所のロケットの名前の字を黒くする
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
			g_td[TEXTURE_ROCKET_1].col = COL_BLACK;// バールと同じ場所のロケットの名前の字を黒くする
			g_td[TEXTURE_ROCKETSELECT_1].col = COL_ORIGINAL;// バールと同じ場所のロケットの名前の字を黒くする
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

	// 選択中のロケットはロックしている場合、値段を表示する
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
// 描画処理
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

	// モデル描画
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
	// スピードの描画処理
	g_td[TEXTURE_STATUSBAR].pos = POS_STATUSBAR;
	DrawTexture2D(&g_td[TEXTURE_STATUSBAR]);
	g_td[TEXTURE_STATUSBAR_POINT].pos = POS_STATUSBARPOINT;
	for (int i = 0; i < g_Rocket[g_ModelNo_Rocket].speed; i++)
	{
		DrawTexture2D(&g_td[TEXTURE_STATUSBAR_POINT]);
		g_td[TEXTURE_STATUSBAR_POINT].pos.x += DISTANCE_STATUSBARPOINT_X;
	}

	// 加速の描画処理
	g_td[TEXTURE_STATUSBAR].pos.y += DISTANCE_STATUSBAR_Y;
	DrawTexture2D(&g_td[TEXTURE_STATUSBAR]);
	g_td[TEXTURE_STATUSBAR_POINT].pos = { 212.5f, g_td[TEXTURE_STATUSBAR_POINT].pos.y + DISTANCE_STATUSBAR_Y };
	for (int i = 0; i < g_Rocket[g_ModelNo_Rocket].accelerate; i++)
	{
		DrawTexture2D(&g_td[TEXTURE_STATUSBAR_POINT]);
		g_td[TEXTURE_STATUSBAR_POINT].pos.x += DISTANCE_STATUSBARPOINT_X;
	}

	// 操作の描画処理
	g_td[TEXTURE_STATUSBAR].pos.y += DISTANCE_STATUSBAR_Y;
	DrawTexture2D(&g_td[TEXTURE_STATUSBAR]);
	g_td[TEXTURE_STATUSBAR_POINT].pos = { 212.5f, g_td[TEXTURE_STATUSBAR_POINT].pos.y + DISTANCE_STATUSBAR_Y };
	for (int i = 0; i < g_Rocket[g_ModelNo_Rocket].control; i++)
	{
		DrawTexture2D(&g_td[TEXTURE_STATUSBAR_POINT]);
		g_td[TEXTURE_STATUSBAR_POINT].pos.x += DISTANCE_STATUSBARPOINT_X;
	}

	// 燃料の描画処理
	g_td[TEXTURE_STATUSBAR].pos.y += DISTANCE_STATUSBAR_Y;
	DrawTexture2D(&g_td[TEXTURE_STATUSBAR]);
	g_td[TEXTURE_STATUSBAR_POINT].pos = { 212.5f, g_td[TEXTURE_STATUSBAR_POINT].pos.y + DISTANCE_STATUSBAR_Y };
	for (int i = 0; i < g_Rocket[g_ModelNo_Rocket].fuel; i++)
	{
		DrawTexture2D(&g_td[TEXTURE_STATUSBAR_POINT]);
		g_td[TEXTURE_STATUSBAR_POINT].pos.x += DISTANCE_STATUSBARPOINT_X;
	}
}
