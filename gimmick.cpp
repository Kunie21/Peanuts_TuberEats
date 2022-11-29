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

#define EX_LENGTH		(1000.0f)
#define EX_ACCEL		(0.98f)

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static int			g_Time = 0;
static BOOL			g_Load = FALSE;

//static GIMMICK		g_GmIce[ICE_NUM];
//static GIMMICK		g_GmRing[RING_NUM];

static MODEL_LABEL	g_Model[GIMMICK_MAX] = {
	MODEL_ICE,
	MODEL_RING
};	// �v���C���[�̃��f���Ǘ�


static float		g_Rotation = 0.0f;

//=============================================================================
// ����������
//=============================================================================
HRESULT InitGimmick(void)
{
	//for (int i = 0; i < ICE_NUM; i++)
	//{
	//	g_GmIce[i].rotPosNo = (i * 5) % 8;
	//	g_GmIce[i].zPosNo = i * 20;
	//}
	//for (int i = 0; i < RING_NUM; i++)
	//{
	//	g_GmRing[i].rotPosNo = (i * 5) % 8;
	//	g_GmRing[i].zPosNo = i * 50;
	//	g_GmRing[i].col = { 2.0f, 2.0f, 0.0f, 2.0f };
	//}


	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitGimmick(void)
{
	if (g_Load == FALSE) return;

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
	STAGE* pStage = GetStage(0);
	for (int i = 0; i < pStage->gmkNum; i++)
	{
		if (pStage->arrGmk[i].type != gimmick) continue;

		zPos = GetZPos(pStage->arrGmk[i].zPosNo);
		if (zPos < -500.0f || 10000.0f < zPos)
			continue;
		
		rot = GetRotPos(pStage->arrGmk[i].rotPosNo);
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
		DrawModel(&g_Model[pStage->arrGmk[i].type], &mtxWorld, &material);	// ���f���`��
	}
}
void DrawGimmickInstancing(GIMMICK_TYPE gimmick, BOOL bOutline)
{
	// �C���X�^���X����o�^
	D3D11_MAPPED_SUBRESOURCE msr;
	GetDeviceContext()->Map(GetInstanceBuffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
	INSTANCE* b_pInstance = (INSTANCE*)msr.pData;
	int instCount = 0;

	float zPos, rot;
	STAGE2* pStage = GetStage2(STAGE_OSAKA);
	for (int i = 0; i < pStage->gmkNum; i++)
	{
		if (!pStage->arrGmk[i].use) {
			if (bOutline) continue;
			if (pStage->arrGmk[i].exSpd > 5.0f) {
				pStage->arrGmk[i].exPos += pStage->arrGmk[i].exSpd;
				pStage->arrGmk[i].exSpd *= EX_ACCEL;
				pStage->arrGmk[i].col.w *= EX_ACCEL * 1.01f;
			}
			else {
				continue;
			}
		}
		if (pStage->arrGmk[i].type != gimmick) continue;

		zPos = GetZPos(pStage->arrGmk[i].zPosNo);
		if (zPos < -500.0f || 10000.0f < zPos)
			continue;

		rot = GetRotPos(pStage->arrGmk[i].rotPosNo);

		switch (gimmick)
		{
		case GIMMICK_ICE:
			b_pInstance->scl[instCount] = { 4.0f, 1.4f, 4.0f, 0.0f };
			b_pInstance->rot[instCount] = { 0.0f, XM_PI - XM_PIDIV4 + 0.3f, rot + XM_PIDIV2, 0.0f };
			b_pInstance->pos[instCount] = { (TUBE_RADIUS - 70.0f) * 0.8f * cosf(rot), (TUBE_RADIUS - 70.0f) * 0.8f * sinf(rot), zPos, pStage->arrGmk[i].exPos };
			b_pInstance->col[instCount] = pStage->arrGmk[i].col;
			break;

		case GIMMICK_RING:
			b_pInstance->scl[instCount] = { 0.8f, 0.8f, 0.8f, 0.0f };
			b_pInstance->rot[instCount] = { XM_PIDIV2, XM_PI, rot + XM_PIDIV2, 0.0f };
			b_pInstance->pos[instCount] = { (TUBE_RADIUS - 80.0f) * 0.8f * cosf(rot), (TUBE_RADIUS - 80.0f) * 0.8f * sinf(rot), zPos, pStage->arrGmk[i].exPos };
			//b_pInstance->col[instCount] = pStage->arrGmk[i].col;
			b_pInstance->col[instCount] = { 2.0f, 2.0f, 0.0f, 2.0f };
			break;
		}
		// �C���X�^���X�����X�V
		instCount++;
	}
	GetDeviceContext()->Unmap(GetInstanceBuffer(), 0);

	if (instCount > 0)
	{
		SetWorldBuffer(&XMMatrixIdentity());	// ���[���h�}�g���b�N�X�̐ݒ�
		static MATERIAL material;
		DrawModelInstanced(&g_Model[gimmick], instCount, &material);	// ���f���`��
	}
}

bool CollisionGimmick(int stageNo, float oldZ, float newZ, float oldRot, float newRot)
{
	float oldZPosNoFloat = oldZ / MESH_SIZE_Z;
	float newZPosNoFloat = newZ / MESH_SIZE_Z;
	int oldZPosNoInt = (int)oldZPosNoFloat;
	int newZPosNoInt = (int)newZPosNoFloat;
	if (oldZPosNoInt == (int)newZPosNoInt) return false;
	float length = newZPosNoFloat - oldZPosNoFloat;
	STAGE2* pStage = GetStage2(stageNo);
	while (oldZPosNoInt <= newZPosNoInt)
	{
		float rate = (1.0f - oldZPosNoFloat + (float)oldZPosNoInt) / length;
		int colZPosNo = oldZPosNoInt + 1;
		float colRot = oldRot + (newRot - oldRot) * rate + XM_PIDIV2;
		for (int i = 0; i < pStage->gmkNum; i++)
		{
			if (pStage->arrGmk[i].use && pStage->arrGmk[i].zPosNo == colZPosNo)
			{
				float rot = colRot + XM_PIDIV2 + XM_2PI * (float)pStage->arrGmk[i].rotPosNo / (float)MESH_NUM_X;
				while (rot < -XM_PI) rot += XM_2PI;
				while (rot > XM_PI) rot -= XM_2PI;
				if (rot < pStage->arrGmk[i].rotSizeHalf &&
					-pStage->arrGmk[i].rotSizeHalf < rot)
				//while (rot < 0.0f) rot += XM_2PI;
				//while (rot > XM_2PI) rot -= XM_2PI;
				//if (rot < pStage->arrGmk[i].rotSizeHalf ||
				//	XM_2PI - pStage->arrGmk[i].rotSizeHalf < rot)
				{
					switch (pStage->arrGmk[i].type)
					{
					case GIMMICK_ICE:
						SetDamageEffect();
						SetPlayerCollisionIce();
						break;
					case GIMMICK_RING:
						SetBoostEffect();
						SetPlayerThroughRing();
						//pStage->arrGmk[i].use = FALSE;
						break;
					case GIMMICK_CRACK:
						SetDamageEffect();
						if (rot > 0.0f) {
							SetPlayerCollisionBlast(1.0f - (rot / pStage->arrGmk[i].rotSizeHalf));
						}
						else {
							SetPlayerCollisionBlast(-1.0f - (rot / pStage->arrGmk[i].rotSizeHalf));
						}
						break;
					}
					return true;
				}
			}
		}
		oldZPosNoInt++;
	}
	return false;
}
bool CollisionMissile(int stageNo, float oldZ, float newZ, float oldRot, float newRot)
{
	float oldZPosNoFloat = oldZ / MESH_SIZE_Z;
	float newZPosNoFloat = newZ / MESH_SIZE_Z;
	int oldZPosNoInt = (int)oldZPosNoFloat;
	int newZPosNoInt = (int)newZPosNoFloat;
	if (oldZPosNoInt == (int)newZPosNoInt) return false;
	float length = newZPosNoFloat - oldZPosNoFloat;
	STAGE2* pStage = GetStage2(stageNo);
	while (oldZPosNoInt <= newZPosNoInt)
	{
		float rate = (1.0f - oldZPosNoFloat + (float)oldZPosNoInt) / length;
		int colZPosNo = oldZPosNoInt + 1;
		float colRot = oldRot + (newRot - oldRot) * rate + XM_PIDIV2;
		for (int i = 0; i < pStage->gmkNum; i++)
		{
			if (pStage->arrGmk[i].use && pStage->arrGmk[i].zPosNo == colZPosNo)
			{
				float rot = colRot + XM_PIDIV2 + XM_2PI * (float)pStage->arrGmk[i].rotPosNo / (float)MESH_NUM_X;
				while (rot < 0.0f) rot += XM_2PI;
				while (rot > XM_2PI) rot -= XM_2PI;
				if (rot < pStage->arrGmk[i].rotSizeHalf ||
					XM_2PI - pStage->arrGmk[i].rotSizeHalf < rot)
				{
					switch (pStage->arrGmk[i].type)
					{
					case GIMMICK_ICE:
						pStage->arrGmk[i].use = FALSE;
						return true;
						break;
					//case GIMMICK_RING:
					//	break;
					default:
						break;
					}
				}
			}
		}
		oldZPosNoInt++;
	}
	return false;
}