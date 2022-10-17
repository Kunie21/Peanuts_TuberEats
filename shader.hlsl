

//*****************************************************************************
// 定数バッファ
//*****************************************************************************

// 変換マトリクス
cbuffer MatrixBuffer : register(b0)
{
	matrix World;		// ワールド変換
	matrix View;		// ビュー変換
	matrix Projection;	// プロジェクション変換
	matrix WVP;			// WVP変換
	matrix VP;			// VP変換
}
// マトリクスバッファ
// cbuffer WorldBuffer : register( b0 )
// {
// 	matrix World;
// }

// cbuffer ViewBuffer : register( b1 )
// {
// 	matrix View;
// }

// cbuffer ProjectionBuffer : register( b2 )
// {
// 	matrix Projection;
// }

cbuffer LViewBuffer : register(b1)
{
	matrix LView;
}

cbuffer LProjectionBuffer : register(b2)
{
	matrix LProjection;
}

// マテリアルバッファ
struct MATERIAL
{
	float4		Ambient;
	float4		Diffuse;
	float4		Specular;
	float4		Emission;
	float		Shininess;
	int			noTexSampling;
	float		Dummy[2];//16byte境界用
};

cbuffer MaterialBuffer : register( b3 )
{
	MATERIAL	Material;
}

// ライト用バッファ
struct LIGHT
{
	float4		Direction[5];
	float4		Position[5];
	float4		Diffuse[5];
	float4		Ambient[5];
	float4		Attenuation[5];
	int4		Flags[5];
	int			Enable;
	int			Dummy[3];//16byte境界用
};

cbuffer LightBuffer : register( b4 )
{
	LIGHT		Light;
}

struct FOG
{
	float4		Distance;
	float4		FogColor;
	int			Enable;
	float		Dummy[3];//16byte境界用
};

// フォグ用バッファ
cbuffer FogBuffer : register( b5 )
{
	FOG			Fog;
};

// 縁取り用バッファ
cbuffer Fuchi : register(b6)
{
	int			fuchi;
	int			fill[3];
};


cbuffer CameraBuffer : register(b7)
{
	float4 Camera;
}

struct TIME
{
	int Time;
	int Dummy[3];
};

cbuffer Time : register(b8)
{
	TIME		TimeValue;
}

cbuffer FuchiColorBuffer : register(b9)
{
	float4 FuchiColor;
}

cbuffer BillboardBuffer : register(b10)
{
	matrix Billboard;
}
struct INSTANCE
{
	float4 pos[1024];
	float4 rot[1024];
	float4 scl[1024];
	float4 col[1024];
};
cbuffer InstanceBuffer : register(b11)
{
	INSTANCE Instance;
};

//=============================================================================
// 頂点シェーダ
//=============================================================================
void VertexShaderPolygon( in  float4 inPosition		: POSITION0,
						  in  float4 inNormal		: NORMAL0,
						  in  float4 inDiffuse		: COLOR0,
						  in  float2 inTexCoord		: TEXCOORD0,

						  out float4 outPosition	: SV_POSITION,
						  out float4 outNormal		: NORMAL0,
						  out float2 outTexCoord	: TEXCOORD0,
						  out float4 outDiffuse		: COLOR0,
						  out float4 outZTex		: TEXCOORD1,
						  out float4 outWorldPos    : POSITION0)
{
	// カメラ目線での投影
	outPosition = mul(inPosition, WVP);

	// ライト目線での投影
	matrix wvp;
	wvp = mul(World, LView);
	wvp = mul(wvp, LProjection);
	outZTex = mul(inPosition, wvp);


	outNormal = normalize(mul(float4(inNormal.xyz, 0.0f), World));

	outTexCoord = inTexCoord;

	outWorldPos = mul(inPosition, World);

	outDiffuse = inDiffuse;
}



//*****************************************************************************
// グローバル変数
//*****************************************************************************
Texture2D		g_Texture : register( t0 );
Texture2D		g_TextureShadowMap : register(t1);
Texture2D		g_TextureToonShadow : register(t2);
Texture2D		g_TextureStageSub : register(t3);
SamplerState	g_SamplerState : register( s0 );



//=============================================================================
// ピクセルシェーダ
//=============================================================================
void PixelShaderPolygon( in  float4 inPosition		: SV_POSITION,
						 in  float4 inNormal		: NORMAL0,
						 in  float2 inTexCoord		: TEXCOORD0,
						 in  float4 inDiffuse		: COLOR0,
						 in  float4 inZTex			: TEXCOORD1,
						 in  float4 inWorldPos      : POSITION0,

						 out float4 outDiffuse		: SV_Target )
{
	float4 color;

	// シャドウ判定処理（シャドウマップ法）
	// ライト目線によるZ値の再算出
	float ZValue = inZTex.z / inZTex.w;

	// 射影空間のXY座標をテクスチャ座標に変換
	float2 TTexCoord;
	TTexCoord.x = (1.0f + inZTex.x / inZTex.w)*0.5f;
	TTexCoord.y = (1.0f - inZTex.y / inZTex.w)*0.5f;

	// シャドウマップからZ値を導く
	float ZV_SM = (g_TextureShadowMap.Sample(g_SamplerState, TTexCoord)).x;

	float ZV_LU = (g_TextureShadowMap.Sample(g_SamplerState, TTexCoord + float2(-0.002f, 0.002f) )).x;
	float ZV_RU = (g_TextureShadowMap.Sample(g_SamplerState, TTexCoord + float2(0.002f, 0.002f))).x;
	float ZV_LL = (g_TextureShadowMap.Sample(g_SamplerState, TTexCoord + float2(-0.002f, -0.002f))).x;
	float ZV_RL = (g_TextureShadowMap.Sample(g_SamplerState, TTexCoord + float2(0.002f, -0.002f))).x;

	if ((TTexCoord.x < 0.0f) || (TTexCoord.x > 1.0f) || (TTexCoord.y > 1.0f) || (TTexCoord.y < 0.0f))
	{
		ZV_SM = 0.0f;
		ZV_LU = 0.0f;
		ZV_RU = 0.0f;
		ZV_LL = 0.0f;
		ZV_RL = 0.0f;
	}


	// テクスチャの有無でcolorの初期化を分岐
	if (Material.noTexSampling == 0)		// テクスチャがある場合
	{
		color = g_Texture.Sample(g_SamplerState, inTexCoord);

		color *= inDiffuse;
	}
	else									// 無い場合
	{
		color = inDiffuse;
	}

	// ライトが有効かどうか
	if (Light.Enable == 0)
	{
		color = color * Material.Diffuse;
	}
	else
	{
		float4 tempColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
		float4 outColor = float4(0.0f, 0.0f, 0.0f, 0.0f);

		for (int i = 0; i < 5; i++)
		{
			float3 lightDir;
			float light;

			if (Light.Flags[i].y == 1)
			{
				if (Light.Flags[i].x == 1)
				{
					lightDir = normalize(Light.Direction[i].xyz);
					light = dot(lightDir, inNormal.xyz);

					light = 0.5 - 0.5 * light;
					tempColor = color * Material.Diffuse * light * Light.Diffuse[i];
				}
				else if (Light.Flags[i].x == 2)
				{
					lightDir = normalize(Light.Position[i].xyz - inWorldPos.xyz);
					light = dot(lightDir, inNormal.xyz);

					tempColor = color * Material.Diffuse * light * Light.Diffuse[i];

					float distance = length(inWorldPos - Light.Position[i]);

					float att = saturate((Light.Attenuation[i].x - distance) / Light.Attenuation[i].x);
					tempColor *= att;
				}
				else
				{
					tempColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
				}

				outColor += tempColor;
			}
		}

		color = outColor;
		color.a = inDiffuse.a * Material.Diffuse.a;
	}

	// シャドウマップ処理
	bool shadow[5];
	for (int i = 0; i < 5; i++) { shadow[i] = false; }
	float3 colorSum = float3(0.0f, 0.0f, 0.0f);
	float3 shadowColor = float3(0.35f, 0.7f, 0.9f);

	// 算出点がシャドウマップのZ値よりも大きければ影と判断
	if (ZValue > ZV_SM + 0.00035f)	shadow[0] = true;
	if (ZValue > ZV_LU + 0.00035f)	shadow[1] = true;
	if (ZValue > ZV_RU + 0.00035f)	shadow[2] = true;
	if (ZValue > ZV_LL + 0.00035f)	shadow[3] = true;
	if (ZValue > ZV_RL + 0.00035f)	shadow[4] = true;

	// 左上　右上　左下　右下　中央　の影色を合計
	for (int i = 0; i < 5; i++)
	{
		if (shadow[i])
		{
			colorSum += shadowColor;
		}
		else
		{
			colorSum += float3(1.0f, 1.0f, 1.0f);
		}
	}

	// 影色の平均値を取る
	colorSum /= 5.0f;
	color.r = color.r * colorSum.r;
	color.g = color.g * colorSum.g;
	color.b = color.b * colorSum.b;

	//color.rgb = normalize(inNormal);	// 法線ビュー


	//フォグ
	if (Fog.Enable == 1)
	{
		float z = inPosition.z*inPosition.w;
		float f = (Fog.Distance.y - z) / (Fog.Distance.y - Fog.Distance.x);
		f = saturate(f);
		outDiffuse = f * color + (1 - f)*Fog.FogColor;
		outDiffuse.a = color.a;
	}
	else
	{
		outDiffuse = color;
	}

	//縁取り
	if (fuchi == 1)
	{
		float angle = dot(normalize(inWorldPos.xyz - Camera.xyz), normalize(inNormal));
		//if ((angle < 0.5f)&&(angle > -0.5f))
		if (angle > -0.3f)
		{
			outDiffuse.rb  = 1.0f;
			outDiffuse.g = 0.0f;			
		}
	}
}


