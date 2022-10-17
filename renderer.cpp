//=============================================================================
//
// レンダリング処理 [renderer.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "renderer.h"

//デバッグ用画面テキスト出力を有効にする
#define DEBUG_DISP_TEXTOUT
//シェーダーデバッグ設定を有効にする
//#define DEBUG_SHADER

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TOON_TEXTURE_MAX			(2)						// テクスチャの数


//*********************************************************
// 構造体
//*********************************************************

// マテリアル用定数バッファ構造体
struct MATERIAL_CBUFFER
{
	XMFLOAT4	Ambient;
	XMFLOAT4	Diffuse;
	XMFLOAT4	Specular;
	XMFLOAT4	Emission;
	float		Shininess;
	int			noTexSampling;
	float		Dummy[2];				// 16byte境界用
};

// ライト用フラグ構造体
struct LIGHTFLAGS
{
	int			Type;		//ライトタイプ（enum LIGHT_TYPE）
	int         OnOff;		//ライトのオンorオフスイッチ
	int			Dummy[2];
};

// ライト用定数バッファ構造体
struct LIGHT_CBUFFER
{
	XMFLOAT4	Direction[LIGHT_MAX];	// ライトの方向
	XMFLOAT4	Position[LIGHT_MAX];	// ライトの位置
	XMFLOAT4	Diffuse[LIGHT_MAX];		// 拡散光の色
	XMFLOAT4	Ambient[LIGHT_MAX];		// 環境光の色
	XMFLOAT4	Attenuation[LIGHT_MAX];	// 減衰率
	LIGHTFLAGS	Flags[LIGHT_MAX];		// ライト種別
	int			Enable;					// ライティング有効・無効フラグ
	int			Dummy[3];				// 16byte境界用
};

// フォグ用定数バッファ構造体
struct FOG_CBUFFER
{
	XMFLOAT4	Fog;					// フォグ量
	XMFLOAT4	FogColor;				// フォグの色
	int			Enable;					// フォグ有効・無効フラグ
	float		Dummy[3];				// 16byte境界用
};

// 縁取り用バッファ
struct FUCHI
{
	int			fuchi;
	int			fill[3];
};


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
static void SetLightBuffer(void);


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static D3D_FEATURE_LEVEL       g_FeatureLevel = D3D_FEATURE_LEVEL_11_0;

static ID3D11Device*           g_D3DDevice = NULL;
static ID3D11DeviceContext*    g_ImmediateContext = NULL;
static IDXGISwapChain*         g_SwapChain = NULL;
static ID3D11RenderTargetView* g_RenderTargetView = NULL;
static ID3D11DepthStencilView* g_DepthStencilView = NULL;

static ID3D11VertexShader*		g_VertexShader = NULL;
static ID3D11PixelShader*		g_PixelShader = NULL;
static ID3D11InputLayout*		g_VertexLayout = NULL;
static ID3D11Buffer*			g_WVPMatrixBuffer = NULL;
//static ID3D11Buffer*			g_WorldBuffer = NULL;
//static ID3D11Buffer*			g_ViewBuffer = NULL;
//static ID3D11Buffer*			g_ProjectionBuffer = NULL;
static ID3D11Buffer*			g_MaterialBuffer = NULL;
static ID3D11Buffer*			g_LightBuffer = NULL;
static ID3D11Buffer*			g_FogBuffer = NULL;
static ID3D11Buffer*			g_FuchiBuffer = NULL;
static ID3D11Buffer*			g_CameraBuffer = NULL;
static ID3D11Buffer*			g_FuchiColorBuffer = NULL;

static ID3D11DepthStencilState* g_DepthStateEnable;
static ID3D11DepthStencilState* g_DepthStateDisable;

static ID3D11BlendState*		g_BlendStateNone;
static ID3D11BlendState*		g_BlendStateAlphaBlend;
static ID3D11BlendState*		g_BlendStateAdd;
static ID3D11BlendState*		g_BlendStateSubtract;
static BLEND_MODE				g_BlendStateParam;


static ID3D11RasterizerState*	g_RasterStateCullOff;
static ID3D11RasterizerState*	g_RasterStateCullCW;
static ID3D11RasterizerState*	g_RasterStateCullCCW;

// インスタンシング描画用
static ID3D11VertexShader*		g_VertexShaderInstancingBillboard = NULL;
static ID3D11PixelShader*		g_PixelShaderNoLighting = NULL;
static ID3D11Buffer*			g_InstanceBuffer = NULL;
static ID3D11Buffer*			g_BillboardBuffer = NULL;

// シャドウマップ関連 
static ID3D11PixelShader*		g_PixelShaderNonShadow = NULL;

static ID3D11RenderTargetView*	g_RenderTargetViewShadowMap = NULL;
static ID3D11Texture2D*			g_TextureShadowMap = NULL;
static ID3D11ShaderResourceView* g_ShaderResourceViewSM = NULL;
static ID3D11VertexShader*		g_VertexShaderShadowMap = NULL;
static ID3D11PixelShader*		g_PixelShaderShadowMap = NULL;
static ID3D11InputLayout*		g_VertexLayoutShadowMap = NULL;
static ID3D11Buffer*			g_LViewBuffer = NULL;
static ID3D11Buffer*			g_LProjectionBuffer = NULL;

// ドメインワープ関連
static ID3D11Buffer*			g_TimeBuffer = NULL;
static TIME				g_Time;
static ID3D11PixelShader*		g_PixelShaderWaterfall = NULL;
static ID3D11PixelShader*		g_PixelShaderFireStage = NULL;
static ID3D11PixelShader*		g_PixelShaderDarkStage = NULL;
static ID3D11PixelShader*		g_PixelShaderIceStage = NULL;
static ID3D11PixelShader*		g_PixelShaderSandStage = NULL;


// トゥーンレンダリング関連
static ID3D11PixelShader*		g_PixelShaderToon = NULL;
static ID3D11PixelShader*		g_PixelShaderHoldToon = NULL;
static ID3D11ShaderResourceView* g_ShaderResourceViewToonMap[TOON_TEXTURE_MAX] = { NULL };
static char* g_TextureName[TOON_TEXTURE_MAX] = {
	"data/TEXTURE/toontable.png",
	"data/TEXTURE/sand001.png",
};

static WVP_MATRIX		g_WVPMatrix;
static MATERIAL_CBUFFER	g_Material;
static LIGHT_CBUFFER	g_Light;
static FOG_CBUFFER		g_Fog;

static FUCHI			g_Fuchi;

static XMFLOAT4			g_FuchiColor;

ID3D11Device* GetDevice( void )
{
	return g_D3DDevice;
}


ID3D11DeviceContext* GetDeviceContext( void )
{
	return g_ImmediateContext;
}


