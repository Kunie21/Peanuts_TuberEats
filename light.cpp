//=============================================================================
//
// ライト処理 [light.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "light.h"
#include "camera.h"
#include "game.h"


//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MAINLIGHT_POS_X		(-500.0f)
#define MAINLIGHT_POS_Y		(500.0f)
#define MAINLIGHT_POS_Z		(500.0f)
#define POINTLIGHT_POS_Y	(-20.0f)
#define	LIGHT_WALL_DIS		(30.0f)		// ライト位置の壁との距離

//*********************************************************
// 構造体
//*********************************************************
struct LIGHTVIEW						// メインのライトによるシャドウマップ作成用ビュー
{
	XMFLOAT4X4			mtxView;		// ビューマトリックス
	XMFLOAT4X4			mtxInvView;		// ビューマトリックス
	XMFLOAT4X4			mtxProjection;	// プロジェクションマトリックス

	XMFLOAT3			up;				// ライトの上方向ベクトル
	XMFLOAT3			at;				// ライトの注視点

	float				viewAngle;
	float				viewAspect;
	float				nearZ;
	float				farZ;
};


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static LIGHT		g_Light[LIGHT_MAX];
static LIGHTVIEW	g_LightView;

static FOG		g_Fog;


//=============================================================================
// 初期化処理
//=============================================================================
void InitLight(void)
{

	// メインライトビュー初期化
	ZeroMemory(&g_LightView, sizeof(LIGHTVIEW));
	g_LightView.up = { 0.0f, 1.0f, 0.0f };
	g_LightView.at = { 0.0f, 0.0f, 0.0f };
	g_LightView.viewAngle = (XMConvertToRadians(60.0f));
	g_LightView.viewAspect = ((float)SCREEN_WIDTH / (float)SCREEN_HEIGHT);
	g_LightView.nearZ = 10.0f;
	g_LightView.farZ = 10000.0f;



	//ライト初期化
	for (int i = 0; i < LIGHT_MAX; i++)
	{
		g_Light[i].Position  = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Light[i].Direction = XMFLOAT3( 0.0f, -1.0f, 0.0f );
		g_Light[i].Diffuse   = XMFLOAT4( 1.0f, 1.0f, 1.0f, 1.0f );
		g_Light[i].Ambient   = XMFLOAT4( 0.0f, 0.0f, 0.0f, 0.0f );
		g_Light[i].Attenuation = 200.0f;	// 減衰距離
		g_Light[i].Type = LIGHT_TYPE_NONE;	// ライトのタイプ
		g_Light[i].Enable = FALSE;			// ON / OFF
		SetLight(i, &g_Light[i]);
	}

	// 並行光源の設定（世界を照らす光）
	XMFLOAT3 dir;
	g_Light[0].Position = XMFLOAT3(MAINLIGHT_POS_X, MAINLIGHT_POS_Y, -MAINLIGHT_POS_Z);		// ライトの位置
	dir = SetMainLightDirectionFromPos();					// 位置から注視点(ワールド原点)への向きを算出
	g_Light[0].Direction = XMFLOAT3(dir.x, dir.y, dir.z);		// 光の向き
	g_Light[0].Diffuse   = XMFLOAT4( 1.0f, 1.0f, 1.0f, 1.0f );	// 光の色
	g_Light[0].Type = LIGHT_TYPE_DIRECTIONAL;					// 並行光源
	g_Light[0].Enable = TRUE;									// このライトをON
	SetLight(0, &g_Light[0]);									// これで設定している


	// ポイントライトの設定（カエルの頭上に固定）
	g_Light[1].Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	g_Light[1].Direction = XMFLOAT3(0.0f, -0.9f, 0.0f);
	g_Light[1].Diffuse = XMFLOAT4(0.9f, 0.9f, 0.9f, 1.0f);
	g_Light[1].Ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	g_Light[1].Attenuation = 75.0f;	// 減衰距離
	g_Light[1].Type = LIGHT_TYPE_POINT;	// ライトのタイプ
	g_Light[1].Enable = TRUE;			// ON / OFF
	SetLight(1, &g_Light[1]);


	// フォグの初期化（霧の効果）
	g_Fog.FogStart = 100.0f;									// 視点からこの距離離れるとフォグがかかり始める
	g_Fog.FogEnd   = 20000.0f;									// ここまで離れるとフォグの色で見えなくなる
	g_Fog.FogColor = XMFLOAT4(0.8f, 0.95f, 1.0f, 1.0f);		// フォグの色
	//g_Fog.FogColor = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);		// フォグの色
	SetFog(&g_Fog);
	SetFogEnable(TRUE);		// 他の場所もチェックする shadow

}

