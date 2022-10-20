//=============================================================================
//
// パイプライン壁の処理 [tube.cpp]
// Author : 國江 翔太
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "tube.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	TUBE_NUM		(2)	

//*****************************************************************************
// 構造体定義
//*****************************************************************************
struct MESH_TUBE
{
	MATERIAL		material;					// マテリアル
	XMFLOAT3		pos;						// 中心座標
	XMFLOAT3		rot;						// 回転
	//XMFLOAT3		scl;						// 拡大率
	int				nX, nZ;		// ブロック数
	int				nVertex;					// 総頂点数	
	int				nVertexIndex;			// 総インデックス数
	int				nPolygon;				// 総ポリゴン数
	float			sizeX, sizeZ;	// ブロックサイズ

	ID3D11Buffer	*vertexBuffer;	// 頂点バッファ
	ID3D11Buffer	*indexBuffer;	// インデックスバッファ
};

enum TUBE_BUFFER
{
	TUBE_BUFFER_FRONT = 0,
	TUBE_BUFFER_BACK,
	TUBE_BUFFER_MAX,
};

enum TUBE_TYPE
{
	TUBE_TYPE_STRAIGHT = 0,
	TUBE_TYPE_CURVE,
	TUBE_TYPE_MAX,
};

enum CURVE_TYPE
{
	CURVE_TYPE_RIGHT = 0,
	CURVE_TYPE_LEFT,
	CURVE_TYPE_UP,
	CURVE_TYPE_DOWN,
};

struct TUBE
{
	XMFLOAT3	pos;	// 中心座標
	XMFLOAT3	rot;	// 回転
	TUBE_TYPE	type;
};

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static MESH_TUBE g_MeshTube[TUBE_TYPE_MAX];
static TUBE g_Tube[TUBE_BUFFER_MAX];

// テクスチャ管理
enum
{
	TEXTURE_NOMAL = 0,
	TEXTURE_MAX,
};
// static TEXTURE2D_DESC	g_td[TEXTURE_MAX];
static ID3D11ShaderResourceView*	g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報
static char*	g_TextureName[TEXTURE_MAX] = {
	"data/TEXTURE/r.tif",
};

static BOOL		g_Load = FALSE;
static float	g_time;
static float	g_worldRot = 0.0f;

