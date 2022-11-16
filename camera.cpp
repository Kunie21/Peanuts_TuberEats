//=============================================================================
//
// �J�������� [camera.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "input.h"
#include "debugproc.h"
#include "camera.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static XMMATRIX				g_CameraViewMatrix;
static XMMATRIX				g_CameraInvViewMatrix;
static XMMATRIX				g_CameraProjectionMatrix;
struct CAMERA_DESC
{
	XMFLOAT3 pos;
	XMFLOAT3 rot;
	XMFLOAT3 at;
	XMFLOAT3 up;
	float len;
};
static CAMERA_DESC			g_Camera;

//=============================================================================
// ����������
//=============================================================================
void InitCamera(void)
{
	g_Camera.pos = XMFLOAT3(0.0f, 0.0f, CAMERA_POS_Z);
	g_Camera.rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
	g_Camera.at = XMFLOAT3(0.0f, 0.0f, 1.0f);
	g_Camera.up = XMFLOAT3(0.0f, 1.0f, 0.0f);

	// ���_�ƒ����_�̋�����v�Z
	float vx, vz;
	vx = g_Camera.pos.x - g_Camera.at.x;
	vz = g_Camera.pos.z - g_Camera.at.z;
	g_Camera.len = sqrtf(vx * vx + vz * vz);

	//SetCameraPosition(&g_CameraPosition);

	CAMERA c;
	c.Position = Float4(g_Camera.pos);
	c.ViewVolume = XMFLOAT4(SCREEN_WIDTH, SCREEN_HEIGHT, VIEW_NEAR_Z, VIEW_FAR_Z);
	SetCameraBuffer(&c);
}

//=============================================================================
// �J�����̏I������
//=============================================================================
void UninitCamera(void) {}

//=============================================================================
// �J�����̍X�V����
//=============================================================================
void UpdateCamera(void)
{
#ifdef _DEBUG

	if (GetKeyboardPress(DIK_Z))
	{// ���_����u���v
		g_Camera.rot.y += VALUE_ROTATE_CAMERA;
		if (g_Camera.rot.y > XM_PI)
		{
			g_Camera.rot.y -= XM_PI * 2.0f;
		}

		g_Camera.pos.x = g_Camera.at.x - sinf(g_Camera.rot.y) * g_Camera.len;
		g_Camera.pos.z = g_Camera.at.z - cosf(g_Camera.rot.y) * g_Camera.len;
	}

	if (GetKeyboardPress(DIK_C))
	{// ���_����u�E�v
		g_Camera.rot.y -= VALUE_ROTATE_CAMERA;
		if (g_Camera.rot.y < -XM_PI)
		{
			g_Camera.rot.y += XM_PI * 2.0f;
		}

		g_Camera.pos.x = g_Camera.at.x - sinf(g_Camera.rot.y) * g_Camera.len;
		g_Camera.pos.z = g_Camera.at.z - cosf(g_Camera.rot.y) * g_Camera.len;
	}

	if (GetKeyboardPress(DIK_Y))
	{// ���_�ړ��u��v
		g_Camera.pos.y += VALUE_MOVE_CAMERA;
	}

	if (GetKeyboardPress(DIK_N))
	{// ���_�ړ��u���v
		g_Camera.pos.y -= VALUE_MOVE_CAMERA;
	}

	if (GetKeyboardPress(DIK_Q))
	{// �����_����u���v
		g_Camera.rot.y -= VALUE_ROTATE_CAMERA;
		if (g_Camera.rot.y < -XM_PI)
		{
			g_Camera.rot.y += XM_PI * 2.0f;
		}

		g_Camera.at.x = g_Camera.pos.x + sinf(g_Camera.rot.y) * g_Camera.len;
		g_Camera.at.z = g_Camera.pos.z + cosf(g_Camera.rot.y) * g_Camera.len;
	}

	if (GetKeyboardPress(DIK_E))
	{// �����_����u�E�v
		g_Camera.rot.y += VALUE_ROTATE_CAMERA;
		if (g_Camera.rot.y > XM_PI)
		{
			g_Camera.rot.y -= XM_PI * 2.0f;
		}

		g_Camera.at.x = g_Camera.pos.x + sinf(g_Camera.rot.y) * g_Camera.len;
		g_Camera.at.z = g_Camera.pos.z + cosf(g_Camera.rot.y) * g_Camera.len;
	}

	if (GetKeyboardPress(DIK_T))
	{// �����_�ړ��u��v
		g_Camera.at.y += VALUE_MOVE_CAMERA;
	}

	if (GetKeyboardPress(DIK_B))
	{// �����_�ړ��u���v
		g_Camera.at.y -= VALUE_MOVE_CAMERA;
	}

	if (GetKeyboardPress(DIK_U))
	{// �߂Â�
		g_Camera.len -= VALUE_MOVE_CAMERA;
		g_Camera.pos.x = g_Camera.at.x - sinf(g_Camera.rot.y) * g_Camera.len;
		g_Camera.pos.z = g_Camera.at.z - cosf(g_Camera.rot.y) * g_Camera.len;
	}

	if (GetKeyboardPress(DIK_M))
	{// �����
		g_Camera.len += VALUE_MOVE_CAMERA;
		g_Camera.pos.x = g_Camera.at.x - sinf(g_Camera.rot.y) * g_Camera.len;
		g_Camera.pos.z = g_Camera.at.z - cosf(g_Camera.rot.y) * g_Camera.len;
	}

	// �J����������ɖ߂�
	if (GetKeyboardPress(DIK_R))
	{
		UninitCamera();
		InitCamera();
	}

	//if (GetKeyboardPress(DIK_UP))
	//{// �߂Â�
	//	g_Camera.pos.z += 50.0f;
	//	g_Camera.at.z += 50.0f;
	//}
	//if (GetKeyboardPress(DIK_DOWN))
	//{// �����
	//	g_Camera.pos.z -= 50.0f;
	//	g_Camera.at.z -= 50.0f;
	//}
	//if (GetKeyboardPress(DIK_RIGHT))
	//{// �����
	//	g_Camera.pos.x += 50.0f;
	//	g_Camera.at.x += 50.0f;
	//}
	//if (GetKeyboardPress(DIK_LEFT))
	//{// �߂Â�
	//	g_Camera.pos.x -= 50.0f;
	//	g_Camera.at.x -= 50.0f;
	//}

	//PrintDebugProc("Camera:ZC QE TB YN UM R\n");
#endif
}

//=============================================================================
// �J�����̍X�V
//=============================================================================
void SetCamera(void) 
{
	// �r���[�s��ݒ�
	g_CameraViewMatrix = XMMatrixLookAtLH(XMLoadFloat3(&g_Camera.pos), XMLoadFloat3(&g_Camera.at), XMLoadFloat3(&g_Camera.up));
	SetViewBuffer(&g_CameraViewMatrix);

	// �v���W�F�N�V�����s��ݒ�
	g_CameraProjectionMatrix = XMMatrixPerspectiveFovLH(VIEW_ANGLE, VIEW_ASPECT, VIEW_NEAR_Z, VIEW_FAR_Z);
	SetProjectionBuffer(&g_CameraProjectionMatrix);
}
