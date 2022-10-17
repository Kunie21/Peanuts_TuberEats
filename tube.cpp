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
#define	TUBE_RADIUS		(200.0f)	// 半径
#define	MESH_NUM_X		(8)			// 断面の角数
#define	MESH_SIZE		(TUBE_RADIUS * 2.0f * XM_PI / MESH_NUM_X)	// マスの1辺の長さ
#define	MESH_NUM_Z		(100)		// 奥行き
#define	TUBE_NUM		(2)	

//*****************************************************************************
// 構造体定義
//*****************************************************************************
typedef struct
{
	MATERIAL		material;					// マテリアル
	XMFLOAT3		pos;						// 中心座標
	XMFLOAT3		rot;						// 回転
	//XMFLOAT3		scl;						// 拡大率
	int				nNumBlockX, nNumBlockZ;		// ブロック数
	int				nNumVertex;					// 総頂点数	
	int				nNumVertexIndex;			// 総インデックス数
	int				nNumPolygon;				// 総ポリゴン数
	float			fBlockSizeX, fBlockSizeZ;	// ブロックサイズ

	ID3D11Buffer	*vertexBuffer;	// 頂点バッファ
	ID3D11Buffer	*indexBuffer;	// インデックスバッファ

} MESH_TUBE;

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static MESH_TUBE g_MeshTube[TUBE_NUM];

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
static int		g_frontTube = 0;
static int		g_backTube = 1;

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitTube(void)
{
	// テクスチャ生成
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		D3DX11CreateShaderResourceViewFromFile(GetDevice(), g_TextureName[i], NULL, NULL, &g_Texture[i], NULL);
	}

	for (int i = 0; i < TUBE_NUM; i++)
	{
		// ポジション設定
		g_MeshTube[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_MeshTube[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);

		// マテリアル情報の初期化
		ZeroMemory(&g_MeshTube[i].material, sizeof(g_MeshTube[i].material));
		g_MeshTube[i].material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		// ブロック数の設定
		g_MeshTube[i].nNumBlockX = MESH_NUM_X;
		g_MeshTube[i].nNumBlockZ = MESH_NUM_Z;

		// 頂点数の設定
		g_MeshTube[i].nNumVertex = (MESH_NUM_X + 1) * (MESH_NUM_Z + 1);

		// インデックス数の設定
		g_MeshTube[i].nNumVertexIndex = (MESH_NUM_X + 1) * 2 * MESH_NUM_Z + (MESH_NUM_Z - 1) * 2;

		// ポリゴン数の設定
		g_MeshTube[i].nNumPolygon = MESH_NUM_X * MESH_NUM_Z * 2 + (MESH_NUM_Z - 1) * 4;

		// ブロックサイズの設定
		g_MeshTube[i].fBlockSizeX = MESH_SIZE;
		g_MeshTube[i].fBlockSizeZ = MESH_SIZE;

		// 頂点バッファ生成
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DYNAMIC;
		bd.ByteWidth = sizeof(VERTEX_3D) * g_MeshTube[i].nNumVertex;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		GetDevice()->CreateBuffer(&bd, NULL, &g_MeshTube[i].vertexBuffer);

		// インデックスバッファ生成
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DYNAMIC;
		bd.ByteWidth = sizeof(unsigned short) * g_MeshTube[i].nNumVertexIndex;
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		GetDevice()->CreateBuffer(&bd, NULL, &g_MeshTube[i].indexBuffer);

		{//頂点バッファの中身を埋める
#if 0
			const float texSizeX = 1.0f / g_nNumBlockX;
			const float texSizeZ = 1.0f / g_nNumBlockZ;
#else
			const float texSizeX = 1.0f;
			const float texSizeZ = 1.0f;
#endif

			// 頂点バッファへのポインタを取得
			D3D11_MAPPED_SUBRESOURCE msr;
			GetDeviceContext()->Map(g_MeshTube[i].vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

			VERTEX_3D* pVtx = (VERTEX_3D*)msr.pData;

			float drad = XM_2PI / g_MeshTube[i].nNumBlockX;	// 単位角度
			float halfNumBlockX = (float)(g_MeshTube[i].nNumBlockX / 2);	// X軸ブロック数の半分の値

			for (int nCntVtxY = 0; nCntVtxY < (g_MeshTube[i].nNumBlockZ + 1); nCntVtxY++)
			{
				for (int nCntVtxX = 0; nCntVtxX < (g_MeshTube[i].nNumBlockX + 1); nCntVtxX++)
				{
					// 頂点座標の設定
					pVtx[nCntVtxY * (g_MeshTube[i].nNumBlockX + 1) + nCntVtxX].Position.x = TUBE_RADIUS * cosf(nCntVtxX * drad);
					pVtx[nCntVtxY * (g_MeshTube[i].nNumBlockX + 1) + nCntVtxX].Position.y = TUBE_RADIUS * sinf(nCntVtxX * drad);
					pVtx[nCntVtxY * (g_MeshTube[i].nNumBlockX + 1) + nCntVtxX].Position.z = g_MeshTube[i].nNumBlockZ * g_MeshTube[i].fBlockSizeZ - nCntVtxY * g_MeshTube[i].fBlockSizeZ;

					// 法線の設定
					pVtx[nCntVtxY * (g_MeshTube[i].nNumBlockX + 1) + nCntVtxX].Normal = XMFLOAT3(cosf(nCntVtxX * drad + XM_PI), sinf(nCntVtxX * drad + XM_PI), 0.0f);

					// 拡散光の設定
					pVtx[nCntVtxY * (g_MeshTube[i].nNumBlockX + 1) + nCntVtxX].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

					// テクスチャ座標の設定
					pVtx[nCntVtxY * (g_MeshTube[i].nNumBlockX + 1) + nCntVtxX].TexCoord.x = texSizeX * nCntVtxX;
					pVtx[nCntVtxY * (g_MeshTube[i].nNumBlockX + 1) + nCntVtxX].TexCoord.y = texSizeZ * nCntVtxY;

					//// 頂点座標をすぼめる
					//int closeVertex = 2;
					//if (nCntVtxY < closeVertex)
					//{
					//	// 法線の設定
					//	pVtx[nCntVtxY * (g_MeshTube[i].nNumBlockX + 1) + nCntVtxX].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);

					//	// 頂点の設定
					//	pVtx[nCntVtxY * (g_MeshTube[i].nNumBlockX + 1) + nCntVtxX].Position.x = nCntVtxY * (1 / closeVertex) * TUBE_RADIUS * cosf(nCntVtxX * drad);
					//	pVtx[nCntVtxY * (g_MeshTube[i].nNumBlockX + 1) + nCntVtxX].Position.y = nCntVtxY * (1 / closeVertex) * TUBE_RADIUS * sinf(nCntVtxX * drad);
					//	pVtx[nCntVtxY * (g_MeshTube[i].nNumBlockX + 1) + nCntVtxX].Position.z = g_MeshTube[i].nNumBlockZ * g_MeshTube[i].fBlockSizeZ - (closeVertex * g_MeshTube[i].fBlockSizeZ);
					//}

					// テクスチャ座標の調整
					//（円柱のテクスチャ座標を途切れさせないため、X軸ブロック数の半分に到達したら座標の値を折り返す。）
					if (nCntVtxX > halfNumBlockX)
					{
						pVtx[nCntVtxY * (g_MeshTube[i].nNumBlockX + 1) + nCntVtxX].TexCoord.x = halfNumBlockX + (halfNumBlockX - nCntVtxX);
					}
				}
			}

			GetDeviceContext()->Unmap(g_MeshTube[i].vertexBuffer, 0);
		}

		{//インデックスバッファの中身を埋める
			// インデックスバッファのポインタを取得
			D3D11_MAPPED_SUBRESOURCE msr;
			GetDeviceContext()->Map(g_MeshTube[i].indexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

			unsigned short *pIdx = (unsigned short*)msr.pData;

			int nCntIdx = 0;
			for (int nCntVtxY = 0; nCntVtxY < g_MeshTube[i].nNumBlockZ; nCntVtxY++)
			{
				if (nCntVtxY > 0)
				{// 縮退ポリゴンのためのダブりの設定
					pIdx[nCntIdx] = (nCntVtxY + 1) * (g_MeshTube[i].nNumBlockX + 1);
					nCntIdx++;
				}

				for (int nCntVtxX = 0; nCntVtxX < (g_MeshTube[i].nNumBlockX + 1); nCntVtxX++)
				{
					pIdx[nCntIdx] = (nCntVtxY + 1) * (g_MeshTube[i].nNumBlockX + 1) + nCntVtxX;
					nCntIdx++;
					pIdx[nCntIdx] = nCntVtxY * (g_MeshTube[i].nNumBlockX + 1) + nCntVtxX;
					nCntIdx++;
				}

				if (nCntVtxY < (g_MeshTube[i].nNumBlockZ - 1))
				{// 縮退ポリゴンのためのダブりの設定
					pIdx[nCntIdx] = nCntVtxY * (g_MeshTube[i].nNumBlockX + 1) + g_MeshTube[i].nNumBlockX;
					nCntIdx++;
				}
			}

			GetDeviceContext()->Unmap(g_MeshTube[i].indexBuffer, 0);
		}
	}

	g_MeshTube[g_backTube].pos.z = g_MeshTube[0].fBlockSizeZ * g_MeshTube[0].nNumBlockZ;

	g_time = 0.0f;

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitTube(void)
{
	if (g_Load == FALSE) return;

	for (int i = 0; i < TUBE_NUM; i++)
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
	for (int i = 0; i < TUBE_NUM; i++)
	{
		g_MeshTube[i].pos.z -= 40.0f;
		if (g_MeshTube[i].pos.z < -g_MeshTube[i].fBlockSizeZ * g_MeshTube[i].nNumBlockZ)
		{
			g_MeshTube[i].pos.z = g_MeshTube[i].fBlockSizeZ * g_MeshTube[i].nNumBlockZ;
		}
	}
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawTube(void)
{
	SetLightEnable(FALSE);
	for (int i = 0; i < TUBE_NUM; i++)
	{
		// 頂点バッファ設定
		UINT stride = sizeof(VERTEX_3D);
		UINT offset = 0;
		GetDeviceContext()->IASetVertexBuffers(0, 1, &g_MeshTube[i].vertexBuffer, &stride, &offset);

		// インデックスバッファ設定
		GetDeviceContext()->IASetIndexBuffer(g_MeshTube[i].indexBuffer, DXGI_FORMAT_R16_UINT, 0);

		// プリミティブトポロジ設定
		GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

		// マテリアル設定
		SetMaterial(g_MeshTube[i].material);

		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[TEXTURE_NOMAL]);


		XMMATRIX mtxRot, mtxTranslate, mtxWorld;

		// ワールドマトリックスの初期化
		mtxWorld = XMMatrixIdentity();

		// 回転を反映
		mtxRot = XMMatrixRotationRollPitchYaw(g_MeshTube[i].rot.x, g_MeshTube[i].rot.y, g_MeshTube[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// 移動を反映
		mtxTranslate = XMMatrixTranslation(g_MeshTube[i].pos.x, g_MeshTube[i].pos.y, g_MeshTube[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ワールドマトリックスの設定
		SetWorldMatrix(&mtxWorld);

		// ポリゴンの描画
		GetDeviceContext()->DrawIndexed(g_MeshTube[i].nNumVertexIndex, 0, 0);
	}
	SetLightEnable(TRUE);
}

void TubeRotation(float rot)
{
	for (int i = 0; i < TUBE_NUM; i++)
	{
		g_MeshTube[i].rot.z += rot;
	}
}