void SetDepthEnable( BOOL Enable )
{
	if( Enable )
		g_ImmediateContext->OMSetDepthStencilState( g_DepthStateEnable, NULL );
	else
		g_ImmediateContext->OMSetDepthStencilState( g_DepthStateDisable, NULL );

}


void SetBlendState(BLEND_MODE bm)
{
	g_BlendStateParam = bm;

	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

	switch (g_BlendStateParam)
	{
	case BLEND_MODE_NONE:
		g_ImmediateContext->OMSetBlendState(g_BlendStateNone, blendFactor, 0xffffffff);
		break;
	case BLEND_MODE_ALPHABLEND:
		g_ImmediateContext->OMSetBlendState(g_BlendStateAlphaBlend, blendFactor, 0xffffffff);
		break;
	case BLEND_MODE_ADD:
		g_ImmediateContext->OMSetBlendState(g_BlendStateAdd, blendFactor, 0xffffffff);
		break;
	case BLEND_MODE_SUBTRACT:
		g_ImmediateContext->OMSetBlendState(g_BlendStateSubtract, blendFactor, 0xffffffff);
		break;
	}
}

void SetCullingMode(CULL_MODE cm)
{
	switch (cm)
	{
	case CULL_MODE_NONE:
		g_ImmediateContext->RSSetState(g_RasterStateCullOff);
		break;
	case CULL_MODE_FRONT:
		g_ImmediateContext->RSSetState(g_RasterStateCullCW);
		break;
	case CULL_MODE_BACK:
		g_ImmediateContext->RSSetState(g_RasterStateCullCCW);
		break;
	}
}

void SetAlphaTestEnable(BOOL flag)
{
	D3D11_BLEND_DESC blendDesc;
	ZeroMemory(&blendDesc, sizeof(blendDesc));

	if (flag)
		blendDesc.AlphaToCoverageEnable = TRUE;
	else
		blendDesc.AlphaToCoverageEnable = FALSE;

	blendDesc.IndependentBlendEnable = FALSE;
	blendDesc.RenderTarget[0].BlendEnable = TRUE;

	switch (g_BlendStateParam)
	{
	case BLEND_MODE_NONE:
		blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
		blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		break;
	case BLEND_MODE_ALPHABLEND:
		blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		break;
	case BLEND_MODE_ADD:
		blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		break;
	case BLEND_MODE_SUBTRACT:
		blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_REV_SUBTRACT;
		blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		break;
	}

	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	ID3D11BlendState* blendState = NULL;
	g_D3DDevice->CreateBlendState(&blendDesc, &blendState);
	g_ImmediateContext->OMSetBlendState(blendState, blendFactor, 0xffffffff);

	if (blendState != NULL)
		blendState->Release();
}


//void SetWorldViewProjection2D( void )
//{
//	XMMATRIX world;
//	world = XMMatrixTranspose(XMMatrixIdentity());
//	GetDeviceContext()->UpdateSubresource(g_WorldBuffer, 0, NULL, &world, 0, 0);
//
//	XMMATRIX view;
//	view = XMMatrixTranspose(XMMatrixIdentity());
//	GetDeviceContext()->UpdateSubresource(g_ViewBuffer, 0, NULL, &view, 0, 0);
//
//	XMMATRIX worldViewProjection;
//	worldViewProjection = XMMatrixOrthographicOffCenterLH(0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f);
//	worldViewProjection = XMMatrixTranspose(worldViewProjection);
//	GetDeviceContext()->UpdateSubresource(g_ProjectionBuffer, 0, NULL, &worldViewProjection, 0, 0);
//}
void SetWorldViewProjection2D(void)
{
	WVP_MATRIX matrix;
	XMMATRIX worldViewProjection = XMMatrixOrthographicOffCenterLH(0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f);
	XMStoreFloat4x4(&matrix.WorldViewProjection, XMMatrixTranspose(worldViewProjection));
	//worldViewProjection = XMMatrixOrthographicOffCenterLH(0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f);
	//worldViewProjection = XMMatrixTranspose(worldViewProjection);
	GetDeviceContext()->UpdateSubresource(g_WVPMatrixBuffer, 0, NULL, &matrix, 0, 0);
}


//void SetWorldMatrix( XMMATRIX *WorldMatrix )
//{
//	XMMATRIX world;
//	world = *WorldMatrix;
//	world = XMMatrixTranspose(world);
//
//	GetDeviceContext()->UpdateSubresource(g_WorldBuffer, 0, NULL, &world, 0, 0);
//}
//
//void SetViewMatrix(XMMATRIX *ViewMatrix )
//{
//	XMMATRIX view;
//	view = *ViewMatrix;
//	view = XMMatrixTranspose(view);
//
//	GetDeviceContext()->UpdateSubresource(g_ViewBuffer, 0, NULL, &view, 0, 0);
//}
//
//void SetProjectionMatrix( XMMATRIX *ProjectionMatrix )
//{
//	XMMATRIX projection;
//	projection = *ProjectionMatrix;
//	projection = XMMatrixTranspose(projection);
//
//	GetDeviceContext()->UpdateSubresource(g_ProjectionBuffer, 0, NULL, &projection, 0, 0);
//}
void SetWorldMatrix(XMMATRIX *WorldMatrix)
{
	XMStoreFloat4x4(&g_WVPMatrix.World, *WorldMatrix);

	XMStoreFloat4x4(&g_WVPMatrix.ViewProjection, XMLoadFloat4x4(&g_WVPMatrix.View) * XMLoadFloat4x4(&g_WVPMatrix.Projection));
	XMStoreFloat4x4(&g_WVPMatrix.WorldViewProjection, XMLoadFloat4x4(&g_WVPMatrix.World) * XMLoadFloat4x4(&g_WVPMatrix.ViewProjection));

	WVP_MATRIX matrix;
	XMStoreFloat4x4(&matrix.World, XMMatrixTranspose(XMLoadFloat4x4(&g_WVPMatrix.World)));
	XMStoreFloat4x4(&matrix.View, XMMatrixTranspose(XMLoadFloat4x4(&g_WVPMatrix.View)));
	XMStoreFloat4x4(&matrix.Projection, XMMatrixTranspose(XMLoadFloat4x4(&g_WVPMatrix.Projection)));
	XMStoreFloat4x4(&matrix.World, XMMatrixTranspose(XMLoadFloat4x4(&g_WVPMatrix.World)));
	XMStoreFloat4x4(&matrix.ViewProjection, XMMatrixTranspose(XMLoadFloat4x4(&g_WVPMatrix.ViewProjection)));
	XMStoreFloat4x4(&matrix.WorldViewProjection, XMMatrixTranspose(XMLoadFloat4x4(&g_WVPMatrix.WorldViewProjection)));
	GetDeviceContext()->UpdateSubresource(g_WVPMatrixBuffer, 0, NULL, &matrix, 0, 0);

}
void SetViewMatrix(XMMATRIX *ViewMatrix)
{
	XMStoreFloat4x4(&g_WVPMatrix.View, *ViewMatrix);
}
void SetProjectionMatrix(XMMATRIX *ProjectionMatrix)
{
	XMStoreFloat4x4(&g_WVPMatrix.Projection, *ProjectionMatrix);
}

