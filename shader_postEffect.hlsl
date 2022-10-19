//************************************
//  ポストエフェクト関連シェーダー
//  クラス　GP12 
//  出席番号　11
//  作者　國江翔太
//************************************
#include "shader.h"

// フィルター ///////////////////////////////////////////////////
void VertexShaderFilter( in  float4 inPosition		: POSITION0,
						  in  float4 inNormal		: NORMAL0,
						  in  float4 inDiffuse		: COLOR0,
						  in  float2 inTexCoord		: TEXCOORD0,

						  out float4 outPosition	: SV_POSITION,
						  out float2 outTexCoord	: TEXCOORD0)
{
	outPosition = mul(inPosition, WVP);
	outTexCoord = inTexCoord;
}
void PixelShaderFilter( in  float4 inPosition		: POSITION0,
						in  float2 inTexCoord		: TEXCOORD0,
						
						out float4 outDiffuse		: SV_Target )
{
	uint width, height;
	g_Texture.GetDimensions(width, height);
	int pos_x = int(inTexCoord.x * width);
	int pos_y = int(inTexCoord.y * height);
	outDiffuse = float4(0.0f, 0.0f, 0.0f, 1.0f);
	for(int x = -1; x <= 1; x++)
	{
		for(int y = -1; y <= 1; y++)
		{
			if(pos_x + x < 0 || pos_y + y < 0 || pos_x + x >= (int)width || pos_y + y >= (int)height)
			{
				outDiffuse.rgb += filter[y + 1][x + 1] * g_Texture.Load(int3(pos_x, pos_y, 0)).rgb;
				continue;
			}
			outDiffuse.rgb += filter[y + 1][x + 1] * g_Texture.Load(int3(pos_x + x, pos_y + y, 0)).rgb;
		}
	}
}

// テクスチャを参照するだけ（変換画像を画面に出力するときに使う）
void PixelShaderOnlyTex( in  float4 inPosition		: POSITION0,
						in  float2 inTexCoord		: TEXCOORD0,
						
						out float4 outDiffuse		: SV_Target )
{
	outDiffuse = g_Texture.Sample(g_SamplerState, inTexCoord);
}

// モザイク ///////////////////////////////////////////////////
void PixelShaderMosaic( in  float4 inPosition		: POSITION0,
						in  float2 inTexCoord		: TEXCOORD0,
						
						out float4 outDiffuse		: SV_Target )
{
	uint width, height;
	g_Texture.GetDimensions(width, height);
	int pos_x = int(inTexCoord.x * width);
	int pos_y = int(inTexCoord.y * height);
	//pos_x -= pos_x % Mosaic.Size;
	//pos_y -= pos_y % Mosaic.Size;
	outDiffuse = g_Texture.Load(int3(pos_x, pos_y, 0));
	
	// 1行バージョン
	//outDiffuse = g_Texture.Sample(g_SamplerState, floor(inTexCoord * Mosaic.Size) / Mosaic.Size);
}

// 色収差(RGBずらし) ///////////////////////////////////////////////////
#define ABERRATION 5	// 収差
void PixelShaderCA( in  float4 inPosition		: POSITION0,
						in  float2 inTexCoord		: TEXCOORD0,
						
						out float4 outDiffuse		: SV_Target )
{
	uint width, height;
	g_Texture.GetDimensions(width, height);
	int pos_x = int(inTexCoord.x * width);
	int pos_y = int(inTexCoord.y * height);
	outDiffuse.r = g_Texture.Load(int3(pos_x + ABERRATION, pos_y + ABERRATION, 0)).r;
	outDiffuse.ga = g_Texture.Load(int3(pos_x, pos_y, 0)).ga;
	outDiffuse.b = g_Texture.Load(int3(pos_x - ABERRATION, pos_y - ABERRATION, 0)).b;
}

// 監視カメラ風 ///////////////////////////////////////////////////
void PixelShaderMonitoring( in  float4 inPosition		: POSITION0,
						in  float2 inTexCoord		: TEXCOORD0,
						
						out float4 outDiffuse		: SV_Target )
{
	outDiffuse = g_Texture.Sample(g_SamplerState, inTexCoord);
	outDiffuse -= abs(sin(inTexCoord.y * 100.0f + Constant.Time * 0.10f)) * 0.04f;
	outDiffuse -= abs(sin(inTexCoord.y * 300.0f + Constant.Time * 0.15f)) * 0.08f;
	outDiffuse.a = 1.0;
}

// ゲームボーイ風 ///////////////////////////////////////////////////
void PixelShaderOldGame( in  float4 inPosition		: POSITION0,
						in  float2 inTexCoord		: TEXCOORD0,
						
						out float4 outDiffuse		: SV_Target )
{
	float4 texColor = g_Texture.Sample(g_SamplerState, inTexCoord);
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
	outDiffuse = texColor;
}

// ノーマルマップ作成 ///////////////////////////////////////////////////
void VertexShaderNormalMap( in  float4 inPosition		: POSITION0,
						  in  float4 inNormal		: NORMAL0,
						  in  float4 inDiffuse		: COLOR0,
						  in  float2 inTexCoord		: TEXCOORD0,

						  out float4 outPosition	: SV_POSITION,
						  out float4 outNormal		: NORMAL0)
{
	outPosition = mul(inPosition, WVP);
	outNormal = normalize(mul(float4(inNormal.xyz, 0.0f), World));
}
void PixelShaderNormalMap( in  float4 inPosition	: POSITION0,
							in  float4 inNormal		: NORMAL0,
						
							out float4 outDiffuse	: SV_Target )
{
	outDiffuse = float4(normalize(inNormal.xyz), 1.0f);	// 法線ベクトルを色として扱う
	outDiffuse.z *= -1.0f;								// zを反転
	outDiffuse.xyz = (outDiffuse.xyz + 1.0f) * 0.5f;	// -1.0f ~ 1.0f を 0.0f ~ 1.0f にする
}

// デプスマップ作成 ///////////////////////////////////////////////////
void VertexShaderDepthMap( in  float4 inPosition	: POSITION0,
						  in  float4 inNormal		: NORMAL0,
						  in  float4 inDiffuse		: COLOR0,
						  in  float2 inTexCoord		: TEXCOORD0,

						  out float4 outPosition	: SV_POSITION)
{
	outPosition = mul(inPosition, WVP);
}
void PixelShaderDepthMap( in  float4 inPosition	: POSITION0,

							out float4 outDiffuse	: SV_Target )
{
	outDiffuse = 1.0f - (inPosition.z - Camera.ViewVolume.z) / (Camera.ViewVolume.w - Camera.ViewVolume.z);
	outDiffuse.a = 1.0f;
}