//=============================================================================
// インスタンシング描画＋ビルボード用
//=============================================================================
void VertexShaderInstancingBillboard(in  float4 inPosition		: POSITION0,
	in  float4 inNormal : NORMAL0,
	in  float4 inDiffuse : COLOR0,
	in  float2 inTexCoord : TEXCOORD0,

	out float4 outPosition : SV_POSITION,
	out float4 outNormal : NORMAL0,
	out float2 outTexCoord : TEXCOORD0,
	out float4 outDiffuse : COLOR0,
	out float4 outWorldPos : POSITION0,
	uint instID : SV_InstanceID)
{
	float4 ps = Instance.pos[instID];
	float4 sc = Instance.scl[instID];
	//ps = float4( -100.0f, 143.0f, 68.5f, 0.0f );
	//sc = float4(1.0f, 1.0f, 1.0f, 0.0f );
	matrix mtxScaleTranslation = {
		sc.x, 0.0f, 0.0f, 0.0f,
		0.0f, sc.y, 0.0f, 0.0f,
		0.0f, 0.0f, sc.z, 0.0f,
		ps.x, ps.y, ps.z, 1.0f,
	};
	matrix mtxWorld = mul(Billboard, mtxScaleTranslation);
	matrix mtxWVP = mul(mtxWorld, VP);

	outPosition = mul(inPosition, mtxWVP);
	outTexCoord = inTexCoord;
	outDiffuse = Instance.col[instID];
}
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


//=============================================================================
// トゥーンピクセルシェーダ
//=============================================================================
void PixelShaderPolygonToon(in  float4 inPosition		: SV_POSITION,
	in  float4 inNormal : NORMAL0,
	in  float2 inTexCoord : TEXCOORD0,
	in  float4 inDiffuse : COLOR0,
	in  float4 inZTex : TEXCOORD1,
	in  float4 inWorldPos : POSITION0,

	out float4 outDiffuse : SV_Target)
{
	float4 color;

	// シャドウ判定処理（シャドウマップ法）
	// ライト目線によるZ値の再算出
	float ZValue = inZTex.z / inZTex.w;

	// 射影空間のXY座標をテクスチャ座標に変換
	float2 TTexCoord;
	TTexCoord.x = (1.0f + inZTex.x / inZTex.w)*0.5f;
	TTexCoord.y = (1.0f - inZTex.y / inZTex.w)*0.5f;

	// シャドウマップからZ値を導く
	float ZV_SM = (g_TextureShadowMap.Sample(g_SamplerState, TTexCoord)).x;

	float ZV_LU = (g_TextureShadowMap.Sample(g_SamplerState, TTexCoord + float2(-0.0002f, 0.0002f))).x;
	float ZV_RU = (g_TextureShadowMap.Sample(g_SamplerState, TTexCoord + float2(0.0002f, 0.0002f))).x;
	float ZV_LL = (g_TextureShadowMap.Sample(g_SamplerState, TTexCoord + float2(-0.0002f, -0.0002f))).x;
	float ZV_RL = (g_TextureShadowMap.Sample(g_SamplerState, TTexCoord + float2(0.0002f, -0.0002f))).x;

	if ((TTexCoord.x < 0.0f) || (TTexCoord.x > 1.0f) || (TTexCoord.y > 1.0f) || (TTexCoord.y < 0.0f))
	{
		ZV_SM = 0.0f;
		ZV_LU = 0.0f;
		ZV_RU = 0.0f;
		ZV_LL = 0.0f;
		ZV_RL = 0.0f;
	}


	// テクスチャの有無でcolorの初期化を分岐
	if (Material.noTexSampling == 0)		// テクスチャがある場合
	{
		color = g_Texture.Sample(g_SamplerState, inTexCoord);

		color *= inDiffuse;
	}
	else									// 無い場合
	{
		color = inDiffuse;
	}

	// ライトが有効かどうか
	if (Light.Enable == 0)
	{
		color = color * Material.Diffuse;
	}
	else
	{
		float4 tempColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
		float4 outColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
		float light1 = 1.0f;
		float light2 = 1.0f;


		for (int i = 0; i < 5; i++)
		{
			float3 lightDir;

			if (Light.Flags[i].y == 1)
			{
				if (Light.Flags[i].x == 1)
				{
					lightDir = normalize(Light.Direction[i].xyz);
					light1 = dot(lightDir, inNormal.xyz);

					light1 = 0.5 - 0.5 * light1;
				}
				else if (Light.Flags[i].x == 2)
				{
					lightDir = normalize(Light.Position[i].xyz - inWorldPos.xyz);
					light2 = dot(lightDir, inNormal.xyz);

					tempColor = color * Material.Diffuse * light2 * Light.Diffuse[i];

					float distance = length(inWorldPos - Light.Position[i]);

					float att = saturate((Light.Attenuation[i].x - distance) / Light.Attenuation[i].x);
					light2 *= att;

				}
				else
				{
					tempColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
				}
			}	

			outColor += tempColor;
		}

		float lightAverage = saturate(light1 + light2 * 0.5f);
		float shadowColor = g_TextureToonShadow.Sample(g_SamplerState, float2(lightAverage, 0.1f));

		shadowColor *= 1.5f;

		color.rgb *= shadowColor;
		color.a = inDiffuse.a * Material.Diffuse.a;
	}

	// シャドウマップ処理
	bool shadow[5];
	for (int i = 0; i < 5; i++) { shadow[i] = false; }
	float3 colorSum = float3(0.0f, 0.0f, 0.0f);
	float3 shadowColor = float3(0.65f, 0.9f, 1.0f);

	// 算出点がシャドウマップのZ値よりも大きければ影と判断
	if (ZValue > ZV_SM + 0.00035f)	shadow[0] = true;
	if (ZValue > ZV_LU + 0.00035f)	shadow[1] = true;
	if (ZValue > ZV_RU + 0.00035f)	shadow[2] = true;
	if (ZValue > ZV_LL + 0.00035f)	shadow[3] = true;
	if (ZValue > ZV_RL + 0.00035f)	shadow[4] = true;

	// 左上　右上　左下　右下　中央　の影色を合計
	for (int i = 0; i < 5; i++)
	{
		if (shadow[i])
		{
			colorSum += shadowColor;
		}
		else
		{
			colorSum += float3(1.0f, 1.0f, 1.0f);
		}
	}

	// 影色の平均値を取る
	colorSum /= 5.0f;
	color.r = color.r * colorSum.r;
	color.g = color.g * colorSum.g;
	color.b = color.b * colorSum.b;

	//color.rgb = normalize(inNormal);	// 法線ビュー


	//フォグ
	if (Fog.Enable == 1)
	{
		float z = inPosition.z*inPosition.w;
		float f = (Fog.Distance.y - z) / (Fog.Distance.y - Fog.Distance.x);
		f = saturate(f);
		outDiffuse = f * color + (1 - f)*Fog.FogColor;
		outDiffuse.a = color.a;
	}
	else
	{
		outDiffuse = color;
	}


	float angle = dot(normalize(inWorldPos.xyz - Camera.xyz), normalize(inNormal));
	//if ((angle < 0.5f)&&(angle > -0.5f))
	if (angle > -0.2f)
	{
		outDiffuse.rb = 0.6f;
		outDiffuse.g = 0.9f;
	}
}





