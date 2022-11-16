//=============================================================================
//
// �X�^�[�g�A�j���[�V�������� [anim_start.cpp]
// Author : ���] �đ�
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "input.h"
#include "anim_start.h"
#include "camera.h"
#include "player.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define RADIUS			(220.0f)	// ��荞�݂̉~�̔��a
#define ROUND_CENTER_Z	(RADIUS + CAMERA_POS_Z)	// ��荞�݂̉~�̒��S
#define BOTTOM_Y		(-10.0f)	// �h���[�A�E�g���ɉ��ɉ������
#define ZOOM_Z			(-50.0f)	// �h���[�A�E�g���Ɍ��ɉ������

// �J�b�g�̎��
enum START_ANIM_CUT_LABEL {
	SAC_DOLLY_OUT = 0,		// �h���[�A�E�g�i���ɉ�����j
	SAC_ROUND_CRANE_TILT,	// ��荞�݁��N���[���ō�����p�C�v�����Ɉړ����e�B���g�Ŏ�����܂������Ɍ�����
};
#define SAC_DO_TIME		(120.0f)	// SAC_DOLLY_OUT�J�b�g�̑S�̎���
#define SAC_RCT_TIME	(180.0f)	// SAC_ROUND_CRANE_TILT�J�b�g�̑S�̎���

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static BOOL		g_Load = FALSE;
static BOOL		g_Use = FALSE;
static XMFLOAT3 g_CameraPos, g_CameraAt, g_CameraUp;

//=============================================================================
// ����������
//=============================================================================
HRESULT InitAnimStart(void)
{
	g_CameraPos = { 0.0f, ROCKET_Y, RADIUS + ROUND_CENTER_Z };
	g_CameraAt = { 0.0f, ROCKET_Y, 0.0f };
	g_CameraUp = { 0.0f, 1.0f, 0.0f };

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitAnimStart(void)
{
	if (g_Load == FALSE) return;
	
	g_Load = FALSE;
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateAnimStart(void)
{
	if (GetKeyboardTrigger(DIK_L)) { SetCameraAnimStart(); }
	if (!g_Use) return;
	static float time = 0.0f;
	static START_ANIM_CUT_LABEL animNo = SAC_DOLLY_OUT;
	float timeRate, timeAngle;
	switch (animNo)
	{
	case SAC_DOLLY_OUT:
		if (time > SAC_DO_TIME) {
			time = 0.0f; animNo = SAC_ROUND_CRANE_TILT; break;
		}
		timeRate = time / SAC_DO_TIME;
		g_CameraAt = { 0.0f, ROCKET_Y, 0.0f };
		g_CameraPos = {
			0.0f,
			ROCKET_Y + BOTTOM_Y * timeRate,
			RADIUS + ROUND_CENTER_Z + ZOOM_Z * (1.0f - timeRate)
		};
		break;

	case SAC_ROUND_CRANE_TILT:
		if (time > SAC_RCT_TIME) {
			time = 0.0f; animNo = SAC_DOLLY_OUT; g_Use = FALSE; break;
		}
		timeRate = time / SAC_RCT_TIME;
		timeAngle = timeRate * XM_PI;
		g_CameraAt = { 0.0f, ROCKET_Y * (1.0f - timeRate), 0.0f };
		g_CameraPos = {
			-RADIUS * sinf(timeAngle),
			(ROCKET_Y + BOTTOM_Y) * (1.0f - timeRate),
			RADIUS * cosf(timeAngle) + ROUND_CENTER_Z
		};
		break;

	//case SAC_TILT:
	//	if (time > SAC_TILT_TIME) {
	//		time = 0.0f; animNo = SAC_FULL; g_Use = FALSE; break;
	//	}
	//	//timeRate = time / SAC_TILT_TIME;
	//	//g_CameraAt = { 0.0f, ROCKET_Y * (1.0f - timeRate), 0.0f };
	//	//g_CameraPos = { 0.0f, 0.0f, -RADIUS + ROUND_CENTER_Z };
	//	break;

	default:
		break;
	}
	time += 1.0f;
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawAnimStart(void)
{
	if (!g_Use) return;
	SetViewBuffer(&XMMatrixLookAtLH(XMLoadFloat3(&g_CameraPos), XMLoadFloat3(&g_CameraAt), XMLoadFloat3(&g_CameraUp)));
}

void SetCameraAnimStart(void) {
	g_Use = TRUE;
}