void SetLViewMatrix(XMMATRIX *ViewMatrix)														// ★
{
	XMMATRIX view;
	view = *ViewMatrix;
	view = XMMatrixTranspose(view);

	GetDeviceContext()->UpdateSubresource(g_LViewBuffer, 0, NULL, &view, 0, 0);
}

void SetLProjectionMatrix(XMMATRIX *ProjectionMatrix)											// ★
{
	XMMATRIX projection;
	projection = *ProjectionMatrix;
	projection = XMMatrixTranspose(projection);

	GetDeviceContext()->UpdateSubresource(g_LProjectionBuffer, 0, NULL, &projection, 0, 0);
}

void SetMaterial( MATERIAL material )
{
	g_Material.Diffuse = material.Diffuse;
	g_Material.Ambient = material.Ambient;
	g_Material.Specular = material.Specular;
	g_Material.Emission = material.Emission;
	g_Material.Shininess = material.Shininess;
	g_Material.noTexSampling = material.noTexSampling;

	GetDeviceContext()->UpdateSubresource( g_MaterialBuffer, 0, NULL, &g_Material, 0, 0 );
}

void SetLightBuffer(void)
{
	GetDeviceContext()->UpdateSubresource(g_LightBuffer, 0, NULL, &g_Light, 0, 0);
}

void SetLightEnable(BOOL flag)
{
	// フラグを更新する
	g_Light.Enable = flag;

	SetLightBuffer();
}

void SetLight(int index, LIGHT* pLight)
{
	g_Light.Position[index] = XMFLOAT4(pLight->Position.x, pLight->Position.y, pLight->Position.z, 0.0f);
	g_Light.Direction[index] = XMFLOAT4(pLight->Direction.x, pLight->Direction.y, pLight->Direction.z, 0.0f);
	g_Light.Diffuse[index] = pLight->Diffuse;
	g_Light.Ambient[index] = pLight->Ambient;
	g_Light.Flags[index].Type = pLight->Type;
	g_Light.Flags[index].OnOff = pLight->Enable;
	g_Light.Attenuation[index].x = pLight->Attenuation;

	SetLightBuffer();
}

void SetFogBuffer(void)
{
	GetDeviceContext()->UpdateSubresource(g_FogBuffer, 0, NULL, &g_Fog, 0, 0);
}

void SetFogEnable(BOOL flag)
{
	// フラグを更新する
	g_Fog.Enable = flag;

	SetFogBuffer();
}

void SetFog(FOG* pFog)
{
	g_Fog.Fog.x = pFog->FogStart;
	g_Fog.Fog.y = pFog->FogEnd;
	g_Fog.FogColor = pFog->FogColor;

	SetFogBuffer();
}

void SetFuchi(int flag)
{
	g_Fuchi.fuchi = flag;
	GetDeviceContext()->UpdateSubresource(g_FuchiBuffer, 0, NULL, &g_Fuchi, 0, 0);
}


void SetShaderCamera(XMFLOAT3 pos)
{
	XMFLOAT4 tmp = XMFLOAT4( pos.x, pos.y, pos.z, 0.0f );

	GetDeviceContext()->UpdateSubresource(g_CameraBuffer, 0, NULL, &tmp, 0, 0);
}

void SetShaderFuchiColor(XMFLOAT4 color)
{
	g_FuchiColor = color;

	GetDeviceContext()->UpdateSubresource(g_FuchiColorBuffer, 0, NULL, &g_FuchiColor, 0, 0);
}

void SetTime(int time)
{
	g_Time.Time = time;
	GetDeviceContext()->UpdateSubresource(g_TimeBuffer, 0, NULL, &g_Time, 0, 0);
}

ID3D11Buffer * GetInstanceBuffer(void)
{
	return g_InstanceBuffer;
}

void SetShaderInstanceingBillboard(XMFLOAT4X4 mtxView)
{
	XMMATRIX invView = XMMatrixIdentity();

	invView.r[0].m128_f32[0] = mtxView.m[0][0];
	invView.r[0].m128_f32[1] = mtxView.m[1][0];
	invView.r[0].m128_f32[2] = mtxView.m[2][0];

	invView.r[1].m128_f32[0] = mtxView.m[0][1];
	invView.r[1].m128_f32[1] = mtxView.m[1][1];
	invView.r[1].m128_f32[2] = mtxView.m[2][1];

	invView.r[2].m128_f32[0] = mtxView.m[0][2];
	invView.r[2].m128_f32[1] = mtxView.m[1][2];
	invView.r[2].m128_f32[2] = mtxView.m[2][2];

	invView = XMMatrixTranspose(invView);

	GetDeviceContext()->UpdateSubresource(g_BillboardBuffer, 0, NULL, &invView, 0, 0);
	
	g_ImmediateContext->VSSetShader(g_VertexShaderInstancingBillboard, NULL, 0);
	g_ImmediateContext->PSSetShader(g_PixelShaderNoLighting, NULL, 0);
	//g_ImmediateContext->PSSetShader(g_PixelShaderNonShadow, NULL, 0);
}

void SetShaderDefault(void)
{
	g_ImmediateContext->VSSetShader(g_VertexShader, NULL, 0);
	g_ImmediateContext->PSSetShader(g_PixelShader, NULL, 0);
}

