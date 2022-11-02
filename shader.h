//=============================================================================
//
// シェーダーヘッダー [shader.h]
// Author : 國江 翔太
//
//=============================================================================

//*****************************************************************************
// 定数バッファと構造体
//*****************************************************************************

// 変換マトリクス
cbuffer MatrixBuffer : register(b0) {
	matrix World;		// ワールド変換
	matrix View;		// ビュー変換
	matrix Projection;	// プロジェクション変換
	matrix WVP;			// WVP変換
	matrix VP;			// VP変換
	matrix AfterRot;	// 
}

// カメラ（位置）
struct CAMERA {
	float4	Position;	// 位置
	float4	ViewVolume;	// スクリーン幅、スクリーン高さ、前方クリップ面、後方クリップ面
};
cbuffer CameraBuffer : register(b1) {
	CAMERA Camera;
}

// マテリアル
struct MATERIAL {
	float4		Ambient;	// 環境光色
	float4		Diffuse;	// 拡散反射光色
	float4		Specular;	// 鏡面反射光色
	float4		Emission;	// 発光色
	float		Shininess;	// 輝度
	int			noTexSampling;
	float2		Dummy;
};
cbuffer MaterialBuffer : register(b2) {
	MATERIAL Material;
}

// ライティング /////////////////////

// 環境光
struct LIGHT_AMBIENT {
	float4		Color;
	float		Intensity;
	int			Use;
	int2		Dummy;
};
cbuffer AmbientLightBuffer : register(b3) {
	LIGHT_AMBIENT AmbientLight;
}

// 平行光源
struct LIGHT_DIRECTIONAL {
	float4		Color;
	float4		Direction;
	float		Intensity;
	int			Use;
	int2		Dummy;
};
#define DirectionalLightMax 5
cbuffer DirectionalLightBuffer : register(b4) {
	LIGHT_DIRECTIONAL DirectionalLight[DirectionalLightMax];
}

// 点光源
struct LIGHT_POINT {
	float4		Color;
	float4		Position;
	float		Intensity;
	float		Attenuation;
	int			Use;
	int			Dummy;
};
#define PointLightMax 10
cbuffer PointLightBuffer : register(b5) {
	LIGHT_POINT PointLight[PointLightMax];
}

// スポットライト
struct LIGHT_SPOT {
	float4		Color;
	float4		Position;
	float4		Direction;
	float		Intensity;
	float		Attenuation;
	float		Angle;
	int			Use;
};
#define SpotLightMax 10
cbuffer SpotLightBuffer : register(b6) {
	LIGHT_SPOT SpotLight[SpotLightMax];
}

// 光源番号
cbuffer LightBuffer : register(b7) {
	int lightNo;
	float3 Dummy;
}

// アウトライン
struct OUTLINE {
	float		Scale;
	float3 		Dummy;
	float4		Color;
};
cbuffer OutlineBuffer : register(b8) {
	OUTLINE Outline;
}

// フィルタ
cbuffer FilterBuffer : register(b9) {
	matrix filter;
}

//// モザイク
//struct MOSAIC {
//	int			Size;
//	float3 		Dummy;
//};
//cbuffer MosaicBuffer : register(b10) {
//	MOSAIC		Mosaic;
//}

// パイプ
struct CURVE {
	float2		Angle;
	float		TexPos;
	float 		Dummy;
};
cbuffer MosaicBuffer : register(b10) {
	CURVE		Curve;
}

// その他定数
struct CONSTANT {
	int			Time;
	float3 		Dummy;
};
cbuffer ConstantBuffer : register(b11) {
	CONSTANT	Constant;
}

// ビルボード
//cbuffer BillboardBuffer : register(b10) {
//	matrix Billboard;
//}
//struct INSTANCE {
//	float4 pos[1024];
//	float4 rot[1024];
//	float4 scl[1024];
//	float4 col[1024];
//};

//インスタンシング
//cbuffer InstanceBuffer : register(b11) {
//	INSTANCE Instance;
//};

//*****************************************************************************
// テクスチャ
//*****************************************************************************
Texture2D		g_Texture : register(t0);

//*****************************************************************************
// サンプラーステート
//*****************************************************************************
SamplerState	g_SamplerState : register(s0);

//*****************************************************************************
// IN_OUT構造体
//*****************************************************************************
struct VS_INPUT {
	float4 Position : SV_POSITION;
	float4 Normal : NORMAL0;
	float4 Diffuse : COLOR0;
	float2 TexCoord : TEXCOORD0;
};
struct VS_OUTPUT {
	float4 Position : SV_POSITION;
	float4 Normal : NORMAL0;
	float2 TexCoord : TEXCOORD0;
	float4 Diffuse : COLOR0;
	float4 WorldPos : POSITION0;
};
struct GS_INPUT {
	float4 Position : SV_POSITION;
	float4 Normal : NORMAL0;
	float2 TexCoord : TEXCOORD0;
	float4 Diffuse : COLOR0;
	float4 WorldPos : POSITION0;
};
struct PS_INPUT {
	float4 Position : SV_POSITION;
	float4 Normal : NORMAL0;
	float2 TexCoord : TEXCOORD0;
	float4 Diffuse : COLOR0;
	float4 WorldPos : POSITION0;
};

//*****************************************************************************
// 関数
//*****************************************************************************

float4 GetTubeCurvePos(float4 worldPos)
{
	float angle = worldPos.z * 0.0001f;
	float4 newPos;
	if (angle < 1.57f && angle > -1.57f)
	{
		float angleX = angle * Curve.Angle.x;
		float angleY = angle * Curve.Angle.y;
		float cx = cos(angleX);
		float cy = cos(angleY);
		float cz = cos(0.0f);
		float sx = sin(angleX);
		float sy = sin(angleY);
		float sz = sin(0.0f);
		matrix mtxRotRPY = {	// Roll*Pitch*Yawを先に自分で計算しておく
			cy*cz, cy*sz, sy, 0.0f,
			-cx * sz - sx * sy*cz, cx*cz - sx * sy*sz, sx*cy, 0.0f,
			sx*sz - cx * sy*cz, -sx * cz - cx * sy*sz, cx*cy, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f,
		};
		newPos = mul(worldPos, mtxRotRPY);
	}
	return mul(mul(newPos, AfterRot), VP);
}

//matrix GetWorldMtx(void)
//{
//	float4 ps = Pos;
//	float4 rt = Rot;
//	float4 sc = Scl;
//	float cx = cos(rt.x);
//	float cy = cos(rt.y);
//	float cz = cos(rt.z);
//	float sx = sin(rt.x);
//	float sy = sin(rt.y);
//	float sz = sin(rt.z);
//	return {
//		cy*cz*sc.x, cy*sz*sc.x, sy*sc.x, 0.0f,
//		(-cx * sz - sx * sy*cz)*sc.y, (cx*cz - sx * sy*sz)*sc.y, sx*cy*sc.y, 0.0f,
//		(sx*sz - cx * sy*cz)*sc.z, (-sx * cz - cx * sy*sz)*sc.z, cx*cy*sc.z, 0.0f,
//		ps.x, ps.y, ps.z, 1.0f,
//	};
//}