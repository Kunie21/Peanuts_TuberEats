//=============================================================================
//
// �p�C�v���C���ǂ̏��� [tube.h]
// Author : ���] �đ�
//
//=============================================================================
#pragma once

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	TUBE_RADIUS		(200.0f)	// �f�ʂ̔��a
#define	MESH_NUM_X		(8)			// �f�ʂ̊p��
#define	MESH_NUM_X2		(36)			// �f�ʂ̊p��
#define	MESH_SIZE_X		(TUBE_RADIUS * 2.0f * XM_PI / MESH_NUM_X2)	// �}�X��1�ӂ̒���
#define	MESH_SIZE_Z		(200.0f)	// �}�X��1�ӂ̒���
#define	MESH_NUM_Z		(150)		// ���s���̃}�X��
#define	TUBE_SIZE		(MESH_SIZE_Z * MESH_NUM_Z)	// 1�{�̒���
//#define	CURVE_RADIUS	(TUBE_SIZE * 0.5f)
//#define	CURVE_CENTER	(CURVE_RADIUS * sinf(XM_PIDIV4))

#define TUBE_TEX_X (2.0f)
#define TUBE_TEX_Z (10.0f)

#define DRAW_DIST_NEAR	(-1000.0f)
#define DRAW_DIST_FAR	(15000.0f)

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitTube(void);
void UninitTube(void);
void UpdateTube(void);
void DrawTube(void);
void DrawTubeLight(void);
void DrawTubeLight2(void);
void RotateTube(float rot);
//void MoveTube(float move);
float GetTubeRotation(void);
//void SetTubeCurveRight(int tube);
//void TestCurveTube(float move);
//void PresentTube(void);
//void SetNextTube(void);

void DrawTubeResult(float pos_z);
