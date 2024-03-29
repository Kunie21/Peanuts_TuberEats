//=============================================================================
//
// パイプライン壁の処理 [tube.cpp]
// Author : 國江 翔太
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "texture2d.h"
#include "tube.h"
#include "input.h"
#include "player.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	TUBE_NUM			(2)	

//*****************************************************************************
// 構造体定義
//*****************************************************************************
struct MESH_TUBE
{
	MATERIAL		material;		// マテリアル
	XMFLOAT3		pos;			// 中心座標
	XMFLOAT3		rot;			// 回転
	//XMFLOAT3		scl;			// 拡大率
	int				nX, nZ;			// ブロック数
	int				nVertex;		// 総頂点数	
	int				nVertexIndex;	// 総インデックス数
	int				nPolygon;		// 総ポリゴン数
	float			sizeX, sizeZ;	// ブロックサイズ

	ID3D11Buffer	*vertexBuffer;	// 頂点バッファ
	ID3D11Buffer	*indexBuffer;	// インデックスバッファ
};

struct TUBE
{
	XMFLOAT3	pos;	// 中心座標
	XMFLOAT3	rot;	// 回転
};

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static MESH_TUBE g_MeshTube;
static MESH_TUBE g_MeshLight;
static TUBE g_Tube;

static BOOL		g_Load = FALSE;
static float	g_worldRot = 0.0f;

//static int	g_bg = 0;
//static int	g_lt = 0;
//static int	g_c1 = 0;
//static int	g_c2 = 1;
static COLOERPATTERN g_cp = { 0,0,ORANGE,SKYBLUE };

#define	BG_NUM	(7)
#define	BGLIGHT_NUM	(8)
#define	COLOR_NUM	(10)
static XMFLOAT4 g_col[COLOR_NUM] = {
	{ 1.0f, 0.5f, 0.25f, 1.0f },	// オレンジ
	{ 0.25f, 1.0f, 1.0f, 1.0f },	// スカイブルー
	{ 0.5f, 1.0f, 0.25f, 1.0f },	// 黄緑
	{ 0.25f, 1.0f, 0.25f, 1.0f },	// 緑
	{ 1.00f, 0.25f, 0.25f, 1.0f },	// 赤
	{ 0.25f, 0.25f, 1.0f, 1.0f },	// 青
	{ 1.00f, 1.0f, 0.25f, 1.0f },	// 黄色
	{ 1.0f, 0.25f, 1.0f, 1.0f },	// ピンク
	{ 1.0f, 1.0f, 1.0f, 1.0f },		// 白
	{ 0.0f, 0.0f, 0.0f, 1.0f },		// なし
};

