//************************************
//  ポストエフェクト関連シェーダー
//  クラス　GP12 
//  出席番号　11
//  作者　國江翔太
//************************************
#include "shader.h"

// フィルター ///////////////////////////////////////////////////
VS_OUTPUT VSFilter(VS_INPUT input) {
	VS_OUTPUT output;
	output.Position = mul(input.Position, WVP);
	output.TexCoord = input.TexCoord;
	output.Diffuse = input.Diffuse * Material.Diffuse;
	return output;
}
float4 PSFilter(VS_OUTPUT input) : SV_Target{
	uint width, height;
	g_Texture.GetDimensions(width, height);
	int pos_x = int(input.TexCoord.x * width);
	int pos_y = int(input.TexCoord.y * height);
	float4 outDiffuse = float4(0.0f, 0.0f, 0.0f, 1.0f);
	for (int x = -1; x <= 1; x++) {
		for (int y = -1; y <= 1; y++) {
			if (pos_x + x < 0 || pos_y + y < 0 || pos_x + x >= (int)width || pos_y + y >= (int)height) {
				outDiffuse.rgb += filter[y + 1][x + 1] * g_Texture.Load(int3(pos_x, pos_y, 0)).rgb;
				continue;
			}
			outDiffuse.rgb += filter[y + 1][x + 1] * g_Texture.Load(int3(pos_x + x, pos_y + y, 0)).rgb;
		}
	}
	return outDiffuse;
}

float4 PSLight(VS_OUTPUT input) : SV_Target{
	uint width, height;
	g_Texture.GetDimensions(width, height);
	int pos_x = int(input.TexCoord.x * width);
	int pos_y = int(input.TexCoord.y * height);
	float4 outDiffuse = float4(0.0f, 0.0f, 0.0f, 1.0f);
	for (int x = -1; x <= 1; x++) {
		for (int y = -1; y <= 1; y++) {
			if (pos_x + x < 0 || pos_y + y < 0 || pos_x + x >= (int)width || pos_y + y >= (int)height) {
				outDiffuse.rgb += filter[y + 1][x + 1] * g_Texture.Load(int3(pos_x, pos_y, 0)).rgb;
				continue;
			}
			outDiffuse.rgb += filter[y + 1][x + 1] * g_Texture.Load(int3(pos_x + x, pos_y + y, 0)).rgb;
		}
	}
	return outDiffuse * 0.7f;
}

// テクスチャを参照するだけ（変換画像を画面に出力するときに使う）
float4 PSOnlyTex(VS_OUTPUT input) : SV_Target{
	return input.Diffuse * g_Texture.Sample(g_SamplerState, input.TexCoord);
}

float4 PSTest(VS_OUTPUT input) : SV_Target{
	return float4(0.0f, 1.0f, 0.0f, 1.0f);
}

// モーションブラー用
//float4 PSMotionBlur(VS_OUTPUT input) : SV_Target{
//	return input.Diffuse * (
//		(g_Texture.Sample(g_SamplerState, input.TexCoord) * 0.5f) +
//		(g_OldScreen.Sample(g_SamplerState, input.TexCoord) * 0.5f)
//	);
//}
#define MBNUM 10.0f
//float4 PSMotionBlur(VS_OUTPUT input) : SV_Target{
//	float4 outDiffuse;
//
//	float2 pos = input.TexCoord;
//	float2 vec = pos - float2(0.5f, 0.5f);
//
//	//if (length(vec) > 0.8f)
//	{
//		float3 color = float3(0.0f, 0.0f, 0.0f);
//		float rand = GetRandom(input.TexCoord, 0);
//		float weightSum = 0.0f;
//		for (float i = 0.0f; i < MBNUM; i += 1.0f)
//		{
//			float rate = (i + rand) / MBNUM;
//			float weight = rate - rate * rate;
//			float2 at = pos - vec * rate * 1.0f / MBNUM;
//			color += g_Texture.Sample(g_SamplerState, at).rgb * weight;
//			weightSum += weight;
//		}
//		outDiffuse = float4(color / weightSum, 1.0f);
//	}
//	//else
//	//{
//	//	outDiffuse = input.Diffuse * g_Texture.Sample(g_SamplerState, input.TexCoord);
//	//}
//	return outDiffuse;
//}
float4 PSMotionBlur(VS_OUTPUT input) : SV_Target{

	float4 color[10];

	//方向ベクトルの取得
	float2 dir = float2(0.5f, 0.5f) - input.TexCoord;
	//長さの取得
	float len = length(dir);
	//オフセットの取得
	float2 offset = normalize(dir) * 0.0001f;
	offset *= (Curve.Spd * Curve.Spd  *  0.015f * len);

	color[0] = g_Texture.Sample(g_SamplerState, input.TexCoord) * 0.19f;
	color[1] = g_Texture.Sample(g_SamplerState, input.TexCoord + offset) * 0.17f;
	color[2] = g_Texture.Sample(g_SamplerState, input.TexCoord + offset * 2.0f) * 0.15f;
	color[3] = g_Texture.Sample(g_SamplerState, input.TexCoord + offset * 3.0f) * 0.13f;
	color[4] = g_Texture.Sample(g_SamplerState, input.TexCoord + offset * 4.0f) * 0.11f;
	color[5] = g_Texture.Sample(g_SamplerState, input.TexCoord + offset * 5.0f) * 0.09f;
	color[6] = g_Texture.Sample(g_SamplerState, input.TexCoord + offset * 6.0f) * 0.07f;
	color[7] = g_Texture.Sample(g_SamplerState, input.TexCoord + offset * 7.0f) * 0.05f;
	color[8] = g_Texture.Sample(g_SamplerState, input.TexCoord + offset * 8.0f) * 0.03f;
	color[9] = g_Texture.Sample(g_SamplerState, input.TexCoord + offset * 9.0f) * 0.01f;

	return color[0] + color[1] + color[2] + color[3] + color[4] + color[5] + color[6] + color[7] + color[8] + color[9];
}

