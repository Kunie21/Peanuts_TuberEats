//=============================================================================
//
// �v���C���[���� [player.cpp]
// Author : ���] �đ�
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
// �}�N����`
//*****************************************************************************
#define DEFAULT_SPEED	(50.0f)
//#define DEFAULT_POS		(310.0f)
#define MAX_SPEED		(150.0f)

//*****************************************************************************
// �O���[�o���ϐ�
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
static MODEL_DATA	g_Model[MODEL_PLAYER_MAX];	// �v���C���[�̃��f���Ǘ�


static float		g_Rotation = 0.0f;
static float		g_TestAddSpeed = 0.0f;

class ROCKET
{
private:
	float m_pos = 0;
	float m_posSpd = 0.0f; // DEFAULT_SPEED;
	float m_addSpd = 0.0f;

	float m_rot = 0.0f;
	float m_rotSpd = 0.0f;
	float m_rotAddSpd = 0.0f;
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
	void Blast(float rotAddSpd) {
		m_rotAddSpd += rotAddSpd;
	}
	void Accel(float posSpd) { m_posSpd += posSpd; }
	void Boost(float addSpd) { m_addSpd += addSpd; }
	void Brake(float posSpd) { m_posSpd -= posSpd; }
	float Drive(void) {
		m_pos += m_posSpd + m_addSpd;
		m_rot += m_rotSpd + m_rotAddSpd;
		while (m_rot < 0.0f) m_rot += XM_2PI;
		while (m_rot > XM_2PI) m_rot -= XM_2PI;
		m_rotSpd *= 0.98f;
		m_rotAddSpd *= 0.98f;
		m_addSpd *= 0.98f;
		m_invTime *= 0.98f;
		m_fuel -= m_posSpd / MAX_SPEED;
		return -(m_rotSpd + m_rotAddSpd) / c_rotSpdMax * XM_PIDIV4 * 0.5f + XM_PI;
	}
	void LostFuel(float lostFuel) { m_fuel -= lostFuel; }
	void Collision(void) { m_invTime = 1.5f; }