//=============================================================================
// トゥーンピクセルシェーダ
//=============================================================================
void PixelShaderPolygonHoldToon(in  float4 inPosition		: SV_POSITION,
	in  float4 inNormal : NORMAL0,
	in  float2 inTexCoord : TEXCOORD0,
	in  float4 inDiffuse : COLOR0,
	in  float4 inZTex : TEXCOORD1,
	in  float4 inWorldPos : POSITION0,

	out float4 outDiffuse : SV_Target)
{
	float4 color;

	// シャドウ判定処理（シャドウマップ法）
	// ライト目線によるZ値の再算出
	float ZValue = inZTex.z / inZTex.w;

	// 射影空間のXY座標をテクスチャ座標に変換
	float2 TTexCoord;
	TTexCoord.x = (1.0f + inZTex.x / inZTex.w)*0.5f;
	TTexCoord.y = (1.0f - inZTex.y / inZTex.w)*0.5f;

	// シャドウマップからZ値を導く
	float ZV_SM = (g_TextureShadowMap.Sample(g_SamplerState, TTexCoord)).x;

	float ZV_LU = (g_TextureShadowMap.Sample(g_SamplerState, TTexCoord + float2(-0.0002f, 0.0002f))).x;
	float ZV_RU = (g_TextureShadowMap.Sample(g_SamplerState, TTexCoord + float2(0.0002f, 0.0002f))).x;
	float ZV_LL = (g_TextureShadowMap.Sample(g_SamplerState, TTexCoord + float2(-0.0002f, -0.0002f))).x;
	float ZV_RL = (g_TextureShadowMap.Sample(g_SamplerState, TTexCoord + float2(0.0002f, -0.0002f))).x;

	if ((TTexCoord.x < 0.0f) || (TTexCoord.x > 1.0f) || (TTexCoord.y > 1.0f) || (TTexCoord.y < 0.0f))
	{
		ZV_SM = 0.0f;
		ZV_LU = 0.0f;
		ZV_RU = 0.0f;
		ZV_LL = 0.0f;
		ZV_RL = 0.0f;
	}


	// テクスチャの有無でcolorの初期化を分岐
	if (Material.noTexSampling == 0)		// テクスチャがある場合
	{
		color = g_Texture.Sample(g_SamplerState, inTexCoord);

		color *= inDiffuse;
	}
	else									// 無い場合
	{
		color = inDiffuse;
	}

	// ライトが有効かどうか
	if (Light.Enable == 0)
	{
		color = color * Material.Diffuse;
	}
	else
	{
		float4 tempColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
		float4 outColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
		float light1 = 1.0f;
		float light2 = 1.0f;


		for (int i = 0; i < 5; i++)
		{
			float3 lightDir;

			if (Light.Flags[i].y == 1)
			{
				if (Light.Flags[i].x == 1)
				{
					lightDir = normalize(Light.Direction[i].xyz);
					light1 = dot(lightDir, inNormal.xyz);

					light1 = 0.5 - 0.5 * light1;
				}
				else if (Light.Flags[i].x == 2)
				{
					lightDir = normalize(Light.Position[i].xyz - inWorldPos.xyz);
					light2 = dot(lightDir, inNormal.xyz);

					tempColor = color * Material.Diffuse * light2 * Light.Diffuse[i];

					float distance = length(inWorldPos - Light.Position[i]);

					float att = saturate((Light.Attenuation[i].x - distance) / Light.Attenuation[i].x);
					light2 *= att;

				}
				else
				{
					tempColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
				}
			}

			outColor += tempColor;
		}

		float lightAverage = saturate(light1 + light2 * 0.5f);
		float shadowColor = g_TextureToonShadow.Sample(g_SamplerState, float2(lightAverage, 0.1f));

		shadowColor *= 1.5f;

		color.rgb *= shadowColor;
		color.a = inDiffuse.a * Material.Diffuse.a;
	}

	// シャドウマップ処理
	bool shadow[5];
	for (int i = 0; i < 5; i++) { shadow[i] = false; }
	float3 colorSum = float3(0.0f, 0.0f, 0.0f);
	float3 shadowColor = float3(0.65f, 0.9f, 1.0f);

	// 算出点がシャドウマップのZ値よりも大きければ影と判断
	if (ZValue > ZV_SM + 0.00035f)	shadow[0] = true;
	if (ZValue > ZV_LU + 0.00035f)	shadow[1] = true;
	if (ZValue > ZV_RU + 0.00035f)	shadow[2] = true;
	if (ZValue > ZV_LL + 0.00035f)	shadow[3] = true;
	if (ZValue > ZV_RL + 0.00035f)	shadow[4] = true;

	// 左上　右上　左下　右下　中央　の影色を合計
	for (int i = 0; i < 5; i++)
	{
		if (shadow[i])
		{
			colorSum += shadowColor;
		}
		else
		{
			colorSum += float3(1.0f, 1.0f, 1.0f);
		}
	}

	// 影色の平均値を取る
	colorSum /= 5.0f;
	color.r = color.r * colorSum.r;
	color.g = color.g * colorSum.g;
	color.b = color.b * colorSum.b;

	//color.rgb = normalize(inNormal);	// 法線ビュー

	float angle = dot(normalize(inWorldPos.xyz - Camera.xyz), normalize(inNormal));
	if (angle > -0.25f)
	{
		color.rgb = FuchiColor.rgb;
	}

	//フォグ
	if (Fog.Enable == 1)
	{
		float z = inPosition.z*inPosition.w;
		float f = (Fog.Distance.y - z) / (Fog.Distance.y - Fog.Distance.x);
		f = saturate(f);
		outDiffuse = f * color + (1 - f)*Fog.FogColor;
		outDiffuse.a = color.a;
	}
	else
	{
		outDiffuse = color;
	}

}



