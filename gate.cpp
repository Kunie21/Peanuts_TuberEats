//=============================================================================
//
// ゲート処理 [gate.cpp]
// Author : 國江 翔太
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "texture2d.h"
#include "model.h"
#include "debugproc.h"
#include "fade.h"
#include "input.h"
#include "gate.h"
#include "tube.h"
#include "gimmick.h"
#include "stage.h"
#include "player.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define GATE_SCL		(40.0f)		// ゲートの大きさ
#define GATE_POS		(250.0f)	// Z座標
#define IMAGE_SCL		(30.0f)		// カウントダウン文字の大きさ
#define IMAGE_POS		(1.0f)		// Z座標ずらす量
#define CD_SPD			(0.11f)		// カウントダウンスピード
#define MONITOR_SPD		(0.0007f)	// 走査線スピード

//*****************************************************************************
// クラス宣言
//*****************************************************************************
enum GATE_STATUS {
	GATE_STATUS_OFF = 0,
	GATE_STATUS_ON,
	GATE_STATUS_CD,
	GATE_STATUS_GOAL,
};
enum {
	GATE_TEX_CD_3 = 0,
	GATE_TEX_CD_2,
	GATE_TEX_CD_1,
	GATE_TEX_CD_GO,
	GATE_TEX_GOAL,
	GATE_TEX_MAX,
};

class GATE
{
private:
	GATE_STATUS					m_Status = GATE_STATUS_OFF;
	//ID3D11ShaderResourceView*	m_Texture[GATE_TEX_MAX] = { NULL };
	TEXTURE_LABEL				m_Texture[GATE_TEX_MAX];
	MODEL_DATA					m_Model;
	MATERIAL					m_MaterialMonitor;
	MATERIAL					m_MaterialImage;
	CURVE_BUFFER				m_bCurve;
	SRT							m_ImageSRT;
	float						m_TimeRad = 0.0f;
	int							m_ImageNo = 0;
	float						m_Pos = GATE_POS;

public:
	GATE();
	~GATE();

	void Update(void);
	void Draw(void);