XMFLOAT4 Float4(XMFLOAT3 f3)
{
	return { f3.x, f3.y, f3.z, 0.0f };
}

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitRenderer(HINSTANCE hInstance, HWND hWnd, BOOL bWindow)
{
	HRESULT hr = S_OK;

	// デバイス、スワップチェーン、コンテキスト生成
	DWORD deviceFlags = 0;
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory( &sd, sizeof( sd ) );
	sd.BufferCount = 1;
	sd.BufferDesc.Width = SCREEN_WIDTH;
	sd.BufferDesc.Height = SCREEN_HEIGHT;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = bWindow;

	//デバッグ文字出力用設定
#if defined(_DEBUG) && defined(DEBUG_DISP_TEXTOUT)
	sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_GDI_COMPATIBLE;
	deviceFlags = D3D11_CREATE_DEVICE_DEBUG | D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#endif

	hr = D3D11CreateDeviceAndSwapChain( NULL,
										D3D_DRIVER_TYPE_HARDWARE,
										NULL,
										deviceFlags,
										NULL,
										0,
										D3D11_SDK_VERSION,
										&sd,
										&g_SwapChain,
										&g_D3DDevice,
										&g_FeatureLevel,
										&g_ImmediateContext );
	if( FAILED( hr ) )
		return hr;

	//デバッグ文字出力用設定
#if defined(_DEBUG) && defined(DEBUG_DISP_TEXTOUT)
	hr = g_SwapChain->ResizeBuffers(0, SCREEN_WIDTH, SCREEN_HEIGHT, DXGI_FORMAT_UNKNOWN, DXGI_SWAP_CHAIN_FLAG_GDI_COMPATIBLE); // N.B. the GDI compatible flag
	if (FAILED(hr))
		return hr;
#endif

	// レンダーターゲットビュー生成、設定
	ID3D11Texture2D* pBackBuffer = NULL;
	g_SwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), ( LPVOID* )&pBackBuffer );
	g_D3DDevice->CreateRenderTargetView( pBackBuffer, NULL, &g_RenderTargetView );
	pBackBuffer->Release();



	//ステンシル用テクスチャー作成
	ID3D11Texture2D* depthTexture = NULL;
	D3D11_TEXTURE2D_DESC td;
	ZeroMemory( &td, sizeof(td) );
	td.Width			= sd.BufferDesc.Width;
	td.Height			= sd.BufferDesc.Height;
	td.MipLevels		= 1;
	td.ArraySize		= 1;
	td.Format			= DXGI_FORMAT_D24_UNORM_S8_UINT;
	td.SampleDesc		= sd.SampleDesc;
	td.Usage			= D3D11_USAGE_DEFAULT;
	td.BindFlags		= D3D11_BIND_DEPTH_STENCIL;
    td.CPUAccessFlags	= 0;
    td.MiscFlags		= 0;
	g_D3DDevice->CreateTexture2D( &td, NULL, &depthTexture );

	//ステンシルターゲット作成
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
	ZeroMemory( &dsvd, sizeof(dsvd) );
	dsvd.Format			= td.Format;
	dsvd.ViewDimension	= D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvd.Flags			= 0;
	g_D3DDevice->CreateDepthStencilView( depthTexture, &dsvd, &g_DepthStencilView );


	g_ImmediateContext->OMSetRenderTargets( 1, &g_RenderTargetView, g_DepthStencilView );





	//====================================================================================　★
	// シャドウマップ書き込み用テクスチャ
	D3D11_TEXTURE2D_DESC renderTextureDesc;
	renderTextureDesc.Width = sd.BufferDesc.Width;
	renderTextureDesc.Height = sd.BufferDesc.Height;
	renderTextureDesc.MipLevels = 1;
	renderTextureDesc.ArraySize = 1;
	renderTextureDesc.Format = DXGI_FORMAT_R32_FLOAT;
	renderTextureDesc.SampleDesc.Count = 1;
	renderTextureDesc.SampleDesc.Quality = 0;
	renderTextureDesc.Usage = D3D11_USAGE_DEFAULT;
	renderTextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	renderTextureDesc.CPUAccessFlags = 0;
	renderTextureDesc.MiscFlags = 0;
	g_D3DDevice->CreateTexture2D(&renderTextureDesc, NULL, &g_TextureShadowMap);

	// レンダーターゲットビュー生成、設定
	g_D3DDevice->CreateRenderTargetView(g_TextureShadowMap, NULL, &g_RenderTargetViewShadowMap);

	//シェーダーリソースビュー作成
	D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
	ZeroMemory(&srvd, sizeof(srvd));
	srvd.Format = DXGI_FORMAT_R32_FLOAT;
	srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvd.Texture2D.MipLevels = 1;
	g_D3DDevice->CreateShaderResourceView(g_TextureShadowMap, &srvd, &g_ShaderResourceViewSM);

	//=======================================================================================





	// ビューポート設定
	D3D11_VIEWPORT vp;
	vp.Width = (FLOAT)SCREEN_WIDTH;
	vp.Height = (FLOAT)SCREEN_HEIGHT;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	g_ImmediateContext->RSSetViewports( 1, &vp );



	// ラスタライザステート作成
	D3D11_RASTERIZER_DESC rd; 
	ZeroMemory( &rd, sizeof( rd ) );
	rd.FillMode = D3D11_FILL_SOLID;
	rd.CullMode = D3D11_CULL_NONE; 
	rd.DepthClipEnable = TRUE; 
	rd.MultisampleEnable = FALSE; 
	g_D3DDevice->CreateRasterizerState( &rd, &g_RasterStateCullOff);

	rd.CullMode = D3D11_CULL_FRONT;
	g_D3DDevice->CreateRasterizerState(&rd, &g_RasterStateCullCW);

	rd.CullMode = D3D11_CULL_BACK;
	g_D3DDevice->CreateRasterizerState(&rd, &g_RasterStateCullCCW);

	// カリングモード設定（CCW）
	SetCullingMode(CULL_MODE_BACK);



	// ブレンドステートの作成
	D3D11_BLEND_DESC blendDesc;
	ZeroMemory( &blendDesc, sizeof( blendDesc ) );
	blendDesc.AlphaToCoverageEnable = FALSE;
	blendDesc.IndependentBlendEnable = FALSE;
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	g_D3DDevice->CreateBlendState( &blendDesc, &g_BlendStateAlphaBlend );

	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	g_D3DDevice->CreateBlendState(&blendDesc, &g_BlendStateNone);

	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	g_D3DDevice->CreateBlendState(&blendDesc, &g_BlendStateAdd);

	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_REV_SUBTRACT;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	g_D3DDevice->CreateBlendState(&blendDesc, &g_BlendStateSubtract);

	// アルファブレンド設定
	SetBlendState(BLEND_MODE_ALPHABLEND);




	// 深度ステンシルステート作成
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	ZeroMemory( &depthStencilDesc, sizeof( depthStencilDesc ) );
	depthStencilDesc.DepthEnable = TRUE;
	depthStencilDesc.DepthWriteMask	= D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	depthStencilDesc.StencilEnable = FALSE;

	g_D3DDevice->CreateDepthStencilState( &depthStencilDesc, &g_DepthStateEnable );//深度有効ステート

	//depthStencilDesc.DepthEnable = FALSE;
	depthStencilDesc.DepthWriteMask	= D3D11_DEPTH_WRITE_MASK_ZERO;
	g_D3DDevice->CreateDepthStencilState( &depthStencilDesc, &g_DepthStateDisable );//深度無効ステート

	// 深度ステンシルステート設定
	SetDepthEnable(true);



	// サンプラーステート設定
	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory( &samplerDesc, sizeof( samplerDesc ) );
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0;
	samplerDesc.MaxAnisotropy = 16;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	ID3D11SamplerState* samplerState = NULL;
	g_D3DDevice->CreateSamplerState( &samplerDesc, &samplerState );

	g_ImmediateContext->PSSetSamplers( 0, 1, &samplerState );



	// 頂点シェーダコンパイル・生成
	ID3DBlob* pErrorBlob;
	ID3DBlob* pVSBlob = NULL;
	DWORD shFlag = D3DCOMPILE_ENABLE_STRICTNESS;

