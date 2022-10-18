//=============================================================================
//
// シェーダー [shader.hlsl]
// Author : 國江 翔太
//
//=============================================================================
#include "shader.h"

//=============================================================================
// 頂点シェーダ
//=============================================================================
// 基本形
void VertexShaderPolygon(
	in  float4 inPosition : POSITION0,
	in  float4 inNormal : NORMAL0,
	in  float4 inDiffuse : COLOR0,
	in  float2 inTexCoord : TEXCOORD0,

	out float4 outPosition : SV_POSITION,
	out float4 outNormal : NORMAL0,
	out float2 outTexCoord : TEXCOORD0,
	out float4 outDiffuse : COLOR0,
	out float4 outWorldPos : POSITION0)
{
	outPosition = mul(inPosition, WVP);
	outWorldPos = mul(inPosition, World);
	outNormal = normalize(mul(float4(inNormal.xyz, 0.0f), World));
	outTexCoord = inTexCoord;
	outDiffuse = inDiffuse * Material.Diffuse;
}

// パイプ専用頂点シェーダ（テクスチャ移動付き）
void VertexShaderTube(
	in  float4 inPosition	: POSITION0,
	in  float4 inNormal : NORMAL0,
	in  float4 inDiffuse : COLOR0,
	in  float2 inTexCoord : TEXCOORD0,

	out float4 outPosition : SV_POSITION,
	//out float4 outNormal : NORMAL0,
	out float2 outTexCoord : TEXCOORD0
	//out float4 outDiffuse : COLOR0,
	//out float4 outWorldPos : POSITION0
	)
{
	// 頂点を移動してカーブさせる
	float4 WorldPos = mul(inPosition, World);
	float angle = WorldPos.z * 0.0001f;
	if (angle < 1.57f && angle > -1.57f)
	{
		float radius = 10000.0f - WorldPos.x;
		WorldPos.x = 10000.0f - radius * cos(angle);
		WorldPos.z = radius * sin(angle);
	}

	// テクスチャ移動
	inTexCoord.y -= 0.255f * Constant.Time;

	outPosition = mul(WorldPos, VP);
	//outWorldPos = WorldPos;
	//outNormal = normalize(mul(float4(inNormal.xyz, 0.0f), World));
	outTexCoord = inTexCoord;
	//outDiffuse = inDiffuse * Material.Diffuse;
}

// パイプ中のギミック用頂点シェーダ（テクスチャ移動無し）
void VertexShaderGimmick(
	in  float4 inPosition : POSITION0,
	in  float4 inNormal : NORMAL0,
	in  float4 inDiffuse : COLOR0,
	in  float2 inTexCoord : TEXCOORD0,

	out float4 outPosition : SV_POSITION,
	//out float4 outNormal : NORMAL0,
	out float2 outTexCoord : TEXCOORD0
	//out float4 outDiffuse : COLOR0,
	//out float4 outWorldPos : POSITION0
)
{
	float4 WorldPos = mul(inPosition, World);
	float angle = WorldPos.z * 0.0001f;
	if (angle < 1.57f && angle > -1.57f)
	{
		float radius = 10000.0f - WorldPos.x;
		WorldPos.x = 10000.0f - radius * cos(angle);
		WorldPos.z = radius * sin(angle);
	}

	outPosition = mul(WorldPos, VP);
	//outWorldPos = WorldPos;
	//outNormal = normalize(mul(float4(inNormal.xyz, 0.0f), World));
	outTexCoord = inTexCoord;
	//outDiffuse = inDiffuse * Material.Diffuse;
}

// アウトライン用
void VertexShaderOutline(in  float4 inPosition		: POSITION0,
	in  float4 inNormal : NORMAL0,
	in  float4 inDiffuse : COLOR0,
	in  float2 inTexCoord : TEXCOORD0,

	out float4 outPosition : SV_POSITION,
	out float2 outTexCoord : TEXCOORD0)
{	// 法線方向に膨らませる
	inPosition.xyz = inPosition.xyz + normalize(inNormal.xyz) * Outline.Scale.x;
	outPosition = mul(inPosition, WVP);
}

