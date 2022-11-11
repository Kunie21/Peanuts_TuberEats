//=============================================================================
//
// �~�T�C������ [missile.cpp]
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
#include "tube.h"
#include "player.h"
#include "ui_game.h"
#include "missile.h"
#include "gimmick.h"
#include "stage.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
//#define MODEL_MAX		(1)
#define MISSILE_NUM		(50)

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static int			g_Time = 0;
static BOOL			g_Load = FALSE;

static MISSILE		g_Ms[MISSILE_NUM];

static DX11_MODEL	g_Model[MISSILE_TYPE_MAX];	// �v���C���[�̃��f���Ǘ�


static float		g_Rotation = 0.0f;

//=============================================================================
// ����������
//=============================================================================
HRESULT InitMissile(void)
{
	LoadModel("data/MODEL/ice_1.obj", &g_Model[MISSILE_TYPE_ICE]);
	//LoadModel("data/MODEL/rocket01.obj", &g_Model[MISSILE_TYPE_RING]);
	LoadModel("data/MODEL/ring_1.obj", &g_Model[MISSILE_TYPE_RING]);

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitMissile(void)
{
	if (g_Load == FALSE) return;

	for (int i = 0; i < MISSILE_TYPE_MAX; i++)
	{
		UnloadModel(&g_Model[i]);
	}

	g_Load = FALSE;
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateMissile(void)
{
	for (int i = 0; i < MISSILE_NUM; i++)
	{
		if (g_Ms[i].use) {
			g_Ms[i].zPos += g_Ms[i].zSpd;
			g_Ms[i].zSpd += g_Ms[i].zAcl;
			g_Ms[i].zRot += g_Ms[i].zRotSpd;

			// �R���W����
			if (CollisionMissile(0, GetPlayerPosition() + g_Ms[i].zPosOld, GetPlayerPosition() + g_Ms[i].zPos, -g_Ms[i].zRotOld + XM_PI, -g_Ms[i].zRot + XM_PI))
				g_Ms[i].use = FALSE;

			g_Ms[i].zPosOld = g_Ms[i].zPos;
			g_Ms[i].zRotOld = g_Ms[i].zRot;

			if (10000.0f < g_Ms[i].zPos) g_Ms[i].use = false;
		}
	}

#ifdef _DEBUG	// �f�o�b�O����\������
#endif
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawMissile(MISSILE_TYPE gimmick)
{
	// �C���X�^���X����o�^
	D3D11_MAPPED_SUBRESOURCE msr;
	GetDeviceContext()->Map(GetInstanceBuffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
	INSTANCE* b_pInstance = (INSTANCE*)msr.pData;
	int instCount = 0;

	float zPos, rot;
	for (int i = 0; i < MISSILE_NUM; i++)
	{
		if (!g_Ms[i].use) continue;
		if (g_Ms[i].type != gimmick) continue;
		if (g_Ms[i].zPos < -500.0f || 10000.0f < g_Ms[i].zPos) continue;
		
		zPos = g_Ms[i].zPos;
		rot = g_Ms[i].zRot + GetTubeRotation() + XM_PIDIV2;

		switch (gimmick)
		{
		case MISSILE_TYPE_ICE:
			b_pInstance->scl[instCount] = { 0.5f, 0.5f, 0.5f, 0.0f };
			b_pInstance->rot[instCount] = { -XM_PIDIV2, 0.0f, rot + XM_PIDIV2, 0.0f };
			b_pInstance->pos[instCount] = { (TUBE_RADIUS - 120.0f) * 0.8f * cosf(rot), (TUBE_RADIUS - 120.0f) * 0.8f * sinf(rot), zPos, 0.0f };
			b_pInstance->col[instCount] = { 1.0f, 1.0f, 1.0f, 1.0f };
			break;

		case MISSILE_TYPE_RING:
			b_pInstance->scl[instCount] = { 0.1f, 0.1f, 0.1f, 0.0f };
			b_pInstance->rot[instCount] = { XM_PIDIV2, XM_PI, rot + XM_PIDIV2, 0.0f };
			b_pInstance->pos[instCount] = { (TUBE_RADIUS - 120.0f) * 0.8f * cosf(rot), (TUBE_RADIUS - 120.0f) * 0.8f * sinf(rot), zPos, 0.0f };
			b_pInstance->col[instCount] = { 0.0f, 0.0f, 1.0f, 1.0f };
			break;
		}
		instCount++;	// �C���X�^���X�����X�V
	}
	GetDeviceContext()->Unmap(GetInstanceBuffer(), 0);

	SetWorldBuffer(&XMMatrixIdentity());	// ���[���h�}�g���b�N�X�̐ݒ�
	static MATERIAL material;
	DrawModelInstanced(&g_Model[gimmick], instCount, &material);	// ���f���`��
}

BOOL LaunchMissile(MISSILE_TYPE type, float zPos, float zSpd, float zRot, float zRotSpd)
{
	for (int i = 0; i < MISSILE_NUM; i++)
	{
		if (g_Ms[i].use) continue;
		g_Ms[i].type = type;

		g_Ms[i].zPos = zPos;
		g_Ms[i].zPosOld = zPos;
		g_Ms[i].zSpd = zSpd;

		g_Ms[i].zRot = zRot;
		g_Ms[i].zRotOld = zRot;
		g_Ms[i].zRotSpd = zRotSpd;

		g_Ms[i].use = TRUE;
		return TRUE;
	}
	return FALSE;
}