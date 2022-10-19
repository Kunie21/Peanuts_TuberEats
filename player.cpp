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

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define MODEL_MAX		(1)
#define DEFAULT_SPEED	(40.0f)

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static BOOL				g_Load = FALSE;

// �e�N�X�`���Ǘ�
enum
{
	TEXTURE_TEAMLOGO = 0,
	TEXTURE_MAX,
};
static TEXTURE2D_DESC	g_td[TEXTURE_MAX];
static ID3D11ShaderResourceView*	g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����
static char*	g_TextureName[TEXTURE_MAX] = {
	"data/TEXTURE/blueberry_.png",
};

static DX11_MODEL	g_Model[MODEL_MAX];	// �v���C���[�̃��f���Ǘ�


static float		g_Rotation = 0.0f;
static float		g_TestAddSpeed = 0.0f;

//=============================================================================
// ����������
//=============================================================================
HRESULT InitPlayer(void)
{
	// �e�N�X�`������
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

	// �ڍאݒ�
	//g_td[TEXTURE_TEAMLOGO].tex = &g_Texture[TEXTURE_TEAMLOGO];

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
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
// �X�V����
//=============================================================================
void UpdatePlayer(void)
{
	// ��]
	if (GetKeyboardPress(DIK_A))
	{
		if (g_Rotation < 0.05f) g_Rotation += 0.002f;
	}
	if (GetKeyboardPress(DIK_D))
	{
		if (g_Rotation > -0.05f) g_Rotation -= 0.002f;
	}
	g_Rotation *= 0.98f;
	RotateTube(g_Rotation);

	static CURVE curve;

	// �X�s�[�h
	if (GetKeyboardTrigger(DIK_SPACE))
	{
		g_TestAddSpeed += 5.0f;
	}
	if (GetKeyboardTrigger(DIK_BACK))
	{
		g_TestAddSpeed -= 5.0f;
	}
	curve.TexSpd = (DEFAULT_SPEED + g_TestAddSpeed) / MESH_SIZE;
	//g_TestAddSpeed *= 0.98f;

#ifdef _DEBUG
	PrintDebugProc("g_TestAddSpeed:%f\n", g_TestAddSpeed);
#endif

	// �p�C�v�Ȃ�
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

#ifdef _DEBUG	// �f�o�b�O����\������
	static int dZMove = 0;
	static int dTime = 0;
	dZMove += 40;
	dTime++;
	PrintDebugProc("Z_MOVE:%d\n", dZMove / (int)MESH_SIZE);
	PrintDebugProc("TUBES:%d\n", dZMove / (int)(MESH_SIZE * MESH_NUM_Z));
	PrintDebugProc("Time:%d\n", dTime / 60);
#endif
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawPlayer(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// ���[���h�}�g���b�N�X�̏�����
	mtxWorld = XMMatrixIdentity();

	// �X�P�[���𔽉f
	mtxScl = XMMatrixScaling(0.8f, 0.8f, 0.8f);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

	// ��]�𔽉f�F�S�̂̊p�x
	mtxRot = XMMatrixRotationRollPitchYaw(-XM_PIDIV2, 0.0f, 0.0f);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

	// �ړ��𔽉f
	mtxTranslate = XMMatrixTranslation(0.0f, -60.0f, 300.0f);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

	// ���[���h�}�g���b�N�X�̐ݒ�
	SetWorldBuffer(&mtxWorld);

	//XMStoreFloat4x4(&g_Player.mtxWorld, mtxWorld);

	// �}�e���A���ݒ�
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = { 1.0f, 1.0f, 1.0f, 0.5f };

	// ���f���`��
	DrawModel(&g_Model[0], NULL, &material);
}

float GetPlayerSpeed(void)
{
	return DEFAULT_SPEED + g_TestAddSpeed;
}