//=============================================================================
// ピクセルシェーダ
//=============================================================================
// アウトライン用
void PixelShaderOutline(in  float4 inPosition		: POSITION0,
	out float4 outDiffuse : SV_Target)
{
	outDiffuse = Outline.Color;
}

//=============================================================================
// インスタンシング描画＋ビルボード用
//=============================================================================
// void VertexShaderInstancingBillboard(in  float4 inPosition		: POSITION0,
// 	in  float4 inNormal : NORMAL0,
// 	in  float4 inDiffuse : COLOR0,
// 	in  float2 inTexCoord : TEXCOORD0,

// 	out float4 outPosition : SV_POSITION,
// 	out float4 outNormal : NORMAL0,
// 	out float2 outTexCoord : TEXCOORD0,
// 	out float4 outDiffuse : COLOR0,
// 	out float4 outWorldPos : POSITION0,
// 	uint instID : SV_InstanceID)
// {
// 	float4 ps = Instance.pos[instID];
// 	float4 sc = Instance.scl[instID];
// 	//ps = float4( -100.0f, 143.0f, 68.5f, 0.0f );
// 	//sc = float4(1.0f, 1.0f, 1.0f, 0.0f );
// 	matrix mtxScaleTranslation = {
// 		sc.x, 0.0f, 0.0f, 0.0f,
// 		0.0f, sc.y, 0.0f, 0.0f,
// 		0.0f, 0.0f, sc.z, 0.0f,
// 		ps.x, ps.y, ps.z, 1.0f,
// 	};
// 	matrix mtxWorld = mul(Billboard, mtxScaleTranslation);
// 	matrix mtxWVP = mul(mtxWorld, VP);

// 	outPosition = mul(inPosition, mtxWVP);
// 	outTexCoord = inTexCoord;
// 	outDiffuse = Instance.col[instID];
// }
void PixelShaderPolygonNoLighting( in  float4 inPosition		: SV_POSITION,
						 in  float4 inNormal		: NORMAL0,
						 in  float2 inTexCoord		: TEXCOORD0,
						 in  float4 inDiffuse		: COLOR0,
						 in  float4 inWorldPos      : POSITION0,

						 out float4 outDiffuse		: SV_Target )
{
	outDiffuse = g_Texture.Sample(g_SamplerState, inTexCoord);
	outDiffuse *= inDiffuse;
}