//=============================================================================
// シャドウマップ無しピクセルシェーダ
//=============================================================================
void PixelShaderPolygonNonShadow(in  float4 inPosition		: SV_POSITION,
	in  float4 inNormal : NORMAL0,
	in  float2 inTexCoord : TEXCOORD0,
	in  float4 inDiffuse : COLOR0,
	in  float4 inZTex : TEXCOORD1,
	in  float4 inWorldPos : POSITION0,

	out float4 outDiffuse : SV_Target)
{
	float4 color;

	// テクスチャの有無でcolorの初期化を分岐
	if (Material.noTexSampling == 0)		// テクスチャがある場合
	{
		color = g_Texture.Sample(g_SamplerState, inTexCoord);

		color *= inDiffuse;
	}
	else									// 無い場合
	{
		color = inDiffuse;
	}

	color = color * Material.Diffuse;


	//フォグ
	if (Fog.Enable == 1)
	{
		float z = inPosition.z*inPosition.w;
		float f = (Fog.Distance.y - z) / (Fog.Distance.y - Fog.Distance.x);
		f = saturate(f);
		outDiffuse = f * color + (1 - f)*Fog.FogColor;
		outDiffuse.a = color.a;
	}
	else
	{
		outDiffuse = color;
	}

	////縁取り
	//if (fuchi == 1)
	//{
	//	float angle = dot(normalize(inWorldPos.xyz - Camera.xyz), normalize(inNormal));
	//	//if ((angle < 0.5f)&&(angle > -0.5f))
	//	if (angle > -0.3f)
	//	{
	//		outDiffuse.rb = 1.0f;
	//		outDiffuse.g = 0.0f;
	//	}
	//}
}




//=============================================================================
//シャドウマップ用頂点シェーダ
//=============================================================================
void VertexShaderShadowMap( in  float4 inPosition	: POSITION0,
							in  float4 inDiffuse : COLOR0,

							out float4 outPosition	: SV_POSITION,
							out float4 outDiffuse	: COLOR0,
							out float4 outTexCoord	: TEXCOORD)

{
	matrix wvp;
	wvp = mul(World, LView);
	wvp = mul(wvp, LProjection);

	outPosition = mul(inPosition, wvp);
	outDiffuse = inDiffuse;
	outTexCoord = mul(inPosition, wvp);
}

//=============================================================================
//シャドウマップ用ピクセルシェーダ
//=============================================================================
void PixelShaderShadowMap(	in  float4 inPosition : SV_POSITION,
							in	float4 inDiffuse : COLOR0,
							in  float4 inTexCoord : TEXCOORD,

							out float4 outDiffuse : SV_Target)
{
	float alpha = 1.0f;


	//0-1の深度を黒-白で描画
	float4 color;

	color = inTexCoord.z / inTexCoord.w;
	color.a = 1.0f;	// 不透明にする


	if (Material.noTexSampling == 0)		// テクスチャがある場合
	{
		float4 colorBuf = g_Texture.Sample(g_SamplerState, inTexCoord);
		colorBuf *= inDiffuse;

		if (colorBuf.a < 0.5f)
		{
			alpha = 0.0f;
		}
	}

	color.a *= alpha;


	outDiffuse = color;
}




float random(in float2 st)
{
	// 疑似乱数
	return frac(sin(dot(st.xy, float2(12.9898, 43.233))) * 120.5453123);
}

float noise(in float2 st)
{
	float2 i = floor(st);
	float2 f = frac(st);

	// Four corners in 2D of a tile
	float a = random(i);
	float b = random(i + float2(1.0f, 0.0f));
	float c = random(i + float2(0.0f, 1.0f));
	float d = random(i + float2(1.0f, 1.0f));

	float2 u = f * f * (3.0f - 2.0f * f);

	return lerp(a, b, u.x) +
		(c - a)* u.y * (1.0f - u.x) +
		(d - b) * u.x * u.y;
}

#define OCTAVES 4
float fbm(in float2 st)
{
	// Initial values
	float value = 0.0f;
	float amplitude = 0.5f;
	//float frequency = 0.;
	//
	// Loop of octaves
	for (int i = 0; i < OCTAVES; i++) {
		value += amplitude * noise(st);
		st *= 2.0f;
		amplitude *= 0.5f;
	}

	return value;
}