#if defined(_DEBUG) && defined(DEBUG_SHADER)
	shFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	// デフォルトバーテックスシェーダー
	hr = D3DX11CompileFromFile("shader.hlsl", NULL, NULL, "VertexShaderPolygon", "vs_4_0", shFlag, 0, NULL, &pVSBlob, &pErrorBlob, NULL);
	if (FAILED(hr))
	{
		MessageBox(NULL, (char*)pErrorBlob->GetBufferPointer(), "VS", MB_OK | MB_ICONERROR);
	}

	g_D3DDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), NULL, &g_VertexShader);

	//hr = D3DX11CompileFromFile("shader.hlsl", NULL, NULL, "VertexShaderInstancingBillboard", "vs_4_0", shFlag, 0, NULL, &pVSBlob, &pErrorBlob, NULL);
	//if (FAILED(hr))
	//{
	//	MessageBox(NULL, (char*)pErrorBlob->GetBufferPointer(), "VS", MB_OK | MB_ICONERROR);
	//}

	//g_D3DDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), NULL, &g_VertexShaderInstancingBillboard);

	// 入力レイアウト生成
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,			0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	UINT numElements = ARRAYSIZE( layout );

	g_D3DDevice->CreateInputLayout( layout,
		numElements,
		pVSBlob->GetBufferPointer(),
		pVSBlob->GetBufferSize(),
		&g_VertexLayout );

	pVSBlob->Release();


	// シャドウマップ書き込み用バーテックスシェーダー
	pVSBlob = NULL;
	hr = D3DX11CompileFromFile("shader.hlsl", NULL, NULL, "VertexShaderShadowMap", "vs_4_0", shFlag, 0, NULL, &pVSBlob, &pErrorBlob, NULL);
	if (FAILED(hr))
	{
		MessageBox(NULL, (char*)pErrorBlob->GetBufferPointer(), "VS", MB_OK | MB_ICONERROR);
	}

	g_D3DDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), NULL, &g_VertexShaderShadowMap);

	// 入力レイアウト生成
	D3D11_INPUT_ELEMENT_DESC layoutShadowMap[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	numElements = ARRAYSIZE(layoutShadowMap);

	g_D3DDevice->CreateInputLayout(layoutShadowMap,
		numElements,
		pVSBlob->GetBufferPointer(),
		pVSBlob->GetBufferSize(),
		&g_VertexLayoutShadowMap);

	pVSBlob->Release();

	// ピクセルシェーダコンパイル・生成
	CreateShader("shader.hlsl", "PixelShaderPolygon", &g_PixelShader, shFlag);					// デフォルトピクセルシェーダー
	CreateShader("shader.hlsl", "PixelShaderPolygonToon", &g_PixelShaderToon, shFlag);			// トゥーンピクセルシェーダー
	CreateShader("shader.hlsl", "PixelShaderPolygonHoldToon", &g_PixelShaderHoldToon, shFlag);	// トゥーンピクセルシェーダー
	CreateShader("shader.hlsl", "PixelShaderPolygonNonShadow", &g_PixelShaderNonShadow, shFlag);// シャドウマップ非適用ピクセルシェーダー
	CreateShader("shader.hlsl", "PixelShaderShadowMap", &g_PixelShaderShadowMap, shFlag);		// シャドウマップ書き込み用ピクセルシェーダー
	CreateShader("shader.hlsl", "PixelShaderWaterfall", &g_PixelShaderWaterfall, shFlag);		// 滝用ピクセルシェーダー
	CreateShader("shader.hlsl", "PixelShaderIceStage", &g_PixelShaderIceStage, shFlag);			// 氷ステージ用ピクセルシェーダー
	CreateShader("shader.hlsl", "PixelShaderSandStage", &g_PixelShaderSandStage, shFlag);		// 砂ステージ用ピクセルシェーダー
	CreateShader("shader.hlsl", "PixelShaderFireStage", &g_PixelShaderFireStage, shFlag);		// 炎ステージ用ピクセルシェーダー
	CreateShader("shader.hlsl", "PixelShaderDarkStage", &g_PixelShaderDarkStage, shFlag);		// 闇ステージ用ピクセルシェーダー

	// インスタンシング描画用シェーダコンパイル・生成
	CreateShader("shader.hlsl", "VertexShaderInstancingBillboard", &g_VertexShaderInstancingBillboard, shFlag);
	CreateShader("shader.hlsl", "PixelShaderPolygonNoLighting", &g_PixelShaderNoLighting, shFlag);

	// 定数バッファ生成
	D3D11_BUFFER_DESC hBufferDesc;
	hBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	hBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	hBufferDesc.CPUAccessFlags = 0;
	hBufferDesc.MiscFlags = 0;
	hBufferDesc.StructureByteStride = sizeof(float);

	CreateBuffer(&hBufferDesc, 0, &g_WVPMatrixBuffer, sizeof(WVP_MATRIX));		// 変換マトリクス群
	CreateBuffer(&hBufferDesc, 1, &g_LViewBuffer, sizeof(XMMATRIX));			// ライトビューマトリクス
	CreateBuffer(&hBufferDesc, 2, &g_LProjectionBuffer, sizeof(XMMATRIX));		// ライトプロジェクションマトリクス
	CreateBuffer(&hBufferDesc, 3, &g_MaterialBuffer, sizeof(MATERIAL_CBUFFER));	// マテリアル情報
	CreateBuffer(&hBufferDesc, 4, &g_LightBuffer, sizeof(LIGHT_CBUFFER));		// ライト情報
	CreateBuffer(&hBufferDesc, 5, &g_FogBuffer, sizeof(FOG_CBUFFER));			// フォグ情報
	CreateBuffer(&hBufferDesc, 6, &g_FuchiBuffer, sizeof(FUCHI));				// 縁取り
	CreateBuffer(&hBufferDesc, 7, &g_CameraBuffer, sizeof(XMFLOAT4));			// カメラ
	CreateBuffer(&hBufferDesc, 8, &g_TimeBuffer, sizeof(TIME));					// 時間（経過フレーム）情報
	CreateBuffer(&hBufferDesc, 9, &g_FuchiColorBuffer, sizeof(XMFLOAT4));		// 縁取り色
	CreateBuffer(&hBufferDesc,10, &g_BillboardBuffer, sizeof(XMMATRIX));		// ビルボード用
	
	// インスタンス
	hBufferDesc.Usage = D3D11_USAGE_DYNAMIC;				// CPU書き込みを許可する
	hBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;	// CPU書き込みを許可する
	hBufferDesc.ByteWidth = sizeof(INSTANCE);
	g_D3DDevice->CreateBuffer(&hBufferDesc, NULL, &g_InstanceBuffer);
	g_ImmediateContext->VSSetConstantBuffers(11, 1, &g_InstanceBuffer);
	g_ImmediateContext->PSSetConstantBuffers(11, 1, &g_InstanceBuffer);


	// 入力レイアウト設定
	g_ImmediateContext->IASetInputLayout( g_VertexLayout );

	// シェーダ設定
	g_ImmediateContext->VSSetShader( g_VertexShader, NULL, 0 );
	g_ImmediateContext->PSSetShader( g_PixelShader, NULL, 0 );

	//ライト初期化
	ZeroMemory(&g_Light, sizeof(LIGHT_CBUFFER));
	g_Light.Direction[0] = XMFLOAT4(1.0f, -1.0f, 1.0f, 0.0f);
	g_Light.Diffuse[0] = XMFLOAT4(0.9f, 0.9f, 0.9f, 1.0f);
	g_Light.Ambient[0] = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	g_Light.Flags[0].Type = LIGHT_TYPE_DIRECTIONAL;
	SetLightBuffer();


	//マテリアル初期化
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	material.Ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);


	// トゥーンレンダリング用カラールックアップテーブルの作成
	for (int i = 0; i < TOON_TEXTURE_MAX; i++)
	{
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_TextureName[i],
			NULL,
			NULL,
			&g_ShaderResourceViewToonMap[i],
			NULL);
	}
	GetDeviceContext()->PSSetShaderResources(2, 1, &g_ShaderResourceViewToonMap[0]);
	GetDeviceContext()->PSSetShaderResources(3, 1, &g_ShaderResourceViewToonMap[1]);



	return S_OK;
}

