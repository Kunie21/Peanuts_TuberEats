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
#define DEFAULT_SPEED	(50.0f)
//#define DEFAULT_POS		(310.0f)
#define MAX_SPEED		(150.0f)
#define DEFAULT_DECEL	(0.98f)
#define BLAST_DECEL	(0.98f)
#define RING_DECEL	(0.985f)
#define RING_BOOST	(40.0f)

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static BOOL				g_Load = FALSE;

enum {
	MODEL_PLAYER_ROCKET1 = 0,
	MODEL_PLAYER_ROCKET2,
	MODEL_PLAYER_ROCKET3,
	MODEL_PLAYER_ROCKET4,
	MODEL_PLAYER_ROCKET5,
	MODEL_PLAYER_FIRE,
	MODEL_PLAYER_MAX,
};
static MODEL_DATA	g_Model[MODEL_PLAYER_MAX];	// プレイヤーのモデル管理

struct ROCKET_STATUS
{
	// 速度
	float	posSpdMax = 60.0f;	// 最大速度

	// 加速
	float	accel = 0.4f;		// 加速度
	float	decel = 0.98f;		// 減速率（ブレーキ）

	// 操作
	float	rotSpd = 0.002f;	// 左右移動速度
	float	rotSpdMax = 0.05f;	// 最大左右移動速度
	float	rotDecel = 0.98f;	// 左右移動減速率

	// 燃料
	float	fuelMax = 5000.0f;	// 燃料

	// ミサイル数
	int		missiles = 0;
};
ROCKET_STATUS g_RS[MODEL_PLAYER_MAX];

static float		g_Rotation = 0.0f;
static float		g_TestAddSpeed = 0.0f;

class ROCKET
{
private:
	ROCKET_STATUS m_status;

	float m_pos = 0;
	float m_posSpd = 0.0f; // DEFAULT_SPEED;
	float m_addSpd = 0.0f;

	float m_rot = 0.0f;
	float m_rotSpd = 0.0f;
	float m_rotAddSpd = 0.0f;
	float c_rotSpdMax = 0.05f;

	float m_fuel = 5000.0f;
	float c_fuelMax = 5000.0f;

	float m_invTime = 0.0f;

	int m_missiles = 5;

	BOOL m_bStart = FALSE;

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

	// 回転
	void Rotate(float rotSpd) {
		m_rotSpd += rotSpd;
		if (m_rotSpd > c_rotSpdMax) m_rotSpd = c_rotSpdMax;
		if (m_rotSpd < -c_rotSpdMax) m_rotSpd = -c_rotSpdMax;
	}
	void RotateLeft(void) {
		m_rotSpd += m_status.rotSpd;
		if (m_rotSpd > m_status.rotSpdMax) m_rotSpd = m_status.rotSpdMax;
	}
	void RotateRight(void) {
		m_rotSpd -= m_status.rotSpd;
		if (m_rotSpd < -m_status.rotSpdMax) m_rotSpd = -m_status.rotSpdMax;
	}
	void RotDecel(void) { m_rotSpd *= m_status.rotDecel; }
	void Blast(float rotAddSpd) {
		m_rotAddSpd += rotAddSpd;
	}

	// 加速
	void Accel(float posSpd) { m_posSpd += posSpd; }
	void Accel(void) { m_posSpd = min(m_posSpd + m_status.accel, m_status.posSpdMax); }
	void Decel(void) { m_posSpd *= m_status.decel; }

	// ブースト（リング）
	void Boost(float addSpd) { m_addSpd += addSpd; }
	void RingBoost(void) { m_addSpd += RING_BOOST; }
	void Start(void) { m_bStart = TRUE; }

	// ブレーキ
	void Brake(float posSpd) { m_posSpd -= posSpd; }
	void BrakeFull(void) { m_posSpd = 0.0f; }
	void Back(float pos) { m_pos -= pos; }

