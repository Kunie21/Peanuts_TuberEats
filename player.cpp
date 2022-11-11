//=============================================================================
//
// プレイヤー処理 [player.cpp]
// Author : 國江 翔太
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "texture2d.h"
#include "debugproc.h"
#include "model.h"
#include "fade.h"
#include "input.h"
#include "player.h"
#include "tube.h"
#include "ui_game.h"
#include "gimmick.h"
#include "stage.h"
#include "missile.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define DEFAULT_SPEED	(40.0f)
//#define DEFAULT_POS		(310.0f)
#define MAX_SPEED		(70.0f)

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static BOOL				g_Load = FALSE;

// テクスチャ管理
enum {
	TEXTURE_TEAMLOGO = 0,
	//TEXTURE_STAR,
	TEXTURE_MAX,
};
static TEXTURE2D_DESC	g_td[TEXTURE_MAX];
static ID3D11ShaderResourceView*	g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報
static char*	g_TextureName[TEXTURE_MAX] = {
	"data/TEXTURE/blueberry_.png",
	//"data/MODEL/star1.jpg",
};

enum {
	MODEL_ROCKET1 = 0,
	MODEL_ROCKET2,
	MODEL_ROCKET3,
	MODEL_ROCKET4,
	MODEL_ROCKET5,
	MODEL_FIRE,
	MODEL_MAX,
};
static MODEL_DATA	g_Model[MODEL_MAX];	// プレイヤーのモデル管理


static float		g_Rotation = 0.0f;
static float		g_TestAddSpeed = 0.0f;

class ROCKET
{
private:
	float m_pos = 0;
	float m_posSpd = DEFAULT_SPEED;
	float m_addSpd = 0.0f;

	float m_rot = 0.0f;
	float m_rotSpd = 0.0f;
	const float c_rotSpdMax = 0.05f;

	float m_fuel = 5000.0f;
	const float c_fuelMax = 5000.0f;

	float m_invTime = 0.0f;

	int m_missiles = 10;

public:
	//ROCKET() {}
	//ROCKET(const ROCKET& rocket) {
	//	m_pos = rocket.m_pos;
	//	m_posSpd = rocket.m_posSpd;
	//	m_rot = rocket.m_rot;
	//	m_rotSpd = rocket.m_rotSpd;
	//}

	ROCKET operator=(const ROCKET& rocket) {
		m_pos = rocket.m_pos;
		m_posSpd = rocket.m_posSpd;
		m_rot = rocket.m_rot;
		m_rotSpd = rocket.m_rotSpd;
		return *this;
	}

	void Rotate(float rotSpd) {
		m_rotSpd += rotSpd;
		if (m_rotSpd > c_rotSpdMax) m_rotSpd = c_rotSpdMax;
		if (m_rotSpd < -c_rotSpdMax) m_rotSpd = -c_rotSpdMax;
	}
	void Accel(float posSpd) { m_posSpd += posSpd; }
	void Boost(float addSpd) { m_addSpd += addSpd; }
	void Brake(float posSpd) { m_posSpd -= posSpd; }
	void Drive(void) {
		m_pos += m_posSpd + m_addSpd;
		m_rot += m_rotSpd;
		while (m_rot < 0.0f) m_rot += XM_2PI;
		while (m_rot > XM_2PI) m_rot -= XM_2PI;
		m_rotSpd *= 0.98f;
		m_addSpd *= 0.98f;
		m_invTime *= 0.98f;
		m_fuel -= m_posSpd / MAX_SPEED;
	}
	void LostFuel(float lostFuel) { m_fuel -= lostFuel; }
	void Collision(void) { m_invTime = 1.5f; }

	bool AbleToCollision(void) const { if (m_invTime < 1.0f) return true; return false; }
	float GetPos(void) const { return m_pos; }
	float GetSpeed(void) const { return m_posSpd + m_addSpd; }
	float GetSpeedRate(void) const { return GetSpeed() / MAX_SPEED; }
	float GetRotate(void) const { return m_rot; }
	float GetFuel(void) const { return m_fuel; }
	float GetFuelRate(void) const { return m_fuel / c_fuelMax; }
	bool Launch(MISSILE_TYPE type) {
		//if (!m_missiles--)
		//	return false;
		return LaunchMissile(type, 0.0f, m_posSpd, -m_rot + XM_PI, m_rotSpd); }
};

