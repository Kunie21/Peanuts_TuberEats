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

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MODEL_MAX		(1)
#define DEFAULT_SPEED	(40.0f)

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static BOOL				g_Load = FALSE;

// テクスチャ管理
enum
{
	TEXTURE_TEAMLOGO = 0,
	TEXTURE_MAX,
};
static TEXTURE2D_DESC	g_td[TEXTURE_MAX];
static ID3D11ShaderResourceView*	g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報
static char*	g_TextureName[TEXTURE_MAX] = {
	"data/TEXTURE/blueberry_.png",
};

static DX11_MODEL	g_Model[MODEL_MAX];	// プレイヤーのモデル管理


static float		g_Rotation = 0.0f;
static float		g_TestAddSpeed = 0.0f;

class ROCKET
{
private:
	float m_pos = 0.0f;
	float m_posSpd = DEFAULT_SPEED;

	float m_rot = 0.0f;
	float m_rotSpd = 0.0f;
	const float c_rotSpdMax = 0.05f;

public:

	void Rotate(float rotSpd) {
		m_rotSpd += rotSpd;
		if (m_rotSpd > c_rotSpdMax) m_rotSpd = c_rotSpdMax;
		if (m_rotSpd < -c_rotSpdMax) m_rotSpd = -c_rotSpdMax;
	}
	void Accel(float posSpd) { m_posSpd += posSpd; }
	void Brake(float posSpd) { m_posSpd -= posSpd; }
	void Drive(void) {
		m_pos += m_posSpd;
		m_rot += m_rotSpd;
		m_rotSpd *= 0.98f;
	}

	float GetPos(void) const { return m_pos; }
	float GetSpeed(void) const { return m_posSpd; }
	float GetRotate(void) const { return m_rot; }
};

static ROCKET g_Rocket;

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

	for (int i = 0; i < MODEL_MAX; i++)
	{
		LoadModel("data/MODEL/ice_1.obj", &g_Model[i]);
		//LoadModel("data/MODEL/aloe.obj", &g_Model[i]);
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
		UnloadModel(&g_Model[i]);
	}

	g_Load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdatePlayer(void)
{
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
	//g_Rotation *= 0.98f;
	//RotateTube(g_Rotation);

	// 世界の回転を反映
	XMMATRIX mtxRot = XMMatrixRotationRollPitchYaw(0.0f, 0.0f, g_Rocket.GetRotate());
	SetAfterRotation(&mtxRot);

	static CURVE curve;

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
	//curve.TexSpd = (DEFAULT_SPEED + g_TestAddSpeed) / MESH_SIZE;
	curve.TexSpd = g_Rocket.GetSpeed() / MESH_SIZE;
	//g_TestAddSpeed *= 0.98f;

	g_Rocket.Drive();

#ifdef _DEBUG
	PrintDebugProc("g_TestAddSpeed:%f\n", g_TestAddSpeed);
#endif

	// パイプ曲げ
	if (GetKeyboardPress(DIK_F))
	{
		curve.Angle.y += 0.005f;
	}
	if (GetKeyboardPress(DIK_G))
	{
		curve.Angle.y -= 0.005f;
	}
	if (GetKeyboardPress(DIK_H))
	{
		curve.Angle.x += 0.005f;
	}
	if (GetKeyboardPress(DIK_J))
	{
		curve.Angle.x -= 0.005f;
	}
	SetCurveBuffer(&curve);

	//MoveTube(40.0f);
	//TestCurveTube(40.0f);

	// GPU_TIME
	static int time = 0;
	SetFrameTime(time++);
	SetMapPosition((float)time / 6000.0f);
	SetSpeedMeter((float)time / 6000.0f);
	SetFuelMeter(1.0f - (float)time / 6000.0f);

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
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// ワールドマトリックスの初期化
	mtxWorld = XMMatrixIdentity();

	// スケールを反映
	mtxScl = XMMatrixScaling(0.8f, 0.8f, 0.8f);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

	// 回転を反映：全体の角度
	mtxRot = XMMatrixRotationRollPitchYaw(-XM_PIDIV2, 0.0f, 0.0f);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

	// 移動を反映
	mtxTranslate = XMMatrixTranslation(0.0f, -60.0f, 300.0f);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

	// ワールドマトリックスの設定
	SetWorldBuffer(&mtxWorld);

	//XMStoreFloat4x4(&g_Player.mtxWorld, mtxWorld);

	// マテリアル設定
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = { 1.0f, 1.0f, 1.0f, 0.5f };

	// モデル描画
	DrawModel(&g_Model[0], NULL, &material);
}

float GetPlayerSpeed(void)
{
	return g_Rocket.GetSpeed();
	//return DEFAULT_SPEED + g_TestAddSpeed;
}