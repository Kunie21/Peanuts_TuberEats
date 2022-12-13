//=============================================================================
//
// パイプライン壁の処理 [tube.h]
// Author : 國江 翔太
//
//=============================================================================
#pragma once

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	TUBE_RADIUS		(200.0f)	// 断面の半径
#define	MESH_NUM_X		(8)			// 断面の角数
#define	MESH_NUM_X2		(36)			// 断面の角数
#define	MESH_SIZE_X		(TUBE_RADIUS * 2.0f * XM_PI / MESH_NUM_X2)	// マスの1辺の長さ
#define	MESH_SIZE_Z		(200.0f)	// マスの1辺の長さ
#define	MESH_NUM_Z		(150)		// 奥行きのマス目
#define	TUBE_SIZE		(MESH_SIZE_Z * MESH_NUM_Z)	// 1本の長さ
//#define	CURVE_RADIUS	(TUBE_SIZE * 0.5f)
//#define	CURVE_CENTER	(CURVE_RADIUS * sinf(XM_PIDIV4))

#define TUBE_TEX_X (2.0f)
#define TUBE_TEX_Z (10.0f)

#define DRAW_DIST_NEAR	(-1000.0f)
#define DRAW_DIST_FAR	(15000.0f)

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitTube(void);
void UninitTube(void);
void UpdateTube(void);
void DrawTube(void);
void DrawTubeLight(void);
void DrawTubeLight2(void);
void RotateTube(float rot);
//void MoveTube(float move);
float GetTubeRotation(void);
//void SetTubeCurveRight(int tube);
//void TestCurveTube(float move);
//void PresentTube(void);
//void SetNextTube(void);

void DrawTubeResult(float pos_z);