	// 運転（全般の処理）
	float Drive(void) {
		m_pos += m_posSpd + m_addSpd;
		m_rot += m_rotSpd + m_rotAddSpd;
		while (m_rot < 0.0f) m_rot += XM_2PI;
		while (m_rot > XM_2PI) m_rot -= XM_2PI;
		m_rotSpd *= DEFAULT_DECEL;
		m_rotAddSpd *= DEFAULT_DECEL;
		m_addSpd *= RING_DECEL;
		m_invTime *= DEFAULT_DECEL;
		m_fuel -= m_posSpd / MAX_SPEED;
		return -(m_rotSpd + m_rotAddSpd) / c_rotSpdMax * XM_PIDIV4 * 0.5f + XM_PI;
	}
	float Drive2(void) {
		if (m_bStart) m_pos += m_posSpd + m_addSpd;
		m_rot += m_rotSpd + m_rotAddSpd;
		while (m_rot < 0.0f) m_rot += XM_2PI;
		while (m_rot > XM_2PI) m_rot -= XM_2PI;
		//m_rotAddSpd *= BLAST_DECEL;
		m_rotAddSpd *= m_status.rotDecel;
		//m_addSpd *= RING_DECEL;
		m_addSpd *= m_status.decel;
		m_invTime *= 0.0f;
		m_fuel -= m_posSpd / MAX_SPEED;	// 燃料消費
		if (GetFuelRate() < 0.25f) SetAlertRedEffect();
		else if (GetFuelRate() < 0.5f) SetAlertYellowEffect();
		return -(m_rotSpd + m_rotAddSpd) / m_status.rotSpdMax * XM_PIDIV4 * 0.5f + XM_PI;
	}

	// 燃料ロスト
	void LostFuel(float lostFuel) { m_fuel -= lostFuel; }

	// コリジョン
	void Collision(void) { m_invTime = 1.5f; }
	bool AbleToCollision(void) const { if (m_invTime < 1.0f) return true; return false; }
	
	// データ取得
	float GetPos(void) const { return m_pos; }
	float GetSpeed(void) const { return m_posSpd + m_addSpd; }
	float GetSpeedRate(void) const {
		if (GetSpeed() > 0.0f) return sqrtf(GetSpeed() / m_status.posSpdMax);
		return 0.0f;
	}
	float GetRotate(void) const { return m_rot; }
	float GetFuel(void) const { return m_fuel; }
	float GetFuelRate(void) const { return m_fuel / c_fuelMax; }

	// ミサイル発射
	int GetMissiles(void) { return m_missiles; }
	void SetMissiles(void) { m_missiles++; }
	bool Launch(void) {
		if (m_missiles <= 0) return false;
		m_missiles--;
		return LaunchMissile(MISSILE_TYPE_01, 0.0f, m_posSpd, -m_rot + XM_PI, m_rotSpd);
	}

	// リセット
	void Reset(void) {
		m_pos = 0;
		m_posSpd = 0.0f;
		m_addSpd = 0.0f;
		m_rot = 0.0f;
		m_rotSpd = 0.0f;
		m_rotAddSpd = 0.0f;
		m_fuel = 5000.0f;
		m_invTime = 0.0f;
		m_missiles = 5;
		m_bStart = FALSE;
	}

	// ロケットのステータスをセット
	void SetStatus(ROCKET_STATUS& rs) {
		m_status = rs;
	}
};

static ROCKET g_Rocket;
static int testNo = 0;

