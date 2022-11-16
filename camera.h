//=============================================================================
//
// �J�������� [camera.h]
// Author : ���] �đ�
//
//=============================================================================
#pragma once

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	VALUE_MOVE_CAMERA	(2.0f)			// �J�����̈ړ���
#define	VALUE_ROTATE_CAMERA	(XM_PI * 0.01f)	// �J�����̉�]��

#define	VIEW_ANGLE		(XMConvertToRadians(45.0f))						// �r���[���ʂ̎���p
#define	VIEW_ASPECT		((float)SCREEN_WIDTH / (float)SCREEN_HEIGHT)	// �r���[���ʂ̃A�X�y�N�g��	
#define	VIEW_NEAR_Z		(10.0f)											// �r���[���ʂ�NearZ�l
#define	VIEW_FAR_Z		(20000.0f)										// �r���[���ʂ�FarZ�l

#define CAMERA_POS_Z	(-310.0f)

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
void InitCamera(void);
void UninitCamera(void);
void UpdateCamera(void);
void SetCamera(void);