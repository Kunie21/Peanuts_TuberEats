//=============================================================================
//
// フェード処理 [fade.h]
// Author : 國江 翔太
//
//=============================================================================
#pragma once

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	FADE_COLOR		{0.0f, 0.0f, 0.0f}

// フェードの状態
typedef enum
{
	FADE_NONE = 0,		// 何もない状態
	FADE_IN,			// フェードイン処理
	FADE_OUT,			// フェードアウト処理
	FADE_MAX
} FADE;

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitFade(void);
void UninitFade(void);
void UpdateFade(void);
void DrawFade(void);

void SetFade(FADE fade, MODE_LABEL modeNext, BOOL load = FALSE);
void SetFadeColor(XMFLOAT3 color = FADE_COLOR);
FADE GetFade(void);

BOOL CheckFadeIn(void);
BOOL CheckFadeOut(void);


