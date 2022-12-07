//=============================================================================
//
// シェーダーヘッダー [shader.h]
// Author : 國江 翔太
//
//=============================================================================

//*****************************************************************************
// 定数
//*****************************************************************************
#define LL_POS (180.0f)

//*****************************************************************************
// 定数バッファと構造体
//*****************************************************************************

// 変換マトリクス
cbuffer MatrixBuffer : register(b0) {
	matrix World;		// ワールド変換
	matrix View;		// ビュー変換
	matrix InvView;		// インバースビュー変換
	matrix Projection;	// プロジェクション変換
	matrix WVP;			// WVP変換
	matrix VP;			// VP変換
	matrix AfterRot;	// 
}

// カメラ（位置）
struct CAMERA {
	float4	Position;	// 位置
	float4	ViewVolume;	// スクリーン幅、スクリーン高さ、前方クリップ面、後方クリップ面
	int		Time;
	float3 	Dummy;
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
	float 		Spd;
};
cbuffer MosaicBuffer : register(b10) {
	CURVE		Curve;
}

// その他定数
//struct CONSTANT {
//	int			Time;
//	float3 		Dummy;
//};
//cbuffer ConstantBuffer : register(b12) {
//	CONSTANT	Constant;
//}

// ビルボード
//cbuffer BillboardBuffer : register(b10) {
//	matrix Billboard;
//}

// インスタンシング
#define INSTANCE_MAX 126
struct INSTANCE {
	float4 pos[INSTANCE_MAX];
	float4 rot[INSTANCE_MAX];
	float4 scl[INSTANCE_MAX];
	float4 col[INSTANCE_MAX];
	float4 txc[INSTANCE_MAX];
};
cbuffer InstanceBuffer : register(b11) {
	INSTANCE Instance;
};

//*****************************************************************************
// テクスチャ
//*****************************************************************************
Texture2D		g_Texture : register(t0);
Texture2D		g_OldScreen : register(t1);
Texture2D		g_TextureArray[INSTANCE_MAX] : register(t2);

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
struct IS_OUTPUT {
	float4 Position : SV_POSITION;
	float4 Normal : NORMAL0;
	float2 TexCoord : TEXCOORD0;
	float4 Diffuse : COLOR0;
	float4 WorldPos : POSITION0;
	int Id : TEXCOORD1;
};

//*****************************************************************************
// 関数
//*****************************************************************************
float GetRandom(float2 texCoord, int Seed)
{
	return frac(sin(dot(texCoord.xy, float2(12.9898, 78.233)) + Seed) * 43758.5453);
}

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

matrix GetMtxWorld(float4 ps, float4 rt, float4 sc)
{
	float cx = cos(rt.x);
	float cy = cos(rt.y);
	float cz = cos(rt.z);
	float sx = sin(rt.x);
	float sy = sin(rt.y);
	float sz = sin(rt.z);
	matrix mtxWorld = {	// ワールド変換マトリクスを手で計算しておく
		cy*cz*sc.x, cy*sz*sc.x, sy*sc.x, 0.0f,
		(-cx * sz - sx * sy*cz)*sc.y, (cx*cz - sx * sy*sz)*sc.y, sx*cy*sc.y, 0.0f,
		(sx*sz - cx * sy*cz)*sc.z, (-sx * cz - cx * sy*sz)*sc.z, cx*cy*sc.z, 0.0f,
		ps.x, ps.y, ps.z, 1.0f,
	};
	return mtxWorld;
}

