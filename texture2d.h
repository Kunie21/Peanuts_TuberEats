//=============================================================================
//
// 2D画像描画処理 [texture2D.h]
// Author : 國江 翔太
//
//=============================================================================
#pragma once

// 2D画像描画用データ
struct TEXTURE2D_DESC
{
	UV_POSITION uv_pos = { 0.0f, 0.0f, 1.0f, 1.0f };	// UV座標
	XMFLOAT4 col = { 1.0f, 1.0f, 1.0f, 1.0f };			// 色
	XMFLOAT4 sd_col = { 0.0f, 0.0f, 0.0f, 0.5f };		// 影色
	XMFLOAT2 sd_pos = { 5.0f, 5.0f };					// 影位置（ずらす量）
	XMFLOAT2 size = { SCREEN_WIDTH, SCREEN_HEIGHT };	// サイズ
	XMFLOAT2 pos = { 0.0f, 0.0f };						// 位置
	XMFLOAT2 scl = { 1.0f, 1.0f };						// 拡大率
	float rot = 0.0f;									// 回転
	CENTER_TYPE ctrType = CENTER_CENTER;				// 中心の種類
	POSITION_TYPE posType = POSITION_RELATIVE;			// 位置の種類
	ID3D11ShaderResourceView** tex = NULL ;				// テクスチャ
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitTexture2D(void);
void UninitTexture2D(void);
void UpdateTexture2D(void);
void DrawTexture2D(TEXTURE2D_DESC* td, BOOL bShadow = FALSE, BOOL bUV = FALSE);
void SetUVTexture2D(UV_POSITION* uv);
void ResetUVTexture2D(void);