//=============================================================================
//
// シェーダーヘッダー [shader.h]
// Author : 國江 翔太
//
//=============================================================================
#pragma once

// 変換マトリクス
cbuffer MatrixBuffer : register(b0)
{
	matrix World;		// ワールド変換
	matrix View;		// ビュー変換
	matrix Projection;	// プロジェクション変換
	matrix WVP;			// WVP変換
	matrix VP;			// VP変換
	matrix AfterRot;	// 
}

// カメラ（位置）
struct CAMERA
{
	float4	Position;	// 位置
	float4	ViewVolume;	// スクリーン幅、スクリーン高さ、前方クリップ面、後方クリップ面
};
cbuffer CameraBuffer : register(b1)
{
	CAMERA Camera;
}

// マテリアル
struct MATERIAL
{
	float4		Ambient;	// 環境光色
	float4		Diffuse;	// 拡散反射光色
	float4		Specular;	// 鏡面反射光色
	float4		Emission;	// 発光色
	float		Shininess;	// 輝度
	int			noTexSampling;
	float2		Dummy;
};
cbuffer MaterialBuffer : register(b2)
{
	MATERIAL Material;
}

// 環境光
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
// 平行光源
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
// 点光源
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
// スポットライト
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
// 光源番号
cbuffer LightBuffer : register(b7)
{
	int lightNo;
	float3 Dummy;
}

// アウトライン
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

// フィルタ
cbuffer FilterBuffer : register(b9)
{
	matrix filter;
}

//// モザイク
//struct MOSAIC
//{
//	int			Size;
//	float3 		Dummy;
//};
//cbuffer MosaicBuffer : register(b10)
//{
//	MOSAIC		Mosaic;
//}
// パイプ
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

// 定数
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

// テクスチャ
Texture2D		g_Texture : register(t0);

// サンプラ
SamplerState	g_SamplerState : register(s0);