//=============================================================================
//
// �Q�[�g���� [gate.cpp]
// Author : ���] �đ�
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
// �}�N����`
//*****************************************************************************
#define GATE_SCL		(40.0f)		// �Q�[�g�̑傫��
#define GATE_POS		(250.0f)	// Z���W
#define IMAGE_SCL		(30.0f)		// �J�E���g�_�E�������̑傫��
#define IMAGE_POS		(1.0f)		// Z���W���炷��
#define CD_SPD			(0.11f)		// �J�E���g�_�E���X�s�[�h
#define MONITOR_SPD		(0.0007f)	// �������X�s�[�h

//*****************************************************************************
// �N���X�錾
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

	void SetOn(void);				// �I���ɂ��邾���i���������j
	void SetStart(void);			// �J�E���g�_�E�����n�߂�
	void SetGoal(float goal_pos);	// �S�[����ݒu����
};

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static BOOL		g_Load = FALSE;
static GATE*	g_StartGate;
static GATE*	g_GoalGate;

//=============================================================================
// ����������
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
// �I������
//=============================================================================
void UninitGate(void)
{
	if (g_Load == FALSE) return;

	if (g_StartGate) delete g_StartGate;
	if (g_GoalGate) delete g_GoalGate;

	g_Load = FALSE;
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateGate(void)
{
	g_StartGate->Update();
	g_GoalGate->Update();
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawGate(void)
{
	SetCullingMode(CULL_MODE_NONE);
	g_GoalGate->Draw();		// �S�[���̕������ɂ���̂Ő�ɕ`��
	g_StartGate->Draw();
	SetCullingMode(CULL_MODE_BACK);
}

// �R���X�g���N�^
GATE::GATE()
{
	//char* TextureName[GATE_TEX_MAX] = {
	//	"data/TEXTURE/game_UI/countdown_3_2.png",
	//	"data/TEXTURE/game_UI/countdown_2_2.png",
	//	"data/TEXTURE/game_UI/countdown_1_2.png",
	//	"data/TEXTURE/game_UI/go_2.png",
	//	"data/TEXTURE/game_UI/go_2.png",
	//};

	// �e�N�X�`������
	//for (int i = 0; i < GATE_TEX_MAX; i++) {
	//	D3DX11CreateShaderResourceViewFromFile(GetDevice(), TextureName[i], NULL, NULL, &m_Texture[i], NULL);
	//}
	for (int i = 0; i < GATE_TEX_MAX; i++) {
		m_Texture[i] = (TEXTURE_LABEL)(i + TEXTURE_LABEL_COUNTDOWN3);
	}

	// ���f���ǂݍ���
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

	// ���f���ʒu����
	m_Model.srt.pos = {
		(float)(rand() % 10) * 0.005f,
		(float)(rand() % 10) * 0.005f,
		m_Pos - GetPlayerPosition()
	};
	// �����ʒu����
	m_ImageSRT.pos = m_Model.srt.pos;
	m_ImageSRT.pos.z += IMAGE_POS;

	// ���j�^�[���e�N�X�`���X�V
	m_bCurve.TexPos += MONITOR_SPD;
	m_bCurve.Angle = GetCurveTestStatus().Angle;
	m_bCurve.Spd = GetCurveTestStatus().Spd;

	// �J�E���g�_�E����
	if (m_Status == GATE_STATUS_CD) {

		// ���ԍX�V
		m_TimeRad += CD_SPD;

		// �J�E���g�_�E���e�N�X�`���X�V
		m_ImageNo = (int)((m_TimeRad + XM_PIDIV2) / XM_2PI);

		// �J�E���g�_�E���I������
		if (m_ImageNo == GATE_TEX_GOAL - 1 &&
			(m_TimeRad - XM_2PI * m_ImageNo) >= XM_PI)	// ���傤��GO���傫���Ȃ����^�C�~���O
		{
			m_Status = GATE_STATUS_OFF;	// �I�t�ɂ���
			SetRocketStart();	// ���P�b�g�X�^�[�g
		}

		// �X�P�[������
		float ds = sinf(m_TimeRad);
		if (ds > 0.0f) ds = 0.0f;
		float scl = IMAGE_SCL * (1.0f + ds);
		m_ImageSRT.scl = { IMAGE_SCL * IMAGE_SCL / scl, scl, 0.0f };
	}
}

void GATE::Draw(void)
{
	//if (m_Status == GATE_STATUS_OFF) return;
	
	// �����\��
	if (m_Status != GATE_STATUS_ON) {
		SetDrawLight();
		DrawModel(&m_Model.model, &m_ImageSRT, m_Texture[m_ImageNo], &m_MaterialImage);
	}

	// ���j�^�[���G�t�F�N�g�`��
	SetCurveBuffer(&m_bCurve);
	SetDrawMonitor();
	DrawModel(&m_Model.model, &m_Model.srt, &m_MaterialMonitor);
}

// �S�[���ݒ�
void GATE::SetOn(void)
{
	m_Status = GATE_STATUS_ON;
}

// �J�E���g�_�E���X�^�[�g
void GATE::SetStart(void)
{
	m_Status = GATE_STATUS_CD;
	m_TimeRad = 0.0f;
	m_ImageNo = 0;
	Update();	// Draw�Ɏx�Ⴊ�Ȃ��悤�m���ɃA�b�v�f�[�g
}
void SetStartGate(void) {
	g_StartGate->SetStart();
}

// �S�[���ݒ�
void GATE::SetGoal(float goal_pos)
{
	m_Pos = goal_pos;
	m_Status = GATE_STATUS_GOAL;
	m_ImageNo = GATE_TEX_GOAL;
	Update();	// Draw�Ɏx�Ⴊ�Ȃ��悤�m���ɃA�b�v�f�[�g
}
void SetGoalGate(float goal_pos) {
	g_GoalGate->SetGoal(goal_pos);
}