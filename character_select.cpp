//=============================================================================
//
// チームロゴ画面処理 [character.cpp]
// Author : 葉志成
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "fade.h"
#include "texture2d.h"
#include "input.h"
#include "character_select.h"
#include "rocket_select.h"
#include "player.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH)	// 背景サイズ横
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)	// 背景サイズ縦

#define TEXTURE_WIDTH_CHARACTER		(505.0f)			// キャラクターサイズ横
#define TEXTURE_HEIGHT_CHARACTER	(880.0f)			// キャラクターサイズ縦
#define TEXTURE_POS_CHARACTER		{1240.0f,60.0f}		// キャラクター座標


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static BOOL				g_Load = FALSE;

// テクスチャ管理
enum
{
	TEXTURE_CHARACTER_01 = 0,
	TEXTURE_CHARACTER_02,
	TEXTURE_CHARACTER_03,
	TEXTURE_CHARACTER_LAST,
	TEXTURE_START_BUTTON_1,
	TEXTURE_START_BUTTON_2,
	TEXTURE_START_BUTTON_3,
	TEXTURE_MAX,
};
static TEXTURE2D_DESC	g_td[TEXTURE_MAX];

static int g_TexNoChar;		//キャラクターのテキスチャー番号
static int g_TexNoButton;	//ボタンのテキスチャー番号

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitCharacterSelect(void)
{
	// テクスチャ生成
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		//D3DX11CreateShaderResourceViewFromFile(GetDevice(), g_TextureName[i], NULL, NULL, &g_Texture[i], NULL);
		g_td[i].tex = (TEXTURE_LABEL)(TEXTURE_LABEL_CHARACTER_01 + i);
	}

	// 詳細設定
	for (int i = TEXTURE_CHARACTER_01; i < TEXTURE_MAX; i++)
	{
		g_td[i].size = { TEXTURE_WIDTH_CHARACTER, TEXTURE_HEIGHT_CHARACTER };
		g_td[i].pos = TEXTURE_POS_CHARACTER;
		g_td[i].ctrType = CENTER_LEFTTOP;
		g_td[i].posType = POSITION_ABSOLUTE;
	}
	g_td[TEXTURE_START_BUTTON_1].size = { 738.0f, 79.0f };
	g_td[TEXTURE_START_BUTTON_1].pos = { 1120.0f,980.0f };
	g_td[TEXTURE_START_BUTTON_2].size = { 822.0f,163.0f };
	g_td[TEXTURE_START_BUTTON_2].pos = { 1080.0f,940.0f };
	g_td[TEXTURE_START_BUTTON_3].size = { 822.0f,163.0f };
	g_td[TEXTURE_START_BUTTON_3].pos = { 1080.0f,940.0f };

	g_TexNoChar = TEXTURE_CHARACTER_01; //SaveDataから最後に選択したキャラクターにする予定

	g_TexNoButton = TEXTURE_START_BUTTON_1;

	g_Load = TRUE;
	return S_OK;

}

//=============================================================================
// 終了処理
//=============================================================================
void UninitCharacterSelect(void)
{
	if (g_Load == FALSE) return;

	g_Load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateCharacterSelect(void)
{
	if (!IsRocketSelectFinished())return;

	if (GetKeyboardTrigger(DIK_A))
	{
		g_TexNoChar = g_TexNoChar-- > TEXTURE_CHARACTER_01 ?
		g_TexNoChar : TEXTURE_CHARACTER_01;
	}
	else if (GetKeyboardTrigger(DIK_D))
	{
		g_TexNoChar = g_TexNoChar++ < TEXTURE_CHARACTER_LAST ?
		g_TexNoChar : TEXTURE_CHARACTER_LAST;
	}
	else if (GetKeyboardTrigger(DIK_RETURN))
	{
		g_TexNoButton = TEXTURE_START_BUTTON_3;
	}
	else if (GetKeyboardRelease(DIK_RETURN))
	{
		g_TexNoButton = TEXTURE_START_BUTTON_2;

		SetMode(MODE_STAGESELECT); // game
		ResetPlayer();
	}
	else if (GetKeyboardRelease(DIK_BACK))
	{
		SetMode(MODE_TITLE_START); // game
	}

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawCharacterSelect(void)
{
	if (!IsRocketSelectFinished())return;

	DrawTexture2D(&g_td[g_TexNoChar]);
	DrawTexture2D(&g_td[g_TexNoButton]);
}