	bool AbleToCollision(void) const { if (m_invTime < 1.0f) return true; return false; }
	float GetPos(void) const { return m_pos; }
	float GetSpeed(void) const { return m_posSpd + m_addSpd; }
	float GetSpeedRate(void) const {
		if (GetSpeed() > 0.0f) return sqrtf(GetSpeed() / MAX_SPEED);
		return 0.0f;
	}
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

static CURVE_BUFFER curveTest;

//=============================================================================
// ����������
//=============================================================================
HRESULT InitPlayer(void)
{

	//for (int i = 0; i < MODEL_MAX; i++)
	{
		//LoadModel("data/MODEL/earth01_sv.obj", &g_Model[0].model);
		//LoadModel("data/MODEL/earth01_adj.obj", &g_Model[0].model);
		//LoadModel("data/MODEL/square2.obj", &g_Model[0].model);

		//LoadModel("data/MODEL/rocket01.obj", &g_Model[0].model);
		//LoadModel("data/MODEL/rocket02.obj", &g_Model[1].model);
		//LoadModel("data/MODEL/rocket03.obj", &g_Model[2].model);
		//LoadModel("data/MODEL/rocket04.obj", &g_Model[3].model);
		//LoadModel("data/MODEL/rocket05.obj", &g_Model[4].model);
		//LoadModel("data/MODEL/fire01.obj", &g_Model[5].model);
		g_Model[0].model = MODEL_ROCKET1;
		g_Model[1].model = MODEL_ROCKET2;
		g_Model[2].model = MODEL_ROCKET3;
		g_Model[3].model = MODEL_ROCKET4;
		g_Model[4].model = MODEL_ROCKET5;
		g_Model[5].model = MODEL_FIRE;
		for (int i = 0; i < MODEL_PLAYER_MAX; i++) {
			g_Model[i].srt.pos = { 0.0f, ROCKET_Y, 0.0f };
			g_Model[i].srt.rot = { XM_PI, 0.0f, XM_PI };
			g_Model[i].srt.scl = { 0.3f, 0.3f, 0.3f };
		}
		g_Model[MODEL_PLAYER_FIRE].srt.pos.z = -30.0f;
	}

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitPlayer(void)
{
	if (g_Load == FALSE) return;


	//for (int i = 0; i < MODEL_MAX; i++) { UnloadModel(&g_Model[i].model); }

	g_Load = FALSE;
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdatePlayer(void)
{
	// ���P�b�g�̎�ޕύX
	if (GetKeyboardPress(DIK_1)) { testNo = 0; }
	if (GetKeyboardPress(DIK_2)) { testNo = 1; }
	if (GetKeyboardPress(DIK_3)) { testNo = 2; }
	if (GetKeyboardPress(DIK_4)) { testNo = 3; }
	if (GetKeyboardPress(DIK_5)) { testNo = 4; }

	// ���P�b�g�̏�Ԃ�ۑ�
	static ROCKET oldRocket;
	oldRocket = g_Rocket;

	// ��]
	if (GetKeyboardPress(DIK_A)) { g_Rocket.Rotate(0.002f); }
	if (GetKeyboardPress(DIK_D)) { g_Rocket.Rotate(-0.002f); }
	SetAfterRotation(&XMMatrixRotationRollPitchYaw(0.0f, 0.0f, g_Rocket.GetRotate()));

	// �A�N�Z��
	if (GetKeyboardTrigger(DIK_SPACE)) { g_Rocket.Accel(5.0f); }
	if (GetKeyboardTrigger(DIK_BACK)) { g_Rocket.Brake(5.0f); }

	// �h���C�u
	g_Model[testNo].srt.rot.z = g_Rocket.Drive();

	// �~�T�C��
	if (GetKeyboardTrigger(DIK_RETURN)) { g_Rocket.Launch(MISSILE_TYPE_01); }
	if (GetKeyboardTrigger(DIK_S)) { g_Rocket.Launch(MISSILE_TYPE_02); }

	// �R���W����
	if (g_Rocket.AbleToCollision()){
		CollisionGimmick(0, oldRocket.GetPos(), g_Rocket.GetPos(), oldRocket.GetRotate(), g_Rocket.GetRotate());
		//SetDamageEffect();
	}

	// �p�C�v�Ȃ��i�蓮�j
	if (GetKeyboardPress(DIK_F)) { curveTest.Angle.y += 0.005f; }
	if (GetKeyboardPress(DIK_G)) { curveTest.Angle.y -= 0.005f; }
	if (GetKeyboardPress(DIK_H)) { curveTest.Angle.x += 0.005f; }
	if (GetKeyboardPress(DIK_J)) { curveTest.Angle.x -= 0.005f; }
	//curveTest.TexPos = g_Rocket.GetPos() / MESH_SIZE_Z / MESH_NUM_Z;
	curveTest.TexPos = g_Rocket.GetPos() / MESH_SIZE_Z;
	curveTest.Spd = g_Rocket.GetSpeed();
	SetCurveBuffer(&curveTest);
	
	// �p�C�v�Ȃ��i�X�e�[�W�ݒ�ɏ]���Ď����ŋȂ���j
	//SetStageCurve(0, g_Rocket.GetPos());

	// GPU_TIME
	static int time = 0;
	SetFrameTime(time++);
	SetMapPosition(g_Rocket.GetPos() / ((float)GetStage(0)->length * MESH_SIZE_Z));
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

#ifdef _DEBUG	// �f�o�b�O����\������
	//PrintDebugProc("ThroughMeshs:%d\n", g_Rocket.GetPos() / (int)MESH_SIZE_Z);
	//PrintDebugProc("ThroughTubes:%d\n", g_Rocket.GetPos() / (int)TUBE_SIZE);
	//PrintDebugProc("Time(sec):%f\n", GetTime());
	//PrintDebugProc("RocketSpeed:%f\n", g_Rocket.GetSpeed());
	//PrintDebugProc("CurveAngleX:%f\n", curveTest.Angle.x);
	//PrintDebugProc("CurveAngleY:%f\n", curveTest.Angle.y);
#endif
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawPlayer(void) {
	SetCullingMode(CULL_MODE_NONE);
	DrawModel(&g_Model[testNo].model, &g_Model[testNo].srt);	// ���f���`��
	SetCullingMode(CULL_MODE_BACK);
}

void DrawPlayerResult(void) {
	SetCullingMode(CULL_MODE_NONE);
	MATERIAL material;
	SRT srt;
	srt.scl = { 1.0f, 1.0f ,1.0f };
	srt.pos = { 0.0f, 0.0f ,0.0f };
	srt.rot = g_Model[testNo].srt.rot;

	DrawModel(&g_Model[testNo].model, &srt, &material);	// ���f���`��
	SetCullingMode(CULL_MODE_BACK);
}

void DrawFire(void) {
	g_Model[MODEL_PLAYER_FIRE].srt.scl.x = (float)(rand() % 10) * 0.003f + 0.3f * g_Rocket.GetSpeedRate();
	g_Model[MODEL_PLAYER_FIRE].srt.scl.y = (float)(rand() % 10) * 0.003f + 0.3f * g_Rocket.GetSpeedRate();
	g_Model[MODEL_PLAYER_FIRE].srt.scl.z = (float)(rand() % 10) * 0.003f + 0.3f * g_Rocket.GetSpeedRate();
	DrawModel(&g_Model[MODEL_PLAYER_FIRE].model, &g_Model[MODEL_PLAYER_FIRE].srt);	// ���f���`��
}

void DrawFireResult(void) {
	SRT srt;
	srt.scl.x = (float)(rand() % 10) * 0.01f + 1.0f;
	srt.scl.y = (float)(rand() % 10) * 0.01f + 1.0f;
	srt.scl.z = (float)(rand() % 10) * 0.01f + 1.0f;
	srt.pos = { 0.0f, 0.0f , -70.0f };
	srt.rot = g_Model[testNo].srt.rot;
	DrawModel(&g_Model[MODEL_PLAYER_FIRE].model, &srt);	// ���f���`��
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
void SetPlayerCollisionBlast(float rotAddSpd) {
	g_Rocket.Blast(rotAddSpd * 0.1f);
	g_Rocket.Collision();
}

void SetRocketStart(void) {
	g_Rocket.Accel(DEFAULT_SPEED);
	g_Rocket.Boost(DEFAULT_SPEED);
	SetBoostEffect();
}

CURVE_BUFFER GetCurveTestStatus(void) {
	return curveTest;
}