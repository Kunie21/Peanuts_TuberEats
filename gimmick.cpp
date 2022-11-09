//=============================================================================
//
// �M�~�b�N���� [gimmick.cpp]
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
#include "gimmick.h"
#include "tube.h"
#include "player.h"
#include "ui_game.h"
#include "stage.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
//#define MODEL_MAX		(1)
#define ICE_NUM		(50)
#define RING_NUM	(20)

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static int				g_Time = 0;
static BOOL				g_Load = FALSE;

static GIMMICK g_GmIce[ICE_NUM];
static GIMMICK g_GmRing[RING_NUM];

static DX11_MODEL	g_Model[GIMMICK_MAX];	// �v���C���[�̃��f���Ǘ�


static float		g_Rotation = 0.0f;

//=============================================================================
// ����������
//=============================================================================
HRESULT InitGimmick(void)
{
	LoadModel("data/MODEL/ice_1.obj", &g_Model[GIMMICK_ICE]);
	LoadModel("data/MODEL/ring_1.obj", &g_Model[GIMMICK_RING]);

	for (int i = 0; i < ICE_NUM; i++)
	{
		g_GmIce[i].rotPosNo = (i * 5) % 8;
		g_GmIce[i].zPosNo = i * 20;
	}
	for (int i = 0; i < RING_NUM; i++)
	{
		g_GmRing[i].rotPosNo = (i * 5) % 8;
		g_GmRing[i].zPosNo = i * 50;
	}

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitGimmick(void)
{
	if (g_Load == FALSE) return;

	for (int i = 0; i < GIMMICK_MAX; i++)
	{
		UnloadModel(&g_Model[i]);
	}

	g_Load = FALSE;
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateGimmick(void)
{

#ifdef _DEBUG	// �f�o�b�O����\������
#endif
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawGimmick(void)
{
	static float d_pos = 0;
	d_pos -= GetPlayerSpeed();
	//XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;
	XMMATRIX mtxWorld;

	float zPos, rot;
	STAGE* pStage = GetStage(0);
	for (int i = 0; i < pStage->gmkNum; i++)
	{
		zPos = d_pos + MESH_SIZE * pStage->arrGmk[i].zPosNo;
		if (-100.0f > zPos || zPos > 10000.0f) continue;
		
		rot = XM_2PI * (float)pStage->arrGmk[i].rotPosNo / (float)MESH_NUM_X + GetTubeRotation();
		mtxWorld = XMMatrixIdentity();	// ���[���h�}�g���b�N�X�̏�����
		switch (pStage->arrGmk[i].type)
		{
		case GIMMICK_ICE:

			mtxWorld = XMMatrixMultiply(mtxWorld, XMMatrixScaling(4.0f, 3.0f, 4.0f));	// �X�P�[���𔽉f
			mtxWorld = XMMatrixMultiply(mtxWorld, XMMatrixRotationRollPitchYaw(0.0f, XM_PIDIV2, 0.0f));	// ��]�𔽉f�F�S�̂̊p�x
			mtxWorld = XMMatrixMultiply(mtxWorld, XMMatrixTranslation(0.0f, 120.0f, 0.0f));					// �ړ��𔽉f
			mtxWorld = XMMatrixMultiply(mtxWorld, XMMatrixRotationRollPitchYaw(0.0f, 0.0f, rot + XM_PIDIV2));	// ��]�𔽉f�F�X�̊p�x
			mtxWorld = XMMatrixMultiply(mtxWorld, XMMatrixTranslation(TUBE_RADIUS * 0.8f * cosf(rot), TUBE_RADIUS * 0.8f * sinf(rot), zPos));	// �ړ��𔽉f
			break;

		case GIMMICK_RING:
			mtxWorld = XMMatrixMultiply(mtxWorld, XMMatrixScaling(1.0f, 1.0f, 1.0f));	// �X�P�[���𔽉f
			mtxWorld = XMMatrixMultiply(mtxWorld, XMMatrixRotationRollPitchYaw(XM_PIDIV2, 0.0f, 0.0f));	// ��]�𔽉f�F�S�̂̊p�x
			mtxWorld = XMMatrixMultiply(mtxWorld, XMMatrixTranslation(0.0f, 50.0f, 0.0f));					// �ړ��𔽉f
			mtxWorld = XMMatrixMultiply(mtxWorld, XMMatrixRotationRollPitchYaw(0.0f, 0.0f, rot + XM_PIDIV2));	// ��]�𔽉f�F�X�̊p�x
			mtxWorld = XMMatrixMultiply(mtxWorld, XMMatrixTranslation(TUBE_RADIUS * 0.8f * cosf(rot), TUBE_RADIUS * 0.8f * sinf(rot), zPos));	// �ړ��𔽉f

			break;
		}
		SetWorldBuffer(&mtxWorld);	// ���[���h�}�g���b�N�X�̐ݒ�
		DrawModel(&g_Model[pStage->arrGmk[i].type]);	// ���f���`��
	}
	//// �X
	//for (int i = 0; i < ICE_NUM; i++)
	//{
	//	float zPos = d_pos + MESH_SIZE * g_GmIce[i].zPosNo;
	//	if (-100.0f > zPos || zPos > 10000.0f)
	//		continue;

	//	// ���[���h�}�g���b�N�X�̏�����
	//	mtxWorld = XMMatrixIdentity();

	//	// �X�P�[���𔽉f
	//	mtxScl = XMMatrixScaling(4.0f, 3.0f, 4.0f);
	//	mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

	//	float rot = XM_2PI * (float)g_GmIce[i].rotPosNo / (float)MESH_NUM_X + GetTubeRotation();

	//	// ��]�𔽉f�F�S�̂̊p�x
	//	mtxRot = XMMatrixRotationRollPitchYaw(0.0f, XM_PIDIV2, 0.0f);
	//	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

	//	// �ړ��𔽉f
	//	mtxTranslate = XMMatrixTranslation(0.0f, 120.0f, 0.0f);
	//	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

	//	// ��]�𔽉f�F�X�̊p�x
	//	mtxRot = XMMatrixRotationRollPitchYaw(0.0f, 0.0f, rot + XM_PIDIV2);
	//	//SetAfterRotation(&mtxRot);
	//	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

	//	// �ړ��𔽉f
	//	mtxTranslate = XMMatrixTranslation(TUBE_RADIUS * 0.8f * cosf(rot), TUBE_RADIUS* 0.8f * sinf(rot), zPos);
	//	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

	//	// ���[���h�}�g���b�N�X�̐ݒ�
	//	SetWorldBuffer(&mtxWorld);

	//	//XMStoreFloat4x4(&g_Gimmick.mtxWorld, mtxWorld);

	//	// ���f���`��
	//	DrawModel(&g_Model[GIMMICK_ICE]);
	//}
	//// �����O
	//for (int i = 0; i < RING_NUM; i++)
	//{
	//	float zPos = d_pos + MESH_SIZE * g_GmRing[i].zPosNo;
	//	if (-100.0f > zPos || zPos > 10000.0f) continue;

	//	// ���[���h�}�g���b�N�X�̏�����
	//	mtxWorld = XMMatrixIdentity();

	//	// �X�P�[���𔽉f
	//	mtxScl = XMMatrixScaling(1.0f, 1.0f, 1.0f);
	//	mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

	//	float rot = XM_2PI * (float)g_GmRing[i].rotPosNo / (float)MESH_NUM_X + GetTubeRotation();

	//	// ��]�𔽉f�F�S�̂̊p�x
	//	mtxRot = XMMatrixRotationRollPitchYaw(XM_PIDIV2, 0.0f, 0.0f);
	//	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

	//	// �ړ��𔽉f
	//	mtxTranslate = XMMatrixTranslation(0.0f, 50.0f, 0.0f);
	//	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

	//	// ��]�𔽉f�F�X�̊p�x
	//	mtxRot = XMMatrixRotationRollPitchYaw(0.0f, 0.0f, rot + XM_PIDIV2);
	//	//SetAfterRotation(&mtxRot);
	//	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

	//	// �ړ��𔽉f
	//	mtxTranslate = XMMatrixTranslation(TUBE_RADIUS * 0.8f * cosf(rot), TUBE_RADIUS* 0.8f * sinf(rot), zPos);
	//	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

	//	// ���[���h�}�g���b�N�X�̐ݒ�
	//	SetWorldBuffer(&mtxWorld);

	//	//XMStoreFloat4x4(&g_Gimmick.mtxWorld, mtxWorld);

	//	// ���f���`��
	//	DrawModel(&g_Model[GIMMICK_RING]);
	//}
}

bool CollisionGimmick(int stageNo, float oldZ, float newZ, float oldRot, float newRot)
{
	float oldZPosNoFloat = oldZ / MESH_SIZE;
	float newZPosNoFloat = newZ / MESH_SIZE;
	int oldZPosNoInt = (int)oldZPosNoFloat;
	int newZPosNoInt = (int)newZPosNoFloat;
	if (oldZPosNoInt == (int)newZPosNoInt) return false;
	float length = newZPosNoFloat - oldZPosNoFloat;
	while (oldZPosNoInt <= newZPosNoInt)
	{
		float rate = (1.0f - oldZPosNoFloat + (float)oldZPosNoInt) / length;
		int colZPosNo = oldZPosNoInt + 1;
		float colRot = oldRot + (newRot - oldRot) * rate;
		for (int i = 0; i < GetStage(stageNo)->gmkNum; i++)
		{
			if (GetStage(stageNo)->arrGmk[i].zPosNo == colZPosNo)
			{
				float rot = colRot + XM_PIDIV2 + XM_2PI * (float)GetStage(stageNo)->arrGmk[i].rotPosNo / (float)MESH_NUM_X;
				while (rot < 0.0f) rot += XM_2PI;
				while (rot > XM_2PI) rot -= XM_2PI;
				if (rot < GetStage(stageNo)->arrGmk[i].rotSizeHalf ||
					XM_2PI - GetStage(stageNo)->arrGmk[i].rotSizeHalf < rot)
				{
					switch (GetStage(stageNo)->arrGmk[i].type)
					{
					case GIMMICK_ICE:
						SetDamageEffect();
						SetPlayerCollisionIce();
						break;
					case GIMMICK_RING:
						SetBoostEffect();
						SetPlayerThroughRing();
						break;
					}
					return true;
				}
			}
		}
		//for (int i = 0; i < ICE_NUM; i++)
		//{
		//	if (g_GmIce[i].zPosNo == colZPosNo)
		//	{
		//		float rot = colRot + XM_PIDIV2 + XM_2PI * (float)g_GmIce[i].rotPosNo / (float)MESH_NUM_X;
		//		while (rot < 0.0f) rot += XM_2PI;
		//		while (rot > XM_2PI) rot -= XM_2PI;
		//		if (rot < g_GmIce[i].rotSizeHalf ||
		//			XM_2PI - g_GmIce[i].rotSizeHalf < rot)
		//		{
		//			SetDamageEffect();
		//			SetPlayerCollisionIce();
		//			return true;
		//		}
		//	}
		//}
		//for (int i = 0; i < RING_NUM; i++)
		//{
		//	if (g_GmRing[i].zPosNo == colZPosNo)
		//	{
		//		float rot = colRot + XM_PIDIV2 + XM_2PI * (float)g_GmRing[i].rotPosNo / (float)MESH_NUM_X;
		//		while (rot < 0.0f) rot += XM_2PI;
		//		while (rot > XM_2PI) rot -= XM_2PI;
		//		if (rot < g_GmRing[i].rotSizeHalf ||
		//			XM_2PI - g_GmRing[i].rotSizeHalf < rot)
		//		{
		//			SetBoostEffect();
		//			SetPlayerThroughRing();
		//			return true;
		//		}
		//	}
		//}
		oldZPosNoInt++;
	}
	return false;
}