// モザイク ///////////////////////////////////////////////////
float4 PSMosaic(VS_OUTPUT input) : SV_Target {
	uint width, height;
	g_Texture.GetDimensions(width, height);
	int pos_x = int(input.TexCoord.x * width);
	int pos_y = int(input.TexCoord.y * height);
	//pos_x -= pos_x % Mosaic.Size;
	//pos_y -= pos_y % Mosaic.Size;
	return g_Texture.Load(int3(pos_x, pos_y, 0));
	
	// 1行バージョン
	//outDiffuse = g_Texture.Sample(g_SamplerState, floor(inTexCoord * Mosaic.Size) / Mosaic.Size);
}

// 色収差(RGBずらし) ///////////////////////////////////////////////////
#define ABERRATION 5	// 収差
float4 PSCA(VS_OUTPUT input) : SV_Target{
	uint width, height;
	g_Texture.GetDimensions(width, height);
	int pos_x = int(input.TexCoord.x * width);
	int pos_y = int(input.TexCoord.y * height);
	float4 outDiffuse;
	outDiffuse.r = g_Texture.Load(int3(pos_x + ABERRATION, pos_y + ABERRATION, 0)).r;
	outDiffuse.ga = g_Texture.Load(int3(pos_x, pos_y, 0)).ga;
	outDiffuse.b = g_Texture.Load(int3(pos_x - ABERRATION, pos_y - ABERRATION, 0)).b;
	return outDiffuse;
}

// 監視カメラ風 ///////////////////////////////////////////////////
float4 PSMonitoring(VS_OUTPUT input) : SV_Target{
	float4 outDiffuse;
	outDiffuse = g_Texture.Sample(g_SamplerState, input.TexCoord);
	//outDiffuse -= abs(sin(input.TexCoord.y * 100.0f + Constant.Time * 0.10f)) * 0.04f;
	//outDiffuse -= abs(sin(input.TexCoord.y * 300.0f + Constant.Time * 0.15f)) * 0.08f;
	outDiffuse.a = 1.0;
	return outDiffuse;
}

// ゲームボーイ風 ///////////////////////////////////////////////////
float4 PSOldGame(VS_OUTPUT input) : SV_Target{
	float4 texColor = g_Texture.Sample(g_SamplerState, input.TexCoord);
	float gamma = 1.0;
	texColor.r = pow(texColor.r, gamma);
	texColor.g = pow(texColor.g, gamma);
	texColor.b = pow(texColor.b, gamma);
	float3 col1 = float3(0.612, 0.725, 0.086);
	float3 col2 = float3(0.549, 0.667, 0.078);
	float3 col3 = float3(0.188, 0.392, 0.188);
	float3 col4 = float3(0.063, 0.247, 0.063);
	float dist1 = length(texColor.rgb - col1);
	float dist2 = length(texColor.rgb - col2);
	float dist3 = length(texColor.rgb - col3);
	float dist4 = length(texColor.rgb - col4);
	float d = min(dist1, dist2);
	d = min(d, dist3);
	d = min(d, dist4);
	if(d==dist1){ texColor.rgb = col1; }
	else if(d==dist2){ texColor.rgb = col2; }
	else if(d==dist3){ texColor.rgb = col3; }
	else { texColor.rgb = col4; }
	texColor.a = 1.0;
	return texColor;
}

// ノーマルマップ作成 ///////////////////////////////////////////////////
VS_OUTPUT VSNormalMap(VS_INPUT input) {
	VS_OUTPUT output;
	output.Position = mul(input.Position, WVP);
	output.Normal = normalize(mul(float4(input.Normal.xyz, 0.0f), World));
	return output;
}
float4 PSNormalMap(VS_OUTPUT input) : SV_Target{
	float4 outDiffuse;
	outDiffuse = float4(normalize(input.Normal.xyz), 1.0f);	// 法線ベクトルを色として扱う
	outDiffuse.z *= -1.0f;								// zを反転
	outDiffuse.xyz = (outDiffuse.xyz + 1.0f) * 0.5f;	// -1.0f ~ 1.0f を 0.0f ~ 1.0f にする
	return outDiffuse;
}

// デプスマップ作成 ///////////////////////////////////////////////////
VS_OUTPUT VSDepthMap(VS_INPUT input) {
	VS_OUTPUT output;
	output.Position = mul(input.Position, WVP);
	return output;
}
float4 PSDepthMap(VS_OUTPUT input) : SV_Target{
	float4 outDiffuse;
	outDiffuse = 1.0f - (input.Position.z - Camera.ViewVolume.z) / (Camera.ViewVolume.w - Camera.ViewVolume.z);
	outDiffuse.a = 1.0f;
	return outDiffuse;
}