// 基本形（光源まとめて計算）
void PixelShaderPolygon(in  float4 inPosition		: SV_Position,
	in  float4 inNormal : NORMAL0,
	in  float2 inTexCoord : TEXCOORD0,
	in  float4 inDiffuse : COLOR0,
	in  float4 inWorldPos : POSITION0,

	out float4 outDiffuse : SV_Target)
{
	// テクスチャから色をサンプリング
	float4 color = inDiffuse * g_Texture.Sample(g_SamplerState, inTexCoord);

	// アルファ値以外を初期化（加算していく）
	outDiffuse = float4(0.0f, 0.0f, 0.0f, color.a);

	// カメラの向きと頂点の法線ベクトル
	float3 cameraDir = normalize(inWorldPos.xyz - Camera.Position.xyz);
	float3 normalDir = normalize(inNormal.xyz);

	int i;

	// 環境光
	if (AmbientLight.Use)
	{
		float3 lightColor = AmbientLight.Color.rgb * AmbientLight.Intensity;
		outDiffuse.rgb += lightColor * Material.Ambient.rgb * color.rgb;
	}

	// 平行光源
	for (i = 0; i < DirectionalLightMax; i++)
	{
		if (DirectionalLight[i].Use)
		{
			float3 lightColor = DirectionalLight[i].Color.rgb * DirectionalLight[i].Intensity;
			float3 lightDir = normalize(DirectionalLight[i].Direction.xyz);

			// ランバート
			//float lambert = dot(-lightDir, normalDir);
			// ハーフランバート
			//float halfLambert = lambert * 0.5f + 0.5f;
			//outDiffuse.rgb += lightColor * halfLambert * Material.Diffuse.rgb * color.rgb;

			// 拡散反射光
			float diffuse = saturate(dot(-lightDir, normalDir));
			outDiffuse.rgb += lightColor * diffuse * Material.Diffuse.rgb * color.rgb;

			// 鏡面反射光
			float3 reflectDir = 2.0f * normalDir * dot(normalDir, -lightDir) + lightDir;
			float specular = pow(saturate(dot(reflectDir, -cameraDir)), Material.Shininess);
			outDiffuse.rgb += lightColor * specular * Material.Specular.rgb;
		}
	}

	// 点光源
	for (i = 0; i < PointLightMax; i++)
	{
		if (PointLight[i].Use)
		{
			float3 lightColor = PointLight[i].Color.rgb * PointLight[i].Intensity;
			float3 lightVec = inWorldPos.xyz - PointLight[i].Position.xyz;
			float3 lightDir = normalize(lightVec);
			float lightLen = length(lightVec);
			float len2 = lightLen * lightLen;
			float att2 = PointLight[i].Attenuation * PointLight[i].Attenuation;
			float attenuation;
			if (PointLight[i].Color.a < 1.0f)
			{
				attenuation = 1.0f - len2 / att2;
			}
			else
			{
				attenuation = 1.0f - lightLen / PointLight[i].Attenuation;
			}
			if (attenuation < 0.0f)
			{
				attenuation = 0.0f;
			}
			lightColor *= attenuation;

			// 拡散反射光
			float diffuse = saturate(dot(-lightDir, normalDir));
			outDiffuse.rgb += lightColor * diffuse  * Material.Diffuse.rgb * color.rgb;

			// 鏡面反射光
			float3 reflectDir = 2.0f * normalDir * dot(normalDir, -lightDir) + lightDir;
			float specular = pow(saturate(dot(reflectDir, -cameraDir)), Material.Shininess);
			outDiffuse.rgb += lightColor * specular * Material.Specular.rgb;
		}
	}

	// スポットライト
	for (i = 0; i < SpotLightMax; i++)
	{
		if (SpotLight[i].Use)
		{
			float3 lightVec = inWorldPos.xyz - SpotLight[i].Position.xyz;
			float3 lightDir = normalize(lightVec);
			float3 lightDir0 = normalize(SpotLight[i].Direction.xyz);
			if (acos(dot(lightDir, lightDir0)) <= SpotLight[i].Angle)
			{
				float3 lightColor = SpotLight[i].Color.rgb * SpotLight[i].Intensity;
				float lightLen = length(lightVec);
				float len2 = lightLen * lightLen;
				float att2 = SpotLight[i].Attenuation * SpotLight[i].Attenuation;
				float attenuation = 1.0f - len2 / att2;
				if (attenuation < 0.0f)
				{
					attenuation = 0.0f;
				}
				lightColor *= attenuation;

				// 拡散反射光
				float diffuse = saturate(dot(-lightDir, normalDir));
				outDiffuse.rgb += lightColor * diffuse  * Material.Diffuse.rgb * color.rgb;

				// 鏡面反射光
				float3 reflectDir = 2.0f * normalDir * dot(normalDir, -lightDir) + lightDir;
				float specular = pow(saturate(dot(reflectDir, -cameraDir)), Material.Shininess);
				outDiffuse.rgb += lightColor * specular * Material.Specular.rgb;
			}
		}
	}
}