static ROCKET g_Rocket;
static int testNo = 0;

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitPlayer(void)
{
	// テクスチャ生成
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		D3DX11CreateShaderResourceViewFromFile(GetDevice(), g_TextureName[i], NULL, NULL, &g_Texture[i], NULL);
		g_td[i].tex = &g_Texture[i];
	}

	//for (int i = 0; i < MODEL_MAX; i++)
	{
		//LoadModel("data/MODEL/earth01.obj", &g_Model[0].model);
		LoadModel("data/MODEL/rocket01.obj", &g_Model[0].model);
		LoadModel("data/MODEL/rocket02.obj", &g_Model[1].model);
		LoadModel("data/MODEL/rocket03.obj", &g_Model[2].model);
		LoadModel("data/MODEL/rocket04.obj", &g_Model[3].model);
		LoadModel("data/MODEL/rocket05.obj", &g_Model[4].model);
		LoadModel("data/MODEL/fire01.obj", &g_Model[5].model);
		for (int i = 0; i < MODEL_MAX; i++) {
			g_Model[i].srt.pos = { 0.0f, -60.0f, 0.0f };
			g_Model[i].srt.rot = { XM_PI, 0.0f, XM_PI };
			g_Model[i].srt.scl = { 0.3f, 0.3f, 0.3f };
		}
		g_Model[MODEL_FIRE].srt.pos.z = -30.0f;
	}

	// 詳細設定
	//g_td[TEXTURE_TEAMLOGO].tex = &g_Texture[TEXTURE_TEAMLOGO];

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitPlayer(void)
{
	if (g_Load == FALSE) return;

	for (int i = 0; i < TEXTURE_MAX; i++) {
		if (g_Texture[i]) {
			g_Texture[i]->Release();
			g_Texture[i] = NULL;
		}
	}

	for (int i = 0; i < MODEL_MAX; i++) { UnloadModel(&g_Model[i].model); }

	g_Load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdatePlayer(void)
{
	// ロケットの種類変更
	if (GetKeyboardPress(DIK_1)) { testNo = 0; }
	if (GetKeyboardPress(DIK_2)) { testNo = 1; }
	if (GetKeyboardPress(DIK_3)) { testNo = 2; }
	if (GetKeyboardPress(DIK_4)) { testNo = 3; }
	if (GetKeyboardPress(DIK_5)) { testNo = 4; }

	// ロケットの状態を保存
	static ROCKET oldRocket;
	oldRocket = g_Rocket;

	// 回転
	if (GetKeyboardPress(DIK_A)) { g_Rocket.Rotate(0.002f); }
	if (GetKeyboardPress(DIK_D)) { g_Rocket.Rotate(-0.002f); }
	SetAfterRotation(&XMMatrixRotationRollPitchYaw(0.0f, 0.0f, g_Rocket.GetRotate()));

	// アクセル
	if (GetKeyboardTrigger(DIK_SPACE)) { g_Rocket.Accel(5.0f); }
	if (GetKeyboardTrigger(DIK_BACK)) { g_Rocket.Brake(5.0f); }

	// ドライブ
	g_Rocket.Drive();

	// ミサイル
	if (GetKeyboardTrigger(DIK_RETURN)) { g_Rocket.Launch(MISSILE_TYPE_ICE); }
	if (GetKeyboardTrigger(DIK_S)) { g_Rocket.Launch(MISSILE_TYPE_RING); }

	// コリジョン
	if (g_Rocket.AbleToCollision()){
		CollisionGimmick(0, oldRocket.GetPos(), g_Rocket.GetPos(), oldRocket.GetRotate(), g_Rocket.GetRotate());
		//SetDamageEffect();
	}


	// パイプ曲げ（手動）
	static CURVE_BUFFER curve;
	if (GetKeyboardPress(DIK_F)) { curve.Angle.y += 0.005f; }
	if (GetKeyboardPress(DIK_G)) { curve.Angle.y -= 0.005f; }
	if (GetKeyboardPress(DIK_H)) { curve.Angle.x += 0.005f; }
	if (GetKeyboardPress(DIK_J)) { curve.Angle.x -= 0.005f; }
	curve.TexPos = g_Rocket.GetPos() / MESH_SIZE_Z;
	SetCurveBuffer(&curve);
	
	// パイプ曲げ（ステージ設定に従って自動で曲げる）
	//SetStageCurve(0, g_Rocket.GetPos());

	// GPU_TIME
	static int time = 0;
	SetFrameTime(time++);
	SetMapPosition(g_Rocket.GetPos() / ((float)GetStage(0)->length * MESH_SIZE_Z));
	SetSpeedMeter(g_Rocket.GetSpeedRate());
	SetFuelMeter(g_Rocket.GetFuelRate());

#ifdef _DEBUG	// デバッグ情報を表示する
	PrintDebugProc("ThroughMeshs:%d\n", g_Rocket.GetPos() / (int)MESH_SIZE_Z);
	PrintDebugProc("ThroughTubes:%d\n", g_Rocket.GetPos() / (int)TUBE_SIZE);
	PrintDebugProc("Time(sec):%f\n", GetTime());
	PrintDebugProc("RocketSpeed:%f\n", g_Rocket.GetSpeed());
	PrintDebugProc("CurveAngleX:%f\n", curve.Angle.x);
	PrintDebugProc("CurveAngleY:%f\n", curve.Angle.y);
#endif
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawPlayer(void) {
	SetCullingMode(CULL_MODE_NONE);
	MATERIAL material;
	DrawModel(&g_Model[testNo].model, &g_Model[testNo].srt, NULL, &material);	// モデル描画
	SetCullingMode(CULL_MODE_BACK);
}
void DrawFire(void) {
	g_Model[MODEL_FIRE].srt.scl.x = (float)(rand() % 10) * 0.003f + 0.3f * g_Rocket.GetSpeedRate();
	g_Model[MODEL_FIRE].srt.scl.y = (float)(rand() % 10) * 0.003f + 0.3f * g_Rocket.GetSpeedRate();
	g_Model[MODEL_FIRE].srt.scl.z = (float)(rand() % 10) * 0.003f + 0.3f * g_Rocket.GetSpeedRate();
	MATERIAL material;
	material.Shininess = 1.0f;
	material.Diffuse.w = 1.0f;
	DrawModel(&g_Model[MODEL_FIRE].model, &g_Model[MODEL_FIRE].srt, NULL, &material);	// モデル描画
}

float GetPlayerSpeed(void) {
	return g_Rocket.GetSpeed();
}
float GetPlayerPosition(void) {
	return g_Rocket.GetPos();
}
float GetPlayerRotation(void) {
	return g_Rocket.GetRotate();
}
void SetPlayerThroughRing(void) {
	g_Rocket.Boost(30.0f);
	g_Rocket.Collision();
}
void SetPlayerCollisionIce(void) {
	g_Rocket.LostFuel(500.0f);
	g_Rocket.Boost(-60.0f);
	g_Rocket.Collision();
}