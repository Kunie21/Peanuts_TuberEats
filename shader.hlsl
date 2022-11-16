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
void VSPolygon(
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
//VS_OUTPUT VSPolygon(VS_INPUT input) {
//	VS_OUTPUT output;
//	output.Position = mul(input.Position, WVP);
//	output.WorldPos = mul(input.Position, World);
//	output.Normal = normalize(mul(float4(input.Normal.xyz, 0.0f), World));
//	output.TexCoord = input.TexCoord;
//	output.Diffuse = input.Diffuse * Material.Diffuse;
//	return output;
//}

// パイプ専用頂点シェーダ（テクスチャ移動付き）
VS_OUTPUT VSTube(VS_INPUT input) {
	VS_OUTPUT output;
	output.WorldPos = mul(input.Position, World);
	output.Position = GetTubeCurvePos(output.WorldPos);
	output.Normal = normalize(mul(float4(input.Normal.xyz, 0.0f), World));
	output.TexCoord = float2(input.TexCoord.x, input.TexCoord.y - Curve.TexPos);	// テクスチャ移動
	output.Diffuse = input.Diffuse * Material.Diffuse;
	return output;
}

// パイプ中のギミック用頂点シェーダ（テクスチャ移動無し）
VS_OUTPUT VSGimmick(VS_INPUT input) {
	VS_OUTPUT output;
	output.WorldPos = mul(input.Position, World);
	output.Position = GetTubeCurvePos(output.WorldPos);
	output.Normal = normalize(mul(float4(input.Normal.xyz, 0.0f), World));
	output.TexCoord = input.TexCoord;
	output.Diffuse = input.Diffuse * Material.Diffuse;
	return output;
}

// プレイヤー用頂点シェーダ
VS_OUTPUT VSPlayer(VS_INPUT input) {
	VS_OUTPUT output;
	output.Position = mul(input.Position, WVP);
	output.WorldPos = mul(mul(input.Position, World), transpose(AfterRot));
	output.Normal = normalize(mul(mul(float4(input.Normal.xyz, 0.0f), World), transpose(AfterRot)));
	output.TexCoord = input.TexCoord;
	output.Diffuse = input.Diffuse * Material.Diffuse;
	return output;
}

// アウトライン用
VS_OUTPUT VSOutline(VS_INPUT input) {	// 法線方向に膨らませる
	VS_OUTPUT output;
	input.Position.xyz = input.Position.xyz + normalize(input.Normal.xyz) * Outline.Scale.x;
	output.Position = GetTubeCurvePos(mul(input.Position, World));
	return output;
}
VS_OUTPUT VSOutlineInstancing(VS_INPUT input, uint instID : SV_InstanceID) {	// 法線方向に膨らませる
	VS_OUTPUT output;
	matrix mtxWorld = GetMtxWorld(Instance.pos[instID], Instance.rot[instID], Instance.scl[instID]);
	input.Position.xyz = input.Position.xyz + normalize(input.Normal.xyz) * Outline.Scale.x;
	output.Position = GetTubeCurvePos(mul(input.Position, mtxWorld));
	return output;
}
float4 PSOutline(VS_OUTPUT input) : SV_Target{
	return Outline.Color;
}


//=============================================================================
// インスタンシング描画用
//=============================================================================
VS_OUTPUT VSInstancing(VS_INPUT input, uint instID : SV_InstanceID) {
	VS_OUTPUT output;
	matrix mtxWorld = GetMtxWorld(Instance.pos[instID], Instance.rot[instID], Instance.scl[instID]);
	output.WorldPos = mul(input.Position, mtxWorld);
	output.Position = GetTubeCurvePos(output.WorldPos);
	output.WorldPos.w = Instance.pos[instID].w;
	output.Normal = normalize(mul(float4(input.Normal.xyz, 0.0f), mtxWorld));
	output.TexCoord = input.TexCoord;
	output.Diffuse = input.Diffuse * Instance.col[instID] * Material.Diffuse;
	return output;
}
// ポリゴン爆発
IS_OUTPUT VSEX(VS_INPUT input, uint instID : SV_InstanceID) {
	IS_OUTPUT output;
	output.Position = input.Position;
	output.WorldPos = input.Position;
	output.Normal = input.Normal;
	output.TexCoord = input.TexCoord;
	output.Id = instID;
	output.Diffuse = input.Diffuse * Instance.col[instID] * Material.Diffuse;
	return output;
}

IS_OUTPUT VSInstancingTexture(VS_INPUT input, uint instID : SV_InstanceID) {
	IS_OUTPUT output;
	matrix mtxWorld = GetMtxWorld(Instance.pos[instID], Instance.rot[instID], Instance.scl[instID]);
	output.Position = mul(input.Position, mul(mtxWorld, VP));
	output.WorldPos = mul(input.Position, mtxWorld);
	output.Normal = normalize(mul(float4(input.Normal.xyz, 0.0f), mtxWorld));
	output.TexCoord = input.TexCoord;
	float4 tc = Instance.txc[instID];
	if (input.TexCoord.x == 0.0f && input.TexCoord.y == 0.0f) output.TexCoord = float2(tc.x, tc.y);
	if (input.TexCoord.x == 1.0f && input.TexCoord.y == 0.0f) output.TexCoord = float2(tc.x + tc.z, tc.y);
	if (input.TexCoord.x == 0.0f && input.TexCoord.y == 1.0f) output.TexCoord = float2(tc.x, tc.y + tc.w);
	if (input.TexCoord.x == 1.0f && input.TexCoord.y == 1.0f) output.TexCoord = float2(tc.x + tc.z, tc.y + tc.w);
	output.Diffuse = input.Diffuse * Instance.col[instID];
	output.Id = instID;
	return output;
 }
float4 PSInstancingOnlyTex(IS_OUTPUT input) : SV_Target{
	return input.Diffuse * GetColorFromTextureArray(input.Id, input.TexCoord);
}

// void VSInstancingBillboard(in  float4 inPosition		: POSITION0,
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

//=============================================================================
// ピクセルシェーダ
//=============================================================================

// 光源別 ////////////////
// 環境光
float4 PSAL(VS_OUTPUT input) : SV_Target {

	// テクスチャから色をサンプリング
	float4 TexColor = input.Diffuse * g_Texture.Sample(g_SamplerState, input.TexCoord);

	// アルファ値以外を初期化（加算していく）
	float4 outDiffuse = float4(0.0f, 0.0f, 0.0f, TexColor.a);

	// 環境光の色を反映
	float3 lightColor = AmbientLight.Color.rgb * AmbientLight.Intensity;
	outDiffuse.rgb += lightColor * Material.Ambient.rgb * TexColor.rgb;

	return outDiffuse;
}

// ラインライト
float4 PSLL(IS_OUTPUT input) : SV_Target {

	//int instID = input.Id;
	//matrix mtxWorld = GetMtxWorld(Instance.pos[instID], Instance.rot[instID], Instance.scl[instID]);
	//input.WorldPos = mul(input.Position, mtxWorld);
	//input.Position = GetTubeCurvePos(input.WorldPos);
	//input.Normal = normalize(mul(float4(input.Normal.xyz, 0.0f), mtxWorld));
	//input.TexCoord = input.TexCoord;
	//input.Diffuse = input.Diffuse * Instance.col[instID] * Material.Diffuse;

	//matrix mtxWorld = GetMtxWorld(Instance.pos[instID], Instance.rot[instID], Instance.scl[instID]);
	////input[i].Position.xyz = input[i].Position.xyz + normalize(input[i].Normal.xyz) * Instance.pos[instID].w;
	//input.Position.xyz = input.Position.xyz + normalize(input.Normal.xyz) * 5.0f;
	//input.WorldPos = mul(input.Position, mtxWorld);
	//input.Position = GetTubeCurvePos(input.WorldPos);
	//input.Normal = normalize(mul(float4(input.Normal.xyz, 0.0f), mtxWorld));

	//input.TexCoord = input.TexCoord;
	//NewVtx.Diffuse = input.Diffuse;
	//input.Diffuse = float4(1.0f, 1.0f, 1.0f, 1.0f);

	// テクスチャから色をサンプリング
	float4 color = input.Diffuse * g_Texture.Sample(g_SamplerState, input.TexCoord);

	// アルファ値以外を初期化（加算していく）
	float4 outDiffuse = float4(0.0f, 0.0f, 0.0f, color.a);

	// カメラの向きと頂点の法線ベクトル
	float3 cameraDir = normalize(input.WorldPos.xyz - Camera.Position.xyz);
	float3 normalDir = normalize(input.Normal.xyz);

	// ライトの色と方向と距離
	float3 lightColor = float3(1.0f, 1.0f, 1.0f) * 0.4f;
	float3 lightVec = float3(input.WorldPos.x, input.WorldPos.y - LL_POS, 0.0f);
	float3 lightDir = normalize(lightVec);
	float lightLen = length(lightVec);
	float len2 = lightLen * lightLen;
	float att2 = 4000000.0f;
	float attenuation;	// 減衰率
	attenuation = 1.0f - len2 / att2;	// 距離の2乗に反比例
	if (attenuation < 0.0f) attenuation = 0.0f;	// 負は無し

	// 減衰させる
	lightColor *= attenuation;

	// 拡散反射光を加算合成
	float diffuse = saturate(dot(-lightDir, normalDir));
	outDiffuse.rgb += lightColor * diffuse  * Material.Diffuse.rgb * color.rgb;

	// 鏡面反射光を加算合成
	float3 reflectDir = 2.0f * normalDir * dot(normalDir, -lightDir) + lightDir;
	float specular = pow(saturate(dot(reflectDir, -cameraDir)), Material.Shininess);
	outDiffuse.rgb += lightColor * specular * Material.Specular.rgb;

	// 環境光の色を反映
	float3 AmbientLightColor = AmbientLight.Color.rgb * AmbientLight.Intensity;
	outDiffuse.rgb += AmbientLightColor * Material.Ambient.rgb * color.rgb;

	// フォグ計算
	float z = input.Position.z * input.Position.w;
	float f = saturate((20000.0f - z) * 0.00005f);
	outDiffuse = f * outDiffuse + (1 - f) * float4(0.8f, 0.9f, 1.0f, 1.0f);
	outDiffuse.a = color.a;

	return outDiffuse;
}

// 平行光源
void PSDL(in  float4 inPosition	: SV_Position,
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
void PSPL(in  float4 inPosition		: SV_Position,
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
void PSSL(in  float4 inPosition		: SV_Position,
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

// 基本形（光源まとめて計算）
void PSPolygon(in  float4 inPosition		: SV_Position,
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


//=====================================
// ジオメトリシェーダー
//=====================================
#define SHADOW_LENGTH 1000.0f	// 影を伸ばす距離
// 平行光源のシャドウボリューム作成
[MaxVertexCount(18)]	// 3edge*1square(=2triangle)*3vertex
void GSDL(triangle GS_INPUT Input[3], inout TriangleStream<PS_INPUT> Output)
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
void GSPL(triangle GS_INPUT Input[3], inout TriangleStream<PS_INPUT> Output)
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
			NewVtx.Position = GetTubeCurvePos(newWorldPos0);
			//NewVtx.Position = mul(newWorldPos0, VP);
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
			NewVtx.Position = GetTubeCurvePos(newWorldPos1);
			//NewVtx.Position = mul(newWorldPos1, VP);
			NewVtx.WorldPos = newWorldPos1;
			NewVtx.Normal = newNormal;
			NewVtx.TexCoord = Input[vtx1].TexCoord;
			NewVtx.Diffuse = Input[vtx1].Diffuse;
			Output.Append(NewVtx);
			NewVtx.Position = GetTubeCurvePos(newWorldPos0);
			//NewVtx.Position = mul(newWorldPos0, VP);
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
void GSSL(triangle GS_INPUT Input[3], inout TriangleStream<PS_INPUT> Output)
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
// 線光源のシャドウボリューム作成
[MaxVertexCount(18)]	// 3edge*1square(=2triangle)*3vertex
void GSLLPlayer(triangle GS_INPUT Input[3], inout TriangleStream<PS_INPUT> Output)
{
	//float4 LineLightPos = GetTubeCurvePos(mul(float4(0.0f, LL_POS, 0.0f, 1.0f), World));
	//float3 lightDir[3] = {
	//	Input[0].WorldPos.xyz - LineLightPos.xyz,
	//	Input[1].WorldPos.xyz - LineLightPos.xyz,
	//	Input[2].WorldPos.xyz - LineLightPos.xyz
	//};

	float4 LineLightPos = { 0.0f, LL_POS, World._43, 1.0f };
	float3 lightDir[3] = {
		Input[0].WorldPos.xyz - LineLightPos.xyz,
		Input[1].WorldPos.xyz - LineLightPos.xyz,
		Input[2].WorldPos.xyz - LineLightPos.xyz
	};

	//float3 lightDir[3] = {
	//	float3(Input[0].WorldPos.x, Input[0].WorldPos.y - LL_POS, 0.0f),
	//	float3(Input[1].WorldPos.x, Input[1].WorldPos.y - LL_POS, 0.0f),
	//	float3(Input[2].WorldPos.x, Input[2].WorldPos.y - LL_POS, 0.0f)
	//};

	//float4 LineLightPos = { 0.0f, LL_POS, World._43, 1.0f };
	//float3 PosAVE = (Input[0].WorldPos + Input[1].WorldPos + Input[2].WorldPos) / 3;
	//float3 lightDir = PosAVE - LineLightPos;

	//float3 lightDir = -float3(0.0f, LL_POS, World._43);
	//float3 lightDir = float3(World._41, World._42 - LL_POS, 0.0f);

	float3 lightDirAVE = (lightDir[0] + lightDir[1] + lightDir[2]) / 3;
	float3 normalDirAVE = (Input[0].Normal.xyz + Input[1].Normal.xyz + Input[2].Normal.xyz) * 0.33333f;
	//float d = dot(-normalize(lightDirAVE), normalize(normalDirAVE));
	//if (-Outline.Color.z < d && d <= 0.0f)	// 陰になる角度
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
			//NewVtx.Position = mul(newWorldPos0, VP);
			//NewVtx.Position = GetTubeCurvePos(newWorldPos0);
			NewVtx.Position = mul(mul(newWorldPos0, AfterRot), VP);
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
			//NewVtx.Position = mul(newWorldPos1, VP);
			//NewVtx.Position = GetTubeCurvePos(newWorldPos1);
			NewVtx.Position = mul(mul(newWorldPos1, AfterRot), VP);
			NewVtx.WorldPos = newWorldPos1;
			NewVtx.Normal = newNormal;
			NewVtx.TexCoord = Input[vtx1].TexCoord;
			NewVtx.Diffuse = Input[vtx1].Diffuse;
			Output.Append(NewVtx);
			//NewVtx.Position = mul(newWorldPos0, VP);
			//NewVtx.Position = GetTubeCurvePos(newWorldPos0);
			NewVtx.Position = mul(mul(newWorldPos0, AfterRot), VP);
			NewVtx.WorldPos = newWorldPos0;
			NewVtx.Normal = newNormal;
			NewVtx.TexCoord = Input[vtx0].TexCoord;
			NewVtx.Diffuse = Input[vtx0].Diffuse;
			Output.Append(NewVtx);
			Output.RestartStrip();
		}
	}
}
[MaxVertexCount(18)]	// 3edge*1square(=2triangle)*3vertex
void GSLLNonPlayer(triangle GS_INPUT Input[3], inout TriangleStream<PS_INPUT> Output)
{
	//float3 lightDir = { 0.0f, -1.0f, World._43 };

	//float4 LineLightPos = { 0.0f, LL_POS, World._43, 1.0f };
	//float3 lightDir[3] = {
	//	Input[0].WorldPos.xyz - LineLightPos.xyz,
	//	Input[1].WorldPos.xyz - LineLightPos.xyz,
	//	Input[2].WorldPos.xyz - LineLightPos.xyz
	//};
	//float3 lightDirAVE = (lightDir[0] + lightDir[1] + lightDir[2]) / 3;

	float3 lightDir = float3(World._41, World._42 - LL_POS, 0.0f);	// ワールド座標（モデルの中心）のx, yからライトの方向を作る
	float3 normalDirAVE = (Input[0].Normal.xyz + Input[1].Normal.xyz + Input[2].Normal.xyz) * 0.33333f;
	//float d = dot(-normalize(lightDir), normalize(normalDirAVE));
	//if (-Outline.Color.z < d && d <= 0.0f)	// 陰になる角度
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
			//NewVtx.Position = mul(newWorldPos0, VP);
			//NewVtx.Position = mul(GetTubeCurvePos(newWorldPos0), transpose(AfterRot));
			NewVtx.Position = GetTubeCurvePos(newWorldPos0);
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
			//NewVtx.Position = mul(newWorldPos1, VP);
			//NewVtx.Position = mul(GetTubeCurvePos(newWorldPos1), transpose(AfterRot));
			NewVtx.Position = GetTubeCurvePos(newWorldPos1);
			NewVtx.WorldPos = newWorldPos1;
			NewVtx.Normal = newNormal;
			NewVtx.TexCoord = Input[vtx1].TexCoord;
			NewVtx.Diffuse = Input[vtx1].Diffuse;
			Output.Append(NewVtx);
			//NewVtx.Position = mul(newWorldPos0, VP);
			//NewVtx.Position = mul(GetTubeCurvePos(newWorldPos0), transpose(AfterRot));
			NewVtx.Position = GetTubeCurvePos(newWorldPos0);
			NewVtx.WorldPos = newWorldPos0;
			NewVtx.Normal = newNormal;
			NewVtx.TexCoord = Input[vtx0].TexCoord;
			NewVtx.Diffuse = Input[vtx0].Diffuse;
			Output.Append(NewVtx);
			Output.RestartStrip();
		}
	}
}

// ポリゴン爆発
[MaxVertexCount(3)]	// 3edge*1square(=2triangle)*3vertex
void GSEX(triangle VS_OUTPUT input[3], inout TriangleStream<PS_INPUT> output)
{
	PS_INPUT NewVtx;

	//int instID = input[0].Id;
	//matrix mtxWorld = GetMtxWorld(Instance.pos[instID], Instance.rot[instID], Instance.scl[instID]);
	float3 normalDirAVE = normalize((input[0].Normal.xyz + input[1].Normal.xyz + input[2].Normal.xyz) * 0.33333f);
	float length = input[0].WorldPos.w;

	// 新しい三角形
	for (int i = 0; i < 3; i++)
	{
		//int instID = input[0].Id;
		//matrix mtxWorld = GetMtxWorld(Instance.pos[instID], Instance.rot[instID], Instance.scl[instID]);

		//input[i].Position.xyz = input[i].Position.xyz + normalize(input[i].Normal.xyz) * Instance.pos[instID].w;
		//input[i].Position.xyz = input[i].Position.xyz + normalize(input[i].Normal.xyz) * 5.0f;
		//NewVtx.WorldPos = mul(input[i].Position, mtxWorld);
		//NewVtx.Position = GetTubeCurvePos(NewVtx.WorldPos);

		input[i].WorldPos.xyz = input[i].WorldPos.xyz + normalDirAVE * length;
		NewVtx.WorldPos = input[i].WorldPos;
		//NewVtx.WorldPos = float4(input[i].WorldPos.xyz, 0.0f);
		NewVtx.Position = GetTubeCurvePos(float4(input[i].WorldPos.xyz, 1.0f));
		NewVtx.Normal = input[i].Normal;
		NewVtx.TexCoord = input[i].TexCoord;
		NewVtx.Diffuse = input[i].Diffuse;
		output.Append(NewVtx);
	}

	output.RestartStrip();
}