TUBE_TYPE testTubeArr[8];

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitTube(void)
{
	for (int i = 0; i < 8; i++)
	{
		//testTubeArr[i] = (TUBE_TYPE)(i % 2);
		testTubeArr[i] = TUBE_TYPE_STRAIGHT;
	}

	// テクスチャ生成
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		D3DX11CreateShaderResourceViewFromFile(GetDevice(), g_TextureName[i], NULL, NULL, &g_Texture[i], NULL);
	}

	// 直線パイプ
	{
		// ポジション設定
		g_MeshTube[TUBE_TYPE_STRAIGHT].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_MeshTube[TUBE_TYPE_STRAIGHT].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);

		// マテリアル情報の初期化
		ZeroMemory(&g_MeshTube[TUBE_TYPE_STRAIGHT].material, sizeof(g_MeshTube[TUBE_TYPE_STRAIGHT].material));
		g_MeshTube[TUBE_TYPE_STRAIGHT].material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		// ブロック数の設定
		g_MeshTube[TUBE_TYPE_STRAIGHT].nX = MESH_NUM_X;
		g_MeshTube[TUBE_TYPE_STRAIGHT].nZ = MESH_NUM_Z;

		// 頂点数の設定
		g_MeshTube[TUBE_TYPE_STRAIGHT].nVertex = (MESH_NUM_X + 1) * (MESH_NUM_Z + 1);

		// インデックス数の設定
		g_MeshTube[TUBE_TYPE_STRAIGHT].nVertexIndex = (MESH_NUM_X + 1) * 2 * MESH_NUM_Z + (MESH_NUM_Z - 1) * 2;

		// ポリゴン数の設定
		g_MeshTube[TUBE_TYPE_STRAIGHT].nPolygon = MESH_NUM_X * MESH_NUM_Z * 2 + (MESH_NUM_Z - 1) * 4;

		// ブロックサイズの設定
		g_MeshTube[TUBE_TYPE_STRAIGHT].sizeX = MESH_SIZE;
		g_MeshTube[TUBE_TYPE_STRAIGHT].sizeZ = MESH_SIZE;

		// 頂点バッファ生成
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DYNAMIC;
		bd.ByteWidth = sizeof(VERTEX_3D) * g_MeshTube[TUBE_TYPE_STRAIGHT].nVertex;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		GetDevice()->CreateBuffer(&bd, NULL, &g_MeshTube[TUBE_TYPE_STRAIGHT].vertexBuffer);

		// インデックスバッファ生成
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DYNAMIC;
		bd.ByteWidth = sizeof(unsigned short) * g_MeshTube[TUBE_TYPE_STRAIGHT].nVertexIndex;
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		GetDevice()->CreateBuffer(&bd, NULL, &g_MeshTube[TUBE_TYPE_STRAIGHT].indexBuffer);

		{//頂点バッファの中身を埋める
#if 0
			const float texSizeX = 1.0f / g_nX;
			const float texSizeZ = 1.0f / g_nZ;
#else
			const float texSizeX = 1.0f;
			const float texSizeZ = 1.0f;
#endif

			// 頂点バッファへのポインタを取得
			D3D11_MAPPED_SUBRESOURCE msr;
			GetDeviceContext()->Map(g_MeshTube[TUBE_TYPE_STRAIGHT].vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

			VERTEX_3D* pVtx = (VERTEX_3D*)msr.pData;

			float drad = XM_2PI / g_MeshTube[TUBE_TYPE_STRAIGHT].nX;	// 単位角度
			float halfNumBlockX = (float)(g_MeshTube[TUBE_TYPE_STRAIGHT].nX / 2);	// X軸ブロック数の半分の値

			for (int nz = 0; nz < (g_MeshTube[TUBE_TYPE_STRAIGHT].nZ + 1); nz++)
			{
				for (int nx = 0; nx < (g_MeshTube[TUBE_TYPE_STRAIGHT].nX + 1); nx++)
				{
					// 頂点座標の設定
					pVtx[nz * (g_MeshTube[TUBE_TYPE_STRAIGHT].nX + 1) + nx].Position.x = TUBE_RADIUS * cosf(nx * drad);
					pVtx[nz * (g_MeshTube[TUBE_TYPE_STRAIGHT].nX + 1) + nx].Position.y = TUBE_RADIUS * sinf(nx * drad);
					pVtx[nz * (g_MeshTube[TUBE_TYPE_STRAIGHT].nX + 1) + nx].Position.z = 0.5f * g_MeshTube[TUBE_TYPE_STRAIGHT].nZ * g_MeshTube[TUBE_TYPE_STRAIGHT].sizeZ - nz * g_MeshTube[TUBE_TYPE_STRAIGHT].sizeZ;

					// 法線の設定
					pVtx[nz * (g_MeshTube[TUBE_TYPE_STRAIGHT].nX + 1) + nx].Normal = XMFLOAT3(cosf(nx * drad + XM_PI), sinf(nx * drad + XM_PI), 0.0f);

					// 拡散光の設定
					pVtx[nz * (g_MeshTube[TUBE_TYPE_STRAIGHT].nX + 1) + nx].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

					// テクスチャ座標の設定
					pVtx[nz * (g_MeshTube[TUBE_TYPE_STRAIGHT].nX + 1) + nx].TexCoord.x = texSizeX * nx;
					pVtx[nz * (g_MeshTube[TUBE_TYPE_STRAIGHT].nX + 1) + nx].TexCoord.y = texSizeZ * nz;

					// テクスチャ座標の調整
					//（円柱のテクスチャ座標を途切れさせないため、X軸ブロック数の半分に到達したら座標の値を折り返す。）
					if (nx > halfNumBlockX)
					{
						pVtx[nz * (g_MeshTube[TUBE_TYPE_STRAIGHT].nX + 1) + nx].TexCoord.x = halfNumBlockX + (halfNumBlockX - nx);
					}
				}
			}

			GetDeviceContext()->Unmap(g_MeshTube[TUBE_TYPE_STRAIGHT].vertexBuffer, 0);
		}

		{//インデックスバッファの中身を埋める
			// インデックスバッファのポインタを取得
			D3D11_MAPPED_SUBRESOURCE msr;
			GetDeviceContext()->Map(g_MeshTube[TUBE_TYPE_STRAIGHT].indexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

			unsigned short *pIdx = (unsigned short*)msr.pData;

			int nCntIdx = 0;
			for (int nz = 0; nz < g_MeshTube[TUBE_TYPE_STRAIGHT].nZ; nz++)
			{
				if (nz > 0)
				{// 縮退ポリゴンのためのダブりの設定
					pIdx[nCntIdx] = (nz + 1) * (g_MeshTube[TUBE_TYPE_STRAIGHT].nX + 1);
					nCntIdx++;
				}

				for (int nx = 0; nx < (g_MeshTube[TUBE_TYPE_STRAIGHT].nX + 1); nx++)
				{
					pIdx[nCntIdx] = (nz + 1) * (g_MeshTube[TUBE_TYPE_STRAIGHT].nX + 1) + nx;
					nCntIdx++;
					pIdx[nCntIdx] = nz * (g_MeshTube[TUBE_TYPE_STRAIGHT].nX + 1) + nx;
					nCntIdx++;
				}

				if (nz < (g_MeshTube[TUBE_TYPE_STRAIGHT].nZ - 1))
				{// 縮退ポリゴンのためのダブりの設定
					pIdx[nCntIdx] = nz * (g_MeshTube[TUBE_TYPE_STRAIGHT].nX + 1) + g_MeshTube[TUBE_TYPE_STRAIGHT].nX;
					nCntIdx++;
				}
			}

			GetDeviceContext()->Unmap(g_MeshTube[TUBE_TYPE_STRAIGHT].indexBuffer, 0);
		}
	}

	// カーブパイプ
	{
		// ポジション設定
		g_MeshTube[TUBE_TYPE_CURVE].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_MeshTube[TUBE_TYPE_CURVE].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);

		// マテリアル情報の初期化
		ZeroMemory(&g_MeshTube[TUBE_TYPE_CURVE].material, sizeof(g_MeshTube[TUBE_TYPE_CURVE].material));
		g_MeshTube[TUBE_TYPE_CURVE].material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		// ブロック数の設定
		g_MeshTube[TUBE_TYPE_CURVE].nX = MESH_NUM_X;
		g_MeshTube[TUBE_TYPE_CURVE].nZ = MESH_NUM_Z;

		// 頂点数の設定
		g_MeshTube[TUBE_TYPE_CURVE].nVertex = (MESH_NUM_X + 1) * (MESH_NUM_Z + 1);

		// インデックス数の設定
		g_MeshTube[TUBE_TYPE_CURVE].nVertexIndex = (MESH_NUM_X + 1) * 2 * MESH_NUM_Z + (MESH_NUM_Z - 1) * 2;

		// ポリゴン数の設定
		g_MeshTube[TUBE_TYPE_CURVE].nPolygon = MESH_NUM_X * MESH_NUM_Z * 2 + (MESH_NUM_Z - 1) * 4;

		// ブロックサイズの設定
		g_MeshTube[TUBE_TYPE_CURVE].sizeX = MESH_SIZE;
		g_MeshTube[TUBE_TYPE_CURVE].sizeZ = MESH_SIZE;

		// 頂点バッファ生成
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DYNAMIC;
		bd.ByteWidth = sizeof(VERTEX_3D) * g_MeshTube[TUBE_TYPE_CURVE].nVertex;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		GetDevice()->CreateBuffer(&bd, NULL, &g_MeshTube[TUBE_TYPE_CURVE].vertexBuffer);

		// インデックスバッファ生成
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DYNAMIC;
		bd.ByteWidth = sizeof(unsigned short) * g_MeshTube[TUBE_TYPE_CURVE].nVertexIndex;
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		GetDevice()->CreateBuffer(&bd, NULL, &g_MeshTube[TUBE_TYPE_CURVE].indexBuffer);

		{//頂点バッファの中身を埋める

			const float texSizeX = 1.0f;
			const float texSizeZ = 1.0f;

			// 頂点バッファへのポインタを取得
			D3D11_MAPPED_SUBRESOURCE msr;
			GetDeviceContext()->Map(g_MeshTube[TUBE_TYPE_CURVE].vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

			VERTEX_3D* pVtx = (VERTEX_3D*)msr.pData;

			float drad = XM_2PI / g_MeshTube[TUBE_TYPE_CURVE].nX;	// 単位角度
			float halfNumBlockX = (float)(g_MeshTube[TUBE_TYPE_CURVE].nX / 2);	// X軸ブロック数の半分の値

			for (int nz = 0; nz < (g_MeshTube[TUBE_TYPE_CURVE].nZ + 1); nz++)
			{
				for (int nx = 0; nx < (g_MeshTube[TUBE_TYPE_CURVE].nX + 1); nx++)
				{
					// カーブの半径
					float rad = CURVE_RADIUS + TUBE_RADIUS * cosf(nx * drad);
					float agl = XM_PIDIV2 * (float)nz / (float)g_MeshTube[TUBE_TYPE_CURVE].nZ - XM_PIDIV2;	// -XM_PIDIV2で右カーブ

					// 頂点座標の設定
					pVtx[nz * (g_MeshTube[TUBE_TYPE_CURVE].nX + 1) + nx].Position.x = rad * sinf(agl) + CURVE_CENTER;
					pVtx[nz * (g_MeshTube[TUBE_TYPE_CURVE].nX + 1) + nx].Position.y = TUBE_RADIUS * sinf(nx * drad);
					pVtx[nz * (g_MeshTube[TUBE_TYPE_CURVE].nX + 1) + nx].Position.z = rad * cosf(agl) - CURVE_CENTER;

					// 法線の設定
					pVtx[nz * (g_MeshTube[TUBE_TYPE_CURVE].nX + 1) + nx].Normal = XMFLOAT3(cosf(nx * drad + XM_PI), sinf(nx * drad + XM_PI), 0.0f);

					// 拡散光の設定
					pVtx[nz * (g_MeshTube[TUBE_TYPE_CURVE].nX + 1) + nx].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

					// テクスチャ座標の設定
					pVtx[nz * (g_MeshTube[TUBE_TYPE_CURVE].nX + 1) + nx].TexCoord.x = texSizeX * nx;
					pVtx[nz * (g_MeshTube[TUBE_TYPE_CURVE].nX + 1) + nx].TexCoord.y = texSizeZ * nz;

					// テクスチャ座標の調整
					//（円柱のテクスチャ座標を途切れさせないため、X軸ブロック数の半分に到達したら座標の値を折り返す。）
					if (nx > halfNumBlockX)
					{
						pVtx[nz * (g_MeshTube[TUBE_TYPE_CURVE].nX + 1) + nx].TexCoord.x = halfNumBlockX + (halfNumBlockX - nx);
					}
				}
			}

			GetDeviceContext()->Unmap(g_MeshTube[TUBE_TYPE_CURVE].vertexBuffer, 0);
		}

		{//インデックスバッファの中身を埋める
			// インデックスバッファのポインタを取得
			D3D11_MAPPED_SUBRESOURCE msr;
			GetDeviceContext()->Map(g_MeshTube[TUBE_TYPE_CURVE].indexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

			unsigned short *pIdx = (unsigned short*)msr.pData;

			int nCntIdx = 0;
			for (int nz = 0; nz < g_MeshTube[TUBE_TYPE_CURVE].nZ; nz++)
			{
				if (nz > 0)
				{// 縮退ポリゴンのためのダブりの設定
					pIdx[nCntIdx] = (nz + 1) * (g_MeshTube[TUBE_TYPE_CURVE].nX + 1);
					nCntIdx++;
				}

				for (int nx = 0; nx < (g_MeshTube[TUBE_TYPE_CURVE].nX + 1); nx++)
				{
					pIdx[nCntIdx] = (nz + 1) * (g_MeshTube[TUBE_TYPE_CURVE].nX + 1) + nx;
					nCntIdx++;
					pIdx[nCntIdx] = nz * (g_MeshTube[TUBE_TYPE_CURVE].nX + 1) + nx;
					nCntIdx++;
				}

				if (nz < (g_MeshTube[TUBE_TYPE_CURVE].nZ - 1))
				{// 縮退ポリゴンのためのダブりの設定
					pIdx[nCntIdx] = nz * (g_MeshTube[TUBE_TYPE_CURVE].nX + 1) + g_MeshTube[TUBE_TYPE_CURVE].nX;
					nCntIdx++;
				}
			}

			GetDeviceContext()->Unmap(g_MeshTube[TUBE_TYPE_CURVE].indexBuffer, 0);
		}
	}

	g_time = 0.0f;

	//SetTubeCurveRight(0);
	//SetTubeCurveRight(1);
	g_Tube[TUBE_BUFFER_FRONT].type = testTubeArr[0];
	SetNextTube();
	
	//g_Tube[TUBE_BUFFER_BACK].rot.y = -XM_PIDIV2;
	//g_Tube[TUBE_BUFFER_BACK].pos.x = CURVE_RADIUS + CURVE_RADIUS * cosf(XM_PIDIV4);
	//g_Tube[TUBE_BUFFER_BACK].pos.z = CURVE_RADIUS * sinf(XM_PIDIV4);

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitTube(void)
{
	if (g_Load == FALSE) return;

	for (int i = 0; i < TUBE_TYPE_MAX; i++)
	{
		// 頂点バッファの解放
		if (g_MeshTube[i].vertexBuffer)
		{
			g_MeshTube[i].vertexBuffer->Release();
			g_MeshTube[i].vertexBuffer = NULL;
		}

		// インデックスバッファの解放
		if (g_MeshTube[i].indexBuffer)
		{
			g_MeshTube[i].indexBuffer->Release();
			g_MeshTube[i].indexBuffer = NULL;
		}
	}

	// テクスチャの解放
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		if (g_Texture[i])
		{
			g_Texture[i]->Release();
			g_Texture[i] = NULL;
		}
	}

	g_Load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateTube(void)
{

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawTube(void)
{
	for (int i = 0; i < TUBE_BUFFER_MAX; i++)
	{
		// 頂点バッファ設定
		UINT stride = sizeof(VERTEX_3D);
		UINT offset = 0;
		GetDeviceContext()->IASetVertexBuffers(0, 1, &g_MeshTube[g_Tube[i].type].vertexBuffer, &stride, &offset);

		// インデックスバッファ設定
		GetDeviceContext()->IASetIndexBuffer(g_MeshTube[g_Tube[i].type].indexBuffer, DXGI_FORMAT_R16_UINT, 0);

		// プリミティブトポロジ設定
		GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

		// マテリアル設定
		SetMaterialBuffer(&g_MeshTube[g_Tube[i].type].material);

		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[TEXTURE_NOMAL]);


		XMMATRIX mtxRot, mtxTranslate, mtxWorld;

		// ワールドマトリックスの初期化
		mtxWorld = XMMatrixIdentity();

		// 回転を反映
		mtxRot = XMMatrixRotationRollPitchYaw(g_Tube[i].rot.x, g_Tube[i].rot.y, g_Tube[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// 移動を反映
		mtxTranslate = XMMatrixTranslation(g_Tube[i].pos.x, g_Tube[i].pos.y, g_Tube[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// 世界の回転を反映
		//mtxRot = XMMatrixRotationRollPitchYaw(0.0f, 0.0f, g_worldRot);
		//SetAfterRotation(&mtxRot);
		//mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// ワールドマトリックスの設定
		SetWorldBuffer(&mtxWorld);

		// ポリゴンの描画
		GetDeviceContext()->DrawIndexed(g_MeshTube[g_Tube[i].type].nVertexIndex, 0, 0);
	}
}

void RotateTube(float rot)
{
	//for (int i = 0; i < TUBE_NUM; i++)
	//{
	//	g_MeshTube[i].rot.z += rot;
	//}

	g_worldRot += rot;
}

void MoveTube(float move)
{
	for (int i = 0; i < TUBE_BUFFER_MAX; i++)
	{
		//g_MeshTube[i].pos.z -= move;
		//if (g_MeshTube[i].pos.z <= -TUBE_SIZE * 0.5f)
		//{
		//	g_MeshTube[i].pos.z += 2.0f * TUBE_SIZE;
		//}
		switch (g_Tube[TUBE_BUFFER_FRONT].type)
		{
		case TUBE_TYPE_STRAIGHT:
			g_Tube[i].pos.z -= move;
			break;
		case TUBE_TYPE_CURVE:
			g_Tube[i].rot.y -= XM_PIDIV2 * move / TUBE_SIZE;
			float agl = -g_Tube[i].rot.y + XM_PIDIV2 + XM_PIDIV4;
			g_Tube[i].pos.x = CURVE_RADIUS + CURVE_RADIUS * cosf(agl);
			g_Tube[i].pos.z = CURVE_RADIUS * sinf(agl);
			break;
		}
	}

	switch (g_Tube[TUBE_BUFFER_FRONT].type)
	{
	case TUBE_TYPE_STRAIGHT:
		if (g_Tube[TUBE_BUFFER_FRONT].pos.z <= -TUBE_SIZE * 0.5f)
		{
			PresentTube();
		}
		break;
	case TUBE_TYPE_CURVE:
		if (g_Tube[TUBE_BUFFER_FRONT].rot.y <= -XM_PIDIV2)
		{
			PresentTube();
		}
		break;
	}
}
void TestCurveTube(float move)
{
	float agl;
	for (int i = 0; i < 2; i++)
	{
		g_MeshTube[i].rot.y -= XM_PIDIV2 * move / TUBE_SIZE;
		agl = -g_MeshTube[i].rot.y + XM_PIDIV2 + XM_PIDIV4;
		g_MeshTube[i].pos.x = CURVE_RADIUS + CURVE_RADIUS * cosf(agl);
		g_MeshTube[i].pos.z = CURVE_RADIUS * sinf(agl);
		if (g_MeshTube[i].rot.y <= -XM_PIDIV2)
		{
			g_MeshTube[i].rot.y += XM_PI;
			agl -= XM_PI;
			g_MeshTube[i].pos.x = CURVE_RADIUS + CURVE_RADIUS * cosf(agl);
			g_MeshTube[i].pos.z = CURVE_RADIUS * sinf(agl);
		}
	}
}

float GetTubeRotation(void)
{
	return g_MeshTube[0].rot.z;
	//return g_worldRot;
}

void PresentTube(void)
{
	g_Tube[TUBE_BUFFER_FRONT] = g_Tube[TUBE_BUFFER_BACK];
	SetNextTube();
}
void SetNextTube(void)
{
	static int now = 0;
	switch (testTubeArr[now])
	{
	case TUBE_TYPE_STRAIGHT:
		switch (testTubeArr[now + 1])
		{
		case TUBE_TYPE_STRAIGHT:
			g_Tube[TUBE_BUFFER_BACK].type = TUBE_TYPE_STRAIGHT;
			g_Tube[TUBE_BUFFER_BACK].rot.y = 0.0f;
			g_Tube[TUBE_BUFFER_BACK].pos.x = 0.0f;
			g_Tube[TUBE_BUFFER_BACK].pos.y = 0.0f;
			g_Tube[TUBE_BUFFER_BACK].pos.z = TUBE_SIZE;
			break;
		case TUBE_TYPE_CURVE:
			g_Tube[TUBE_BUFFER_BACK].type = TUBE_TYPE_CURVE;
			g_Tube[TUBE_BUFFER_BACK].rot.y = 0.0f;
			g_Tube[TUBE_BUFFER_BACK].pos.x = CURVE_RADIUS + CURVE_RADIUS * cosf(XM_PIDIV4 + XM_PIDIV2);
			g_Tube[TUBE_BUFFER_BACK].pos.y = 0.0f;
			g_Tube[TUBE_BUFFER_BACK].pos.z = TUBE_SIZE * 0.5f + CURVE_RADIUS * sinf(XM_PIDIV4 + XM_PIDIV2);
			break;
		}
		break;
	case TUBE_TYPE_CURVE:
		switch (testTubeArr[now + 1])
		{
		case TUBE_TYPE_STRAIGHT:
			g_Tube[TUBE_BUFFER_BACK].type = TUBE_TYPE_STRAIGHT;
			g_Tube[TUBE_BUFFER_BACK].rot.y = XM_PIDIV2;
			g_Tube[TUBE_BUFFER_BACK].pos.x = TUBE_SIZE;
			g_Tube[TUBE_BUFFER_BACK].pos.y = 0.0f;
			g_Tube[TUBE_BUFFER_BACK].pos.z = TUBE_SIZE * 0.5f;
			break;
		case TUBE_TYPE_CURVE:
			g_Tube[TUBE_BUFFER_BACK].type = TUBE_TYPE_CURVE;
			g_Tube[TUBE_BUFFER_BACK].rot.y = -XM_PIDIV2;
			g_Tube[TUBE_BUFFER_BACK].pos.x = CURVE_RADIUS + CURVE_RADIUS * cosf(XM_PIDIV4);
			g_Tube[TUBE_BUFFER_BACK].pos.y = 0.0f;
			g_Tube[TUBE_BUFFER_BACK].pos.z = CURVE_RADIUS * sinf(XM_PIDIV4);
			break;
		}
		break;
	}
	now++;
}

void SetTubeCurveRight(int tube)
{
	int i = tube;

	//頂点バッファを書き換える
	const float texSizeX = 1.0f;
	const float texSizeZ = 1.0f;

	// 頂点バッファへのポインタを取得
	D3D11_MAPPED_SUBRESOURCE msr;
	GetDeviceContext()->Map(g_MeshTube[i].vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

	VERTEX_3D* pVtx = (VERTEX_3D*)msr.pData;

	float drad = XM_2PI / g_MeshTube[i].nX;	// 単位角度
	float halfNumBlockX = (float)(g_MeshTube[i].nX / 2);	// X軸ブロック数の半分の値

	for (int nz = 0; nz < (g_MeshTube[i].nZ + 1); nz++)
	{
		for (int nx = 0; nx < (g_MeshTube[i].nX + 1); nx++)
		{
			// カーブの半径
			float rad = CURVE_RADIUS + TUBE_RADIUS * cosf(nx * drad);
			float agl = XM_PIDIV2 * (float)nz / (float)g_MeshTube[i].nZ - XM_PIDIV2;	// -XM_PIDIV2で右カーブ

			// 頂点座標の設定
			pVtx[nz * (g_MeshTube[i].nX + 1) + nx].Position.x = rad * sinf(agl) + CURVE_CENTER;
			pVtx[nz * (g_MeshTube[i].nX + 1) + nx].Position.y = TUBE_RADIUS * sinf(nx * drad);
			pVtx[nz * (g_MeshTube[i].nX + 1) + nx].Position.z = rad * cosf(agl) - CURVE_CENTER;

			// 法線の設定
			pVtx[nz * (g_MeshTube[i].nX + 1) + nx].Normal = XMFLOAT3(cosf(nx * drad + XM_PI), sinf(nx * drad + XM_PI), 0.0f);

			// 拡散光の設定
			pVtx[nz * (g_MeshTube[i].nX + 1) + nx].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

			// テクスチャ座標の設定
			pVtx[nz * (g_MeshTube[i].nX + 1) + nx].TexCoord.x = texSizeX * nx;
			pVtx[nz * (g_MeshTube[i].nX + 1) + nx].TexCoord.y = texSizeZ * nz;

			// テクスチャ座標の調整
			//（円柱のテクスチャ座標を途切れさせないため、X軸ブロック数の半分に到達したら座標の値を折り返す。）
			if (nx > halfNumBlockX)
			{
				pVtx[nz * (g_MeshTube[i].nX + 1) + nx].TexCoord.x = halfNumBlockX + (halfNumBlockX - nx);
			}
		}
	}

	GetDeviceContext()->Unmap(g_MeshTube[i].vertexBuffer, 0);
}