	void SetOn(void);				// オンにするだけ（文字無し）
	void SetStart(void);			// カウントダウンを始める
	void SetGoal(float goal_pos);	// ゴールを設置する
};

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static BOOL		g_Load = FALSE;
static GATE*	g_StartGate;
static GATE*	g_GoalGate;

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitGate(void)
{
	g_StartGate = new GATE;
	g_StartGate->SetOn();

	g_GoalGate = new GATE;
	g_GoalGate->SetGoal(50000.0f);

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitGate(void)
{
	if (g_Load == FALSE) return;

	if (g_StartGate) delete g_StartGate;
	if (g_GoalGate) delete g_GoalGate;

	g_Load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateGate(void)
{
	g_StartGate->Update();
	g_GoalGate->Update();
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawGate(void)
{
	SetCullingMode(CULL_MODE_NONE);
	g_GoalGate->Draw();		// ゴールの方が後ろにあるので先に描画
	g_StartGate->Draw();
	SetCullingMode(CULL_MODE_BACK);
}

// コンストラクタ
GATE::GATE()
{
	//char* TextureName[GATE_TEX_MAX] = {
	//	"data/TEXTURE/game_UI/countdown_3_2.png",
	//	"data/TEXTURE/game_UI/countdown_2_2.png",
	//	"data/TEXTURE/game_UI/countdown_1_2.png",
	//	"data/TEXTURE/game_UI/go_2.png",
	//	"data/TEXTURE/game_UI/go_2.png",
	//};

	// テクスチャ生成
	//for (int i = 0; i < GATE_TEX_MAX; i++) {
	//	D3DX11CreateShaderResourceViewFromFile(GetDevice(), TextureName[i], NULL, NULL, &m_Texture[i], NULL);
	//}
	for (int i = 0; i < GATE_TEX_MAX; i++) {
		m_Texture[i] = (TEXTURE_LABEL)(i + TEXTURE_LABEL_COUNTDOWN3);
	}

	// モデル読み込み
	{
		//LoadModel("data/MODEL/wall.obj", &m_Model.model);
		m_Model.model = MODEL_GATE;

		m_Model.srt.pos = { 0.0f, 0.0f, m_Pos };
		m_Model.srt.rot = { 0.0f, 0.0f, 0.0f };
		m_Model.srt.scl = { GATE_SCL, GATE_SCL, 0.0f };

		m_ImageSRT = m_Model.srt;
		m_ImageSRT.pos.z += IMAGE_POS;
		m_ImageSRT.scl = { IMAGE_SCL, IMAGE_SCL, 0.0f };
	}

	//m_MaterialImage.Diffuse = { 0.15f, 0.15f, 0.15f, 1.0f };
	//m_MaterialMonitor.Diffuse = { 0.0f, 0.1f, 0.08f, 1.0f };
	m_MaterialImage.Diffuse = { 0.55f, 0.55f, 0.55f, 1.0f };
	m_MaterialMonitor.Diffuse = { 0.0f, 0.3f, 0.25f, 1.0f };
}

GATE::~GATE()
{
	//for (int i = 0; i < GATE_TEX_MAX; i++) {
	//	if (m_Texture[i]) {
	//		m_Texture[i]->Release();
	//		m_Texture[i] = NULL;
	//	}
	//}
	
	//UnloadModel(&m_Model.model);
}

void GATE::Update(void)
{
	//if (m_Status == GATE_STATUS_OFF) return;

	// モデル位置調整
	m_Model.srt.pos = {
		(float)(rand() % 10) * 0.005f,
		(float)(rand() % 10) * 0.005f,
		m_Pos - GetPlayerPosition()
	};
	// 文字位置調整
	m_ImageSRT.pos = m_Model.srt.pos;
	m_ImageSRT.pos.z += IMAGE_POS;

	// モニター風テクスチャ更新
	m_bCurve.TexPos += MONITOR_SPD;
	m_bCurve.Angle = GetCurveTestStatus().Angle;
	m_bCurve.Spd = GetCurveTestStatus().Spd;

	// カウントダウン中
	if (m_Status == GATE_STATUS_CD) {

		// 時間更新
		m_TimeRad += CD_SPD;

		// カウントダウンテクスチャ更新
		m_ImageNo = (int)((m_TimeRad + XM_PIDIV2) / XM_2PI);

		// カウントダウン終了判定
		if (m_ImageNo == GATE_TEX_GOAL - 1 &&
			(m_TimeRad - XM_2PI * m_ImageNo) >= XM_PI)	// ちょうどGOが大きくなったタイミング
		{
			m_Status = GATE_STATUS_OFF;	// オフにする
			SetRocketStart();	// ロケットスタート
		}

		// スケール調整
		float ds = sinf(m_TimeRad);
		if (ds > 0.0f) ds = 0.0f;
		float scl = IMAGE_SCL * (1.0f + ds);
		m_ImageSRT.scl = { IMAGE_SCL * IMAGE_SCL / scl, scl, 0.0f };
	}
}

void GATE::Draw(void)
{
	//if (m_Status == GATE_STATUS_OFF) return;
	
	// 文字表示
	if (m_Status != GATE_STATUS_ON) {
		SetDrawLight();
		DrawModel(&m_Model.model, &m_ImageSRT, m_Texture[m_ImageNo], &m_MaterialImage);
	}

	// モニター風エフェクト描画
	SetCurveBuffer(&m_bCurve);
	SetDrawMonitor();
	DrawModel(&m_Model.model, &m_Model.srt, &m_MaterialMonitor);
}

// ゴール設定
void GATE::SetOn(void)
{
	m_Status = GATE_STATUS_ON;
}

// カウントダウンスタート
void GATE::SetStart(void)
{
	m_Status = GATE_STATUS_CD;
	m_TimeRad = 0.0f;
	m_ImageNo = 0;
	Update();	// Drawに支障がないよう確実にアップデート
}
void SetStartGate(void) {
	g_StartGate->SetStart();
}

// ゴール設定
void GATE::SetGoal(float goal_pos)
{
	m_Pos = goal_pos;
	m_Status = GATE_STATUS_GOAL;
	m_ImageNo = GATE_TEX_GOAL;
	Update();	// Drawに支障がないよう確実にアップデート
}
void SetGoalGate(float goal_pos) {
	g_GoalGate->SetGoal(goal_pos);
}