float4 GetColorFromTextureArray(int id, float2 texCoord)
{
	switch (id)
	{
	case 0: return g_TextureArray[0].Sample(g_SamplerState, texCoord);
	case 1: return g_TextureArray[1].Sample(g_SamplerState, texCoord);
	case 2: return g_TextureArray[2].Sample(g_SamplerState, texCoord);
	case 3: return g_TextureArray[3].Sample(g_SamplerState, texCoord);
	case 4: return g_TextureArray[4].Sample(g_SamplerState, texCoord);
	case 5: return g_TextureArray[5].Sample(g_SamplerState, texCoord);
	case 6: return g_TextureArray[6].Sample(g_SamplerState, texCoord);
	case 7: return g_TextureArray[7].Sample(g_SamplerState, texCoord);
	case 8: return g_TextureArray[8].Sample(g_SamplerState, texCoord);
	case 9: return g_TextureArray[9].Sample(g_SamplerState, texCoord);
	case 10: return g_TextureArray[10].Sample(g_SamplerState, texCoord);
	case 11: return g_TextureArray[11].Sample(g_SamplerState, texCoord);
	case 12: return g_TextureArray[12].Sample(g_SamplerState, texCoord);
	case 13: return g_TextureArray[13].Sample(g_SamplerState, texCoord);
	case 14: return g_TextureArray[14].Sample(g_SamplerState, texCoord);
	case 15: return g_TextureArray[15].Sample(g_SamplerState, texCoord);
	case 16: return g_TextureArray[16].Sample(g_SamplerState, texCoord);
	case 17: return g_TextureArray[17].Sample(g_SamplerState, texCoord);
	case 18: return g_TextureArray[18].Sample(g_SamplerState, texCoord);
	case 19: return g_TextureArray[19].Sample(g_SamplerState, texCoord);
	case 20: return g_TextureArray[20].Sample(g_SamplerState, texCoord);
	case 21: return g_TextureArray[21].Sample(g_SamplerState, texCoord);
	case 22: return g_TextureArray[22].Sample(g_SamplerState, texCoord);
	case 23: return g_TextureArray[23].Sample(g_SamplerState, texCoord);
	case 24: return g_TextureArray[24].Sample(g_SamplerState, texCoord);
	case 25: return g_TextureArray[25].Sample(g_SamplerState, texCoord);
	case 26: return g_TextureArray[26].Sample(g_SamplerState, texCoord);
	case 27: return g_TextureArray[27].Sample(g_SamplerState, texCoord);
	case 28: return g_TextureArray[28].Sample(g_SamplerState, texCoord);
	case 29: return g_TextureArray[29].Sample(g_SamplerState, texCoord);
	case 30: return g_TextureArray[30].Sample(g_SamplerState, texCoord);
	case 31: return g_TextureArray[31].Sample(g_SamplerState, texCoord);
	case 32: return g_TextureArray[32].Sample(g_SamplerState, texCoord);
	case 33: return g_TextureArray[33].Sample(g_SamplerState, texCoord);
	case 34: return g_TextureArray[34].Sample(g_SamplerState, texCoord);
	case 35: return g_TextureArray[35].Sample(g_SamplerState, texCoord);
	case 36: return g_TextureArray[36].Sample(g_SamplerState, texCoord);
	case 37: return g_TextureArray[37].Sample(g_SamplerState, texCoord);
	case 38: return g_TextureArray[38].Sample(g_SamplerState, texCoord);
	case 39: return g_TextureArray[39].Sample(g_SamplerState, texCoord);
	case 40: return g_TextureArray[40].Sample(g_SamplerState, texCoord);
	case 41: return g_TextureArray[41].Sample(g_SamplerState, texCoord);
	case 42: return g_TextureArray[42].Sample(g_SamplerState, texCoord);
	case 43: return g_TextureArray[43].Sample(g_SamplerState, texCoord);
	case 44: return g_TextureArray[44].Sample(g_SamplerState, texCoord);
	case 45: return g_TextureArray[45].Sample(g_SamplerState, texCoord);
	case 46: return g_TextureArray[46].Sample(g_SamplerState, texCoord);
	case 47: return g_TextureArray[47].Sample(g_SamplerState, texCoord);
	case 48: return g_TextureArray[48].Sample(g_SamplerState, texCoord);
	case 49: return g_TextureArray[49].Sample(g_SamplerState, texCoord);
	case 50: return g_TextureArray[50].Sample(g_SamplerState, texCoord);
	case 51: return g_TextureArray[51].Sample(g_SamplerState, texCoord);
	case 52: return g_TextureArray[52].Sample(g_SamplerState, texCoord);
	case 53: return g_TextureArray[53].Sample(g_SamplerState, texCoord);
	case 54: return g_TextureArray[54].Sample(g_SamplerState, texCoord);
	case 55: return g_TextureArray[55].Sample(g_SamplerState, texCoord);
	case 56: return g_TextureArray[56].Sample(g_SamplerState, texCoord);
	case 57: return g_TextureArray[57].Sample(g_SamplerState, texCoord);
	case 58: return g_TextureArray[58].Sample(g_SamplerState, texCoord);
	case 59: return g_TextureArray[59].Sample(g_SamplerState, texCoord);
	case 60: return g_TextureArray[60].Sample(g_SamplerState, texCoord);
	case 61: return g_TextureArray[61].Sample(g_SamplerState, texCoord);
	case 62: return g_TextureArray[62].Sample(g_SamplerState, texCoord);
	case 63: return g_TextureArray[63].Sample(g_SamplerState, texCoord);
	case 64: return g_TextureArray[64].Sample(g_SamplerState, texCoord);
	case 65: return g_TextureArray[65].Sample(g_SamplerState, texCoord);
	case 66: return g_TextureArray[66].Sample(g_SamplerState, texCoord);
	case 67: return g_TextureArray[67].Sample(g_SamplerState, texCoord);
	case 68: return g_TextureArray[68].Sample(g_SamplerState, texCoord);
	case 69: return g_TextureArray[69].Sample(g_SamplerState, texCoord);
	case 70: return g_TextureArray[70].Sample(g_SamplerState, texCoord);
	case 71: return g_TextureArray[71].Sample(g_SamplerState, texCoord);
	case 72: return g_TextureArray[72].Sample(g_SamplerState, texCoord);
	case 73: return g_TextureArray[73].Sample(g_SamplerState, texCoord);
	case 74: return g_TextureArray[74].Sample(g_SamplerState, texCoord);
	case 75: return g_TextureArray[75].Sample(g_SamplerState, texCoord);
	case 76: return g_TextureArray[76].Sample(g_SamplerState, texCoord);
	case 77: return g_TextureArray[77].Sample(g_SamplerState, texCoord);
	case 78: return g_TextureArray[78].Sample(g_SamplerState, texCoord);
	case 79: return g_TextureArray[79].Sample(g_SamplerState, texCoord);
	case 80: return g_TextureArray[80].Sample(g_SamplerState, texCoord);
	case 81: return g_TextureArray[81].Sample(g_SamplerState, texCoord);
	case 82: return g_TextureArray[82].Sample(g_SamplerState, texCoord);
	case 83: return g_TextureArray[83].Sample(g_SamplerState, texCoord);
	case 84: return g_TextureArray[84].Sample(g_SamplerState, texCoord);
	case 85: return g_TextureArray[85].Sample(g_SamplerState, texCoord);
	case 86: return g_TextureArray[86].Sample(g_SamplerState, texCoord);
	case 87: return g_TextureArray[87].Sample(g_SamplerState, texCoord);
	case 88: return g_TextureArray[88].Sample(g_SamplerState, texCoord);
	case 89: return g_TextureArray[89].Sample(g_SamplerState, texCoord);
	case 90: return g_TextureArray[90].Sample(g_SamplerState, texCoord);
	case 91: return g_TextureArray[91].Sample(g_SamplerState, texCoord);
	case 92: return g_TextureArray[92].Sample(g_SamplerState, texCoord);
	case 93: return g_TextureArray[93].Sample(g_SamplerState, texCoord);
	case 94: return g_TextureArray[94].Sample(g_SamplerState, texCoord);
	case 95: return g_TextureArray[95].Sample(g_SamplerState, texCoord);
	case 96: return g_TextureArray[96].Sample(g_SamplerState, texCoord);
	case 97: return g_TextureArray[97].Sample(g_SamplerState, texCoord);
	case 98: return g_TextureArray[98].Sample(g_SamplerState, texCoord);
	case 99: return g_TextureArray[99].Sample(g_SamplerState, texCoord);
	case 100: return g_TextureArray[100].Sample(g_SamplerState, texCoord);
	case 101: return g_TextureArray[101].Sample(g_SamplerState, texCoord);
	case 102: return g_TextureArray[102].Sample(g_SamplerState, texCoord);
	case 103: return g_TextureArray[103].Sample(g_SamplerState, texCoord);
	case 104: return g_TextureArray[104].Sample(g_SamplerState, texCoord);
	case 105: return g_TextureArray[105].Sample(g_SamplerState, texCoord);
	case 106: return g_TextureArray[106].Sample(g_SamplerState, texCoord);
	case 107: return g_TextureArray[107].Sample(g_SamplerState, texCoord);
	case 108: return g_TextureArray[108].Sample(g_SamplerState, texCoord);
	case 109: return g_TextureArray[109].Sample(g_SamplerState, texCoord);
	case 110: return g_TextureArray[110].Sample(g_SamplerState, texCoord);
	case 111: return g_TextureArray[111].Sample(g_SamplerState, texCoord);
	case 112: return g_TextureArray[112].Sample(g_SamplerState, texCoord);
	case 113: return g_TextureArray[113].Sample(g_SamplerState, texCoord);
	case 114: return g_TextureArray[114].Sample(g_SamplerState, texCoord);
	case 115: return g_TextureArray[115].Sample(g_SamplerState, texCoord);
	case 116: return g_TextureArray[116].Sample(g_SamplerState, texCoord);
	case 117: return g_TextureArray[117].Sample(g_SamplerState, texCoord);
	case 118: return g_TextureArray[118].Sample(g_SamplerState, texCoord);
	case 119: return g_TextureArray[119].Sample(g_SamplerState, texCoord);
	case 120: return g_TextureArray[120].Sample(g_SamplerState, texCoord);
	case 121: return g_TextureArray[121].Sample(g_SamplerState, texCoord);
	case 122: return g_TextureArray[122].Sample(g_SamplerState, texCoord);
	case 123: return g_TextureArray[123].Sample(g_SamplerState, texCoord);
	case 124: return g_TextureArray[124].Sample(g_SamplerState, texCoord);
	case 125: return g_TextureArray[125].Sample(g_SamplerState, texCoord);
	default: return g_TextureArray[0].Sample(g_SamplerState, texCoord);
	}
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