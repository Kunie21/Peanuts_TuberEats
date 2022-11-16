//=============================================================================
//
// 2D画像描画処理 [2Dtexture.cpp]
// Author : 國江 翔太
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "texture2d.h"
#include "camera.h"

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer*				g_VertexBuffer = NULL;		// 頂点情報
static BOOL							g_Load = FALSE;

static INSTANCE						g_Instance;
static int							g_InstenceCount = 0;
static ID3D11ShaderResourceView**	g_pTexture[INSTANCE_MAX];	// テクスチャ情報

//#define NO_INSTANCING	// インスタンシングしないバージョン

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitTexture2D(void)
{
	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);

	//頂点バッファの中身を埋める
	D3D11_MAPPED_SUBRESOURCE msr;
	GetDeviceContext()->Map(g_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

	VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

	// 頂点座標の設定
	vertex[0].Position = XMFLOAT3(-0.5f, -0.5f, 0.0f);
	vertex[1].Position = XMFLOAT3(0.5f, -0.5f, 0.0f);
	vertex[2].Position = XMFLOAT3(-0.5f, 0.5f, 0.0f);
	vertex[3].Position = XMFLOAT3(0.5f, 0.5f, 0.0f);

	// 法線の設定
	vertex[0].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
	vertex[1].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
	vertex[2].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
	vertex[3].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);

	// 反射光の設定
	vertex[0].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[1].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[2].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[3].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	// テクスチャ座標の設定
	vertex[0].TexCoord = { 0.0f, 0.0f };
	vertex[1].TexCoord = { 1.0f, 0.0f };
	vertex[2].TexCoord = { 0.0f, 1.0f };
	vertex[3].TexCoord = { 1.0f, 1.0f };

	GetDeviceContext()->Unmap(g_VertexBuffer, 0);

	g_Load = TRUE;
	return S_OK;
}

