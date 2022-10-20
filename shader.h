//=============================================================================
//
// �V�F�[�_�[�w�b�_�[ [shader.h]
// Author : ���] �đ�
//
//=============================================================================
#pragma once

// �ϊ��}�g���N�X
cbuffer MatrixBuffer : register(b0)
{
	matrix World;		// ���[���h�ϊ�
	matrix View;		// �r���[�ϊ�
	matrix Projection;	// �v���W�F�N�V�����ϊ�
	matrix WVP;			// WVP�ϊ�
	matrix VP;			// VP�ϊ�
	matrix AfterRot;	// 
}

// �J�����i�ʒu�j
struct CAMERA
{
	float4	Position;	// �ʒu
	float4	ViewVolume;	// �X�N���[�����A�X�N���[�������A�O���N���b�v�ʁA����N���b�v��
};
cbuffer CameraBuffer : register(b1)
{
	CAMERA Camera;
}

// �}�e���A��
struct MATERIAL
{
	float4		Ambient;	// �����F
	float4		Diffuse;	// �g�U���ˌ��F
	float4		Specular;	// ���ʔ��ˌ��F
	float4		Emission;	// �����F
	float		Shininess;	// �P�x
	int			noTexSampling;
	float2		Dummy;
};
cbuffer MaterialBuffer : register(b2)
{
	MATERIAL Material;
}

// ����
struct LIGHT_AMBIENT
{
	float4		Color;
	float		Intensity;
	int			Use;
	int2		Dummy;
};
cbuffer AmbientLightBuffer : register(b3)
{
	LIGHT_AMBIENT AmbientLight;
}
// ���s����
struct LIGHT_DIRECTIONAL
{
	float4		Color;
	float4		Direction;
	float		Intensity;
	int			Use;
	int2		Dummy;
};
#define DirectionalLightMax 5
cbuffer DirectionalLightBuffer : register(b4)
{
	LIGHT_DIRECTIONAL DirectionalLight[DirectionalLightMax];
}
// �_����
struct LIGHT_POINT
{
	float4		Color;
	float4		Position;
	float		Intensity;
	float		Attenuation;
	int			Use;
	int			Dummy;
};
#define PointLightMax 10
cbuffer PointLightBuffer : register(b5)
{
	LIGHT_POINT PointLight[PointLightMax];
}
// �X�|�b�g���C�g
struct LIGHT_SPOT
{
	float4		Color;
	float4		Position;
	float4		Direction;
	float		Intensity;
	float		Attenuation;
	float		Angle;
	int			Use;
};
#define SpotLightMax 10
cbuffer SpotLightBuffer : register(b6)
{
	LIGHT_SPOT SpotLight[SpotLightMax];
}
// �����ԍ�
cbuffer LightBuffer : register(b7)
{
	int lightNo;
	float3 Dummy;
}

// �A�E�g���C��
struct OUTLINE
{
	float		Scale;
	float3 		Dummy;
	float4		Color;
};
cbuffer OutlineBuffer : register(b8)
{
	OUTLINE Outline;
}

// �t�B���^
cbuffer FilterBuffer : register(b9)
{
	matrix filter;
}

//// ���U�C�N
//struct MOSAIC
//{
//	int			Size;
//	float3 		Dummy;
//};
//cbuffer MosaicBuffer : register(b10)
//{
//	MOSAIC		Mosaic;
//}
// �p�C�v
struct CURVE
{
	float2		Angle;
	float		TexPos;
	float 		Dummy;
};
cbuffer MosaicBuffer : register(b10)
{
	CURVE		Curve;
}

// �萔
struct CONSTANT
{
	int			Time;
	float3 		Dummy;
};
cbuffer ConstantBuffer : register(b11)
{
	CONSTANT	Constant;
}

//cbuffer BillboardBuffer : register(b10)
//{
//	matrix Billboard;
//}
//struct INSTANCE
//{
//	float4 pos[1024];
//	float4 rot[1024];
//	float4 scl[1024];
//	float4 col[1024];
//};
//cbuffer InstanceBuffer : register(b11)
//{
//	INSTANCE Instance;
//};

// �e�N�X�`��
Texture2D		g_Texture : register(t0);

// �T���v��
SamplerState	g_SamplerState : register(s0);