//=============================================================================
// 滝用ピクセルシェーダ
//=============================================================================
void PixelShaderWaterfall(in  float4 inPosition		: SV_POSITION,
	in  float4 inNormal : NORMAL0,
	in  float2 inTexCoord : TEXCOORD0,
	in  float4 inDiffuse : COLOR0,
	in  float4 inZTex : TEXCOORD1,
	in  float4 inWorldPos : POSITION0,

	out float4 outDiffuse : SV_Target)
{
	float4 color;

	// シャドウ判定処理（シャドウマップ法-----------------------------------------------------------------
	// ライト目線によるZ値の再算出
	float ZValue = inZTex.z / inZTex.w;

	// 射影空間のXY座標をテクスチャ座標に変換
	float2 TTexCoord;
	TTexCoord.x = (1.0f + inZTex.x / inZTex.w)*0.5f;
	TTexCoord.y = (1.0f - inZTex.y / inZTex.w)*0.5f;

	// シャドウマップからZ値を導く
	float ZV_SM = (g_TextureShadowMap.Sample(g_SamplerState, TTexCoord)).x;

	float ZV_LU = (g_TextureShadowMap.Sample(g_SamplerState, TTexCoord + float2(-0.002f, 0.002f))).x;
	float ZV_RU = (g_TextureShadowMap.Sample(g_SamplerState, TTexCoord + float2(0.002f, 0.002f))).x;
	float ZV_LL = (g_TextureShadowMap.Sample(g_SamplerState, TTexCoord + float2(-0.002f, -0.002f))).x;
	float ZV_RL = (g_TextureShadowMap.Sample(g_SamplerState, TTexCoord + float2(0.002f, -0.002f))).x;

	if ((TTexCoord.x < 0.0f) || (TTexCoord.x > 1.0f) || (TTexCoord.y > 1.0f) || (TTexCoord.y < 0.0f) || 
		(0.0f < dot(normalize(inWorldPos.xyz - Camera.xyz), normalize(inNormal))) )
	{
		ZV_SM = 0.0f;
		ZV_LU = 0.0f;
		ZV_RU = 0.0f;
		ZV_LL = 0.0f;
		ZV_RL = 0.0f;
	}
	// ----------------------------------------------------------------------------------------------------



	// デフューズのセット
	color = inDiffuse;

	// FBMによるカラーの抽出-----------------------------
	float3 mixColor1 = float3(0.55f, 0.95f, 0.72f);
	float3 mixColor2 = float3(0.33f, 0.75f, 0.89f);

	float3 fbmColor = 0.0f;

	float2 st = inTexCoord.xy;		// fbmにはテクスチャ座標を利用する
	st.x *= 10.0f;					// 横方向を圧縮　縦方向の流線に見立てる
	st.y -= TimeValue.Time * 0.05f;	// 時間経過で縦方向にさせるアニメーション

	float2 q = 0.0f;
	q.x = fbm(st + float2(0.0f, 0.0f));
	q.y = fbm(st + float2(1.0f, 1.0f));

	float2 r = 0.0f;
	r.x = fbm(st + (4.0f * q) + float2(1.7f, 9.2f) + (0.015f * TimeValue.Time));
	r.y = fbm(st + (4.0f * q) + float2(8.3f, 2.8f) + (0.062f * TimeValue.Time));

	fbmColor = lerp(fbmColor, mixColor1, clamp(length(q), 0.5, 1.0));
	fbmColor = lerp(fbmColor, mixColor2, clamp(length(r), 0.5, 1.0));

	float f = fbm(st + 4.0f * r);
	float coef = (f * f * f + (0.6 * f * f) + (0.5 * f));
	fbmColor *= coef;

	color = float4(fbmColor, 1.0f);

	// ----------------------------------------------------


	// ライティング ---------------------------------------
	if (Light.Enable == 0)
	{
		color = color * Material.Diffuse;
	}
	else
	{
		float4 tempColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
		float4 outColor = float4(0.0f, 0.0f, 0.0f, 0.0f);

		for (int i = 0; i < 5; i++)
		{
			float3 lightDir;
			float light;

			if (Light.Flags[i].y == 1)
			{
				if (Light.Flags[i].x == 1)
				{
					lightDir = normalize(Light.Direction[i].xyz);
					light = dot(lightDir, inNormal.xyz);

					light = 0.5 - 0.5 * light;

					//if (light < 0.3f)	// 確認用
					//{
					//	light = 1.0f;
					//}
					//else if (light > 0.7f)
					//{
					//	light = 0.0f;
					//}

					tempColor = color * Material.Diffuse * light * Light.Diffuse[i];
				}
				else
				{
					tempColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
				}

				outColor += tempColor;
			}
		}

		color = outColor;
		color.a = inDiffuse.a * Material.Diffuse.a;
	}
	// ----------------------------------------------------



	// シャドウマップ処理 -------------------------------
	bool shadow[5];
	for (int i = 0; i < 5; i++) { shadow[i] = false; }
	float3 colorSum = float3(0.0f, 0.0f, 0.0f);
	float3 shadowColor = float3(0.35f, 0.7f, 0.9f);

	// 算出点がシャドウマップのZ値よりも大きければ影と判断
	if (ZValue > ZV_SM + 0.00035f)	shadow[0] = true;
	if (ZValue > ZV_LU + 0.00035f)	shadow[1] = true;
	if (ZValue > ZV_RU + 0.00035f)	shadow[2] = true;
	if (ZValue > ZV_LL + 0.00035f)	shadow[3] = true;
	if (ZValue > ZV_RL + 0.00035f)	shadow[4] = true;

	// 左上　右上　左下　右下　中央　の影色を合計
	for (int i = 0; i < 5; i++)
	{
		if (shadow[i])
		{
			colorSum += shadowColor;
		}
		else
		{
			colorSum += float3(1.0f, 1.0f, 1.0f);
		}
	}

	// 影色の平均値を取る
	colorSum /= 5.0f;
	color.r = color.r * colorSum.r;
	color.g = color.g * colorSum.g;
	color.b = color.b * colorSum.b;
	// -----------------------------------------------------


	//((fbmColor.r + fbmColor.g + fbmColor.b) / 3.0f) * float3(0.35f, 0.75f, 0.92f)

	// 上の方　色を変える---------------------------------
	float angle = dot(normalize(inWorldPos.xyz - Camera.xyz), normalize(inNormal));
	angle = clamp(angle, -1.0f, 0.0f);
	if (angle > -0.25f)
	{
		angle = (1.0 + angle - 0.75f) * 4.0f;
		float3 reflectColor = (color.rgb * (1.0f - angle)) + (angle * float3(0.35f, 0.75f, 0.92f));
		color.rgb = reflectColor;
	}
	//------------------------------------------------------
	//color.rgb = normalize(inNormal);	// 法線ビュー
	//color.rgb = dot(normalize(inWorldPos.xyz - Camera.xyz), normalize(inNormal));	//

	//float3 lightDir = normalize(Light.Direction[0].xyz);
	//float light = dot(lightDir, inNormal.xyz);
	//light = 0.5 - 0.5 * light;
	//color.rgb = light;




	//フォグ ----------------------------------------------
	if (Fog.Enable == 1)
	{
		float z = inPosition.z*inPosition.w;
		float f = (Fog.Distance.y - z) / (Fog.Distance.y - Fog.Distance.x);
		f = saturate(f);
		outDiffuse = f * color + (1 - f)*Fog.FogColor;
		outDiffuse.a = color.a;
	}
	else
	{
		outDiffuse = color;
	}
	// ------------------------------------------------------




	//縁取り
	if (fuchi == 1)
	{
		float angle = dot(normalize(inWorldPos.xyz - Camera.xyz), normalize(inNormal));
		//if ((angle < 0.5f)&&(angle > -0.5f))
		if (angle > -0.3f)
		{
			outDiffuse.rb = 1.0f;
			outDiffuse.g = 0.0f;
		}
	}
}


