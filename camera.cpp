//=============================================================================
//
// カメラ処理 [camera.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "input.h"
#include "debugproc.h"
#include "camera.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define DEFAULT_POS		(-310.0f)

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static XMMATRIX				g_CameraViewMatrix;
static XMMATRIX				g_CameraInvViewMatrix;
static XMMATRIX				g_CameraProjectionMatrix;
struct CAMERA_DESC
{
	XMFLOAT3 pos;
	XMFLOAT3 rot;
	XMFLOAT3 at;
	XMFLOAT3 up;
	float len;
};
static CAMERA_DESC			g_Camera;

//=============================================================================
// 初期化処理
//=============================================================================
void InitCamera(void)
{
	g_Camera.pos = XMFLOAT3(0.0f, 0.0f, DEFAULT_POS);
	g_Camera.rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
	g_Camera.at = XMFLOAT3(0.0f, 0.0f, 1.0f);
	g_Camera.up = XMFLOAT3(0.0f, 1.0f, 0.0f);

	// 視点と注視点の距離を計算
	float vx, vz;
	vx = g_Camera.pos.x - g_Camera.at.x;
	vz = g_Camera.pos.z - g_Camera.at.z;
	g_Camera.len = sqrtf(vx * vx + vz * vz);

	//SetCameraPosition(&g_CameraPosition);

	CAMERA c;
	c.Position = Float4(g_Camera.pos);
	c.ViewVolume = XMFLOAT4(SCREEN_WIDTH, SCREEN_HEIGHT, VIEW_NEAR_Z, VIEW_FAR_Z);
	SetCameraBuffer(&c);
}

//=============================================================================
// カメラの終了処理
//=============================================================================
void UninitCamera(void) {}

//=============================================================================
// カメラの更新処理
//=============================================================================
void UpdateCamera(void)
{
#ifdef _DEBUG

	if (GetKeyboardPress(DIK_Z))
	{// 視点旋回「左」
		g_Camera.rot.y += VALUE_ROTATE_CAMERA;
		if (g_Camera.rot.y > XM_PI)
		{
			g_Camera.rot.y -= XM_PI * 2.0f;
		}

		g_Camera.pos.x = g_Camera.at.x - sinf(g_Camera.rot.y) * g_Camera.len;
		g_Camera.pos.z = g_Camera.at.z - cosf(g_Camera.rot.y) * g_Camera.len;
	}

	if (GetKeyboardPress(DIK_C))
	{// 視点旋回「右」
		g_Camera.rot.y -= VALUE_ROTATE_CAMERA;
		if (g_Camera.rot.y < -XM_PI)
		{
			g_Camera.rot.y += XM_PI * 2.0f;
		}

		g_Camera.pos.x = g_Camera.at.x - sinf(g_Camera.rot.y) * g_Camera.len;
		g_Camera.pos.z = g_Camera.at.z - cosf(g_Camera.rot.y) * g_Camera.len;
	}

	if (GetKeyboardPress(DIK_Y))
	{// 視点移動「上」
		g_Camera.pos.y += VALUE_MOVE_CAMERA;
	}

	if (GetKeyboardPress(DIK_N))
	{// 視点移動「下」
		g_Camera.pos.y -= VALUE_MOVE_CAMERA;
	}

	if (GetKeyboardPress(DIK_Q))
	{// 注視点旋回「左」
		g_Camera.rot.y -= VALUE_ROTATE_CAMERA;
		if (g_Camera.rot.y < -XM_PI)
		{
			g_Camera.rot.y += XM_PI * 2.0f;
		}

		g_Camera.at.x = g_Camera.pos.x + sinf(g_Camera.rot.y) * g_Camera.len;
		g_Camera.at.z = g_Camera.pos.z + cosf(g_Camera.rot.y) * g_Camera.len;
	}

	if (GetKeyboardPress(DIK_E))
	{// 注視点旋回「右」
		g_Camera.rot.y += VALUE_ROTATE_CAMERA;
		if (g_Camera.rot.y > XM_PI)
		{
			g_Camera.rot.y -= XM_PI * 2.0f;
		}

		g_Camera.at.x = g_Camera.pos.x + sinf(g_Camera.rot.y) * g_Camera.len;
		g_Camera.at.z = g_Camera.pos.z + cosf(g_Camera.rot.y) * g_Camera.len;
	}

	if (GetKeyboardPress(DIK_T))
	{// 注視点移動「上」
		g_Camera.at.y += VALUE_MOVE_CAMERA;
	}

	if (GetKeyboardPress(DIK_B))
	{// 注視点移動「下」
		g_Camera.at.y -= VALUE_MOVE_CAMERA;
	}

	if (GetKeyboardPress(DIK_U))
	{// 近づく
		g_Camera.len -= VALUE_MOVE_CAMERA;
		g_Camera.pos.x = g_Camera.at.x - sinf(g_Camera.rot.y) * g_Camera.len;
		g_Camera.pos.z = g_Camera.at.z - cosf(g_Camera.rot.y) * g_Camera.len;
	}

	if (GetKeyboardPress(DIK_M))
	{// 離れる
		g_Camera.len += VALUE_MOVE_CAMERA;
		g_Camera.pos.x = g_Camera.at.x - sinf(g_Camera.rot.y) * g_Camera.len;
		g_Camera.pos.z = g_Camera.at.z - cosf(g_Camera.rot.y) * g_Camera.len;
	}

	// カメラを初期に戻す
	if (GetKeyboardPress(DIK_R))
	{
		UninitCamera();
		InitCamera();
	}

	if (GetKeyboardPress(DIK_UP))
	{// 近づく
		g_Camera.pos.z += 50.0f;
		g_Camera.at.z += 50.0f;
	}
	if (GetKeyboardPress(DIK_DOWN))
	{// 離れる
		g_Camera.pos.z -= 50.0f;
		g_Camera.at.z -= 50.0f;
	}
	if (GetKeyboardPress(DIK_RIGHT))
	{// 離れる
		g_Camera.pos.x += 50.0f;
		g_Camera.at.x += 50.0f;
	}
	if (GetKeyboardPress(DIK_LEFT))
	{// 近づく
		g_Camera.pos.x -= 50.0f;
		g_Camera.at.x -= 50.0f;
	}

	//PrintDebugProc("Camera:ZC QE TB YN UM R\n");
#endif
}

//=============================================================================
// カメラの更新
//=============================================================================
void SetCamera(void) 
{
	// ビュー行列設定
	g_CameraViewMatrix = XMMatrixLookAtLH(XMLoadFloat3(&g_Camera.pos), XMLoadFloat3(&g_Camera.at), XMLoadFloat3(&g_Camera.up));
	SetViewBuffer(&g_CameraViewMatrix);

	// プロジェクション行列設定
	g_CameraProjectionMatrix = XMMatrixPerspectiveFovLH(VIEW_ANGLE, VIEW_ASPECT, VIEW_NEAR_Z, VIEW_FAR_Z);
	SetProjectionBuffer(&g_CameraProjectionMatrix);
}