// 光源別 ////////////////
// 環境光
void PixelShaderAL(in  float4 inPosition	: SV_Position,
	in  float4 inNormal : NORMAL0,
	in  float2 inTexCoord : TEXCOORD0,
	in  float4 inDiffuse : COLOR0,
	in  float4 inWorldPos : POSITION0,

	out float4 outDiffuse : SV_Target)
{
	// テクスチャから色をサンプリング
	float4 color = inDiffuse * g_Texture.Sample(g_SamplerState, inTexCoord);

	// アルファ値以外を初期化（加算していく）
	outDiffuse = float4(0.0f, 0.0f, 0.0f, color.a);

	// 環境光の色を反映
	float3 lightColor = AmbientLight.Color.rgb * AmbientLight.Intensity;
	outDiffuse.rgb += lightColor * Material.Ambient.rgb * color.rgb;
}
// 平行光源
void PixelShaderDL(in  float4 inPosition	: SV_Position,
	in  float4 inNormal : NORMAL0,
	in  float2 inTexCoord : TEXCOORD0,
	in  float4 inDiffuse : COLOR0,
	in  float4 inWorldPos : POSITION0,

	out float4 outDiffuse : SV_Target)
{
	// テクスチャから色をサンプリング
	float4 color = inDiffuse * g_Texture.Sample(g_SamplerState, inTexCoord);

	// アルファ値以外を初期化（加算していく）
	outDiffuse = float4(0.0f, 0.0f, 0.0f, color.a);

	// カメラの向きと頂点の法線ベクトル
	float3 cameraDir = normalize(inWorldPos.xyz - Camera.Position.xyz);
	float3 normalDir = normalize(inNormal.xyz);

	// ライトの色と方向
	float3 lightColor = DirectionalLight[lightNo].Color.rgb * DirectionalLight[lightNo].Intensity;
	float3 lightDir = normalize(DirectionalLight[lightNo].Direction.xyz);

	// 拡散反射光
	float diffuse = saturate(dot(-lightDir, normalDir));
	outDiffuse.rgb += lightColor * diffuse * Material.Diffuse.rgb * color.rgb;
	// float diffuse = saturate(dot(-lightDir, normalDir));
	// float toon = (float)(int)(lightColor * diffuse * Material.Diffuse.rgb * 4.0f)*0.25f;
	// outDiffuse.rgb += toon * color.rgb;

	// 鏡面反射光
	float3 reflectDir = 2.0f * normalDir * dot(normalDir, -lightDir) + lightDir;
	float specular = pow(saturate(dot(reflectDir, -cameraDir)), Material.Shininess);
	outDiffuse.rgb += lightColor * specular * Material.Specular.rgb;
	// toon = (float)(int)(lightColor * specular * 2.0f)*0.5f;
	// outDiffuse.rgb += toon * Material.Specular.rgb;
}
// 点光源
void PixelShaderPL(in  float4 inPosition		: SV_Position,
	in  float4 inNormal : NORMAL0,
	in  float2 inTexCoord : TEXCOORD0,
	in  float4 inDiffuse : COLOR0,
	in  float4 inWorldPos : POSITION0,

	out float4 outDiffuse : SV_Target)
{
	// テクスチャから色をサンプリング
	float4 color = inDiffuse * g_Texture.Sample(g_SamplerState, inTexCoord);

	// アルファ値以外を初期化（加算していく）
	outDiffuse = float4(0.0f, 0.0f, 0.0f, color.a);

	// カメラの向きと頂点の法線ベクトル
	float3 cameraDir = normalize(inWorldPos.xyz - Camera.Position.xyz);
	float3 normalDir = normalize(inNormal.xyz);

	// ライトの色と方向と距離
	float3 lightColor = PointLight[lightNo].Color.rgb * PointLight[lightNo].Intensity;
	float3 lightVec = inWorldPos.xyz - PointLight[lightNo].Position.xyz;
	float3 lightDir = normalize(lightVec);
	float lightLen = length(lightVec);
	float len2 = lightLen * lightLen;
	float att2 = PointLight[lightNo].Attenuation * PointLight[lightNo].Attenuation;
	float attenuation;	// 減衰率
	if (PointLight[lightNo].Color.a > 0.0f)
	{
		attenuation = 1.0f - len2 / att2;	// 距離の2乗に反比例
	}
	else
	{
		attenuation = 1.0f;	// 減衰無し
		//attenuation = 1.0f - lightLen/PointLight[lightNo].Attenuation;	// 距離に反比例
	}
	if (attenuation < 0.0f) attenuation = 0.0f;	// 負は無し

	// 減衰させる
	lightColor *= attenuation;

	// 拡散反射光
	float diffuse = saturate(dot(-lightDir, normalDir));
	outDiffuse.rgb += lightColor * diffuse  * Material.Diffuse.rgb * color.rgb;
	// float diffuse = saturate(dot(-lightDir, normalDir));
	// float toon = (float)(int)(lightColor * diffuse * Material.Diffuse.rgb * 4.0f)*0.25f;
	// outDiffuse.rgb += toon * color.rgb;

	// 鏡面反射光
	float3 reflectDir = 2.0f * normalDir * dot(normalDir, -lightDir) + lightDir;
	float specular = pow(saturate(dot(reflectDir, -cameraDir)), Material.Shininess);
	outDiffuse.rgb += lightColor * specular * Material.Specular.rgb;
	// toon = (float)(int)(lightColor * specular * 2.0f)*0.5f;
	// outDiffuse.rgb += toon * Material.Specular.rgb;
}
// スポットライト
void PixelShaderSL(in  float4 inPosition		: SV_Position,
	in  float4 inNormal : NORMAL0,
	in  float2 inTexCoord : TEXCOORD0,
	in  float4 inDiffuse : COLOR0,
	in  float4 inWorldPos : POSITION0,

	out float4 outDiffuse : SV_Target)
{
	// テクスチャから色をサンプリング
	float4 color = inDiffuse * g_Texture.Sample(g_SamplerState, inTexCoord);

	// アルファ値以外を初期化（加算していく）
	outDiffuse = float4(0.0f, 0.0f, 0.0f, color.a);

	// ライトの方向と角度
	float3 lightVec = inWorldPos.xyz - SpotLight[lightNo].Position.xyz;
	float3 lightDir = normalize(lightVec);
	float3 lightDir0 = normalize(SpotLight[lightNo].Direction.xyz);
	if (acos(dot(lightDir, lightDir0)) <= SpotLight[lightNo].Angle)
	{
		// カメラの向きと頂点の法線ベクトル
		float3 cameraDir = normalize(inWorldPos.xyz - Camera.Position.xyz);
		float3 normalDir = normalize(inNormal.xyz);

		// ライトの色と距離
		float3 lightColor = SpotLight[lightNo].Color.rgb * SpotLight[lightNo].Intensity;
		float lightLen = length(lightVec);
		float len2 = lightLen * lightLen;
		float att2 = SpotLight[lightNo].Attenuation * SpotLight[lightNo].Attenuation;
		float attenuation = 1.0f - len2 / att2;	// 減衰率
		if (attenuation < 0.0f) attenuation = 0.0f;	// 負は無し

		// 減衰させる
		lightColor *= attenuation;

		// 拡散反射光
		float diffuse = saturate(dot(-lightDir, normalDir));
		outDiffuse.rgb += lightColor * diffuse  * Material.Diffuse.rgb * color.rgb;
		// float diffuse = saturate(dot(-lightDir, normalDir));
		// float toon = (float)(int)(lightColor * diffuse * Material.Diffuse.rgb * 4.0f)*0.25f;
		// outDiffuse.rgb += toon * color.rgb;

		// 鏡面反射光
		float3 reflectDir = 2.0f * normalDir * dot(normalDir, -lightDir) + lightDir;
		float specular = pow(saturate(dot(reflectDir, -cameraDir)), Material.Shininess);
		outDiffuse.rgb += lightColor * specular * Material.Specular.rgb;
		//toon = (float)(int)(lightColor * specular * 2.0f)*0.5f;
		//outDiffuse.rgb += toon * Material.Specular.rgb;
	}
}