static CURVE_BUFFER curveTest;

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitPlayer(void)
{
	g_Model[MODEL_PLAYER_ROCKET1].model = MODEL_ROCKET1;
	g_Model[MODEL_PLAYER_ROCKET2].model = MODEL_ROCKET2;
	g_Model[MODEL_PLAYER_ROCKET3].model = MODEL_ROCKET3;
	g_Model[MODEL_PLAYER_ROCKET4].model = MODEL_ROCKET4;
	g_Model[MODEL_PLAYER_ROCKET5].model = MODEL_ROCKET5;
	g_Model[MODEL_PLAYER_FIRE].model = MODEL_FIRE;
	for (int i = 0; i < MODEL_PLAYER_MAX; i++) {
		g_Model[i].srt.pos = { 0.0f, ROCKET_Y, 0.0f };
		g_Model[i].srt.rot = { XM_PI, 0.0f, XM_PI };
		g_Model[i].srt.scl = { 0.3f, 0.3f, 0.3f };
	}
	g_Model[MODEL_PLAYER_FIRE].srt.pos.z = -30.0f;


	g_RS[MODEL_PLAYER_ROCKET2].posSpdMax = 80.0f;
	g_RS[MODEL_PLAYER_ROCKET2].accel = 0.6f;
	g_RS[MODEL_PLAYER_ROCKET2].decel = 0.98f;
	g_RS[MODEL_PLAYER_ROCKET2].rotSpd = 0.003f;
	g_RS[MODEL_PLAYER_ROCKET2].rotSpdMax = 0.05f;
	g_RS[MODEL_PLAYER_ROCKET2].rotDecel = 0.98f;
	g_RS[MODEL_PLAYER_ROCKET2].fuelMax = 5000.0f;
	g_RS[MODEL_PLAYER_ROCKET2].missiles = 0;

	g_RS[MODEL_PLAYER_ROCKET3].posSpdMax = 200.0f;
	g_RS[MODEL_PLAYER_ROCKET3].accel = 0.2f;
	g_RS[MODEL_PLAYER_ROCKET3].decel = 0.99f;
	g_RS[MODEL_PLAYER_ROCKET3].rotSpd = 0.001f;
	g_RS[MODEL_PLAYER_ROCKET3].rotSpdMax = 0.03f;
	g_RS[MODEL_PLAYER_ROCKET3].rotDecel = 0.99f;
	g_RS[MODEL_PLAYER_ROCKET3].fuelMax = 5000.0f;
	g_RS[MODEL_PLAYER_ROCKET3].missiles = 0;

	g_RS[MODEL_PLAYER_ROCKET4].posSpdMax = 110.0f;
	g_RS[MODEL_PLAYER_ROCKET4].accel = 0.8f;
	g_RS[MODEL_PLAYER_ROCKET4].decel = 0.98f;
	g_RS[MODEL_PLAYER_ROCKET4].rotSpd = 0.004f;
	g_RS[MODEL_PLAYER_ROCKET4].rotSpdMax = 0.08f;
	g_RS[MODEL_PLAYER_ROCKET4].rotDecel = 0.98f;
	g_RS[MODEL_PLAYER_ROCKET4].fuelMax = 5000.0f;
	g_RS[MODEL_PLAYER_ROCKET4].missiles = 0;

	g_RS[MODEL_PLAYER_ROCKET5].posSpdMax = 200.0f;
	g_RS[MODEL_PLAYER_ROCKET5].accel = 5.0f;
	g_RS[MODEL_PLAYER_ROCKET5].decel = 0.5f;
	g_RS[MODEL_PLAYER_ROCKET5].rotSpd = 0.004f;
	g_RS[MODEL_PLAYER_ROCKET5].rotSpdMax = 0.1f;
	g_RS[MODEL_PLAYER_ROCKET5].rotDecel = 0.5f;
	g_RS[MODEL_PLAYER_ROCKET5].fuelMax = 5000.0f;
	g_RS[MODEL_PLAYER_ROCKET5].missiles = 0;

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitPlayer(void)
{
	if (g_Load == FALSE) return;

	g_Load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdatePlayer(void)
{
	// ロケットの種類変更
	if (GetKeyboardPress(DIK_1)) { testNo = 0; g_Rocket.SetStatus(g_RS[testNo]); }
	if (GetKeyboardPress(DIK_2)) { testNo = 1; g_Rocket.SetStatus(g_RS[testNo]); }
	if (GetKeyboardPress(DIK_3)) { testNo = 2; g_Rocket.SetStatus(g_RS[testNo]); }
	if (GetKeyboardPress(DIK_4)) { testNo = 3; g_Rocket.SetStatus(g_RS[testNo]); }
	if (GetKeyboardPress(DIK_5)) { testNo = 4; g_Rocket.SetStatus(g_RS[testNo]); }

	// ロケットの状態を保存
	static ROCKET oldRocket;
	oldRocket = g_Rocket;

	// 回転
	//if (GetKeyboardPress(DIK_A)) { g_Rocket.Rotate(0.002f); }
	//if (GetKeyboardPress(DIK_D)) { g_Rocket.Rotate(-0.002f); }
	if (GetKeyboardPress(DIK_A)) { g_Rocket.RotateLeft(); }
	else if (GetKeyboardPress(DIK_D)) { g_Rocket.RotateRight(); }
	else { g_Rocket.RotDecel(); }
	SetAfterRotation(&XMMatrixRotationRollPitchYaw(0.0f, 0.0f, g_Rocket.GetRotate()));

	// アクセル
	//if (GetKeyboardTrigger(DIK_SPACE)) { g_Rocket.Accel(5.0f); }
	if (GetKeyboardPress(DIK_SPACE)) { g_Rocket.Accel(); }
	else { g_Rocket.Decel(); }
	if (GetKeyboardTrigger(DIK_BACK)) { g_Rocket.Brake(5.0f); }

	// ドライブ
	//g_Model[testNo].srt.rot.z = g_Rocket.Drive();
	g_Model[testNo].srt.rot.z = g_Rocket.Drive2();

	// ミサイル
	//if (GetKeyboardTrigger(DIK_RETURN)) { g_Rocket.Launch(MISSILE_TYPE_01); }
	if (GetKeyboardTrigger(DIK_S)) { g_Rocket.Launch(); }

	// コリジョン
	if (g_Rocket.AbleToCollision()) {
		CollisionGimmick(oldRocket.GetPos(), g_Rocket.GetPos(), oldRocket.GetRotate(), g_Rocket.GetRotate());
		//SetDamageEffect();
	}

	// ゴール判定
	if (CheckGoal(oldRocket.GetPos(), g_Rocket.GetPos())) {
		OffTimer();
		SetFade(FADE_OUT, MODE_RESULT);
	}

	// パイプ曲げ（手動）
	//if (GetKeyboardPress(DIK_F)) { curveTest.Angle.y += 0.005f; }
	//if (GetKeyboardPress(DIK_G)) { curveTest.Angle.y -= 0.005f; }
	//if (GetKeyboardPress(DIK_H)) { curveTest.Angle.x += 0.005f; }
	//if (GetKeyboardPress(DIK_J)) { curveTest.Angle.x -= 0.005f; }
	////curveTest.TexPos = g_Rocket.GetPos() / MESH_SIZE_Z / MESH_NUM_Z;
	//curveTest.TexPos = g_Rocket.GetPos() / MESH_SIZE_Z;
	//curveTest.Spd = g_Rocket.GetSpeed();
	//SetCurveBuffer(&curveTest);
	
	// パイプ曲げ（ステージ設定に従って自動で曲げる）
	//SetStageCurve(STAGE_OSAKA, g_Rocket.GetPos(), g_Rocket.GetSpeed());

	// GPU_TIME
	static int time = 0;
	SetFrameTime(time++);
	SetMapPosition(g_Rocket.GetPos() / GetStage2()->goal);
	SetSpeedMeter(g_Rocket.GetSpeedRate());
	SetFuelMeter(g_Rocket.GetFuelRate());

//#ifdef _DEBUG
//	static LARGE_INTEGER Shadow_S, Shadow_E;
//	static int oldTime, nowTime;
//	nowTime++;
//	if (nowTime - oldTime >= 20) { QueryPerformanceCounter(&Shadow_S); }
//#endif
//
//#ifdef _DEBUG
//	if (nowTime - oldTime >= 20) { QueryPerformanceCounter(&Shadow_E); }
//	if (nowTime - oldTime >= 20) oldTime = nowTime;
//	PrintDebugProc("GameUpdateTime:%d\n", Shadow_E.QuadPart - Shadow_S.QuadPart);
//#endif

#ifdef _DEBUG	// デバッグ情報を表示する
	//PrintDebugProc("ThroughMeshs:%d\n", g_Rocket.GetPos() / (int)MESH_SIZE_Z);
	//PrintDebugProc("ThroughTubes:%d\n", g_Rocket.GetPos() / (int)TUBE_SIZE);
	//PrintDebugProc("Time(sec):%f\n", GetTime());
	//PrintDebugProc("RocketSpeed:%f\n", g_Rocket.GetSpeed());
	//PrintDebugProc("CurveAngleX:%f\n", curveTest.Angle.x);
	//PrintDebugProc("CurveAngleY:%f\n", curveTest.Angle.y);
#endif
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawPlayer(void) {
	SetCullingMode(CULL_MODE_NONE);

	//g_Model[testNo].srt.scl.x = (float)(rand() % 50) * 0.0003f * g_Rocket.GetSpeedRate() + 0.3f;
	//g_Model[testNo].srt.scl.y = (float)(rand() % 50) * 0.0003f * g_Rocket.GetSpeedRate() + 0.3f;
	//g_Model[testNo].srt.scl.z = (float)(rand() % 50) * 0.0003f * g_Rocket.GetSpeedRate() + 0.3f;

	//g_Model[testNo].srt.pos.x = (float)(rand() % 10) * 0.001f * g_Rocket.GetSpeed();
	g_Model[testNo].srt.pos.y = (float)(rand() % 20) * 0.0125f + ROCKET_Y;
	//g_Model[testNo].srt.pos.z = (float)(rand() % 20) * 0.05f - 0.5f;

	DrawModel(&g_Model[testNo].model, &g_Model[testNo].srt);	// モデル描画
	SetCullingMode(CULL_MODE_BACK);
}

void DrawPlayerResult(void) {
	SetCullingMode(CULL_MODE_NONE);
	MATERIAL material;
	SRT srt;
	srt.scl = { 1.0f, 1.0f ,1.0f };
	srt.pos = { 0.0f, 0.0f ,0.0f };
	srt.rot = g_Model[testNo].srt.rot;

	DrawModel(&g_Model[testNo].model, &srt, &material);	// モデル描画
	SetCullingMode(CULL_MODE_BACK);
}

void DrawFire(void) {
	float rate = g_Rocket.GetSpeedRate() * 0.3f;
	g_Model[MODEL_PLAYER_FIRE].srt.scl.x = (float)(rand() % 10) * 0.02f * rate + rate;
	g_Model[MODEL_PLAYER_FIRE].srt.scl.y = (float)(rand() % 10) * 0.02f * rate + rate;
	g_Model[MODEL_PLAYER_FIRE].srt.scl.z = (float)(rand() % 10) * 0.02f * rate + rate;
	//g_Model[MODEL_PLAYER_FIRE].srt.scl.x = (float)(rand() % 10) * 0.03f * g_Rocket.GetSpeedRate();
	//g_Model[MODEL_PLAYER_FIRE].srt.scl.y = g_Model[MODEL_PLAYER_FIRE].srt.scl.x;
	//g_Model[MODEL_PLAYER_FIRE].srt.scl.z = g_Model[MODEL_PLAYER_FIRE].srt.scl.x;
	DrawModel(&g_Model[MODEL_PLAYER_FIRE].model, &g_Model[MODEL_PLAYER_FIRE].srt);	// モデル描画
}

void DrawFireResult(void) {
	SRT srt;
	srt.scl.x = (float)(rand() % 10) * 0.01f + 1.0f;
	srt.scl.y = (float)(rand() % 10) * 0.01f + 1.0f;
	srt.scl.z = (float)(rand() % 10) * 0.01f + 1.0f;
	srt.pos = { 0.0f, 0.0f , -70.0f };
	srt.rot = g_Model[testNo].srt.rot;
	DrawModel(&g_Model[MODEL_PLAYER_FIRE].model, &srt);	// モデル描画
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
int GetPlayerMissiles(void) {
	return g_Rocket.GetMissiles();
}
void SetPlayerThroughRing(void) {
	g_Rocket.Boost(30.0f);
	g_Rocket.Collision();
}
void SetPlayerCollisionIce(void) {
	g_Rocket.LostFuel(500.0f);
	g_Rocket.Back(g_Rocket.GetSpeed());
	g_Rocket.Boost(-g_Rocket.GetSpeed() * 0.5f);
	g_Rocket.BrakeFull();
	g_Rocket.Collision();
}
void SetPlayerCollisionSushi(void) {
	g_Rocket.SetMissiles();
}
void SetPlayerCollisionBlast(float rotAddSpd) {
	g_Rocket.Blast(rotAddSpd * 0.1f);
	g_Rocket.Collision();
}

void SetRocketStart(void) {
	g_Rocket.Accel(DEFAULT_SPEED);
	g_Rocket.Boost(DEFAULT_SPEED);
	g_Rocket.Start();
	SetBoostEffect();
	OnTimer();
}

CURVE_BUFFER GetCurveTestStatus(void) {
	//return curveTest;
	return *GetCurveBuffer();
}

void SetStageCurvePlayer(void) {
	SetStageCurve(g_Rocket.GetPos(), g_Rocket.GetSpeed());
}

void ResetPlayer(void) {
	g_Rocket.Reset();
	ResetTimer();
}