//=============================================================================
//
// ���C�g���� [light.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "light.h"
#include "camera.h"
#include "game.h"


//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define MAINLIGHT_POS_X		(-500.0f)
#define MAINLIGHT_POS_Y		(500.0f)
#define MAINLIGHT_POS_Z		(500.0f)
#define POINTLIGHT_POS_Y	(-20.0f)
#define	LIGHT_WALL_DIS		(30.0f)		// ���C�g�ʒu�̕ǂƂ̋���

//*********************************************************
// �\����
//*********************************************************
struct LIGHTVIEW						// ���C���̃��C�g�ɂ��V���h�E�}�b�v�쐬�p�r���[
{
	XMFLOAT4X4			mtxView;		// �r���[�}�g���b�N�X
	XMFLOAT4X4			mtxInvView;		// �r���[�}�g���b�N�X
	XMFLOAT4X4			mtxProjection;	// �v���W�F�N�V�����}�g���b�N�X

	XMFLOAT3			up;				// ���C�g�̏�����x�N�g��
	XMFLOAT3			at;				// ���C�g�̒����_

	float				viewAngle;
	float				viewAspect;
	float				nearZ;
	float				farZ;
};


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static LIGHT		g_Light[LIGHT_MAX];
static LIGHTVIEW	g_LightView;

static FOG		g_Fog;


//=============================================================================
// ����������
//=============================================================================
void InitLight(void)
{

	// ���C�����C�g�r���[������
	ZeroMemory(&g_LightView, sizeof(LIGHTVIEW));
	g_LightView.up = { 0.0f, 1.0f, 0.0f };
	g_LightView.at = { 0.0f, 0.0f, 0.0f };
	g_LightView.viewAngle = (XMConvertToRadians(60.0f));
	g_LightView.viewAspect = ((float)SCREEN_WIDTH / (float)SCREEN_HEIGHT);
	g_LightView.nearZ = 10.0f;
	g_LightView.farZ = 10000.0f;



	//���C�g������
	for (int i = 0; i < LIGHT_MAX; i++)
	{
		g_Light[i].Position  = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Light[i].Direction = XMFLOAT3( 0.0f, -1.0f, 0.0f );
		g_Light[i].Diffuse   = XMFLOAT4( 1.0f, 1.0f, 1.0f, 1.0f );
		g_Light[i].Ambient   = XMFLOAT4( 0.0f, 0.0f, 0.0f, 0.0f );
		g_Light[i].Attenuation = 200.0f;	// ��������
		g_Light[i].Type = LIGHT_TYPE_NONE;	// ���C�g�̃^�C�v
		g_Light[i].Enable = FALSE;			// ON / OFF
		SetLight(i, &g_Light[i]);
	}

	// ���s�����̐ݒ�i���E���Ƃ炷���j
	XMFLOAT3 dir;
	g_Light[0].Position = XMFLOAT3(MAINLIGHT_POS_X, MAINLIGHT_POS_Y, -MAINLIGHT_POS_Z);		// ���C�g�̈ʒu
	dir = SetMainLightDirectionFromPos();					// �ʒu���璍���_(���[���h���_)�ւ̌������Z�o
	g_Light[0].Direction = XMFLOAT3(dir.x, dir.y, dir.z);		// ���̌���
	g_Light[0].Diffuse   = XMFLOAT4( 1.0f, 1.0f, 1.0f, 1.0f );	// ���̐F
	g_Light[0].Type = LIGHT_TYPE_DIRECTIONAL;					// ���s����
	g_Light[0].Enable = TRUE;									// ���̃��C�g��ON
	SetLight(0, &g_Light[0]);									// ����Őݒ肵�Ă���


	// �|�C���g���C�g�̐ݒ�i�J�G���̓���ɌŒ�j
	g_Light[1].Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	g_Light[1].Direction = XMFLOAT3(0.0f, -0.9f, 0.0f);
	g_Light[1].Diffuse = XMFLOAT4(0.9f, 0.9f, 0.9f, 1.0f);
	g_Light[1].Ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	g_Light[1].Attenuation = 75.0f;	// ��������
	g_Light[1].Type = LIGHT_TYPE_POINT;	// ���C�g�̃^�C�v
	g_Light[1].Enable = TRUE;			// ON / OFF
	SetLight(1, &g_Light[1]);


	// �t�H�O�̏������i���̌��ʁj
	g_Fog.FogStart = 100.0f;									// ���_���炱�̋��������ƃt�H�O��������n�߂�
	g_Fog.FogEnd   = 20000.0f;									// �����܂ŗ����ƃt�H�O�̐F�Ō����Ȃ��Ȃ�
	g_Fog.FogColor = XMFLOAT4(0.8f, 0.95f, 1.0f, 1.0f);		// �t�H�O�̐F
	//g_Fog.FogColor = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);		// �t�H�O�̐F
	SetFog(&g_Fog);
	SetFogEnable(TRUE);		// ���̏ꏊ���`�F�b�N���� shadow

}

