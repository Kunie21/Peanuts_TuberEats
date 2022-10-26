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

// ブランチテスト
#define test

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define DEFAULT_SPEED	(40.0f)
#define DEFAULT_POS		(310.0f)
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
	MODEL_MAX,
};
static MODEL_DATA	g_Model[MODEL_MAX];	// プレイヤーのモデル管理


static float		g_Rotation = 0.0f;
static float		g_TestAddSpeed = 0.0f;

class ROCKET
{
private:
	float m_pos = DEFAULT_POS;
	float m_posSpd = DEFAULT_SPEED;
	float m_addSpd = 0.0f;

	float m_rot = 0.0f;
	float m_rotSpd = 0.0f;
	const float c_rotSpdMax = 0.05f;

	float m_fuel = 5000.0f;
	const float c_fuelMax = 5000.0f;

	float m_invTime = 0.0f;

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
	float GetRotate(void) const { return m_rot; }
	float GetFuel(void) const { return m_fuel; }
	float GetFuelRate(void) const { return m_fuel / c_fuelMax; }
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
		LoadModel("data/MODEL/rocket01.obj", &g_Model[0].model);
		LoadModel("data/MODEL/rocket02.obj", &g_Model[1].model);
		LoadModel("data/MODEL/rocket03.obj", &g_Model[2].model);
		LoadModel("data/MODEL/rocket04.obj", &g_Model[3].model);
		LoadModel("data/MODEL/rocket05.obj", &g_Model[4].model);
		//LoadModel("data/MODEL/earth01.obj", &g_Model[4].model);
		for (int i = 0; i < MODEL_MAX; i++)
		{
			g_Model[i].pos = { 0.0f, -60.0f, DEFAULT_POS };
			g_Model[i].rot = { XM_PI, 0.0f, XM_PI };
			g_Model[i].scl = { 0.3f, 0.3f, 0.3f };
		}
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

	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		if (g_Texture[i])
		{
			g_Texture[i]->Release();
			g_Texture[i] = NULL;
		}
	}

	for (int i = 0; i < MODEL_MAX; i++)
	{
		UnloadModel(&g_Model[i].model);
	}

	g_Load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdatePlayer(void)
{
	static ROCKET oldRocket;
	oldRocket = g_Rocket;

	// 回転
	if (GetKeyboardPress(DIK_A))
	{
		g_Rocket.Rotate(0.002f);
		//if (g_Rotation < 0.05f) g_Rotation += 0.002f;
	}
	if (GetKeyboardPress(DIK_D))
	{
		g_Rocket.Rotate(-0.002f);
		//if (g_Rotation > -0.05f) g_Rotation -= 0.002f;
	}
	if (GetKeyboardPress(DIK_1)) { testNo = 0; }
	if (GetKeyboardPress(DIK_2)) { testNo = 1; }
	if (GetKeyboardPress(DIK_3)) { testNo = 2; }
	if (GetKeyboardPress(DIK_4)) { testNo = 3; }
	if (GetKeyboardPress(DIK_5)) { testNo = 4; }
	//g_Rotation *= 0.98f;
	//RotateTube(g_Rotation);

	// 世界の回転を反映
	XMMATRIX mtxRot = XMMatrixRotationRollPitchYaw(0.0f, 0.0f, g_Rocket.GetRotate());
	SetAfterRotation(&mtxRot);

	static CURVE_BUFFER curve;

	// スピード
	if (GetKeyboardTrigger(DIK_SPACE))
	{
		g_Rocket.Accel(5.0f);
		//g_TestAddSpeed += 5.0f;
	}
	if (GetKeyboardTrigger(DIK_BACK))
	{
		g_Rocket.Brake(5.0f);
		//g_TestAddSpeed -= 5.0f;
	}

	g_Rocket.Drive();

	////curve.TexSpd = (DEFAULT_SPEED + g_TestAddSpeed) / MESH_SIZE;
	////curve.TexSpd = g_Rocket.GetSpeed() / MESH_SIZE;
	//curve.TexPos = g_Rocket.GetPos() / MESH_SIZE;
	
	//g_TestAddSpeed *= 0.98f;

	if (g_Rocket.AbleToCollision())
	{
		CollisionGimmick(0, oldRocket.GetPos(), g_Rocket.GetPos(), oldRocket.GetRotate(), g_Rocket.GetRotate());
		//SetDamageEffect();
	}

#ifdef _DEBUG
	PrintDebugProc("g_TestAddSpeed:%f\n", g_TestAddSpeed);
#endif

	// パイプ曲げ
	//if (GetKeyboardPress(DIK_F))
	//{
	//	curve.Angle.y += 0.005f;
	//}
	//if (GetKeyboardPress(DIK_G))
	//{
	//	curve.Angle.y -= 0.005f;
	//}
	//if (GetKeyboardPress(DIK_H))
	//{
	//	curve.Angle.x += 0.005f;
	//}
	//if (GetKeyboardPress(DIK_J))
	//{
	//	curve.Angle.x -= 0.005f;
	//}
	//SetCurveBuffer(&curve);
	SetStageCurve(0, g_Rocket.GetPos());

	//MoveTube(40.0f);
	//TestCurveTube(40.0f);

	// GPU_TIME
	static int time = 0;
	SetFrameTime(time++);
	SetMapPosition(g_Rocket.GetPos() / ((float)GetStage(0)->length * MESH_SIZE));
	SetSpeedMeter(g_Rocket.GetSpeed() / MAX_SPEED);
	SetFuelMeter(g_Rocket.GetFuelRate());

#ifdef _DEBUG	// デバッグ情報を表示する
	static int dZMove = 0;
	static int dTime = 0;
	dZMove += 40;
	dTime++;
	PrintDebugProc("Meshs:%d\n", dZMove / (int)MESH_SIZE);
	PrintDebugProc("Tubes:%d\n", dZMove / (int)(MESH_SIZE * MESH_NUM_Z));
	PrintDebugProc("Time:%d\n", dTime / 60);
	PrintDebugProc("Speed:%f\n", g_Rocket.GetSpeed());
#endif
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawPlayer(void)
{
	SetCullingMode(CULL_MODE_NONE);

	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// ワールドマトリックスの初期化
	mtxWorld = XMMatrixIdentity();

	// スケールを反映
	mtxScl = XMMatrixScaling(g_Model[testNo].scl.x, g_Model[testNo].scl.y, g_Model[testNo].scl.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

	// 回転を反映：全体の角度
	mtxRot = XMMatrixRotationRollPitchYaw(g_Model[testNo].rot.x, g_Model[testNo].rot.y, g_Model[testNo].rot.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

	// 移動を反映
	mtxTranslate = XMMatrixTranslation(g_Model[testNo].pos.x, g_Model[testNo].pos.y, g_Model[testNo].pos.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

	// ワールドマトリックスの設定
	SetWorldBuffer(&mtxWorld);

	// マテリアル設定
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
	//material.Diffuse = { 1.0f, 1.0f, 1.0f, 0.5f };

	// モデル描画
	DrawModel(&g_Model[testNo].model, NULL, &material);
	//DrawModel(&g_Model[0].model, &g_Texture[TEXTURE_STAR], &material);

	SetCullingMode(CULL_MODE_BACK);
}

float GetPlayerSpeed(void)
{
	return g_Rocket.GetSpeed();
}
void SetPlayerThroughRing(void)
{
	g_Rocket.Boost(30.0f);
	g_Rocket.Collision();
}
void SetPlayerCollisionIce(void)
{
	g_Rocket.LostFuel(500.0f);
	g_Rocket.Boost(-60.0f);
	g_Rocket.Collision();
}