void SetUVTexture2D(UV_POSITION* uv)
{
	D3D11_MAPPED_SUBRESOURCE msr;
	GetDeviceContext()->Map(g_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

	VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

	// 頂点座標の設定
	vertex[0].Position = XMFLOAT3(-0.5f, -0.5f, 0.0f);
	vertex[1].Position = XMFLOAT3(0.5f, -0.5f, 0.0f);
	vertex[2].Position = XMFLOAT3(-0.5f, 0.5f, 0.0f);
	vertex[3].Position = XMFLOAT3(0.5f, 0.5f, 0.0f);

	// 法線の設定
	vertex[0].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
	vertex[1].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
	vertex[2].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
	vertex[3].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);

	// 反射光の設定
	vertex[0].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[1].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[2].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[3].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	// テクスチャ座標の設定
	vertex[0].TexCoord = { uv->u, uv->v };
	vertex[1].TexCoord = { uv->u + uv->uw, uv->v };
	vertex[2].TexCoord = { uv->u, uv->v + uv->vh };
	vertex[3].TexCoord = { uv->u + uv->uw, uv->v + uv->vh };

	GetDeviceContext()->Unmap(g_VertexBuffer, 0);
}
void ResetUVTexture2D(void)
{
	D3D11_MAPPED_SUBRESOURCE msr;
	GetDeviceContext()->Map(g_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

	VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

	// 頂点座標の設定
	vertex[0].Position = XMFLOAT3(-0.5f, -0.5f, 0.0f);
	vertex[1].Position = XMFLOAT3(0.5f, -0.5f, 0.0f);
	vertex[2].Position = XMFLOAT3(-0.5f, 0.5f, 0.0f);
	vertex[3].Position = XMFLOAT3(0.5f, 0.5f, 0.0f);

	// 法線の設定
	vertex[0].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
	vertex[1].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
	vertex[2].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
	vertex[3].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);

	// 反射光の設定
	vertex[0].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[1].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[2].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[3].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	// テクスチャ座標の設定
	vertex[0].TexCoord = { 0.0f, 0.0f };
	vertex[1].TexCoord = { 1.0f, 0.0f };
	vertex[2].TexCoord = { 0.0f, 1.0f };
	vertex[3].TexCoord = { 1.0f, 1.0f };

	GetDeviceContext()->Unmap(g_VertexBuffer, 0);
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitTexture2D(void)
{
	if (g_Load == FALSE) return;

	if (g_VertexBuffer)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	g_Load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateTexture2D(void)
{

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawTexture2D(TEXTURE2D_DESC* td, BOOL bShadow, BOOL bUV)
{
#ifdef NO_INSTANCING
	// UV座標の再設定
	if (bUV) { SetUVTexture2D(&td->uv_pos); };
#endif

	// 位置の計算
	XMFLOAT2 pos = td->pos;
	if (td->posType == POSITION_RELATIVE)
	{
		pos.x += SCREEN_CENTER_X;
		pos.y += SCREEN_CENTER_Y;
	}
	if (td->ctrType == CENTER_LEFTTOP)
	{
		pos.x += td->size.x * 0.5f;
		pos.y += td->size.y * 0.5f;
	}

	// マテリアル設定
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = td->col;
	// 影が設定されている場合
	if (bShadow)
	{
		pos.x += td->sd_pos.x;
		pos.y += td->sd_pos.y;
		material.Diffuse.x *= td->sd_col.x;
		material.Diffuse.y *= td->sd_col.y;
		material.Diffuse.z *= td->sd_col.z;
		material.Diffuse.w *= td->sd_col.w;
	}

#ifdef NO_INSTANCING
	SetMaterialBuffer(&material);

	// テクスチャ設定
	GetDeviceContext()->PSSetShaderResources(0, 1, td->tex);

	SetDepthEnable(FALSE);

	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// プリミティブトポロジ設定
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// 2Dマトリクス設定
	SetViewBuffer(&XMMatrixIdentity());
	SetProjectionBuffer(&XMMatrixOrthographicOffCenterLH(0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f));


	// ワールドマトリクスの初期化
	XMMATRIX mtxWorld = XMMatrixIdentity();

	// ①S拡大率の反映
	mtxWorld = XMMatrixMultiply(mtxWorld, XMMatrixScaling(
		td->size.x * td->scl.x,
		td->size.y * td->scl.y,
		1.0f
	));

	// ②R回転の反映
	mtxWorld = XMMatrixMultiply(mtxWorld, XMMatrixRotationRollPitchYaw(
		0.0f,
		0.0f,
		td->rot
	));

	// ③T位置の反映
	mtxWorld = XMMatrixMultiply(mtxWorld, XMMatrixTranslation(
		pos.x,
		pos.y,
		0.0f
	));

	// ワールドマトリクスをセット
	SetWorldBuffer(&mtxWorld);

	// ポリゴンの描画
	GetDeviceContext()->Draw(4, 0);

	SetDepthEnable(TRUE);

#else
	// テクスチャ設定
	g_pTexture[g_InstenceCount] = td->tex;

	// インスタンス情報を登録
	g_Instance.scl[g_InstenceCount] = { td->size.x * td->scl.x, td->size.y * td->scl.y, 1.0f , 0.0f };
	g_Instance.rot[g_InstenceCount] = { 0.0f, 0.0f, td->rot, 0.0f };
	g_Instance.pos[g_InstenceCount] = { pos.x, pos.y, 0.0f, 0.0f };
	g_Instance.col[g_InstenceCount] = material.Diffuse;
	g_Instance.txc[g_InstenceCount] = { td->uv_pos.u, td->uv_pos.v, td->uv_pos.uw, td->uv_pos.vh };

	// インスタンス数を更新
	g_InstenceCount++;
#endif

	// 影が設定されている場合
	if (bShadow) { DrawTexture2D(td); }	// 本体を描画する

#ifdef NO_INSTANCING
	// UV座標のリセット
	if (bUV) { ResetUVTexture2D(); };
#endif
}


void DrawTexture2DAll(BOOL bInterrupt)
{
	// インスタンス情報を登録
	D3D11_MAPPED_SUBRESOURCE msr;
	GetDeviceContext()->Map(GetInstanceBuffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
	*(INSTANCE*)msr.pData = g_Instance;
	GetDeviceContext()->Unmap(GetInstanceBuffer(), 0);
	
	// テクスチャ設定
	for (int i = 0; i < g_InstenceCount; i++) { GetDeviceContext()->PSSetShaderResources(i + 1, 1, g_pTexture[i]); }

	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// プリミティブトポロジ設定
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// 2Dマトリクス設定
	SetViewBuffer(&XMMatrixIdentity());
	SetProjectionBuffer(&XMMatrixOrthographicOffCenterLH(0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f));
	SetWorldBuffer(&XMMatrixIdentity());

	// インスタンシング描画設定
	SetShaderInstanceingOnlyTex(bInterrupt);
	
	// インスタンシング描画
	GetDeviceContext()->DrawInstanced(4, g_InstenceCount, 0, 0);

	// インスタンス数を更新
	g_InstenceCount = 0;

	// ビュー・プロジェクションマトリクスを元に戻す
	SetCamera();
}