void CreateBuffer(D3D11_BUFFER_DESC* pDesc, int slot, ID3D11Buffer** pBuffer, UINT size)
{
	pDesc->ByteWidth = size;
	g_D3DDevice->CreateBuffer(pDesc, NULL, pBuffer);
	g_ImmediateContext->VSSetConstantBuffers(slot, 1, pBuffer);
	g_ImmediateContext->GSSetConstantBuffers(slot, 1, pBuffer);
	g_ImmediateContext->PSSetConstantBuffers(slot, 1, pBuffer);
}

void CreateShader(const char* pFileName, const char* pEntryPoint, ID3D11VertexShader** pVS, DWORD shFlag)
{
	ID3DBlob* pErrorBlob;
	ID3DBlob* pBlob = NULL;
	HRESULT hr = D3DX11CompileFromFile(pFileName, NULL, NULL, pEntryPoint, "vs_4_0", shFlag, 0, NULL, &pBlob, &pErrorBlob, NULL);
	if (FAILED(hr))
	{
		MessageBox(NULL, (char*)pErrorBlob->GetBufferPointer(), "VS", MB_OK | MB_ICONERROR);
	}
	g_D3DDevice->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, pVS);
	pBlob->Release();
}
void CreateShader(const char* pFileName, const char* pEntryPoint, ID3D11PixelShader** pPS, DWORD shFlag)
{
	ID3DBlob* pErrorBlob;
	ID3DBlob* pBlob = NULL;
	HRESULT hr = D3DX11CompileFromFile(pFileName, NULL, NULL, pEntryPoint, "ps_4_0", shFlag, 0, NULL, &pBlob, &pErrorBlob, NULL);
	if (FAILED(hr))
	{
		MessageBox(NULL, (char*)pErrorBlob->GetBufferPointer(), "PS", MB_OK | MB_ICONERROR);
	}
	g_D3DDevice->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, pPS);
	pBlob->Release();
}
void CreateShader(const char* pFileName, const char* pEntryPoint, ID3D11GeometryShader** pGS, DWORD shFlag)
{
	ID3DBlob* pErrorBlob;
	ID3DBlob* pBlob = NULL;
	HRESULT hr = D3DX11CompileFromFile(pFileName, NULL, NULL, pEntryPoint, "gs_4_0", shFlag, 0, NULL, &pBlob, &pErrorBlob, NULL);
	if (FAILED(hr))
	{
		MessageBox(NULL, (char*)pErrorBlob->GetBufferPointer(), "GS", MB_OK | MB_ICONERROR);
	}
	g_D3DDevice->CreateGeometryShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, pGS);
	pBlob->Release();
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitRenderer(void)
{
	// トゥーンレンダリング用カラールックアップテーブルの解放
	for (int i = 0; i < TOON_TEXTURE_MAX; i++)
	{
		if (g_ShaderResourceViewToonMap[i])
		{
			g_ShaderResourceViewToonMap[i]->Release();
			g_ShaderResourceViewToonMap[i] = NULL;
		}
	}

	// オブジェクト解放
	if (g_DepthStateEnable)		g_DepthStateEnable->Release();
	if (g_DepthStateDisable)	g_DepthStateDisable->Release();
	if (g_BlendStateNone)		g_BlendStateNone->Release();
	if (g_BlendStateAlphaBlend)	g_BlendStateAlphaBlend->Release();
	if (g_BlendStateAdd)		g_BlendStateAdd->Release();
	if (g_BlendStateSubtract)	g_BlendStateSubtract->Release();
	if (g_RasterStateCullOff)	g_RasterStateCullOff->Release();
	if (g_RasterStateCullCW)	g_RasterStateCullCW->Release();
	if (g_RasterStateCullCCW)	g_RasterStateCullCCW->Release();

	if (g_WVPMatrixBuffer)		g_WVPMatrixBuffer->Release();
	//if (g_WorldBuffer)			g_WorldBuffer->Release();
	//if (g_ViewBuffer)			g_ViewBuffer->Release();
	//if (g_ProjectionBuffer)		g_ProjectionBuffer->Release();
	if (g_LViewBuffer)			g_LViewBuffer->Release();
	if (g_LProjectionBuffer)	g_LProjectionBuffer->Release();
	if (g_MaterialBuffer)		g_MaterialBuffer->Release();
	if (g_LightBuffer)			g_LightBuffer->Release();
	if (g_FogBuffer)			g_FogBuffer->Release();

	if (g_PixelShaderNonShadow)	g_PixelShaderNonShadow->Release();
	if (g_VertexLayout)			g_VertexLayout->Release();
	if (g_VertexLayoutShadowMap) g_VertexLayoutShadowMap->Release();
	if (g_VertexShader)			g_VertexShader->Release();
	if (g_PixelShader)			g_PixelShader->Release();
	if (g_VertexShaderShadowMap) g_VertexShaderShadowMap->Release();
	if (g_PixelShaderShadowMap)	g_PixelShaderShadowMap->Release();

	if (g_TimeBuffer)			g_TimeBuffer->Release();
	if (g_PixelShaderWaterfall)	g_PixelShaderWaterfall->Release();
	if (g_PixelShaderFireStage)	g_PixelShaderFireStage->Release();
	if (g_PixelShaderDarkStage)	g_PixelShaderDarkStage->Release();
	if (g_PixelShaderIceStage)	g_PixelShaderIceStage->Release();
	if (g_PixelShaderSandStage)	g_PixelShaderSandStage->Release();

	if (g_PixelShaderToon)		g_PixelShaderToon->Release();
	if (g_PixelShaderHoldToon)		g_PixelShaderHoldToon->Release();

	if (g_ImmediateContext)		g_ImmediateContext->ClearState();
	if (g_RenderTargetView)		g_RenderTargetView->Release();
	if (g_RenderTargetViewShadowMap) g_RenderTargetViewShadowMap->Release();
	if (g_ShaderResourceViewSM)	g_ShaderResourceViewSM->Release();
	if (g_SwapChain)			g_SwapChain->Release();
	if (g_ImmediateContext)		g_ImmediateContext->Release();
	if (g_D3DDevice)			g_D3DDevice->Release();

	// インスタンシング描画関連
	if (g_VertexShaderInstancingBillboard)	g_VertexShaderInstancingBillboard->Release();
	if (g_PixelShaderNoLighting)	g_PixelShaderNoLighting->Release();
	if (g_InstanceBuffer)		g_InstanceBuffer->Release();
	if (g_BillboardBuffer)		g_BillboardBuffer->Release();
}



