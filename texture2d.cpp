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
#include "load.h"

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer*				g_VertexBuffer = NULL;		// 頂点情報
static BOOL							g_Load = FALSE;

static INSTANCE						g_Instance;
static int							g_InstenceCount = 0;
//static ID3D11ShaderResourceView**	g_pTexture[INSTANCE_MAX];	// テクスチャ情報
static TEXTURE_LABEL	g_pTexture[INSTANCE_MAX];	// テクスチャ情報
static ID3D11ShaderResourceView*	g_Texture[TEXTURE_LABEL_MAX];
//#define NO_INSTANCING	// インスタンシングしないバージョン


static char* TextureName[TEXTURE_LABEL_MAX] = {

	"data/TEXTURE/white.png",
	"data/TEXTURE/peanuts_bg_1.png",

	"data/TEXTURE/load/nowloading.png",
	"data/TEXTURE/load/nowloading2.png",
	"data/TEXTURE/load/frog_jump_animation.png",
	"data/TEXTURE/load/empty_bar.png",
	"data/TEXTURE/load/full_bar.png",
	"data/TEXTURE/load/loadbg.png",

	"data/TEXTURE/game_UI/countdown_3_3.png",
	"data/TEXTURE/game_UI/countdown_2_3.png",
	"data/TEXTURE/game_UI/countdown_1_3.png",
	"data/TEXTURE/game_UI/go_3.png",
	"data/TEXTURE/game_UI/goal_ui3.png",
	"data/TEXTURE/particle3.jpg",

	"data/TEXTURE/tube10.jpg",
	//"data/TEXTURE/tube21.jpg",
	"data/TEXTURE/background.png",
	//"data/TEXTURE/background2.png",

	"data/TEXTURE/game_UI/fuel_empty.png",
	"data/TEXTURE/game_UI/fuel_font.png",
	"data/TEXTURE/game_UI/fuel_full.png",
	"data/TEXTURE/game_UI/goalpin_UI.png",
	"data/TEXTURE/game_UI/icon_UI.png",
	"data/TEXTURE/game_UI/map_UI.png",
	"data/TEXTURE/game_UI/rocket_map.png",
	"data/TEXTURE/game_UI/speed_empty.png",
	"data/TEXTURE/game_UI/speed_font.png",
	"data/TEXTURE/game_UI/speed_gauge.png",
	"data/TEXTURE/game_UI/timer_backdrop.png",
	"data/TEXTURE/game_UI/timer_UI.png",
	"data/TEXTURE/game_UI/timer_semicolon.png",

	"data/TEXTURE/home_menu_gamen/character.png",
	"data/TEXTURE/home_menu_gamen/character01.png",
	"data/TEXTURE/home_menu_gamen/character02.png",
	"data/TEXTURE/home_menu_gamen/character03.png",
	"data/TEXTURE/home_menu_gamen/delivery_start_button_1.png",
	"data/TEXTURE/home_menu_gamen/delivery_start_button_2.png",
	"data/TEXTURE/home_menu_gamen/delivery_start_button_3.png",

	"data/TEXTURE/home_menu_gamen/customize_bg.png",
	"data/TEXTURE/home_menu_gamen/shop_menu.png",
	"data/TEXTURE/home_menu_gamen/statusbar_0.png",
	"data/TEXTURE/home_menu_gamen/statusbar_point.png",
	"data/TEXTURE/home_menu_gamen/rocket_icon_lock.png",
	"data/TEXTURE/home_menu_gamen/rocket_icon_new.png",
	"data/TEXTURE/home_menu_gamen/rocket_icon.png",
	"data/TEXTURE/home_menu_gamen/rocket_icon_equip.png",
	"data/TEXTURE/home_menu_gamen/rocket_1.png",
	"data/TEXTURE/home_menu_gamen/rocket_2.png",
	"data/TEXTURE/home_menu_gamen/rocket_3.png",
	"data/TEXTURE/home_menu_gamen/rocket_4.png",
	"data/TEXTURE/home_menu_gamen/rocket_select_1.png",
	"data/TEXTURE/home_menu_gamen/rocket_select_2.png",
	"data/TEXTURE/home_menu_gamen/rocket_select_3.png",
	"data/TEXTURE/home_menu_gamen/rocket_select_4.png",

	"data/TEXTURE/home_menu_gamen/player_wallet.png",
	"data/TEXTURE/home_menu_gamen/player_wallet_total.png",
	"data/TEXTURE/home_menu_gamen/player_wallet_dot.png",
	"data/TEXTURE/home_menu_gamen/player_wallet_comma.png",
	"data/TEXTURE/home_menu_gamen/yen_wallet.png",
	"data/TEXTURE/home_menu_gamen/minus_wallet_total.png",
	"data/TEXTURE/home_menu_gamen/minus_wallet_dot.png",
	"data/TEXTURE/home_menu_gamen/minus_wallet_comma.png",
	"data/TEXTURE/home_menu_gamen/minus_wallet.png",
	"data/TEXTURE/home_menu_gamen/shop_menu_opacity.png",
	"data/TEXTURE/home_menu_gamen/buy_menu.png",
	"data/TEXTURE/home_menu_gamen/buy_menu_selection.png",
	"data/TEXTURE/home_menu_gamen/no_select_ui.png",
	"data/TEXTURE/home_menu_gamen/buy_select_ui.png",

	"data/TEXTURE/result_gamen/result_banner.png",
	"data/TEXTURE/result_gamen/result_bg.png",
	"data/TEXTURE/result_gamen/no_star.png",
	"data/TEXTURE/result_gamen/green_star.png",
	"data/TEXTURE/result_gamen/expression_1.png",
	"data/TEXTURE/result_gamen/expression_2.png",
	"data/TEXTURE/result_gamen/expression_3.png",
	"data/TEXTURE/result_gamen/scrollbar.png",
	"data/TEXTURE/result_gamen/scroll.png",
	"data/TEXTURE/result_gamen/delivery_distance.png",
	"data/TEXTURE/result_gamen/delivery_time.png",
	"data/TEXTURE/result_gamen/delivery_fee.png",
	"data/TEXTURE/result_gamen/tip.png",
	"data/TEXTURE/result_gamen/damage.png",
	"data/TEXTURE/result_gamen/total_amount.png",
	"data/TEXTURE/result_gamen/yen.png",
	"data/TEXTURE/result_gamen/minus_yen.png",
	"data/TEXTURE/result_gamen/total_yen.png",
	"data/TEXTURE/result_gamen/result_number.png",
	"data/TEXTURE/result_gamen/minus_result_number.png",
	"data/TEXTURE/result_gamen/total_result_number.png",
	"data/TEXTURE/result_gamen/point.png",
	"data/TEXTURE/result_gamen/minus_point.png",
	"data/TEXTURE/result_gamen/total_point.png",
	"data/TEXTURE/result_gamen/firework1.png",
	"data/TEXTURE/result_gamen/firework2.png",
	"data/TEXTURE/result_gamen/firework3.png",
	"data/TEXTURE/result_gamen/firework4.png",
	"data/TEXTURE/result_gamen/firework5.png",

	//"data/TEXTURE/result_country/oosaka.jpg",
	//"data/TEXTURE/result_country/shanghai.jpg",
	//"data/TEXTURE/result_country/France.jpg",
	//"data/TEXTURE/result_country/America.jpg",
	"data/TEXTURE/result_country/Oosaka.jpg",
	"data/TEXTURE/result_country/Hokkaido.jpg",
	"data/TEXTURE/result_country/Okinawa.jpg",
	"data/TEXTURE/result_country/shanghai.jpg",
	"data/TEXTURE/result_country/Seoul.jpg",
	"data/TEXTURE/result_country/Singapore.jpg",
	"data/TEXTURE/result_country/France.jpg",
	"data/TEXTURE/result_country/London.jpg",
	"data/TEXTURE/result_country/Italy.jpg",
	"data/TEXTURE/result_country/america.jpg",
	"data/TEXTURE/result_country/Canada.jpg",
	"data/TEXTURE/result_country/Mexico.jpg",

	"data/MODEL/star1.jpg",

	"data/TEXTURE/title_menu_gamen/menu_board.png",
	"data/TEXTURE/title_menu_gamen/menu_line.png",
	"data/TEXTURE/title_menu_gamen/start_1.png",
	"data/TEXTURE/title_menu_gamen/start_2.png",
	"data/TEXTURE/title_menu_gamen/start_setsumei.png",
	"data/TEXTURE/title_menu_gamen/menupannel2_1.png",
	"data/TEXTURE/title_menu_gamen/option_1.png",
	"data/TEXTURE/title_menu_gamen/option_2.png",
	"data/TEXTURE/title_menu_gamen/option_setsumei.png",
	"data/TEXTURE/title_menu_gamen/menupannel2_2.png",
	"data/TEXTURE/title_menu_gamen/gallery_1.png",
	"data/TEXTURE/title_menu_gamen/gallery_2.png",
	"data/TEXTURE/title_menu_gamen/gallery_setsumei.png",
	"data/TEXTURE/title_menu_gamen/menupannel2_3.png",
	"data/TEXTURE/title_menu_gamen/credit_1.png",
	"data/TEXTURE/title_menu_gamen/credit_2.png",
	"data/TEXTURE/title_menu_gamen/credit_setsumei.png",
	"data/TEXTURE/title_menu_gamen/menupannel2_4.png",
	"data/TEXTURE/title_menu_gamen/quit_1.png",
	"data/TEXTURE/title_menu_gamen/quit_2.png",
	"data/TEXTURE/title_menu_gamen/quit_setsumei.png",
	"data/TEXTURE/title_menu_gamen/menupannel2_5.png",

	"data/TEXTURE/title_menu_gamen/title_logo.png",
	"data/TEXTURE/title_menu_gamen/press_button.png",
	"data/TEXTURE/title_menu_gamen/peanuts_copyright.png",
};

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
	
	// テクスチャ生成
	for (int i = 0; i < TEXTURE_LABEL_COUNTDOWN3; i++) {
		D3DX11CreateShaderResourceViewFromFile(GetDevice(), TextureName[i], NULL, NULL, &g_Texture[i], NULL);
	}

	//LoadTexture2D();
	//g_LoadPoint = TEXTURE_LABEL_COUNTDOWN3;

	//g_Load = TRUE;
	return S_OK;
}
HRESULT LoadTexture2D(void)
{
	// テクスチャ生成
	for (int i = TEXTURE_LABEL_COUNTDOWN3; i < TEXTURE_LABEL_MAX; i++) {
		D3DX11CreateShaderResourceViewFromFile(GetDevice(), TextureName[i], NULL, NULL, &g_Texture[i], NULL);
	}

	return S_OK;
}