//=============================================================================
//　炎ステージ用ピクセルシェーダ
//=============================================================================
void PixelShaderFireStage(in  float4 inPosition		: SV_POSITION,
	in  float4 inNormal : NORMAL0,
	in  float2 inTexCoord : TEXCOORD0,
	in  float4 inDiffuse : COLOR0,
	in  float4 inZTex : TEXCOORD1,
	in  float4 inWorldPos : POSITION0,

	out float4 outDiffuse : SV_Target)
{
	float4 color;

	// シャドウ判定処理（シャドウマップ法-----------------------------------------------------------------
	// ライト目線によるZ値の再算出
	float ZValue = inZTex.z / inZTex.w;

	// 射影空間のXY座標をテクスチャ座標に変換
	float2 TTexCoord;
	TTexCoord.x = (1.0f + inZTex.x / inZTex.w)*0.5f;
	TTexCoord.y = (1.0f - inZTex.y / inZTex.w)*0.5f;

	// シャドウマップからZ値を導く
	float ZV_SM = (g_TextureShadowMap.Sample(g_SamplerState, TTexCoord)).x;

	float ZV_LU = (g_TextureShadowMap.Sample(g_SamplerState, TTexCoord + float2(-0.002f, 0.002f))).x;
	float ZV_RU = (g_TextureShadowMap.Sample(g_SamplerState, TTexCoord + float2(0.002f, 0.002f))).x;
	float ZV_LL = (g_TextureShadowMap.Sample(g_SamplerState, TTexCoord + float2(-0.002f, -0.002f))).x;
	float ZV_RL = (g_TextureShadowMap.Sample(g_SamplerState, TTexCoord + float2(0.002f, -0.002f))).x;

	if ((TTexCoord.x < 0.0f) || (TTexCoord.x > 1.0f) || (TTexCoord.y > 1.0f) || (TTexCoord.y < 0.0f) ||
		(0.0f < dot(normalize(inWorldPos.xyz - Camera.xyz), normalize(inNormal))))
	{
		ZV_SM = 0.0f;
		ZV_LU = 0.0f;
		ZV_RU = 0.0f;
		ZV_LL = 0.0f;
		ZV_RL = 0.0f;
	}
	// ----------------------------------------------------------------------------------------------------



	// デフューズのセット
	color = inDiffuse;

	// FBMによるカラーの抽出-----------------------------
	float3 mixColor1 = float3(0.125f, 0.145f, 0.032f);
	float3 mixColor2 = float3(0.93f, 0.35f, 0.19f);

	float3 fbmColor = 0.0f;

	float2 st = inTexCoord.xy;		// fbmにはテクスチャ座標を利用する
	//st.x *= 10.0f;					// 横方向を圧縮　縦方向の流線に見立てる
	st.y -= TimeValue.Time * 0.00075f;	// 時間経過で縦方向にさせるアニメーション

	float2 q = 0.0f;
	q.x = fbm(st + float2(0.0f, 0.0f));
	q.y = fbm(st + float2(1.0f, 1.0f));

	float2 r = 0.0f;
	r.x = fbm(st + (4.0f * q) + float2(1.7f, 9.2f) + (0.00015f * TimeValue.Time));
	r.y = fbm(st + (4.0f * q) + float2(8.3f, 2.8f) + (0.00062f * TimeValue.Time));

	fbmColor = lerp(fbmColor, mixColor1, clamp(length(q), 0.0, 1.0));
	fbmColor = lerp(fbmColor, mixColor2, clamp(length(r), 0.0, 1.0));

	float f = fbm(st + 4.0f * r);
	float coef = (f * f * f + (0.6 * f * f) + (0.5 * f));
	fbmColor *= coef;

	color = float4(fbmColor, 1.0f);
	
	
	float3 addColor = float3(0.0f, 0.0f, 0.0f);
	float brightness = color.r + color.g + color.b / 3.0f;
	if (brightness > 0.4f)
	{
		addColor = color.rgb;

		// 輝度0.6f ~ 0.7fの間を線形補間してグラデーションにする
		if (brightness < 0.7f)
		{
			addColor.rgb *= (brightness - 0.4f) / 0.3f;
		}
	}

	// ----------------------------------------------------


	// ライティング ---------------------------------------
	if (Light.Enable == 0)
	{
		color = color * Material.Diffuse;
	}
	else
	{
		float4 tempColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
		float4 outColor = float4(0.0f, 0.0f, 0.0f, 0.0f);

		for (int i = 0; i < 5; i++)
		{
			float3 lightDir;
			float light;

			if (Light.Flags[i].y == 1)
			{
				if (Light.Flags[i].x == 1)
				{
					lightDir = normalize(Light.Direction[i].xyz);
					light = dot(lightDir, inNormal.xyz);

					light = 0.5 - 0.5 * light;
					tempColor = color * Material.Diffuse * light * Light.Diffuse[i];
				}
				else
				{
					tempColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
				}

				outColor += tempColor;
			}
		}

		color = outColor;
		color.a = inDiffuse.a * Material.Diffuse.a;
	}
	// ----------------------------------------------------

	color.rgb += addColor;

	// シャドウマップ処理 -------------------------------
	bool shadow[5];
	for (int i = 0; i < 5; i++) { shadow[i] = false; }
	float3 colorSum = float3(0.0f, 0.0f, 0.0f);
	float3 shadowColor = float3(0.75f, 0.7f, 0.9f);

	// 算出点がシャドウマップのZ値よりも大きければ影と判断
	if (ZValue > ZV_SM + 0.00035f)	shadow[0] = true;
	if (ZValue > ZV_LU + 0.00035f)	shadow[1] = true;
	if (ZValue > ZV_RU + 0.00035f)	shadow[2] = true;
	if (ZValue > ZV_LL + 0.00035f)	shadow[3] = true;
	if (ZValue > ZV_RL + 0.00035f)	shadow[4] = true;

	// 左上　右上　左下　右下　中央　の影色を合計
	for (int i = 0; i < 5; i++)
	{
		if (shadow[i])
		{
			colorSum += shadowColor;
		}
		else
		{
			colorSum += float3(1.0f, 1.0f, 1.0f);
		}
	}

	// 影色の平均値を取る
	colorSum /= 5.0f;
	color.r = color.r * colorSum.r;
	color.g = color.g * colorSum.g;
	color.b = color.b * colorSum.b;
	// -----------------------------------------------------


	//((fbmColor.r + fbmColor.g + fbmColor.b) / 3.0f) * float3(0.35f, 0.75f, 0.92f)

	// 側面　中央　色を変える---------------------------------
	float angle = dot(normalize(inWorldPos.xyz - Camera.xyz), normalize(inNormal));
	angle = clamp(angle, -1.0f, 0.0f);
	if (angle > -0.2f)
	{
		float a1 = (1.0 + angle - 0.8f) * 5.0f;
		float3 glowcolor1 = (a1 * float3(1.00f, 0.55f, 0.22f));
		color.rgb += glowcolor1;
	}

	if (angle < -0.2f)
	{
		float a2 = ((-angle) - 0.2f) * 1.4f;
		float3 glowcolor2 = (a2 * float3(0.5f, 0.3f, 0.1f));
		color.rgb += glowcolor2;
	}
	//------------------------------------------------------

	color.rgb *= color.rgb;


	//フォグ ----------------------------------------------
	if (Fog.Enable == 1)
	{
		float z = inPosition.z*inPosition.w;
		float f = (Fog.Distance.y - z) / (Fog.Distance.y - Fog.Distance.x);
		f = saturate(f);
		outDiffuse = f * color + (1 - f)*Fog.FogColor;
		outDiffuse.a = color.a;
	}
	else
	{
		outDiffuse = color;
	}
	// ------------------------------------------------------




	//縁取り
	if (fuchi == 1)
	{
		float angle = dot(normalize(inWorldPos.xyz - Camera.xyz), normalize(inNormal));
		//if ((angle < 0.5f)&&(angle > -0.5f))
		if (angle > -0.3f)
		{
			outDiffuse.rb = 1.0f;
			outDiffuse.g = 0.0f;
		}
	}
}




