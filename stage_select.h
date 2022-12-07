//=============================================================================
//
//  [StageSelect.h]
// Author : 
//
//=============================================================================
#pragma once

//*****************************************************************************
// 
//*****************************************************************************
HRESULT InitStageSelect(void);
void UninitStageSelect(void);
void UpdateStageSelect(void);
void DrawStageSelect(void);

void DownColChange(int a, int b, int c);
void UpColChange(int a, int b);
void PannelPlus(void);
//void PressedAnyButton(void);
int GetStageNumber(void);