BOOL LoadTexture(int loadpoint)
{
	g_Texture[loadpoint] = NULL;
	D3DX11CreateShaderResourceViewFromFile(GetDevice(), TextureName[loadpoint], NULL, NULL, &g_Texture[loadpoint], NULL);
	return TRUE;
}
BOOL LoadTextureKernel(void)
{
	static int loadpoint = TEXTURE_LABEL_COUNTDOWN3;
	if (loadpoint < TEXTURE_LABEL_MAX)
	{
		if (LoadTexture(loadpoint))
		{
			loadpoint++;
			AddLoadSum();
		}
		if (loadpoint == TEXTURE_LABEL_MAX)
		{
			g_Load = TRUE;
			return TRUE;
		}
	}
	return FALSE;
}

ID3D11ShaderResourceView** GetTexture(TEXTURE_LABEL texture) {
	return &g_Texture[texture];
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

	// テクスチャの解放
	for (int i = 0; i < TEXTURE_LABEL_MAX; i++)
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

	if (g_InstenceCount > 100) return;

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
	//ZeroMemory(&material, sizeof(material));
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
	if (g_InstenceCount <= 0) return;

	// デバイス取得
	ID3D11DeviceContext* device = GetDeviceContext();

	// インスタンス情報を登録
	D3D11_MAPPED_SUBRESOURCE msr;
	device->Map(GetInstanceBuffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
	*(INSTANCE*)msr.pData = g_Instance;
	device->Unmap(GetInstanceBuffer(), 0);
	
	// テクスチャ設定
	for (int i = 0; i < g_InstenceCount; i++) { device->PSSetShaderResources(i + 2, 1, GetTexture(g_pTexture[i])); }

	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	device->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// プリミティブトポロジ設定
	device->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// 2Dマトリクス設定
	SetViewBuffer(&XMMatrixIdentity());
	SetProjectionBuffer(&XMMatrixOrthographicOffCenterLH(0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f));
	SetWorldBuffer(&XMMatrixIdentity());

	//// マテリアル設定
	//MATERIAL material;
	//material.Diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
	//SetMaterialBuffer(&material);

	// インスタンシング描画設定
	SetShaderInstanceingOnlyTex(bInterrupt);
	
	// インスタンシング描画
	device->DrawInstanced(4, g_InstenceCount, 0, 0);

	// インスタンス数を更新
	g_InstenceCount = 0;

	// ビュー・プロジェクションマトリクスを元に戻す
	SetCamera();
}