//=============================================================================
//　闇ステージ用ピクセルシェーダ
//=============================================================================
void PixelShaderDarkStage(in  float4 inPosition		: SV_POSITION,
	in  float4 inNormal : NORMAL0,
	in  float2 inTexCoord : TEXCOORD0,
	in  float4 inDiffuse : COLOR0,
	in  float4 inZTex : TEXCOORD1,
	in  float4 inWorldPos : POSITION0,

	out float4 outDiffuse : SV_Target)
{
	float4 color;


	// デフューズのセット
	color = inDiffuse;

	// FBMによるカラーの抽出-----------------------------
	float3 mixColor1 = float3(0.05f, 0.12f, 0.032f);
	float3 mixColor2 = float3(0.73f, 0.15f, 0.79f);

	float3 fbmColor = 0.0f;

	float2 st = inTexCoord.xy;		// fbmにはテクスチャ座標を利用する
	st.x *= 1.5f;					// 横方向を圧縮　縦方向の流線に見立てる
	st.y *= 1.5f;					// 横方向を圧縮　縦方向の流線に見立てる
	st.y -= TimeValue.Time * 0.00075f;	// 時間経過で縦方向にさせるアニメーション

	float2 q = 0.0f;
	q.x = fbm(st + float2(0.0f, 0.0f));
	q.y = fbm(st + float2(1.0f, 1.0f));

	float2 r = 0.0f;
	r.x = fbm(st + (4.0f * q) + float2(1.7f, 9.2f) + (0.035f * TimeValue.Time));
	r.y = fbm(st + (4.0f * q) + float2(8.3f, 2.8f) + (0.062f * TimeValue.Time));

	fbmColor = lerp(fbmColor, mixColor1, clamp(length(q), 0.0, 1.0));
	fbmColor = lerp(fbmColor, mixColor2, clamp(length(r), 0.0, 1.0));

	float f = fbm(st + 4.0f * r);
	float coef = (f * f * f + (0.6 * f * f) + (0.5 * f));
	fbmColor *= coef;

	color = float4(fbmColor, 1.0f);


	if (color.r > 0.8f)
	{
		color.r = 1.0f;
		color.g *= 0.2f;
	}

	if (color.b > 0.75f)
	{
		color.b = 1.0f;
	}
	else
	{
		color.rgb *= 0.5f;
	}

	//if (color.r > 0.8f)
	//{
	//	color.r = 1.0f;
	//	color.gb *= 0.2f;
	//}

	// ----------------------------------------------------




	//((fbmColor.r + fbmColor.g + fbmColor.b) / 3.0f) * float3(0.35f, 0.75f, 0.92f)

	//// 側面　中央　色を変える---------------------------------
	//float angle = dot(normalize(inWorldPos.xyz - Camera.xyz), normalize(inNormal));
	//angle = clamp(angle, -1.0f, 0.0f);
	//if (angle > -0.2f)
	//{
	//	float a1 = (1.0 + angle - 0.8f) * 5.0f;
	//	float3 glowcolor1 = (a1 * float3(1.00f, 0.55f, 0.22f));
	//	color.rgb += glowcolor1;
	//}

	//if (angle < -0.2f)
	//{
	//	float a2 = ((-angle) - 0.2f) * 1.2f;
	//	float3 glowcolor2 = (a2 * float3(0.5f, 0.3f, 0.1f));
	//	color.rgb += glowcolor2;
	//}
	////------------------------------------------------------

	color.rgb *= color.rgb;


	//フォグ ----------------------------------------------
	if (Fog.Enable == 1)
	{
		float z = inPosition.z*inPosition.w;
		float f = (Fog.Distance.y - z) / (Fog.Distance.y - Fog.Distance.x);
		f = saturate(f);
		outDiffuse = f * color + (1 - f)*Fog.FogColor;
		outDiffuse.a = color.a;
	}
	else
	{
		outDiffuse = color;
	}
	// ------------------------------------------------------




	//縁取り
	float angle = dot(normalize(inWorldPos.xyz - Camera.xyz), normalize(inNormal));
	if (angle > -0.05f)
	{
		outDiffuse.rb = 1.0f;
	}
}




//=============================================================================
//　氷ステージ用ピクセルシェーダ
//=============================================================================
void PixelShaderIceStage(in  float4 inPosition		: SV_POSITION,
	in  float4 inNormal : NORMAL0,
	in  float2 inTexCoord : TEXCOORD0,
	in  float4 inDiffuse : COLOR0,
	in  float4 inZTex : TEXCOORD1,
	in  float4 inWorldPos : POSITION0,

	out float4 outDiffuse : SV_Target)
{
	float4 color;


	// デフューズのセット
	color = inDiffuse;

	// FBMによるカラーの抽出-----------------------------
	float3 mixColor1 = float3(0.5f, 0.42f, 0.92f);
	float3 mixColor2 = float3(0.6f, 0.75f, 1.0f);

	float3 fbmColor = 0.0f;

	float2 st = inTexCoord.xy;		// fbmにはテクスチャ座標を利用する
	st.x *= 5.0f;					// 横方向を圧縮　縦方向の流線に見立てる


	float2 q = 0.0f;
	q.x = fbm(st + float2(0.0f, 0.0f));
	q.y = fbm(st + float2(1.0f, 1.0f));

	float2 r = 0.0f;
	r.x = fbm(st + (4.0f * q) + float2(1.7f, 9.2f) + (0.00035f * TimeValue.Time));
	r.y = fbm(st + (4.0f * q) + float2(8.3f, 2.8f) + (0.00092f * TimeValue.Time));

	fbmColor = lerp(fbmColor, mixColor1, clamp(length(q), 0.0, 1.0));
	fbmColor = lerp(fbmColor, mixColor2, clamp(length(r), 0.0, 1.0));

	float f = fbm(st + 4.0f * r);

	float coef = (f + (0.6));


	fbmColor *= coef;

	color = float4(fbmColor, 1.0f);


	// ----------------------------------------------------



	// 側面　中央　色を変える---------------------------------
	float angle = dot(normalize(inWorldPos.xyz - Camera.xyz), normalize(inNormal));
	angle = clamp(angle, -1.0f, 0.0f);
	if (angle > -0.4f)
	{
		float a1 = (1.0 + angle - 0.6f) * 2.0f;
		float3 glowcolor1 = (a1 * float3(0.0f, 0.4f, 0.575f));
		color.rgb += glowcolor1;
	}

	if (angle < -0.2f)
	{
		float a2 = ((-angle) - 0.2f) * 1.2f;
		float3 glowcolor2 = (a2 * float3(0.0f, 0.25f, 0.3f));
		color.rgb += glowcolor2;
	}
	//------------------------------------------------------


	//フォグ ----------------------------------------------
	if (Fog.Enable == 1)
	{
		float z = inPosition.z*inPosition.w;
		float f = (Fog.Distance.y - z) / (Fog.Distance.y - Fog.Distance.x);
		f = saturate(f);
		outDiffuse = f * color + (1 - f)*Fog.FogColor;
		outDiffuse.a = color.a;
	}
	else
	{
		outDiffuse = color;
	}
	// ------------------------------------------------------




	//縁取り
	float angleFuchi = dot(normalize(inWorldPos.xyz - Camera.xyz), normalize(inNormal));
	if (angleFuchi > -0.05f)
	{
		outDiffuse.rgb = 1.0f;
	}
}