//TUBE_TYPE testTubeArr[8];
static int testNo = 0;

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitTube(void)
{
	// 直線パイプ
	{
		g_MeshTube.material.Diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
		g_MeshTube.material.Specular = { 0.0f, 0.0f, 0.0f, 0.0f };

		// ポジション設定
		g_MeshTube.pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_MeshTube.rot = XMFLOAT3(0.0f, 0.0f, 0.0f);

		// ブロック数の設定
		g_MeshTube.nX = MESH_NUM_X2;
		g_MeshTube.nZ = MESH_NUM_Z;

		// 頂点数の設定
		g_MeshTube.nVertex = (MESH_NUM_X2 + 1) * (MESH_NUM_Z + 1);

		// インデックス数の設定
		g_MeshTube.nVertexIndex = (MESH_NUM_X2 + 1) * 2 * MESH_NUM_Z + (MESH_NUM_Z - 1) * 2;

		// ポリゴン数の設定
		g_MeshTube.nPolygon = MESH_NUM_X2 * MESH_NUM_Z * 2 + (MESH_NUM_Z - 1) * 4;

		// ブロックサイズの設定
		g_MeshTube.sizeX = MESH_SIZE_X;
		g_MeshTube.sizeZ = MESH_SIZE_Z;

		// 頂点バッファ生成
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DYNAMIC;
		bd.ByteWidth = sizeof(VERTEX_3D) * g_MeshTube.nVertex;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		GetDevice()->CreateBuffer(&bd, NULL, &g_MeshTube.vertexBuffer);

		// インデックスバッファ生成
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DYNAMIC;
		bd.ByteWidth = sizeof(unsigned short) * g_MeshTube.nVertexIndex;
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		GetDevice()->CreateBuffer(&bd, NULL, &g_MeshTube.indexBuffer);

		{//頂点バッファの中身を埋める
#if 1
			const float texSizeX = (1.0f / g_MeshTube.nX) * TUBE_TEX_X;
			const float texSizeZ = (1.0f / g_MeshTube.nZ) * TUBE_TEX_Z;
#else
			const float texSizeX = 1.0f;
			const float texSizeZ = 1.0f;
#endif

			// 頂点バッファへのポインタを取得
			D3D11_MAPPED_SUBRESOURCE msr;
			GetDeviceContext()->Map(g_MeshTube.vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

			VERTEX_3D* pVtx = (VERTEX_3D*)msr.pData;

			float drad = XM_2PI / g_MeshTube.nX;	// 単位角度
			float halfNumBlockX = (float)(g_MeshTube.nX / 2);	// X軸ブロック数の半分の値

			for (int nz = 0; nz < (g_MeshTube.nZ + 1); nz++)
			{
				for (int nx = 0; nx < (g_MeshTube.nX + 1); nx++)
				{
					// 頂点座標の設定
					pVtx[nz * (g_MeshTube.nX + 1) + nx].Position.x = TUBE_RADIUS * cosf(nx * drad + XM_PIDIV2);
					pVtx[nz * (g_MeshTube.nX + 1) + nx].Position.y = TUBE_RADIUS * sinf(nx * drad + XM_PIDIV2);
					pVtx[nz * (g_MeshTube.nX + 1) + nx].Position.z = 0.5f * g_MeshTube.nZ * g_MeshTube.sizeZ - nz * g_MeshTube.sizeZ;

					// 法線の設定
					pVtx[nz * (g_MeshTube.nX + 1) + nx].Normal = XMFLOAT3(cosf(nx * drad + XM_PI + XM_PIDIV2), sinf(nx * drad + XM_PI + XM_PIDIV2), 0.0f);

					// 拡散光の設定
					pVtx[nz * (g_MeshTube.nX + 1) + nx].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

					// テクスチャ座標の設定
					pVtx[nz * (g_MeshTube.nX + 1) + nx].TexCoord.x = texSizeX * nx;
					pVtx[nz * (g_MeshTube.nX + 1) + nx].TexCoord.y = texSizeZ * nz;

					// テクスチャ座標の調整
					//（円柱のテクスチャ座標を途切れさせないため、X軸ブロック数の半分に到達したら座標の値を折り返す。）
					if (nx > halfNumBlockX)
					{
						//pVtx[nz * (g_MeshTube.nX + 1) + nx].TexCoord.x = halfNumBlockX + (halfNumBlockX - nx);
					}
				}
			}

			GetDeviceContext()->Unmap(g_MeshTube.vertexBuffer, 0);
		}

		{//インデックスバッファの中身を埋める
			// インデックスバッファのポインタを取得
			D3D11_MAPPED_SUBRESOURCE msr;
			GetDeviceContext()->Map(g_MeshTube.indexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

			unsigned short *pIdx = (unsigned short*)msr.pData;

			int nCntIdx = 0;
			for (int nz = 0; nz < g_MeshTube.nZ; nz++)
			{
				if (nz > 0)
				{// 縮退ポリゴンのためのダブりの設定
					pIdx[nCntIdx] = (nz + 1) * (g_MeshTube.nX + 1);
					nCntIdx++;
				}

				for (int nx = 0; nx < (g_MeshTube.nX + 1); nx++)
				{
					pIdx[nCntIdx] = (nz + 1) * (g_MeshTube.nX + 1) + nx;
					nCntIdx++;
					pIdx[nCntIdx] = nz * (g_MeshTube.nX + 1) + nx;
					nCntIdx++;
				}

				if (nz < (g_MeshTube.nZ - 1))
				{// 縮退ポリゴンのためのダブりの設定
					pIdx[nCntIdx] = nz * (g_MeshTube.nX + 1) + g_MeshTube.nX;
					nCntIdx++;
				}
			}

			GetDeviceContext()->Unmap(g_MeshTube.indexBuffer, 0);
		}
	}

	// ライトパイプ
	{
		// マテリアル設定
		g_MeshLight.material.Diffuse = { 1.5f, 1.5f, 1.5f, 1.5f };

		// ポジション設定
		g_MeshLight.pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_MeshLight.rot = XMFLOAT3(0.0f, 0.0f, 0.0f);

		// ブロック数の設定
		g_MeshLight.nX = MESH_NUM_X2;
		g_MeshLight.nZ = MESH_NUM_Z;

		// 頂点数の設定
		g_MeshLight.nVertex = (MESH_NUM_X2 + 1) * (MESH_NUM_Z + 1);

		// インデックス数の設定
		g_MeshLight.nVertexIndex = (MESH_NUM_X2 + 1) * 2 * MESH_NUM_Z + (MESH_NUM_Z - 1) * 2;

		// ポリゴン数の設定
		g_MeshLight.nPolygon = MESH_NUM_X2 * MESH_NUM_Z * 2 + (MESH_NUM_Z - 1) * 4;

		// ブロックサイズの設定
		g_MeshLight.sizeX = MESH_SIZE_X;
		g_MeshLight.sizeZ = MESH_SIZE_Z;

		// 頂点バッファ生成
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DYNAMIC;
		bd.ByteWidth = sizeof(VERTEX_3D) * g_MeshLight.nVertex;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		GetDevice()->CreateBuffer(&bd, NULL, &g_MeshLight.vertexBuffer);

		// インデックスバッファ生成
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DYNAMIC;
		bd.ByteWidth = sizeof(unsigned short) * g_MeshLight.nVertexIndex;
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		GetDevice()->CreateBuffer(&bd, NULL, &g_MeshLight.indexBuffer);

		{//頂点バッファの中身を埋める
#if 1
			const float texSizeX = 1.0f / g_MeshLight.nX;
			const float texSizeZ = 1.0f / g_MeshLight.nZ;
#else
			const float texSizeX = 1.0f;
			const float texSizeZ = 1.0f;
#endif

			// 頂点バッファへのポインタを取得
			D3D11_MAPPED_SUBRESOURCE msr;
			GetDeviceContext()->Map(g_MeshLight.vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

			VERTEX_3D* pVtx = (VERTEX_3D*)msr.pData;

			float drad = -XM_2PI / g_MeshLight.nX;	// 単位角度
			float halfNumBlockX = (float)(g_MeshLight.nX / 2);	// X軸ブロック数の半分の値

			for (int nz = 0; nz < (g_MeshLight.nZ + 1); nz++)
			{
				for (int nx = 0; nx < (g_MeshLight.nX + 1); nx++)
				{
					// 頂点座標の設定
					pVtx[nz * (g_MeshLight.nX + 1) + nx].Position.x = TUBE_RADIUS * cosf(nx * drad + XM_PIDIV2);
					pVtx[nz * (g_MeshLight.nX + 1) + nx].Position.y = TUBE_RADIUS * sinf(nx * drad + XM_PIDIV2);
					pVtx[nz * (g_MeshLight.nX + 1) + nx].Position.z = 0.5f * g_MeshLight.nZ * g_MeshLight.sizeZ - nz * g_MeshLight.sizeZ;

					// 法線の設定
					pVtx[nz * (g_MeshLight.nX + 1) + nx].Normal = XMFLOAT3(cosf(nx * drad + XM_PI + XM_PIDIV2), sinf(nx * drad + XM_PI + XM_PIDIV2), 0.0f);

					// 拡散光の設定
					pVtx[nz * (g_MeshLight.nX + 1) + nx].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

					// テクスチャ座標の設定
					pVtx[nz * (g_MeshLight.nX + 1) + nx].TexCoord.x = texSizeX * nx;
					pVtx[nz * (g_MeshLight.nX + 1) + nx].TexCoord.y = texSizeZ * nz;

					// テクスチャ座標の調整
					//（円柱のテクスチャ座標を途切れさせないため、X軸ブロック数の半分に到達したら座標の値を折り返す。）
					if (nx > halfNumBlockX)
					{
						pVtx[nz * (g_MeshLight.nX + 1) + nx].TexCoord.x = halfNumBlockX + (halfNumBlockX - nx);
					}
				}
			}

			GetDeviceContext()->Unmap(g_MeshLight.vertexBuffer, 0);
		}

		{//インデックスバッファの中身を埋める
			// インデックスバッファのポインタを取得
			D3D11_MAPPED_SUBRESOURCE msr;
			GetDeviceContext()->Map(g_MeshLight.indexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

			unsigned short *pIdx = (unsigned short*)msr.pData;

			int nCntIdx = 0;
			for (int nz = 0; nz < g_MeshLight.nZ; nz++)
			{
				if (nz > 0)
				{// 縮退ポリゴンのためのダブりの設定
					pIdx[nCntIdx] = (nz + 1) * (g_MeshLight.nX + 1);
					nCntIdx++;
				}

				for (int nx = 0; nx < (g_MeshLight.nX + 1); nx++)
				{
					pIdx[nCntIdx] = (nz + 1) * (g_MeshLight.nX + 1) + nx;
					nCntIdx++;
					pIdx[nCntIdx] = nz * (g_MeshLight.nX + 1) + nx;
					nCntIdx++;
				}

				if (nz < (g_MeshLight.nZ - 1))
				{// 縮退ポリゴンのためのダブりの設定
					pIdx[nCntIdx] = nz * (g_MeshLight.nX + 1) + g_MeshLight.nX;
					nCntIdx++;
				}
			}

			GetDeviceContext()->Unmap(g_MeshLight.indexBuffer, 0);
		}
	}

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitTube(void)
{
	if (g_Load == FALSE) return;

	// 頂点バッファの解放
	if (g_MeshTube.vertexBuffer)
	{
		g_MeshTube.vertexBuffer->Release();
		g_MeshTube.vertexBuffer = NULL;
	}

	// インデックスバッファの解放
	if (g_MeshTube.indexBuffer)
	{
		g_MeshTube.indexBuffer->Release();
		g_MeshTube.indexBuffer = NULL;
	}

	g_Load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateTube(void)
{
#ifdef _DEBUG
	if (GetKeyboardRepeat(DIK_Z)) { g_cp.bg = (g_cp.bg + 1) % BG_NUM; }
	if (GetKeyboardRepeat(DIK_X)) { g_cp.lt = (g_cp.lt + 1) % BGLIGHT_NUM; }
	if (GetKeyboardRepeat(DIK_C)) { g_cp.c1 = (g_cp.c1 + 1) % COLOR_NUM; }
	if (GetKeyboardRepeat(DIK_V)) { g_cp.c2 = (g_cp.c2 + 1) % COLOR_NUM; }
#endif
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawTube(void)
{
	SetStageCurvePlayer();
	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_MeshTube.vertexBuffer, &stride, &offset);

	// インデックスバッファ設定
	GetDeviceContext()->IASetIndexBuffer(g_MeshTube.indexBuffer, DXGI_FORMAT_R16_UINT, 0);

	// プリミティブトポロジ設定
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// マテリアル設定
	SetMaterialBuffer(&g_MeshTube.material);

	// テクスチャ設定
	GetDeviceContext()->PSSetShaderResources(0, 1, GetTexture((TEXTURE_LABEL)(TEXTURE_LABEL_TUBE_BG01 + g_cp.bg)));

	// ワールドマトリックスの設定
	SetWorldBuffer(&XMMatrixIdentity());

	// ポリゴンの描画
	GetDeviceContext()->DrawIndexed(g_MeshTube.nVertexIndex, 0, 0);
}

void DrawTubeResult(float pos_z)
{
	SetStageCurvePlayer();
	MATERIAL g_MeshTubeResult;
	g_MeshTubeResult.Diffuse.w = 0.6f;
	g_MeshTubeResult.Specular = { 0.0f, 0.0f, 0.0f, 0.0f };

	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_MeshTube.vertexBuffer, &stride, &offset);

	// インデックスバッファ設定
	GetDeviceContext()->IASetIndexBuffer(g_MeshTube.indexBuffer, DXGI_FORMAT_R16_UINT, 0);

	// プリミティブトポロジ設定
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// マテリアル設定
	SetMaterialBuffer(&g_MeshTubeResult);

	// テクスチャ設定
	GetDeviceContext()->PSSetShaderResources(0, 1, GetTexture((TEXTURE_LABEL)(TEXTURE_LABEL_TUBE_BG01 + g_cp.bg)));

	// ワールドマトリックスの設定
	XMMATRIX mtxWorld = XMMatrixIdentity();
	MulMtxPos(mtxWorld, 0.0f, 0.0f, pos_z);	// 移動を反映
	SetWorldBuffer(&mtxWorld);

	// ポリゴンの描画
	GetDeviceContext()->DrawIndexed(g_MeshTube.nVertexIndex, 0, 0);
}


void DrawTubeLight2(void)
{
	SetStageCurvePlayer();
	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_MeshTube.vertexBuffer, &stride, &offset);

	// インデックスバッファ設定
	GetDeviceContext()->IASetIndexBuffer(g_MeshTube.indexBuffer, DXGI_FORMAT_R16_UINT, 0);

	// プリミティブトポロジ設定
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// テクスチャ設定
	GetDeviceContext()->PSSetShaderResources(0, 1, GetTexture((TEXTURE_LABEL)(TEXTURE_LABEL_TUBE_LIGHT01 + g_cp.lt)));

	// マテリアル設定
	//SetMaterialBuffer(&g_MeshLight.material);
	MATERIAL material; 
	material.Diffuse = g_col[g_cp.c1];
	//material.Diffuse = { 1.0f, 0.5f, 0.25f, 1.0f };
	//material.Diffuse = { 2.0f, 1.0f, 0.5f, 1.0f };
	SetMaterialBuffer(&material);

	//XMMATRIX mtxWorld = XMMatrixIdentity();			// ワールドマトリックスの初期化
	//MulMtxScl(mtxWorld, 0.05f, 0.05f, 1.0f);		// スケールを反映
	//MulMtxPos(mtxWorld, 0.0f, TUBE_RADIUS, 0.0f);	// 移動を反映
	//SetWorldBuffer(&mtxWorld);						// ワールドマトリックスの設定

	XMMATRIX mtxWorld = XMMatrixIdentity();			// ワールドマトリックスの初期化
	MulMtxScl(mtxWorld, 0.90f, 0.90f, 1.0f);		// スケールを反映
	SetWorldBuffer(&mtxWorld);						// ワールドマトリックスの設定

	// ポリゴンの描画
	GetDeviceContext()->DrawIndexed(g_MeshTube.nVertexIndex, 0, 0);

	material.Diffuse = g_col[g_cp.c2];
	//aterial.Diffuse = { 0.25f, 1.0f, 1.0f, 1.0f };
	//material.Diffuse = { 0.5f, 2.0f, 2.0f, 1.0f };
	SetMaterialBuffer(&material);

	mtxWorld = XMMatrixIdentity();			// ワールドマトリックスの初期化
	MulMtxScl(mtxWorld, 0.8f, 0.8f, 1.0f);		// スケールを反映
	MulMtxRot(mtxWorld, 0.0f, 0.0f, XM_PIDIV2);	// 移動を反映
	SetWorldBuffer(&mtxWorld);						// ワールドマトリックスの設定

	// ポリゴンの描画
	GetDeviceContext()->DrawIndexed(g_MeshTube.nVertexIndex, 0, 0);
	SetMaterialBuffer(GetDefaultMaterial());
}
void DrawTubeLight(void)
{
	SetStageCurvePlayer();
	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_MeshLight.vertexBuffer, &stride, &offset);

	// インデックスバッファ設定
	GetDeviceContext()->IASetIndexBuffer(g_MeshLight.indexBuffer, DXGI_FORMAT_R16_UINT, 0);

	// プリミティブトポロジ設定
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// テクスチャ設定
	GetDeviceContext()->PSSetShaderResources(0, 1, GetTexture(TEXTURE_LABEL_WHITE));

	// マテリアル設定
	SetMaterialBuffer(&g_MeshLight.material);

	XMMATRIX mtxWorld = XMMatrixIdentity();			// ワールドマトリックスの初期化
	MulMtxScl(mtxWorld, 0.05f, 0.05f, 1.0f);		// スケールを反映
	MulMtxPos(mtxWorld, 0.0f, TUBE_RADIUS, 0.0f);	// 移動を反映
	SetWorldBuffer(&mtxWorld);						// ワールドマトリックスの設定

	// ポリゴンの描画
	GetDeviceContext()->DrawIndexed(g_MeshLight.nVertexIndex, 0, 0);

}

void RotateTube(float rot)
{
	g_worldRot += rot;
}

float GetTubeRotation(void)
{
	return g_MeshTube.rot.z;
}

void SetTubePattern(COLOERPATTERN cp)
{
	g_cp = cp;
}