void SetFogColor(XMFLOAT4 color)
{
	g_Fog.FogColor = color;
	SetFog(&g_Fog);
}

void SetFogStartAndEnd(float start, float end)
{
	g_Fog.FogStart = start;
	g_Fog.FogEnd = end;
	SetFog(&g_Fog);
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateLight(void)
{
	//PLAYER *player = GetPlayer();
	////int mode = GetGameMode();

	////if (mode == GAME_MODE_STAGESELECT)
	////{
	////	CAMERA *camera = GetCamera();

	////	//g_Light[0].Position.x = MAINLIGHT_POS_X + camera[0].pos.x;
	////	g_Light[0].Position.y = MAINLIGHT_POS_Y + camera[0].pos.y + 100.0f;
	////	//g_Light[0].Position.z = MAINLIGHT_POS_Z + camera[0].pos.z;
	////}
	////else
	//{
	//	// ライトの座標をプレイヤーの位置に合わせる
	//	g_Light[0].Position.x = MAINLIGHT_POS_X + player[0].pos.x;
	//	g_Light[0].Position.y = MAINLIGHT_POS_Y + player[0].pos.y;
	//	g_Light[0].Position.z = MAINLIGHT_POS_Z + player[0].pos.z;
	//}


	//g_Light[1].Position.y = player[0].pos.y + POINTLIGHT_POS_Y;

	////g_Light[1].Position.x = player[0].pos.x + (LIGHT_WALL_DIS) * cosf(-player[0].rot.y);
	////g_Light[1].Position.z = player[0].pos.z + (LIGHT_WALL_DIS) * sinf(-player[0].rot.y);


	////g_Light[1].Position.x = player[0].pos.x;
	////g_Light[1].Position.z = player[0].pos.z;


	//g_Light[1].Position.x = (WALL_RADIUS + LIGHT_WALL_DIS) * cosf(-player[0].rot.y);
	//g_Light[1].Position.z = (WALL_RADIUS + LIGHT_WALL_DIS) * sinf(-player[0].rot.y);

	//SetLight(1, &g_Light[1]);


}


//=============================================================================
// ライトの設定
// Typeによってセットするメンバー変数が変わってくる
//=============================================================================
void SetLightData(int index, LIGHT *light)
{
	SetLight(index, light);
}


LIGHT *GetLightData(int index)
{
	return(&g_Light[index]);
}


//=============================================================================
// フォグの設定
//=============================================================================
void SetFogData(FOG *fog)
{
	SetFog(fog);
}



//=============================================================================
// メインライトの更新★
//=============================================================================
void SetMainLightView(BOOL isPers)
{
	//// プレイヤーの先頭アドレスを取得
	//PLAYER *player = GetPlayer();

	//// ビューマトリックス設定　　　　　　　　　　　↓視点　　　　　　　　　↓ベクトルの方向　　　　　　　　　↓アップベクトル（どちらが上か）
	//XMMATRIX mtxView;
	////mtxView = XMMatrixLookAtLH(XMLoadFloat3(&g_Light[0].Position), XMLoadFloat3(&g_Light[0].Direction), XMLoadFloat3(&g_LightView.up));
	//mtxView = XMMatrixLookAtLH(XMLoadFloat3(&g_Light[0].Position), XMLoadFloat3(&player[0].pos), XMLoadFloat3(&g_LightView.up));
	//SetLViewMatrix(&mtxView);	//ビューマトリクスをセット
	//XMStoreFloat4x4(&g_LightView.mtxView, mtxView);	//ライトのグローバル構造体へコピー（保存）している

	//XMMATRIX mtxInvView;
	//mtxInvView = XMMatrixInverse(nullptr, mtxView);
	//XMStoreFloat4x4(&g_LightView.mtxInvView, mtxInvView);


	//// プロジェクションマトリックス設定　　　↓視野角　　↓アスペクト比　↓
	//XMMATRIX mtxProjection;

	//if (isPers)
	//{
	//	mtxProjection = XMMatrixPerspectiveFovLH(g_LightView.viewAngle, g_LightView.viewAspect, g_LightView.nearZ, g_LightView.farZ);
	//}
	//else
	//{
	//	mtxProjection = XMMatrixOrthographicLH((float)SCREEN_WIDTH * 0.5f, (float)SCREEN_HEIGHT * 0.5f, g_LightView.nearZ, g_LightView.farZ);
	//}

	//SetLProjectionMatrix(&mtxProjection);
	//XMStoreFloat4x4(&g_LightView.mtxProjection, mtxProjection);

}


//=============================================================================
// メインライトの座標から注視点までの方向を求める★
//=============================================================================
XMFLOAT3 SetMainLightDirectionFromPos(void)
{
	// XYZそれぞれの視点から注視点までの移動量を求める
	XMFLOAT3 posbuf = { g_LightView.at.x - g_Light[0].Position.x,
						g_LightView.at.y - g_Light[0].Position.y,
						g_LightView.at.z - g_Light[0].Position.z };

	// 最も移動量の絶対値が大きい軸を求める
	int greaterPos = 0;
	if (fabsf(posbuf.x) > fabsf(posbuf.y))
	{
		if (fabsf(posbuf.x) > fabsf(posbuf.z))
		{
			// Xが一番大きい
			greaterPos = 0;
		}
		else
		{
			// Zが一番大きい
			greaterPos = 2;
		}
	}
	else if (fabsf(posbuf.y) > fabsf(posbuf.z))
	{
		// Yが一番大きい
		greaterPos = 1;
	}
	else
	{
		// Zが一番大きい
		greaterPos = 2;
	}

	// ライトの方向を求める
	XMFLOAT3 dirbuf = { 0.0f, 0.0f, 0.0f };
	switch (greaterPos)
	{
	case 0:
		if (posbuf.x < 0.0f)
		{
			dirbuf.x = -1.0f;
			dirbuf.y = -(posbuf.y / posbuf.x);
			dirbuf.z = -(posbuf.z / posbuf.x);
		}
		else
		{
			dirbuf.x = 1.0f;
			dirbuf.y = posbuf.y / posbuf.x;
			dirbuf.z = posbuf.z / posbuf.x;
		}
		break;

	case 1:
		if (posbuf.y < 0.0f)
		{
			dirbuf.x = -(posbuf.x / posbuf.y);
			dirbuf.y = -1.0f;
			dirbuf.z = -(posbuf.z / posbuf.y);
		}
		else
		{
			dirbuf.x = posbuf.x / posbuf.y;
			dirbuf.y = 1.0f;
			dirbuf.z = posbuf.z / posbuf.y;
		}
		break;

	case 2:
		if (posbuf.z < 0.0f)
		{
			dirbuf.x = -(posbuf.x / posbuf.z);
			dirbuf.y = -(posbuf.y / posbuf.z);
			dirbuf.z = -1.0f;
		}
		else
		{
			dirbuf.x = posbuf.x / posbuf.z;
			dirbuf.y = posbuf.y / posbuf.z;
			dirbuf.z = 1.0f;
		}
		break;
	}

	return dirbuf;
}
