//=============================================================================
//
// カメラ処理 [camera.h]
// Author : 國江 翔太
//
//=============================================================================
#pragma once

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	VALUE_MOVE_CAMERA	(2.0f)			// カメラの移動量
#define	VALUE_ROTATE_CAMERA	(XM_PI * 0.01f)	// カメラの回転量

#define	VIEW_ANGLE		(XMConvertToRadians(45.0f))						// ビュー平面の視野角
#define	VIEW_ASPECT		((float)SCREEN_WIDTH / (float)SCREEN_HEIGHT)	// ビュー平面のアスペクト比	
#define	VIEW_NEAR_Z		(10.0f)											// ビュー平面のNearZ値
#define	VIEW_FAR_Z		(20000.0f)										// ビュー平面のFarZ値

#define CAMERA_POS_Z	(-310.0f)

struct CAMERA_DESC
{
	XMFLOAT3 pos;
	XMFLOAT3 rot;
	XMFLOAT3 at;
	XMFLOAT3 up;
	float len;
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void InitCamera(void);
void UninitCamera(void);
void UpdateCamera(void);
void SetCamera(void);
CAMERA_DESC* GetCamera(void);