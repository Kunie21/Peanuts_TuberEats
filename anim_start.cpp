//=============================================================================
//
// スタートアニメーション処理 [anim_start.cpp]
// Author : 國江 翔太
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "input.h"
#include "anim_start.h"
#include "camera.h"
#include "player.h"
#include "gate.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define RADIUS			(220.0f)	// 回り込みの円の半径
#define ROUND_CENTER_Z	(RADIUS + CAMERA_POS_Z)	// 回り込みの円の中心
#define BOTTOM_Y		(-10.0f)	// ドリーアウト時に下に下がる量
#define ZOOM_Z			(-50.0f)	// ドリーアウト時に後ろに下がる量

// カットの種類
enum START_ANIM_CUT_LABEL {
	SAC_DOLLY_OUT = 0,		// ドリーアウト（後ろに下がる）
	SAC_ROUND_CRANE_TILT,	// 回り込み＆クレーンで高さをパイプ中央に移動＆ティルトで視線をまっすぐに向ける
};
#define SAC_DO_TIME		(120.0f)	// SAC_DOLLY_OUTカットの全体時間
#define SAC_RCT_TIME	(180.0f)	// SAC_ROUND_CRANE_TILTカットの全体時間

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static BOOL		g_Load = FALSE;
static BOOL		g_Use = FALSE;
static XMFLOAT3 g_CameraPos, g_CameraAt, g_CameraUp;

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitAnimStart(void)
{
	g_CameraPos = { 0.0f, ROCKET_Y, RADIUS + ROUND_CENTER_Z };
	g_CameraAt = { 0.0f, ROCKET_Y, 0.0f };
	g_CameraUp = { 0.0f, 1.0f, 0.0f };

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitAnimStart(void)
{
	if (g_Load == FALSE) return;
	
	g_Load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateAnimStart(void)
{
#ifdef _DEBUG
	if (GetKeyboardTrigger(DIK_L)) { SetCameraAnimStart(); }
#endif

	if (!g_Use) return;
	static float time = 0.0f;
	static START_ANIM_CUT_LABEL animNo = SAC_DOLLY_OUT;
	float timeRate, timeAngle;

	if (GetKeyboardTrigger(DIK_RETURN)) {
		time = 0.0f;
		animNo = SAC_DOLLY_OUT;
		g_Use = FALSE;
		SetStartGate();
		timeRate = 1.0f;
		timeAngle = timeRate * XM_PI;
		g_CameraAt = { 0.0f, ROCKET_Y * (1.0f - timeRate), 0.0f };
		g_CameraPos = {
			-RADIUS * sinf(timeAngle),
			(ROCKET_Y + BOTTOM_Y) * (1.0f - timeRate),
			RADIUS * cosf(timeAngle) + ROUND_CENTER_Z
		};
		return;
	}

	switch (animNo)
	{
	case SAC_DOLLY_OUT:
		if (time > SAC_DO_TIME) {
			time = 0.0f; animNo = SAC_ROUND_CRANE_TILT; break;
		}
		timeRate = time / SAC_DO_TIME;
		g_CameraAt = { 0.0f, ROCKET_Y, 0.0f };
		g_CameraPos = {
			0.0f,
			ROCKET_Y + BOTTOM_Y * timeRate,
			RADIUS + ROUND_CENTER_Z + ZOOM_Z * (1.0f - timeRate)
		};
		break;

	case SAC_ROUND_CRANE_TILT:
		if (time > SAC_RCT_TIME) {	// カメラワークの終了
			time = 0.0f; animNo = SAC_DOLLY_OUT; g_Use = FALSE; SetStartGate(); break;
		}
		timeRate = time / SAC_RCT_TIME;
		timeAngle = timeRate * XM_PI;
		g_CameraAt = { 0.0f, ROCKET_Y * (1.0f - timeRate), 0.0f };
		g_CameraPos = {
			-RADIUS * sinf(timeAngle),
			(ROCKET_Y + BOTTOM_Y) * (1.0f - timeRate),
			RADIUS * cosf(timeAngle) + ROUND_CENTER_Z
		};
		break;

	//case SAC_TILT:
	//	if (time > SAC_TILT_TIME) {
	//		time = 0.0f; animNo = SAC_FULL; g_Use = FALSE; break;
	//	}
	//	//timeRate = time / SAC_TILT_TIME;
	//	//g_CameraAt = { 0.0f, ROCKET_Y * (1.0f - timeRate), 0.0f };
	//	//g_CameraPos = { 0.0f, 0.0f, -RADIUS + ROUND_CENTER_Z };
	//	break;

	default:
		break;
	}
	time += 1.0f;
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawAnimStart(void)
{
	if (!g_Use) return;
	SetViewBuffer(&XMMatrixLookAtLH(XMLoadFloat3(&g_CameraPos), XMLoadFloat3(&g_CameraAt), XMLoadFloat3(&g_CameraUp)));
}

void SetCameraAnimStart(void) {
	g_Use = TRUE;
}