//=============================================================================
// バックバッファクリア
//=============================================================================
//float g_ClearColor[4] = { 0.2f, 0.5f, 1.0f, 1.0f };
float g_ClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
void Clear(void)
{
	// バックバッファクリア
	g_ImmediateContext->ClearRenderTargetView( g_RenderTargetView, g_ClearColor);
	g_ImmediateContext->ClearDepthStencilView( g_DepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void SetBackColor(XMFLOAT4 color)
{
	g_ClearColor[0] = color.x;
	g_ClearColor[1] = color.y;
	g_ClearColor[2] = color.z;
	g_ClearColor[3] = color.w;
}


//=============================================================================
// プレゼント
//=============================================================================
void Present(void)
{
	g_SwapChain->Present( 0, 0 );
}


//=============================================================================
// デバッグ用テキスト出力
//=============================================================================
void DebugTextOut(char* text, int x, int y)
{
#if defined(_DEBUG) && defined(DEBUG_DISP_TEXTOUT)
	HRESULT hr;

	//バックバッファからサーフェスを取得する
	IDXGISurface1* pBackSurface = NULL;
	hr = g_SwapChain->GetBuffer(0, __uuidof(IDXGISurface1), (void**)&pBackSurface);

	if (SUCCEEDED(hr))
	{
		//取得したサーフェスからデバイスコンテキストを取得する
		HDC hdc;
		hr = pBackSurface->GetDC(FALSE, &hdc);

		if (SUCCEEDED(hr))
		{
			//文字色を白に変更
			SetTextColor(hdc, RGB(255, 255, 255));

			//背景を透明に変更
			SetBkMode(hdc, TRANSPARENT);

			RECT rect;
			rect.left = 0;
			rect.top = 0;
			rect.right = SCREEN_WIDTH;
			rect.bottom = SCREEN_HEIGHT;

			//テキスト出力
			DrawText(hdc, text, (int)strlen(text), &rect, DT_LEFT);

			//デバイスコンテキストを解放する
			pBackSurface->ReleaseDC(NULL);
		}
		//サーフェスを解放する
		pBackSurface->Release();

		//レンダリングターゲットがリセットされるのでセットしなおす
		g_ImmediateContext->OMSetRenderTargets(1, &g_RenderTargetView, g_DepthStencilView);
	}
#endif
}


//=============================================================================
// シェーダーモードのセット
//=============================================================================
void SetShaderMode(int mode)
{
	switch (mode)
	{
	case SHADER_MODE_DEFAULT:
		// 入力レイアウト設定
		g_ImmediateContext->IASetInputLayout(g_VertexLayout);

		//シェーダーを通常のものにセット
		g_ImmediateContext->VSSetShader(g_VertexShader, NULL, 0);
		g_ImmediateContext->PSSetShader(g_PixelShader, NULL, 0);

		break;

	case SHADER_MODE_TOON:
		// 入力レイアウト設定
		g_ImmediateContext->IASetInputLayout(g_VertexLayout);

		//シェーダーを通常のものにセット
		g_ImmediateContext->VSSetShader(g_VertexShader, NULL, 0);
		g_ImmediateContext->PSSetShader(g_PixelShaderToon, NULL, 0);

		break;

	case SHADER_MODE_HOLD_TOON:
		// 入力レイアウト設定
		g_ImmediateContext->IASetInputLayout(g_VertexLayout);

		//シェーダーを通常のものにセット
		g_ImmediateContext->VSSetShader(g_VertexShader, NULL, 0);
		g_ImmediateContext->PSSetShader(g_PixelShaderHoldToon, NULL, 0);

		break;

	case SHADER_MODE_NON_SHADOW:
		// 入力レイアウト設定
		g_ImmediateContext->IASetInputLayout(g_VertexLayout);

		//シェーダーを通常のものにセット
		g_ImmediateContext->VSSetShader(g_VertexShader, NULL, 0);
		g_ImmediateContext->PSSetShader(g_PixelShaderNonShadow, NULL, 0);

		break;

	case SHADER_MODE_SHADOWMAPDRAW:
		// 入力レイアウト設定
		g_ImmediateContext->IASetInputLayout(g_VertexLayoutShadowMap);

		//シェーダーをZバッファ書き込み用のものにセット
		g_ImmediateContext->VSSetShader(g_VertexShaderShadowMap, NULL, 0);
		g_ImmediateContext->PSSetShader(g_PixelShaderShadowMap, NULL, 0);

		break;

	case SHADER_MODE_WATERFALL:
		// 入力レイアウト設定
		g_ImmediateContext->IASetInputLayout(g_VertexLayout);

		//シェーダーを滝用のものにセット
		g_ImmediateContext->VSSetShader(g_VertexShader, NULL, 0);
		g_ImmediateContext->PSSetShader(g_PixelShaderWaterfall, NULL, 0);
		break;

	case SHADER_MODE_FIRESTAGE:
		// 入力レイアウト設定
		g_ImmediateContext->IASetInputLayout(g_VertexLayout);

		//シェーダーを滝用のものにセット
		g_ImmediateContext->VSSetShader(g_VertexShader, NULL, 0);
		g_ImmediateContext->PSSetShader(g_PixelShaderFireStage, NULL, 0);
		break;

	case SHADER_MODE_DARKSTAGE:
		// 入力レイアウト設定
		g_ImmediateContext->IASetInputLayout(g_VertexLayout);

		//シェーダーを滝用のものにセット
		g_ImmediateContext->VSSetShader(g_VertexShader, NULL, 0);
		g_ImmediateContext->PSSetShader(g_PixelShaderDarkStage, NULL, 0);
		break;

	case SHADER_MODE_ICESTAGE:
		// 入力レイアウト設定
		g_ImmediateContext->IASetInputLayout(g_VertexLayout);

		//シェーダーを滝用のものにセット
		g_ImmediateContext->VSSetShader(g_VertexShader, NULL, 0);
		g_ImmediateContext->PSSetShader(g_PixelShaderIceStage, NULL, 0);
		break;

	case SHADER_MODE_SANDSTAGE:
		// 入力レイアウト設定
		g_ImmediateContext->IASetInputLayout(g_VertexLayout);

		//シェーダーを滝用のものにセット
		g_ImmediateContext->VSSetShader(g_VertexShader, NULL, 0);
		g_ImmediateContext->PSSetShader(g_PixelShaderSandStage, NULL, 0);
		break;
	}
}


//=============================================================================
// レンダ―ターゲットの変更
//=============================================================================
void LimitRenderTargetViewShadowMap(BOOL mode)
{
	if (mode == TRUE)
	{
		// レンダ―ターゲットをシャドウマップ書き込み用のものにセット
		g_ImmediateContext->OMSetRenderTargets(1, &g_RenderTargetViewShadowMap, g_DepthStencilView);
	}
	else
	{
		// レンダ―ターゲットをデフォルトにセット
		g_ImmediateContext->OMSetRenderTargets(1, &g_RenderTargetView, g_DepthStencilView);
	}
}


//=============================================================================
// シェーダーリソースとしてのシャドウマップのセットとクリア
//=============================================================================
void SetShaderResourceShadowMap(BOOL mode)
{
	if (mode == TRUE)
	{
		// シェーダーリソースとしてシャドウマップ書き込み用テクスチャを(レジスタt1に)セット
		GetDeviceContext()->PSSetShaderResources(1, 1, &g_ShaderResourceViewSM);
	}
	else
	{
		// シェーダーリソースをクリア
		ID3D11ShaderResourceView* clearsrv = NULL;
		GetDeviceContext()->PSSetShaderResources(1, 1, &clearsrv);
	}
}


//=============================================================================
// デプスステンシルビューのクリア
//=============================================================================
void ClearOnlyDepthStencil(void)
{
	g_ImmediateContext->ClearDepthStencilView(g_DepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);	//深度ステンシルビュークリア
}




//=============================================================================
// ウィンドウのサイズ変更に対応
//=============================================================================
void Resize(long winW, long winH)
{
	g_ImmediateContext->OMSetRenderTargets(0, NULL, NULL);
	g_RenderTargetView->Release();
	g_RenderTargetViewShadowMap->Release();
	g_DepthStencilView->Release();
	g_SwapChain->ResizeBuffers(1, winW, winH, DXGI_FORMAT_R8G8B8A8_UNORM, 0);

	// バックバッファのレンダーターゲットビュー再設定
	ID3D11Texture2D* pBackBuffer = NULL;
	g_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);	// 0番目のバックバッファを2Dとして取得
	g_D3DDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_RenderTargetView);	// そのバックバッファをレンダーターゲットビューに設定し、インターフェースを受け取る
	pBackBuffer->Release();

	// 深度・ステンシルテクスチャをリソースとして作成
	ID3D11Texture2D* depthTexture;
	D3D11_TEXTURE2D_DESC td;
	ZeroMemory(&td, sizeof(td));
	td.Width = winW;									// バッファの横幅
	td.Height = winH;									// バッファの縦幅
	td.MipLevels = 1;									// ミニマップのレベルを指定（1はミニマップなし）
	td.ArraySize = 1;									// テクスチャ配列のサイズを指定
	td.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;			// テクスチャのフォーマットを指定（Depth24bit、Stencil8bit、0.0~1.0）
	td.SampleDesc.Count = 1;							// マルチサンプリングの設定を指定
	td.SampleDesc.Quality = 0;							// マルチサンプリングの設定を指定
	td.Usage = D3D11_USAGE_DEFAULT;						// テクスチャの使用方法を指定
	td.BindFlags = D3D11_BIND_DEPTH_STENCIL;			// Bindの設定を指定
	td.CPUAccessFlags = 0;								// テクスチャへのCPUのアクセス権限を設定（0でいい）
	td.MiscFlags = 0;									// テクスチャオプションのフラグ（0でいい）
	g_D3DDevice->CreateTexture2D(&td, NULL, &depthTexture);

	// 作成したテクスチャを深度・ステンシルビューとして設定
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
	ZeroMemory(&dsvd, sizeof(dsvd));
	dsvd.Format = td.Format;							// Viewのフォーマットを指定
	dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;	// テクスチャの種類を指定
	dsvd.Flags = 0;										// 読み取り専用フラグ
	g_D3DDevice->CreateDepthStencilView(depthTexture, &dsvd, &g_DepthStencilView);

	g_ImmediateContext->OMSetRenderTargets(1, &g_RenderTargetView, g_DepthStencilView);
	
	//==============
	// シャドウマップ書き込み用テクスチャ
	D3D11_TEXTURE2D_DESC renderTextureDesc;
	renderTextureDesc.Width = winW;
	renderTextureDesc.Height = winH;
	renderTextureDesc.MipLevels = 1;
	renderTextureDesc.ArraySize = 1;
	renderTextureDesc.Format = DXGI_FORMAT_R32_FLOAT;
	renderTextureDesc.SampleDesc.Count = 1;
	renderTextureDesc.SampleDesc.Quality = 0;
	renderTextureDesc.Usage = D3D11_USAGE_DEFAULT;
	renderTextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	renderTextureDesc.CPUAccessFlags = 0;
	renderTextureDesc.MiscFlags = 0;
	g_D3DDevice->CreateTexture2D(&renderTextureDesc, NULL, &g_TextureShadowMap);

	// レンダーターゲットビュー生成、設定
	g_D3DDevice->CreateRenderTargetView(g_TextureShadowMap, NULL, &g_RenderTargetViewShadowMap);

	//シェーダーリソースビュー作成
	D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
	ZeroMemory(&srvd, sizeof(srvd));
	srvd.Format = DXGI_FORMAT_R32_FLOAT;
	srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvd.Texture2D.MipLevels = 1;
	g_D3DDevice->CreateShaderResourceView(g_TextureShadowMap, &srvd, &g_ShaderResourceViewSM);
	//==============

	D3D11_VIEWPORT vp[1];
	vp[0].Width = (FLOAT)winW;
	vp[0].Height = (FLOAT)winH;
	vp[0].MinDepth = 0.0f;
	vp[0].MaxDepth = 1.0f;
	vp[0].TopLeftX = 0;
	vp[0].TopLeftY = 0;
	g_ImmediateContext->RSSetViewports(1, vp);			// ラスタライザで使用（数字は配列の長さ）
}