//=====================================
// ジオメトリシェーダー
//=====================================
struct GS_INPUT {
	float4 Position	: SV_POSITION;
	float4 Normal	: NORMAL0;
	float2 TexCoord	: TEXCOORD0;
	float4 Diffuse	: COLOR0;
	float4 WorldPos	: POSITION0;
};
struct PS_INPUT {
	float4 Position	: SV_POSITION;
	float4 Normal	: NORMAL0;
	float2 TexCoord	: TEXCOORD0;
	float4 Diffuse	: COLOR0;
	float4 WorldPos	: POSITION0;
};
#define SHADOW_LENGTH 1000.0f	// 影を伸ばす距離
// 平行光源のシャドウボリューム作成
[MaxVertexCount(18)]	// 3edge*1square(=2triangle)*3vertex
void GeometryShaderDL(triangle GS_INPUT Input[3], inout TriangleStream<PS_INPUT> Output)
{
	float3 lightDir = DirectionalLight[lightNo].Direction.xyz;
	float3 normalDirAVE = (Input[0].Normal.xyz + Input[1].Normal.xyz + Input[2].Normal.xyz) / 3;
	if (dot(-lightDir, normalDirAVE) <= 0.0f)	// 陰になる角度
	{
		PS_INPUT NewVtx;
		uint vtx0, vtx1;
		float4 newWorldPos0, newWorldPos1, newNormal;
		for (int e = 0; e < 3; e++)	// 辺ごとに影を伸ばす
		{
			vtx0 = e;
			vtx1 = (e + 1) % 3;
			newWorldPos0 = float4(Input[vtx0].WorldPos.xyz + lightDir * SHADOW_LENGTH, 1.0f);	// wは1.0f
			newWorldPos1 = float4(Input[vtx1].WorldPos.xyz + lightDir * SHADOW_LENGTH, 1.0f);
			newNormal = float4(cross(newWorldPos1.xyz - newWorldPos0.xyz, lightDir), 1.0f);

			// 1つ目の三角形
			NewVtx.Position = Input[vtx0].Position;
			NewVtx.WorldPos = Input[vtx0].WorldPos;
			NewVtx.Normal = newNormal;
			NewVtx.TexCoord = Input[vtx0].TexCoord;
			NewVtx.Diffuse = Input[vtx0].Diffuse;
			Output.Append(NewVtx);
			NewVtx.Position = Input[vtx1].Position;
			NewVtx.WorldPos = Input[vtx1].WorldPos;
			NewVtx.Normal = newNormal;
			NewVtx.TexCoord = Input[vtx1].TexCoord;
			NewVtx.Diffuse = Input[vtx1].Diffuse;
			Output.Append(NewVtx);
			NewVtx.Position = mul(newWorldPos0, VP);
			NewVtx.WorldPos = newWorldPos0;
			NewVtx.Normal = newNormal;
			NewVtx.TexCoord = Input[vtx0].TexCoord;
			NewVtx.Diffuse = Input[vtx0].Diffuse;
			Output.Append(NewVtx);
			Output.RestartStrip();

			// 2つ目の三角形
			NewVtx.Position = Input[vtx1].Position;
			NewVtx.WorldPos = Input[vtx1].WorldPos;
			NewVtx.Normal = newNormal;
			NewVtx.TexCoord = Input[vtx1].TexCoord;
			NewVtx.Diffuse = Input[vtx1].Diffuse;
			Output.Append(NewVtx);
			NewVtx.Position = mul(newWorldPos1, VP);
			NewVtx.WorldPos = newWorldPos1;
			NewVtx.Normal = newNormal;
			NewVtx.TexCoord = Input[vtx1].TexCoord;
			NewVtx.Diffuse = Input[vtx1].Diffuse;
			Output.Append(NewVtx);
			NewVtx.Position = mul(newWorldPos0, VP);
			NewVtx.WorldPos = newWorldPos0;
			NewVtx.Normal = newNormal;
			NewVtx.TexCoord = Input[vtx0].TexCoord;
			NewVtx.Diffuse = Input[vtx0].Diffuse;
			Output.Append(NewVtx);
			Output.RestartStrip();
		}
	}
}
// 点光源のシャドウボリューム作成
[MaxVertexCount(18)]	// 3edge*1square(=2triangle)*3vertex
void GeometryShaderPL(triangle GS_INPUT Input[3], inout TriangleStream<PS_INPUT> Output)
{
	float3 lightDir[3] = {
		Input[0].WorldPos.xyz - PointLight[lightNo].Position.xyz,
		Input[1].WorldPos.xyz - PointLight[lightNo].Position.xyz,
		Input[2].WorldPos.xyz - PointLight[lightNo].Position.xyz
	};
	float3 lightDirAVE = (lightDir[0] + lightDir[1] + lightDir[2]) / 3;
	float3 normalDirAVE = (Input[0].Normal.xyz + Input[1].Normal.xyz + Input[2].Normal.xyz) / 3;
	if (dot(-lightDirAVE, normalDirAVE) <= 0.0f)	// 陰になる角度
	{
		PS_INPUT NewVtx;
		uint vtx0, vtx1;
		float4 newWorldPos0, newWorldPos1, newNormal;
		for (int e = 0; e < 3; e++)	// 辺ごとに影を伸ばす
		{
			vtx0 = e;
			vtx1 = (e + 1) % 3;
			newWorldPos0 = float4(Input[vtx0].WorldPos.xyz + lightDir[vtx0] * SHADOW_LENGTH, 1.0f);	// wは1.0f
			newWorldPos1 = float4(Input[vtx1].WorldPos.xyz + lightDir[vtx1] * SHADOW_LENGTH, 1.0f);
			newNormal = float4(cross(newWorldPos1.xyz - newWorldPos0.xyz, lightDir[vtx0]), 1.0f);

			// 1つ目の三角形
			NewVtx.Position = Input[vtx0].Position;
			NewVtx.WorldPos = Input[vtx0].WorldPos;
			NewVtx.Normal = newNormal;
			NewVtx.TexCoord = Input[vtx0].TexCoord;
			NewVtx.Diffuse = Input[vtx0].Diffuse;
			Output.Append(NewVtx);
			NewVtx.Position = Input[vtx1].Position;
			NewVtx.WorldPos = Input[vtx1].WorldPos;
			NewVtx.Normal = newNormal;
			NewVtx.TexCoord = Input[vtx1].TexCoord;
			NewVtx.Diffuse = Input[vtx1].Diffuse;
			Output.Append(NewVtx);
			NewVtx.Position = mul(newWorldPos0, VP);
			NewVtx.WorldPos = newWorldPos0;
			NewVtx.Normal = newNormal;
			NewVtx.TexCoord = Input[vtx0].TexCoord;
			NewVtx.Diffuse = Input[vtx0].Diffuse;
			Output.Append(NewVtx);
			Output.RestartStrip();

			// 2つ目の三角形
			NewVtx.Position = Input[vtx1].Position;
			NewVtx.WorldPos = Input[vtx1].WorldPos;
			NewVtx.Normal = newNormal;
			NewVtx.TexCoord = Input[vtx1].TexCoord;
			NewVtx.Diffuse = Input[vtx1].Diffuse;
			Output.Append(NewVtx);
			NewVtx.Position = mul(newWorldPos1, VP);
			NewVtx.WorldPos = newWorldPos1;
			NewVtx.Normal = newNormal;
			NewVtx.TexCoord = Input[vtx1].TexCoord;
			NewVtx.Diffuse = Input[vtx1].Diffuse;
			Output.Append(NewVtx);
			NewVtx.Position = mul(newWorldPos0, VP);
			NewVtx.WorldPos = newWorldPos0;
			NewVtx.Normal = newNormal;
			NewVtx.TexCoord = Input[vtx0].TexCoord;
			NewVtx.Diffuse = Input[vtx0].Diffuse;
			Output.Append(NewVtx);
			Output.RestartStrip();
		}
	}
}
// スポットライトのシャドウボリューム作成
[MaxVertexCount(18)]	// 3edge*1square(=2triangle)*3vertex
void GeometryShaderSL(triangle GS_INPUT Input[3], inout TriangleStream<PS_INPUT> Output)
{
	float3 lightDir[3] = {
		Input[0].WorldPos.xyz - SpotLight[lightNo].Position.xyz,
		Input[1].WorldPos.xyz - SpotLight[lightNo].Position.xyz,
		Input[2].WorldPos.xyz - SpotLight[lightNo].Position.xyz
	};
	float3 lightDirAVE = (lightDir[0] + lightDir[1] + lightDir[2]) / 3;
	float3 normalDirAVE = (Input[0].Normal.xyz + Input[1].Normal.xyz + Input[2].Normal.xyz) / 3;
	if (dot(-lightDirAVE, normalDirAVE) <= 0.0f)	// 陰になる角度
	{
		PS_INPUT NewVtx;
		uint vtx0, vtx1;
		float4 newWorldPos0, newWorldPos1, newNormal;
		for (int e = 0; e < 3; e++)	// 辺ごとに影を伸ばす
		{
			vtx0 = e;
			vtx1 = (e + 1) % 3;
			newWorldPos0 = float4(Input[vtx0].WorldPos.xyz + lightDir[vtx0] * SHADOW_LENGTH, 1.0f);	// wは1.0f
			newWorldPos1 = float4(Input[vtx1].WorldPos.xyz + lightDir[vtx1] * SHADOW_LENGTH, 1.0f);
			newNormal = float4(cross(newWorldPos1.xyz - newWorldPos0.xyz, lightDir[vtx0]), 1.0f);

			// 1つ目の三角形
			NewVtx.Position = Input[vtx0].Position;
			NewVtx.WorldPos = Input[vtx0].WorldPos;
			NewVtx.Normal = newNormal;
			NewVtx.TexCoord = Input[vtx0].TexCoord;
			NewVtx.Diffuse = Input[vtx0].Diffuse;
			Output.Append(NewVtx);
			NewVtx.Position = Input[vtx1].Position;
			NewVtx.WorldPos = Input[vtx1].WorldPos;
			NewVtx.Normal = newNormal;
			NewVtx.TexCoord = Input[vtx1].TexCoord;
			NewVtx.Diffuse = Input[vtx1].Diffuse;
			Output.Append(NewVtx);
			NewVtx.Position = mul(newWorldPos0, VP);
			NewVtx.WorldPos = newWorldPos0;
			NewVtx.Normal = newNormal;
			NewVtx.TexCoord = Input[vtx0].TexCoord;
			NewVtx.Diffuse = Input[vtx0].Diffuse;
			Output.Append(NewVtx);
			Output.RestartStrip();

			// 2つ目の三角形
			NewVtx.Position = Input[vtx1].Position;
			NewVtx.WorldPos = Input[vtx1].WorldPos;
			NewVtx.Normal = newNormal;
			NewVtx.TexCoord = Input[vtx1].TexCoord;
			NewVtx.Diffuse = Input[vtx1].Diffuse;
			Output.Append(NewVtx);
			NewVtx.Position = mul(newWorldPos1, VP);
			NewVtx.WorldPos = newWorldPos1;
			NewVtx.Normal = newNormal;
			NewVtx.TexCoord = Input[vtx1].TexCoord;
			NewVtx.Diffuse = Input[vtx1].Diffuse;
			Output.Append(NewVtx);
			NewVtx.Position = mul(newWorldPos0, VP);
			NewVtx.WorldPos = newWorldPos0;
			NewVtx.Normal = newNormal;
			NewVtx.TexCoord = Input[vtx0].TexCoord;
			NewVtx.Diffuse = Input[vtx0].Diffuse;
			Output.Append(NewVtx);
			Output.RestartStrip();
		}
	}
}