//=============================================================================
//
// パーティクル処理 [particle.h]
// Author : 國江翔太
//
//=============================================================================
#pragma once

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitParticle(void);
void UninitParticle(void);
void UpdateParticle(void);
void DrawParticle(void);

BOOL SetEmitter(int zPosNo, int rotPosNo);
BOOL SetEmitterExp(int zPosNo, int rotPosNo);