//=============================================================================
//　砂ステージ用ピクセルシェーダ
//=============================================================================
void PixelShaderSandStage(in  float4 inPosition		: SV_POSITION,
	in  float4 inNormal : NORMAL0,
	in  float2 inTexCoord : TEXCOORD0,
	in  float4 inDiffuse : COLOR0,
	in  float4 inZTex : TEXCOORD1,
	in  float4 inWorldPos : POSITION0,

	out float4 outDiffuse : SV_Target)
{
	float4 color;


	// シャドウ判定処理（シャドウマップ法-----------------------------------------------------------------
	// ライト目線によるZ値の再算出
	float ZValue = inZTex.z / inZTex.w;

	// 射影空間のXY座標をテクスチャ座標に変換
	float2 TTexCoord;
	TTexCoord.x = (1.0f + inZTex.x / inZTex.w)*0.5f;
	TTexCoord.y = (1.0f - inZTex.y / inZTex.w)*0.5f;

	// シャドウマップからZ値を導く
	float ZV_SM = (g_TextureShadowMap.Sample(g_SamplerState, TTexCoord)).x;

	float ZV_LU = (g_TextureShadowMap.Sample(g_SamplerState, TTexCoord + float2(-0.0002f, 0.0002f))).x;
	float ZV_RU = (g_TextureShadowMap.Sample(g_SamplerState, TTexCoord + float2(0.0002f, 0.0002f))).x;
	float ZV_LL = (g_TextureShadowMap.Sample(g_SamplerState, TTexCoord + float2(-0.0002f, -0.0002f))).x;
	float ZV_RL = (g_TextureShadowMap.Sample(g_SamplerState, TTexCoord + float2(0.0002f, -0.0002f))).x;

	if ((TTexCoord.x < 0.0f) || (TTexCoord.x > 1.0f) || (TTexCoord.y > 1.0f) || (TTexCoord.y < 0.0f) ||
		(0.0f < dot(normalize(inWorldPos.xyz - Camera.xyz), normalize(inNormal))))
	{
		ZV_SM = 0.0f;
		ZV_LU = 0.0f;
		ZV_RU = 0.0f;
		ZV_LL = 0.0f;
		ZV_RL = 0.0f;
	}
	// ----------------------------------------------------------------------------------------------------

	// デフューズのセット
	color = inDiffuse;

	// FBMによるカラーの抽出-----------------------------
	float3 mixColor1 = float3(0.95f, 0.85f, 0.75f);
	float3 mixColor2 = float3(0.7f, 0.55f, 0.3f);

	float3 fbmColor = 0.0f;

	float2 st = inTexCoord.xy;		// fbmにはテクスチャ座標を利用する
	st.x *= 0.75f;					// 横方向を圧縮
	st.y *= 0.75f;					// 縦方向を圧縮
	st.y -= TimeValue.Time * 0.01f;	// 時間経過で縦方向にさせるアニメーション


	float2 q = 0.0f;
	q.x = fbm(st + float2(0.0f, 0.0f));
	q.y = fbm(st + float2(1.0f, 1.0f));

	float2 r = 0.0f;
	r.x = fbm(st + (4.0f * q) + float2(1.7f, 9.2f) + (0.00035f * TimeValue.Time));
	r.y = fbm(st + (4.0f * q) + float2(8.3f, 2.8f) + (0.00092f * TimeValue.Time));

	fbmColor = lerp(fbmColor, mixColor1, clamp(length(q), 0.0, 1.0));
	fbmColor = lerp(fbmColor, mixColor2, clamp(length(r), 0.0, 1.0));

	float f = fbm(st + 4.0f * r);
	float coef = (f + (0.6));
	fbmColor *= coef;

	color = float4(fbmColor, 1.0f);


	// ----------------------------------------------------

	st.y -= TimeValue.Time * 0.01f;	// 時間経過で縦方向にさせるアニメーション
	float4 sandTex = g_TextureStageSub.Sample(g_SamplerState, st);

	color.rgb = color.rgb * (1.0f - sandTex.a) + sandTex.rgb * sandTex.a;

	// 側面　中央　色を変える---------------------------------
	float angle = dot(normalize(inWorldPos.xyz - Camera.xyz), normalize(inNormal));
	angle = clamp(angle, -1.0f, 0.0f);
	if (angle > -0.4f)
	{
		float a1 = (1.0 + angle - 0.6f) * 2.0f;
		float3 glowcolor1 = (a1 * float3(0.35f, 0.1f, 0.12f));
		color.rgb += glowcolor1;
	}

	//------------------------------------------------------


		// ライティング ---------------------------------------
	if (Light.Enable == 0)
	{
		color = color * Material.Diffuse;
	}
	else
	{
		float4 tempColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
		float4 outColor = float4(0.0f, 0.0f, 0.0f, 0.0f);

		for (int i = 0; i < 5; i++)
		{
			float3 lightDir;
			float light;

			if (Light.Flags[i].y == 1)
			{
				if (Light.Flags[i].x == 1)
				{
					lightDir = normalize(Light.Direction[i].xyz);
					light = dot(lightDir, inNormal.xyz);

					light = 0.5 - 0.5 * light;
					tempColor = color * Material.Diffuse * light * Light.Diffuse[i];
				}
				else
				{
					tempColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
				}

				outColor += tempColor;
			}
		}

		color = outColor;
		color.a = inDiffuse.a * Material.Diffuse.a;
	}
	// ----------------------------------------------------

		// シャドウマップ処理 -------------------------------
	bool shadow[5];
	for (int i = 0; i < 5; i++) { shadow[i] = false; }
	float3 colorSum = float3(0.0f, 0.0f, 0.0f);
	float3 shadowColor = float3(0.95f, 0.8f, 0.6f);

	// 算出点がシャドウマップのZ値よりも大きければ影と判断
	if (ZValue > ZV_SM + 0.00035f)	shadow[0] = true;
	if (ZValue > ZV_LU + 0.00035f)	shadow[1] = true;
	if (ZValue > ZV_RU + 0.00035f)	shadow[2] = true;
	if (ZValue > ZV_LL + 0.00035f)	shadow[3] = true;
	if (ZValue > ZV_RL + 0.00035f)	shadow[4] = true;

	// 左上　右上　左下　右下　中央　の影色を合計
	for (int i = 0; i < 5; i++)
	{
		if (shadow[i])
		{
			colorSum += shadowColor;
		}
		else
		{
			colorSum += float3(1.0f, 1.0f, 1.0f);
		}
	}

	// 影色の平均値を取る
	colorSum /= 5.0f;
	color.r = color.r * colorSum.r;
	color.g = color.g * colorSum.g;
	color.b = color.b * colorSum.b;
	// -----------------------------------------------------


	//フォグ ----------------------------------------------
	if (Fog.Enable == 1)
	{
		float z = inPosition.z*inPosition.w;
		float f = (Fog.Distance.y - z) / (Fog.Distance.y - Fog.Distance.x);
		f = saturate(f);
		outDiffuse = f * color + (1 - f)*Fog.FogColor;
		outDiffuse.a = color.a;
	}
	else
	{
		outDiffuse = color;
	}
	// ------------------------------------------------------




	//縁取り
	float angleFuchi = dot(normalize(inWorldPos.xyz - Camera.xyz), normalize(inNormal));
	if (angleFuchi > -0.05f)
	{
		outDiffuse.rgb = 1.0f;
	}
}
