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
static int			g_Time = 0;
static BOOL			g_Load = FALSE;

static GIMMICK		g_GmIce[ICE_NUM];
static GIMMICK		g_GmRing[RING_NUM];

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
void DrawGimmick(GIMMICK_TYPE gimmick)
{
	XMMATRIX mtxWorld;
	float zPos, rot;
	float d_pos = -GetPlayerPosition();
	STAGE* pStage = GetStage(0);
	for (int i = 0; i < pStage->gmkNum; i++)
	{
		if (pStage->arrGmk[i].type != gimmick) continue;

		zPos = d_pos + MESH_SIZE_Z * pStage->arrGmk[i].zPosNo;
		if (zPos < -500.0f || 10000.0f < zPos)
			continue;
		
		rot = XM_2PI * (float)pStage->arrGmk[i].rotPosNo / (float)MESH_NUM_X + GetTubeRotation() + XM_PIDIV2;
		mtxWorld = XMMatrixIdentity();	// ���[���h�}�g���b�N�X�̏�����

		MATERIAL material;
		switch (gimmick)
		{
		case GIMMICK_ICE:
			//MulMtxScl(mtxWorld, 4.0f, 1.8f, 4.0f);				// �X�P�[���𔽉f
			//MulMtxRot(mtxWorld, 0.0f, -XM_PIDIV4 + 0.3f, 0.0f);	// ��]�𔽉f�F�S�̂̊p�x
			//MulMtxPos(mtxWorld, 0.0f, 80.0f, 0.0f);				// �ړ��𔽉f
			//MulMtxRot(mtxWorld, 0.0f, 0.0f, rot + XM_PIDIV2);	// ��]�𔽉f�F�X�̊p�x
			//MulMtxPos(mtxWorld, TUBE_RADIUS * 0.8f * cosf(rot), TUBE_RADIUS * 0.8f * sinf(rot), zPos);	// �ړ��𔽉f
			MulMtxScl(mtxWorld, 4.0f, 1.8f, 4.0f);				// �X�P�[���𔽉f
			MulMtxRot(mtxWorld, 0.0f, -XM_PIDIV4 + 0.3f, rot + XM_PIDIV2);	// ��]�𔽉f�F�S�̂̊p�x
			MulMtxPos(mtxWorld, (TUBE_RADIUS - 80.0f) * 0.8f * cosf(rot), (TUBE_RADIUS - 80.0f) * 0.8f * sinf(rot), zPos);				// �ړ��𔽉f
			break;

		case GIMMICK_RING:
			material.Diffuse = { 2.0f, 2.0f, 0.0f, 2.0f };
			MulMtxScl(mtxWorld, 1.0f, 1.0f, 1.0f);				// �X�P�[���𔽉f
			MulMtxRot(mtxWorld, XM_PIDIV2, 0.0f, 0.0f);			// ��]�𔽉f�F�S�̂̊p�x
			MulMtxPos(mtxWorld, 0.0f, 50.0f, 0.0f);				// �ړ��𔽉f
			MulMtxRot(mtxWorld, 0.0f, 0.0f, rot + XM_PIDIV2);	// ��]�𔽉f�F�X�̊p�x
			MulMtxPos(mtxWorld, TUBE_RADIUS * 0.8f * cosf(rot), TUBE_RADIUS * 0.8f * sinf(rot), zPos);	// �ړ��𔽉f
			break;
		}
		DrawModel(&g_Model[pStage->arrGmk[i].type], &mtxWorld, NULL, &material);	// ���f���`��
	}
}
void DrawGimmickInstancing(GIMMICK_TYPE gimmick)
{
	// �C���X�^���X����o�^
	D3D11_MAPPED_SUBRESOURCE msr;
	GetDeviceContext()->Map(GetInstanceBuffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
	INSTANCE* b_pInstance = (INSTANCE*)msr.pData;
	int instCount = 0;

	MATERIAL material;
	XMMATRIX mtxWorld;
	float zPos, rot;
	float d_pos = -GetPlayerPosition();
	STAGE* pStage = GetStage(0);
	for (int i = 0; i < pStage->gmkNum; i++)
	{
		if (pStage->arrGmk[i].type != gimmick) continue;

		zPos = d_pos + MESH_SIZE_Z * pStage->arrGmk[i].zPosNo;
		if (zPos < -500.0f || 10000.0f < zPos)
			continue;

		rot = XM_2PI * (float)pStage->arrGmk[i].rotPosNo / (float)MESH_NUM_X + GetTubeRotation() + XM_PIDIV2;
		mtxWorld = XMMatrixIdentity();	// ���[���h�}�g���b�N�X�̏�����


		switch (gimmick)
		{
		case GIMMICK_ICE:
			b_pInstance->scl[instCount] = { 4.0f, 1.8f, 4.0f , 0.0f };
			b_pInstance->rot[instCount] = { 0.0f, -XM_PIDIV4 + 0.3f, rot + XM_PIDIV2, 0.0f };
			b_pInstance->pos[instCount] = { (TUBE_RADIUS - 80.0f) * 0.8f * cosf(rot), (TUBE_RADIUS - 80.0f) * 0.8f * sinf(rot), zPos, 0.0f };
			b_pInstance->col[instCount] = { 1.0f, 1.0f, 1.0f, 1.0f };
			break;

		case GIMMICK_RING:
			b_pInstance->scl[instCount] = { 4.0f, 1.8f, 4.0f , 0.0f };
			b_pInstance->rot[instCount] = { 0.0f, XM_PIDIV2, rot + XM_PIDIV2, 0.0f };
			b_pInstance->pos[instCount] = { (TUBE_RADIUS - 50.0f) * 0.8f * cosf(rot), (TUBE_RADIUS - 50.0f) * 0.8f * sinf(rot), zPos, 0.0f };
			b_pInstance->col[instCount] = { 2.0f, 2.0f, 0.0f, 2.0f };
			break;
		}
		// �C���X�^���X�����X�V
		instCount++;
	}
	GetDeviceContext()->Unmap(GetInstanceBuffer(), 0);
	DrawModelInstanced(&g_Model[gimmick], instCount);	// ���f���`��
}

bool CollisionGimmick(int stageNo, float oldZ, float newZ, float oldRot, float newRot)
{
	float oldZPosNoFloat = oldZ / MESH_SIZE_Z;
	float newZPosNoFloat = newZ / MESH_SIZE_Z;
	int oldZPosNoInt = (int)oldZPosNoFloat;
	int newZPosNoInt = (int)newZPosNoFloat;
	if (oldZPosNoInt == (int)newZPosNoInt) return false;
	float length = newZPosNoFloat - oldZPosNoFloat;
	while (oldZPosNoInt <= newZPosNoInt)
	{
		float rate = (1.0f - oldZPosNoFloat + (float)oldZPosNoInt) / length;
		int colZPosNo = oldZPosNoInt + 1;
		float colRot = oldRot + (newRot - oldRot) * rate + XM_PIDIV2;
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