void SetFogColor(XMFLOAT4 color)
{
	g_Fog.FogColor = color;
	SetFog(&g_Fog);
}

void SetFogStartAndEnd(float start, float end)
{
	g_Fog.FogStart = start;
	g_Fog.FogEnd = end;
	SetFog(&g_Fog);
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateLight(void)
{
	//PLAYER *player = GetPlayer();
	////int mode = GetGameMode();

	////if (mode == GAME_MODE_STAGESELECT)
	////{
	////	CAMERA *camera = GetCamera();

	////	//g_Light[0].Position.x = MAINLIGHT_POS_X + camera[0].pos.x;
	////	g_Light[0].Position.y = MAINLIGHT_POS_Y + camera[0].pos.y + 100.0f;
	////	//g_Light[0].Position.z = MAINLIGHT_POS_Z + camera[0].pos.z;
	////}
	////else
	//{
	//	// ���C�g�̍��W���v���C���[�̈ʒu�ɍ��킹��
	//	g_Light[0].Position.x = MAINLIGHT_POS_X + player[0].pos.x;
	//	g_Light[0].Position.y = MAINLIGHT_POS_Y + player[0].pos.y;
	//	g_Light[0].Position.z = MAINLIGHT_POS_Z + player[0].pos.z;
	//}


	//g_Light[1].Position.y = player[0].pos.y + POINTLIGHT_POS_Y;

	////g_Light[1].Position.x = player[0].pos.x + (LIGHT_WALL_DIS) * cosf(-player[0].rot.y);
	////g_Light[1].Position.z = player[0].pos.z + (LIGHT_WALL_DIS) * sinf(-player[0].rot.y);


	////g_Light[1].Position.x = player[0].pos.x;
	////g_Light[1].Position.z = player[0].pos.z;


	//g_Light[1].Position.x = (WALL_RADIUS + LIGHT_WALL_DIS) * cosf(-player[0].rot.y);
	//g_Light[1].Position.z = (WALL_RADIUS + LIGHT_WALL_DIS) * sinf(-player[0].rot.y);

	//SetLight(1, &g_Light[1]);


}


//=============================================================================
// ���C�g�̐ݒ�
// Type�ɂ���ăZ�b�g���郁���o�[�ϐ����ς���Ă���
//=============================================================================
void SetLightData(int index, LIGHT *light)
{
	SetLight(index, light);
}


LIGHT *GetLightData(int index)
{
	return(&g_Light[index]);
}


//=============================================================================
// �t�H�O�̐ݒ�
//=============================================================================
void SetFogData(FOG *fog)
{
	SetFog(fog);
}



//=============================================================================
// ���C�����C�g�̍X�V��
//=============================================================================
void SetMainLightView(BOOL isPers)
{
	//// �v���C���[�̐擪�A�h���X���擾
	//PLAYER *player = GetPlayer();

	//// �r���[�}�g���b�N�X�ݒ�@�@�@�@�@�@�@�@�@�@�@�����_�@�@�@�@�@�@�@�@�@���x�N�g���̕����@�@�@�@�@�@�@�@�@���A�b�v�x�N�g���i�ǂ��炪�ォ�j
	//XMMATRIX mtxView;
	////mtxView = XMMatrixLookAtLH(XMLoadFloat3(&g_Light[0].Position), XMLoadFloat3(&g_Light[0].Direction), XMLoadFloat3(&g_LightView.up));
	//mtxView = XMMatrixLookAtLH(XMLoadFloat3(&g_Light[0].Position), XMLoadFloat3(&player[0].pos), XMLoadFloat3(&g_LightView.up));
	//SetLViewMatrix(&mtxView);	//�r���[�}�g���N�X���Z�b�g
	//XMStoreFloat4x4(&g_LightView.mtxView, mtxView);	//���C�g�̃O���[�o���\���̂փR�s�[�i�ۑ��j���Ă���

	//XMMATRIX mtxInvView;
	//mtxInvView = XMMatrixInverse(nullptr, mtxView);
	//XMStoreFloat4x4(&g_LightView.mtxInvView, mtxInvView);


	//// �v���W�F�N�V�����}�g���b�N�X�ݒ�@�@�@������p�@�@���A�X�y�N�g��@��
	//XMMATRIX mtxProjection;

	//if (isPers)
	//{
	//	mtxProjection = XMMatrixPerspectiveFovLH(g_LightView.viewAngle, g_LightView.viewAspect, g_LightView.nearZ, g_LightView.farZ);
	//}
	//else
	//{
	//	mtxProjection = XMMatrixOrthographicLH((float)SCREEN_WIDTH * 0.5f, (float)SCREEN_HEIGHT * 0.5f, g_LightView.nearZ, g_LightView.farZ);
	//}

	//SetLProjectionMatrix(&mtxProjection);
	//XMStoreFloat4x4(&g_LightView.mtxProjection, mtxProjection);

}


//=============================================================================
// ���C�����C�g�̍��W���璍���_�܂ł̕��������߂遚
//=============================================================================
XMFLOAT3 SetMainLightDirectionFromPos(void)
{
	// XYZ���ꂼ��̎��_���璍���_�܂ł̈ړ��ʂ����߂�
	XMFLOAT3 posbuf = { g_LightView.at.x - g_Light[0].Position.x,
						g_LightView.at.y - g_Light[0].Position.y,
						g_LightView.at.z - g_Light[0].Position.z };

	// �ł��ړ��ʂ̐�Βl���傫���������߂�
	int greaterPos = 0;
	if (fabsf(posbuf.x) > fabsf(posbuf.y))
	{
		if (fabsf(posbuf.x) > fabsf(posbuf.z))
		{
			// X����ԑ傫��
			greaterPos = 0;
		}
		else
		{
			// Z����ԑ傫��
			greaterPos = 2;
		}
	}
	else if (fabsf(posbuf.y) > fabsf(posbuf.z))
	{
		// Y����ԑ傫��
		greaterPos = 1;
	}
	else
	{
		// Z����ԑ傫��
		greaterPos = 2;
	}

	// ���C�g�̕��������߂�
	XMFLOAT3 dirbuf = { 0.0f, 0.0f, 0.0f };
	switch (greaterPos)
	{
	case 0:
		if (posbuf.x < 0.0f)
		{
			dirbuf.x = -1.0f;
			dirbuf.y = -(posbuf.y / posbuf.x);
			dirbuf.z = -(posbuf.z / posbuf.x);
		}
		else
		{
			dirbuf.x = 1.0f;
			dirbuf.y = posbuf.y / posbuf.x;
			dirbuf.z = posbuf.z / posbuf.x;
		}
		break;

	case 1:
		if (posbuf.y < 0.0f)
		{
			dirbuf.x = -(posbuf.x / posbuf.y);
			dirbuf.y = -1.0f;
			dirbuf.z = -(posbuf.z / posbuf.y);
		}
		else
		{
			dirbuf.x = posbuf.x / posbuf.y;
			dirbuf.y = 1.0f;
			dirbuf.z = posbuf.z / posbuf.y;
		}
		break;

	case 2:
		if (posbuf.z < 0.0f)
		{
			dirbuf.x = -(posbuf.x / posbuf.z);
			dirbuf.y = -(posbuf.y / posbuf.z);
			dirbuf.z = -1.0f;
		}
		else
		{
			dirbuf.x = posbuf.x / posbuf.z;
			dirbuf.y = posbuf.y / posbuf.z;
			dirbuf.z = 1.0f;
		}
		break;
	}

	return dirbuf;
}
