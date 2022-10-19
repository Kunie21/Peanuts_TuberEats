//************************************
//  �|�X�g�G�t�F�N�g�֘A�V�F�[�_�[
//  �N���X�@GP12 
//  �o�Ȕԍ��@11
//  ��ҁ@���]�đ�
//************************************
#include "shader.h"

// �t�B���^�[ ///////////////////////////////////////////////////
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

// �e�N�X�`�����Q�Ƃ��邾���i�ϊ��摜����ʂɏo�͂���Ƃ��Ɏg���j
void PixelShaderOnlyTex( in  float4 inPosition		: POSITION0,
						in  float2 inTexCoord		: TEXCOORD0,
						
						out float4 outDiffuse		: SV_Target )
{
	outDiffuse = g_Texture.Sample(g_SamplerState, inTexCoord);
}

// ���U�C�N ///////////////////////////////////////////////////
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
	
	// 1�s�o�[�W����
	//outDiffuse = g_Texture.Sample(g_SamplerState, floor(inTexCoord * Mosaic.Size) / Mosaic.Size);
}

// �F����(RGB���炵) ///////////////////////////////////////////////////
#define ABERRATION 5	// ����
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

// �Ď��J������ ///////////////////////////////////////////////////
void PixelShaderMonitoring( in  float4 inPosition		: POSITION0,
						in  float2 inTexCoord		: TEXCOORD0,
						
						out float4 outDiffuse		: SV_Target )
{
	outDiffuse = g_Texture.Sample(g_SamplerState, inTexCoord);
	outDiffuse -= abs(sin(inTexCoord.y * 100.0f + Constant.Time * 0.10f)) * 0.04f;
	outDiffuse -= abs(sin(inTexCoord.y * 300.0f + Constant.Time * 0.15f)) * 0.08f;
	outDiffuse.a = 1.0;
}

// �Q�[���{�[�C�� ///////////////////////////////////////////////////
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

// �m�[�}���}�b�v�쐬 ///////////////////////////////////////////////////
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
	outDiffuse = float4(normalize(inNormal.xyz), 1.0f);	// �@���x�N�g����F�Ƃ��Ĉ���
	outDiffuse.z *= -1.0f;								// z�𔽓]
	outDiffuse.xyz = (outDiffuse.xyz + 1.0f) * 0.5f;	// -1.0f ~ 1.0f �� 0.0f ~ 1.0f �ɂ���
}

// �f�v�X�}�b�v�쐬 ///////////////////////////////////////////////////
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