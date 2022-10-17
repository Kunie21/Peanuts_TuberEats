//=============================================================================
//
// ライト処理 [light.h]
// Author : 
//
//=============================================================================
#pragma once
#include "renderer.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void InitLight(void);
void UpdateLight(void);

void SetLightData(int index, LIGHT *light);
void SetFogData(FOG *fog);
LIGHT *GetLightData(int index);
void SetMainLightView(BOOL isPers);
XMFLOAT3 SetMainLightDirectionFromPos(void);

void SetFogColor(XMFLOAT4 color);
void SetFogStartAndEnd(float start, float end);