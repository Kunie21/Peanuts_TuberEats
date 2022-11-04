//=============================================================================
//
// レンダリング処理 [renderer.cpp]
// Author : 國江 翔太
//
//=============================================================================
#include "main.h"
#include "renderer.h"

// デバッグ用画面テキスト出力を有効にする
#define DEBUG_DISP_TEXTOUT
// シェーダーデバッグ設定を有効にする
//#define DEBUG_SHADER

//*****************************************************************************
// 構造体
//*****************************************************************************
float g_Filter[FILTER_MODE_NUM][3][3] = {
	{// フィルタなし
		{ 0.0f, 0.0f, 0.0f},
		{ 0.0f, 1.0f, 0.0f},
		{ 0.0f, 0.0f, 0.0f},
	},
	{// 平均化フィルタ
		{ 1.0f / 9.0f, 1.0f / 9.0f, 1.0f / 9.0f},
		{ 1.0f / 9.0f, 1.0f / 9.0f, 1.0f / 9.0f},
		{ 1.0f / 9.0f, 1.0f / 9.0f, 1.0f / 9.0f},
	},
	{// ガウシアンフィルタ
		{ 1.0f / 16.0f, 2.0f / 16.0f, 1.0f / 16.0f},
		{ 2.0f / 16.0f, 4.0f / 16.0f, 2.0f / 16.0f},
		{ 1.0f / 16.0f, 2.0f / 16.0f, 1.0f / 16.0f},
	},
	{// 鮮鋭化フィルタ弱
		{ -1.0f / 3.0f, -1.0f / 3.0f, -1.0f / 3.0f},
		{ -1.0f / 3.0f, 11.0f / 3.0f, -1.0f / 3.0f},
		{ -1.0f / 3.0f, -1.0f / 3.0f, -1.0f / 3.0f},
	},
	{// 鮮鋭化フィルタ強
		{ -1.0f, -1.0f, -1.0f},
		{ -1.0f,  9.0f, -1.0f},
		{ -1.0f, -1.0f, -1.0f},
	},
	{// ラプラシアンフィルタ
		{ 1.0f, 1.0f, 1.0f},
		{ 1.0f,-8.0f, 1.0f},
		{ 1.0f, 1.0f, 1.0f},
	},
	{// ラプラシアンフィルタカラー
		{ 1.0f, 1.0f, 1.0f},
		{ 1.0f,-7.0f, 1.0f},
		{ 1.0f, 1.0f, 1.0f},
	},
	{// プリューウィットフィルタ横
		{ -1.0f, 0.0f, 1.0f},
		{ -1.0f, 0.0f, 1.0f},
		{ -1.0f, 0.0f, 1.0f},
	},
	{// プリューウィットフィルタ縦
		{ 1.0f, 1.0f, 1.0f},
		{ 0.0f, 0.0f, 0.0f},
		{-1.0f,-1.0f,-1.0f},
	},
	{// ソーベルフィルタ横
		{ -1.0f, 0.0f, 1.0f},
		{ -2.0f, 0.0f, 2.0f},
		{ -1.0f, 0.0f, 1.0f},
	},
	{// ソーベルフィルタ横
		{ 1.0f, 2.0f, 1.0f},
		{ 0.0f, 0.0f, 0.0f},
		{-1.0f,-2.0f,-1.0f},
	},
	//{// 斜めぼかし
	//	{ 1.0f / 3.0f, 0.0f, 0.0f},
	//	{ 0.0f, 1.0f / 3.0f, 0.0f},
	//	{ 0.0f, 0.0f, 1.0f / 3.0f},
	//},
};

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static D3D_FEATURE_LEVEL       g_FeatureLevel = D3D_FEATURE_LEVEL_11_0;

// 標準装置
static ID3D11Device*           g_D3DDevice = NULL;
static ID3D11DeviceContext*    g_ImmediateContext = NULL;
static IDXGISwapChain*         g_SwapChain = NULL;
static ID3D11RenderTargetView* g_RenderTargetView = NULL;
static ID3D11DepthStencilView* g_DepthStencilView = NULL;

// ノーマルマップ用
static ID3D11RenderTargetView*	g_RenderTargetViewNormalMap = NULL;
static ID3D11ShaderResourceView*g_NormalMapTexture = NULL;
static ID3D11VertexShader*		g_VertexShaderNormalMap = NULL;
static ID3D11PixelShader*		g_PixelShaderNormalMap = NULL;

// デプスマップ用
static ID3D11RenderTargetView*	g_RenderTargetViewDepthMap = NULL;
static ID3D11ShaderResourceView*g_DepthMapTexture = NULL;
static ID3D11VertexShader*		g_VertexShaderDepthMap = NULL;
static ID3D11PixelShader*		g_PixelShaderDepthMap = NULL;

// フィルター用
static ID3D11VertexShader*		g_VertexShaderFilter = NULL;
static ID3D11PixelShader*		g_PixelShaderFilter = NULL;
static ID3D11PixelShader*		g_PixelShaderOnlyTex = NULL;
static ID3D11PixelShader*		g_PixelShaderMosaic = NULL;
static ID3D11PixelShader*		g_PixelShaderCA = NULL;
static ID3D11PixelShader*		g_PixelShaderMonitoring = NULL;
static ID3D11PixelShader*		g_PixelShaderOldGame = NULL;

// マルチレンダーターゲット
static ID3D11RenderTargetView*	g_RenderTargetViewWrite[2] = { NULL, NULL };
static ID3D11ShaderResourceView*g_WrittenTexture[2] = { NULL, NULL };
static int						g_CurrentTarget = 0;
static int						g_CurrentResource = 0;

// その他シェーダー
static ID3D11VertexShader*		g_VertexShader = NULL;
static ID3D11VertexShader*		g_VertexShaderOutline = NULL;
static ID3D11VertexShader*		g_VertexShaderTube = NULL;
static ID3D11VertexShader*		g_VertexShaderGimmick = NULL;
static ID3D11VertexShader*		g_VertexShaderPlayer = NULL;
static ID3D11PixelShader*		g_PixelShader = NULL;
static ID3D11PixelShader*		g_PixelShaderOutline = NULL;
static ID3D11PixelShader*		g_PixelShaderAL = NULL;
static ID3D11PixelShader*		g_PixelShaderDL = NULL;
static ID3D11PixelShader*		g_PixelShaderPL = NULL;
static ID3D11PixelShader*		g_PixelShaderSL = NULL;
static ID3D11PixelShader*		g_PixelShaderLL = NULL;
static ID3D11GeometryShader*	g_GeometryShaderDL = NULL;
static ID3D11GeometryShader*	g_GeometryShaderPL = NULL;
static ID3D11GeometryShader*	g_GeometryShaderSL = NULL;
static ID3D11GeometryShader*	g_GeometryShaderLLPlayer = NULL;
static ID3D11GeometryShader*	g_GeometryShaderLLNonPlayer = NULL;

// インプットレイアウト
static ID3D11InputLayout*		g_VertexLayout = NULL;

// 定数バッファ
static ID3D11Buffer*			g_MatrixBuffer = NULL;
static ID3D11Buffer*			g_CameraBuffer = NULL;
static ID3D11Buffer*			g_MaterialBuffer = NULL;
static ID3D11Buffer*			g_AmbientLightBuffer = NULL;
static ID3D11Buffer*			g_DirectionalLightBuffer = NULL;
static ID3D11Buffer*			g_PointLightBuffer = NULL;
static ID3D11Buffer*			g_SpotLightBuffer = NULL;
static ID3D11Buffer*			g_LightBuffer = NULL;
static ID3D11Buffer*			g_OutlineBuffer = NULL;
static ID3D11Buffer*			g_FilterBuffer = NULL;
static ID3D11Buffer*			g_MosaicBuffer = NULL;
static ID3D11Buffer*			g_CurveBuffer = NULL;
static ID3D11Buffer*			g_ConstantBuffer = NULL;

// スクリーンの頂点バッファ
static ID3D11Buffer*			g_ScreenVertexBuffer = NULL;

// 各構造体
static MATRIX					g_Matrix;
static XMFLOAT4					g_LightVector;
static XMFLOAT4					g_CameraPosition;
static XMFLOAT4					g_BackGroundColor = { 0.3f, 0.3f, 0.3f, 1.0f };

// デプスステンシル設定
static ID3D11DepthStencilState* g_DepthStateEnable;
static ID3D11DepthStencilState* g_DepthStateDisable;
static ID3D11DepthStencilState* g_StencilWrite;
static ID3D11DepthStencilState* g_StencilRead;

// ブレンド設定
static ID3D11BlendState*		g_BlendStateNone;
static ID3D11BlendState*		g_BlendStateAlphaBlend;
static ID3D11BlendState*		g_BlendStateAdd;
static ID3D11BlendState*		g_BlendStateSubtract;
static BLEND_MODE				g_BlendStateParam;

// ラスタライザ設定
static ID3D11RasterizerState*	g_RasterStateCullOff;
static ID3D11RasterizerState*	g_RasterStateCullCW;
static ID3D11RasterizerState*	g_RasterStateCullCCW;

// インスタンシング描画用
static ID3D11VertexShader*		g_VertexShaderInstancingBillboard = NULL;
static ID3D11PixelShader*		g_PixelShaderNoLighting = NULL;
static ID3D11Buffer*			g_InstanceBuffer = NULL;
static ID3D11Buffer*			g_BillboardBuffer = NULL;

//*****************************************************************************
// 終了処理
//*****************************************************************************
void UninitRenderer(void)
{
	// ノーマルマップ用
	if (g_RenderTargetViewNormalMap)	g_RenderTargetViewNormalMap->Release();
	if (g_NormalMapTexture)		g_NormalMapTexture->Release();
	if (g_VertexShaderNormalMap)	g_VertexShaderNormalMap->Release();
	if (g_PixelShaderNormalMap)	g_PixelShaderNormalMap->Release();

	// デプスマップ用
	if (g_RenderTargetViewDepthMap)	g_RenderTargetViewDepthMap->Release();
	if (g_DepthMapTexture)		g_DepthMapTexture->Release();
	if (g_VertexShaderDepthMap)	g_VertexShaderDepthMap->Release();
	if (g_PixelShaderDepthMap)	g_PixelShaderDepthMap->Release();

	// フィルター用シェーダー
	if (g_VertexShaderFilter)	g_VertexShaderFilter->Release();
	if (g_PixelShaderFilter)	g_PixelShaderFilter->Release();
	if (g_PixelShaderOnlyTex)	g_PixelShaderOnlyTex->Release();
	if (g_PixelShaderMosaic)	g_PixelShaderMosaic->Release();
	if (g_PixelShaderCA)		g_PixelShaderCA->Release();
	if (g_PixelShaderMonitoring)g_PixelShaderMonitoring->Release();
	if (g_PixelShaderOldGame)	g_PixelShaderOldGame->Release();

	// マルチレンダーターゲット
	if (g_RenderTargetViewWrite[0])	g_RenderTargetViewWrite[0]->Release();
	if (g_RenderTargetViewWrite[1])	g_RenderTargetViewWrite[1]->Release();

	// その他シェーダー
	if (g_VertexShader)			g_VertexShader->Release();
	if (g_VertexShaderOutline)	g_VertexShaderOutline->Release();
	if (g_VertexShaderTube)		g_VertexShaderTube->Release();
	if (g_VertexShaderGimmick)	g_VertexShaderGimmick->Release();
	if (g_VertexShaderPlayer)	g_VertexShaderPlayer->Release();
	if (g_PixelShader)			g_PixelShader->Release();
	if (g_PixelShaderOutline)	g_PixelShaderOutline->Release();
	if (g_PixelShaderAL)		g_PixelShaderAL->Release();
	if (g_PixelShaderDL)		g_PixelShaderDL->Release();
	if (g_PixelShaderPL)		g_PixelShaderPL->Release();
	if (g_PixelShaderSL)		g_PixelShaderSL->Release();
	if (g_PixelShaderLL)		g_PixelShaderLL->Release();
	if (g_GeometryShaderDL)		g_GeometryShaderDL->Release();
	if (g_GeometryShaderPL)		g_GeometryShaderPL->Release();
	if (g_GeometryShaderSL)		g_GeometryShaderSL->Release();
	if (g_GeometryShaderLLPlayer)	g_GeometryShaderLLPlayer->Release();
	if (g_GeometryShaderLLNonPlayer)	g_GeometryShaderLLNonPlayer->Release();

	// 頂点レイアウト
	if (g_VertexLayout)			g_VertexLayout->Release();

	// 定数バッファ
	if (g_MatrixBuffer)			g_MatrixBuffer->Release();
	if (g_CameraBuffer)			g_CameraBuffer->Release();
	if (g_MaterialBuffer)		g_MaterialBuffer->Release();
	if (g_AmbientLightBuffer)	g_AmbientLightBuffer->Release();
	if (g_DirectionalLightBuffer)	g_DirectionalLightBuffer->Release();
	if (g_PointLightBuffer)		g_PointLightBuffer->Release();
	if (g_SpotLightBuffer)		g_SpotLightBuffer->Release();
	if (g_LightBuffer)			g_LightBuffer->Release();
	if (g_OutlineBuffer)		g_OutlineBuffer->Release();
	if (g_FilterBuffer)			g_FilterBuffer->Release();
	if (g_MosaicBuffer)			g_MosaicBuffer->Release();
	if (g_CurveBuffer)			g_CurveBuffer->Release();
	if (g_ConstantBuffer)		g_ConstantBuffer->Release();

	// スクリーンの頂点バッファ
	if (g_ScreenVertexBuffer)	g_ScreenVertexBuffer->Release();

	// デプスステンシル設定
	if (g_DepthStateEnable)		g_DepthStateEnable->Release();
	if (g_DepthStateDisable)	g_DepthStateDisable->Release();
	if (g_StencilWrite)			g_StencilWrite->Release();
	if (g_StencilRead)			g_StencilRead->Release();

	// ブレンド設定
	if (g_BlendStateNone)		g_BlendStateNone->Release();
	if (g_BlendStateAlphaBlend)	g_BlendStateAlphaBlend->Release();
	if (g_BlendStateAdd)		g_BlendStateAdd->Release();
	if (g_BlendStateSubtract)	g_BlendStateSubtract->Release();

	// ラスタライザ設定
	if (g_RasterStateCullOff)	g_RasterStateCullOff->Release();
	if (g_RasterStateCullCW)	g_RasterStateCullCW->Release();
	if (g_RasterStateCullCCW)	g_RasterStateCullCCW->Release();

	// インスタンシング描画関連
	if (g_VertexShaderInstancingBillboard)	g_VertexShaderInstancingBillboard->Release();
	if (g_PixelShaderNoLighting)	g_PixelShaderNoLighting->Release();
	if (g_InstanceBuffer)		g_InstanceBuffer->Release();
	if (g_BillboardBuffer)		g_BillboardBuffer->Release();

	// 基本装置
	if (g_ImmediateContext)		g_ImmediateContext->ClearState();
	if (g_RenderTargetView)		g_RenderTargetView->Release();
	if (g_SwapChain)			g_SwapChain->Release();
	if (g_ImmediateContext)		g_ImmediateContext->Release();
	if (g_D3DDevice)			g_D3DDevice->Release();
}

//*****************************************************************************
// 初期化処理
//*****************************************************************************
HRESULT InitRenderer(HINSTANCE hInstance, HWND hWnd, BOOL bWindow)
{
	HRESULT hr = S_OK;

	// デバイス、スワップチェーン、コンテキスト生成
	DXGI_SWAP_CHAIN_DESC sd;
	{
		DWORD deviceFlags = 0;
		ZeroMemory(&sd, sizeof(sd));
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

		hr = D3D11CreateDeviceAndSwapChain(NULL,
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
			&g_ImmediateContext);
		if (FAILED(hr))
			return hr;

		//デバッグ文字出力用設定
#if defined(_DEBUG) && defined(DEBUG_DISP_TEXTOUT)
		hr = g_SwapChain->ResizeBuffers(0, SCREEN_WIDTH, SCREEN_HEIGHT, DXGI_FORMAT_UNKNOWN, DXGI_SWAP_CHAIN_FLAG_GDI_COMPATIBLE); // N.B. the GDI compatible flag
		if (FAILED(hr))
			return hr;
#endif
	}

	// レンダーターゲットビュー生成、設定
	{
		ID3D11Texture2D* pBackBuffer = NULL;
		g_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
		g_D3DDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_RenderTargetView);
		pBackBuffer->Release();
	}

	//ステンシル用テクスチャー作成
	{
		ID3D11Texture2D* depthTexture = NULL;
		D3D11_TEXTURE2D_DESC td;
		ZeroMemory(&td, sizeof(td));
		td.Width = sd.BufferDesc.Width;
		td.Height = sd.BufferDesc.Height;
		td.MipLevels = 1;
		td.ArraySize = 1;
		td.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		td.SampleDesc = sd.SampleDesc;
		td.Usage = D3D11_USAGE_DEFAULT;
		td.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		td.CPUAccessFlags = 0;
		td.MiscFlags = 0;
		g_D3DDevice->CreateTexture2D(&td, NULL, &depthTexture);

		//ステンシルターゲット作成
		D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
		ZeroMemory(&dsvd, sizeof(dsvd));
		dsvd.Format = td.Format;
		dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		dsvd.Flags = 0;
		g_D3DDevice->CreateDepthStencilView(depthTexture, &dsvd, &g_DepthStencilView);

		g_ImmediateContext->OMSetRenderTargets(1, &g_RenderTargetView, g_DepthStencilView);
	}

	// 書き込み用テクスチャ作成
	for (int i = 0; i < 2; i++)
	{
		ID3D11Texture2D* writeTexture = NULL;
		D3D11_TEXTURE2D_DESC td;
		ZeroMemory(&td, sizeof(td));
		td.Width = sd.BufferDesc.Width;
		td.Height = sd.BufferDesc.Height;
		td.MipLevels = 1;
		td.ArraySize = 1;
		td.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		td.SampleDesc.Count = 1;
		td.SampleDesc.Quality = 0;
		td.Usage = D3D11_USAGE_DEFAULT;
		td.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		td.CPUAccessFlags = 0;
		td.MiscFlags = 0;
		g_D3DDevice->CreateTexture2D(&td, NULL, &writeTexture);

		// レンダーターゲットビュー生成、設定
		g_D3DDevice->CreateRenderTargetView(writeTexture, NULL, &g_RenderTargetViewWrite[i]);

		//シェーダーリソースビュー作成
		D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
		ZeroMemory(&srvd, sizeof(srvd));
		srvd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvd.Texture2D.MipLevels = 1;
		g_D3DDevice->CreateShaderResourceView(writeTexture, &srvd, &g_WrittenTexture[i]);
	}

	// ノーマルマップ用テクスチャ作成
	{
		ID3D11Texture2D* normalTexture = NULL;
		D3D11_TEXTURE2D_DESC td;
		ZeroMemory(&td, sizeof(td));
		td.Width = sd.BufferDesc.Width;
		td.Height = sd.BufferDesc.Height;
		td.MipLevels = 1;
		td.ArraySize = 1;
		td.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		td.SampleDesc.Count = 1;
		td.SampleDesc.Quality = 0;
		td.Usage = D3D11_USAGE_DEFAULT;
		td.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		td.CPUAccessFlags = 0;
		td.MiscFlags = 0;
		g_D3DDevice->CreateTexture2D(&td, NULL, &normalTexture);

		// レンダーターゲットビュー生成、設定
		g_D3DDevice->CreateRenderTargetView(normalTexture, NULL, &g_RenderTargetViewNormalMap);

		//シェーダーリソースビュー作成
		D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
		ZeroMemory(&srvd, sizeof(srvd));
		srvd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvd.Texture2D.MipLevels = 1;
		g_D3DDevice->CreateShaderResourceView(normalTexture, &srvd, &g_NormalMapTexture);
	}

	// デプスマップ用テクスチャ作成
	{
		ID3D11Texture2D* Texture = NULL;
		D3D11_TEXTURE2D_DESC td;
		ZeroMemory(&td, sizeof(td));
		td.Width = sd.BufferDesc.Width;
		td.Height = sd.BufferDesc.Height;
		td.MipLevels = 1;
		td.ArraySize = 1;
		td.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		td.SampleDesc.Count = 1;
		td.SampleDesc.Quality = 0;
		td.Usage = D3D11_USAGE_DEFAULT;
		td.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		td.CPUAccessFlags = 0;
		td.MiscFlags = 0;
		g_D3DDevice->CreateTexture2D(&td, NULL, &Texture);

		// レンダーターゲットビュー生成、設定
		g_D3DDevice->CreateRenderTargetView(Texture, NULL, &g_RenderTargetViewDepthMap);

		//シェーダーリソースビュー作成
		D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
		ZeroMemory(&srvd, sizeof(srvd));
		srvd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvd.Texture2D.MipLevels = 1;
		g_D3DDevice->CreateShaderResourceView(Texture, &srvd, &g_DepthMapTexture);
	}

	// ビューポート設定
	{
		D3D11_VIEWPORT vp;
		vp.Width = (FLOAT)SCREEN_WIDTH;
		vp.Height = (FLOAT)SCREEN_HEIGHT;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;

		// 初期設定
		g_ImmediateContext->RSSetViewports(1, &vp);
	}

	// ラスタライザステート作成
	{
		D3D11_RASTERIZER_DESC rd;
		ZeroMemory(&rd, sizeof(rd));
		rd.FillMode = D3D11_FILL_SOLID;
		rd.CullMode = D3D11_CULL_NONE;
		rd.DepthClipEnable = TRUE;
		rd.MultisampleEnable = FALSE;
		g_D3DDevice->CreateRasterizerState(&rd, &g_RasterStateCullOff);

		rd.CullMode = D3D11_CULL_FRONT;
		g_D3DDevice->CreateRasterizerState(&rd, &g_RasterStateCullCW);

		rd.CullMode = D3D11_CULL_BACK;
		g_D3DDevice->CreateRasterizerState(&rd, &g_RasterStateCullCCW);

		// 初期設定
		SetCullingMode(CULL_MODE_BACK);
	}

	// ブレンドステートの作成
	{
		D3D11_BLEND_DESC blendDesc;
		ZeroMemory(&blendDesc, sizeof(blendDesc));
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
		g_D3DDevice->CreateBlendState(&blendDesc, &g_BlendStateAlphaBlend);

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

		// 初期設定
		SetBlendState(BLEND_MODE_ALPHABLEND);
	}

	// 深度ステンシルステート作成
	{
		D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
		ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
		depthStencilDesc.DepthEnable = TRUE;
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		depthStencilDesc.StencilEnable = FALSE;

		g_D3DDevice->CreateDepthStencilState(&depthStencilDesc, &g_DepthStateEnable);//深度有効ステート

		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		g_D3DDevice->CreateDepthStencilState(&depthStencilDesc, &g_DepthStateDisable);//深度無効ステート

		ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
		depthStencilDesc.DepthEnable = TRUE;
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		depthStencilDesc.StencilEnable = TRUE;
		depthStencilDesc.StencilReadMask = 0xff;
		depthStencilDesc.StencilWriteMask = 0xff;
		depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;	// ステンシルテストを常に行う
		depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_INCR;
		depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;	// ステンシルテストを常に行う
		depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_DECR;
		depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		g_D3DDevice->CreateDepthStencilState(&depthStencilDesc, &g_StencilWrite);

		ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
		depthStencilDesc.DepthEnable = TRUE;
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		depthStencilDesc.StencilEnable = TRUE;					// ステンシルバッファ書き込み宣言
		depthStencilDesc.StencilReadMask = 0xff;				// ステンシル参照値
		depthStencilDesc.StencilWriteMask = 0xff;			// ステンシルマスク
		depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;	// ステンシルテストを常に行う
		depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_NOT_EQUAL;	// ステンシルテストを常に行う
		depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
		depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		g_D3DDevice->CreateDepthStencilState(&depthStencilDesc, &g_StencilRead);

		g_ImmediateContext->OMSetDepthStencilState(g_DepthStateEnable, NULL);

		// 初期設定
		SetDepthEnable(true);
	}

	// サンプラーステート設定
	{
		D3D11_SAMPLER_DESC samplerDesc;
		ZeroMemory(&samplerDesc, sizeof(samplerDesc));
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
		g_D3DDevice->CreateSamplerState(&samplerDesc, &samplerState);

		// 初期設定
		g_ImmediateContext->PSSetSamplers(0, 1, &samplerState);
	}

	// シェーダーのコンパイル・生成
	{
		DWORD shFlag = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(_DEBUG) && defined(DEBUG_SHADER)
		shFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

		// 頂点シェーダ
		ID3DBlob* pErrorBlob;
		ID3DBlob* pVSBlob = NULL;
		hr = D3DX11CompileFromFile("shader.hlsl", NULL, NULL, "VertexShaderPolygon", "vs_4_0", shFlag, 0, NULL, &pVSBlob, &pErrorBlob, NULL);
		if (FAILED(hr)) {
			MessageBox(NULL, (char*)pErrorBlob->GetBufferPointer(), "VS", MB_OK | MB_ICONERROR);
		}
		g_D3DDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), NULL, &g_VertexShader);

		// 入力レイアウト生成
		D3D11_INPUT_ELEMENT_DESC layout[] ={
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,			0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};
		UINT numElements = ARRAYSIZE(layout);

		g_D3DDevice->CreateInputLayout(layout,
			numElements,
			pVSBlob->GetBufferPointer(),
			pVSBlob->GetBufferSize(),
			&g_VertexLayout);
		pVSBlob->Release();

		// 初期設定
		g_ImmediateContext->IASetInputLayout(g_VertexLayout);

		// その他の頂点シェーダコンパイル・生成
		CreateShader("shader.hlsl", "VertexShaderOutline", &g_VertexShaderOutline, shFlag);
		CreateShader("shader.hlsl", "VertexShaderTube", &g_VertexShaderTube, shFlag);
		CreateShader("shader.hlsl", "VertexShaderGimmick", &g_VertexShaderGimmick, shFlag);
		CreateShader("shader.hlsl", "VertexShaderPlayer", &g_VertexShaderPlayer, shFlag);
		CreateShader("shader_postEffect.hlsl", "VertexShaderFilter", &g_VertexShaderFilter, shFlag);
		CreateShader("shader_postEffect.hlsl", "VertexShaderNormalMap", &g_VertexShaderNormalMap, shFlag);
		CreateShader("shader_postEffect.hlsl", "VertexShaderDepthMap", &g_VertexShaderDepthMap, shFlag);

		// ジオメトリシェーダコンパイル・生成
		CreateShader("shader.hlsl", "GeometryShaderDL", &g_GeometryShaderDL, shFlag);
		CreateShader("shader.hlsl", "GeometryShaderPL", &g_GeometryShaderPL, shFlag);
		CreateShader("shader.hlsl", "GeometryShaderSL", &g_GeometryShaderSL, shFlag);
		CreateShader("shader.hlsl", "GeometryShaderLLPlayer", &g_GeometryShaderLLPlayer, shFlag);
		CreateShader("shader.hlsl", "GeometryShaderLLNonPlayer", &g_GeometryShaderLLNonPlayer, shFlag);

		// ピクセルシェーダコンパイル・生成
		CreateShader("shader.hlsl", "PixelShaderPolygon", &g_PixelShader, shFlag);
		CreateShader("shader.hlsl", "PixelShaderAL", &g_PixelShaderAL, shFlag);
		CreateShader("shader.hlsl", "PixelShaderDL", &g_PixelShaderDL, shFlag);
		CreateShader("shader.hlsl", "PixelShaderPL", &g_PixelShaderPL, shFlag);
		CreateShader("shader.hlsl", "PixelShaderSL", &g_PixelShaderSL, shFlag);
		CreateShader("shader.hlsl", "PixelShaderLL", &g_PixelShaderLL, shFlag);
		CreateShader("shader.hlsl", "PixelShaderOutline", &g_PixelShaderOutline, shFlag);
		CreateShader("shader_postEffect.hlsl", "PixelShaderFilter", &g_PixelShaderFilter, shFlag);
		CreateShader("shader_postEffect.hlsl", "PixelShaderOnlyTex", &g_PixelShaderOnlyTex, shFlag);
		CreateShader("shader_postEffect.hlsl", "PixelShaderMosaic", &g_PixelShaderMosaic, shFlag);
		CreateShader("shader_postEffect.hlsl", "PixelShaderNormalMap", &g_PixelShaderNormalMap, shFlag);
		CreateShader("shader_postEffect.hlsl", "PixelShaderDepthMap", &g_PixelShaderDepthMap, shFlag);
		CreateShader("shader_postEffect.hlsl", "PixelShaderCA", &g_PixelShaderCA, shFlag);
		CreateShader("shader_postEffect.hlsl", "PixelShaderMonitoring", &g_PixelShaderMonitoring, shFlag);
		CreateShader("shader_postEffect.hlsl", "PixelShaderOldGame", &g_PixelShaderOldGame, shFlag);

		// インスタンシング描画用シェーダコンパイル・生成
		//CreateShader("shader.hlsl", "VertexShaderInstancingBillboard", &g_VertexShaderInstancingBillboard, shFlag);
		//CreateShader("shader.hlsl", "PixelShaderPolygonNoLighting", &g_PixelShaderNoLighting, shFlag);

		// 初期設定
		g_ImmediateContext->VSSetShader(g_VertexShader, NULL, 0);
		g_ImmediateContext->PSSetShader(g_PixelShader, NULL, 0);
	}

	// 定数バッファ生成
	{
		D3D11_BUFFER_DESC hBufferDesc;
		hBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		hBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		hBufferDesc.CPUAccessFlags = 0;
		hBufferDesc.MiscFlags = 0;
		hBufferDesc.StructureByteStride = sizeof(float);

		CreateBuffer(&hBufferDesc, 0, &g_MatrixBuffer, sizeof(MATRIX));
		CreateBuffer(&hBufferDesc, 1, &g_CameraBuffer, sizeof(CAMERA));
		CreateBuffer(&hBufferDesc, 2, &g_MaterialBuffer, sizeof(MATERIAL));
		CreateBuffer(&hBufferDesc, 3, &g_AmbientLightBuffer, sizeof(LIGHT_AMBIENT));
		CreateBuffer(&hBufferDesc, 4, &g_DirectionalLightBuffer, sizeof(LIGHT_DIRECTIONAL) * LIGHT_DIRECTIONAL_MAX);
		CreateBuffer(&hBufferDesc, 5, &g_PointLightBuffer, sizeof(LIGHT_POINT) * LIGHT_POINT_MAX);
		CreateBuffer(&hBufferDesc, 6, &g_SpotLightBuffer, sizeof(LIGHT_SPOT) * LIGHT_SPOT_MAX);
		CreateBuffer(&hBufferDesc, 7, &g_LightBuffer, sizeof(LIGHT_NO));
		CreateBuffer(&hBufferDesc, 8, &g_OutlineBuffer, sizeof(OUTLINE));
		CreateBuffer(&hBufferDesc, 9, &g_FilterBuffer, sizeof(XMMATRIX));
		CreateBuffer(&hBufferDesc, 10, &g_MosaicBuffer, sizeof(MOSAIC));
		CreateBuffer(&hBufferDesc, 10, &g_CurveBuffer, sizeof(CURVE_BUFFER));
		CreateBuffer(&hBufferDesc, 11, &g_ConstantBuffer, sizeof(CONSTANT));

		// インスタンス
		//hBufferDesc.Usage = D3D11_USAGE_DYNAMIC;				// CPU書き込みを許可する
		//hBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;	// CPU書き込みを許可する
		//hBufferDesc.ByteWidth = sizeof(INSTANCE);
		//g_D3DDevice->CreateBuffer(&hBufferDesc, NULL, &g_InstanceBuffer);
		//g_ImmediateContext->VSSetConstantBuffers(11, 1, &g_InstanceBuffer);
		//g_ImmediateContext->PSSetConstantBuffers(11, 1, &g_InstanceBuffer);
	}

	// スクリーン用頂点バッファ作成
	{
		// 頂点バッファ生成
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DYNAMIC;
		bd.ByteWidth = sizeof(VERTEX_3D) * 4;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		GetDevice()->CreateBuffer(&bd, NULL, &g_ScreenVertexBuffer);

		//頂点バッファの中身を埋める
		D3D11_MAPPED_SUBRESOURCE msr;
		GetDeviceContext()->Map(g_ScreenVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

		VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

		// 頂点座標の設定
		vertex[0].Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
		vertex[1].Position = XMFLOAT3(SCREEN_WIDTH, 0.0f, 0.0f);
		vertex[2].Position = XMFLOAT3(0.0f, SCREEN_HEIGHT, 0.0f);
		vertex[3].Position = XMFLOAT3(SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f);

		// 法線ベクトルの設定
		vertex[0].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
		vertex[1].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
		vertex[2].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
		vertex[3].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);

		// 拡散光の設定
		vertex[0].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[1].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[2].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[3].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		// テクスチャ座標の設定
		vertex[0].TexCoord = XMFLOAT2(0.0f, 0.0f);
		vertex[1].TexCoord = XMFLOAT2(1.0f, 0.0f);
		vertex[2].TexCoord = XMFLOAT2(0.0f, 1.0f);
		vertex[3].TexCoord = XMFLOAT2(1.0f, 1.0f);

		GetDeviceContext()->Unmap(g_ScreenVertexBuffer, 0);
	}

	return S_OK;
}

//*****************************************************************************
// 各種処理
//*****************************************************************************
ID3D11Device* GetDevice(void) { return g_D3DDevice; }
ID3D11DeviceContext* GetDeviceContext(void) { return g_ImmediateContext; }
void Present(void) { g_SwapChain->Present(0, 0); }

// 深度テストのセッター
void SetDepthEnable(BOOL Enable) {
	if (Enable)
		g_ImmediateContext->OMSetDepthStencilState(g_DepthStateEnable, NULL);
	else
		g_ImmediateContext->OMSetDepthStencilState(g_DepthStateDisable, NULL);
}

// ブレンドのセッター
void SetBlendState(BLEND_MODE bm) {
	g_BlendStateParam = bm;

	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

	switch (g_BlendStateParam) {
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

// カリングのセッター
void SetCullingMode(CULL_MODE cm) {
	switch (cm) {
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

// マトリクスバッファのセッター
void SetWorldViewProjection2D(void) {
	MATRIX matrix;
	XMMATRIX worldViewProjection = XMMatrixOrthographicOffCenterLH(0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f);
	XMStoreFloat4x4(&matrix.WorldViewProjection, XMMatrixTranspose(worldViewProjection));
	GetDeviceContext()->UpdateSubresource(g_MatrixBuffer, 0, NULL, &matrix, 0, 0);
}
void SetWorldBuffer(SRT& srt) {
	XMMATRIX mtxWorld = XMMatrixIdentity();		// ワールドマトリックスの初期化
	MulMtxScl(mtxWorld, srt.scl);	// スケールを反映
	MulMtxRot(mtxWorld, srt.rot);	// 回転を反映：全体の角度
	MulMtxPos(mtxWorld, srt.pos);	// 移動を反映
	SetWorldBuffer(&mtxWorld);		// ワールドマトリックスの設定
}
void SetWorldBuffer(XMMATRIX* WorldMatrix) {
	XMStoreFloat4x4(&g_Matrix.World, *WorldMatrix);

	XMStoreFloat4x4(&g_Matrix.ViewProjection, XMLoadFloat4x4(&g_Matrix.View) * XMLoadFloat4x4(&g_Matrix.Projection));
	XMStoreFloat4x4(&g_Matrix.WorldViewProjection, XMLoadFloat4x4(&g_Matrix.World) * XMLoadFloat4x4(&g_Matrix.ViewProjection));

	MATRIX matrix;
	XMStoreFloat4x4(&matrix.World, XMMatrixTranspose(XMLoadFloat4x4(&g_Matrix.World)));
	XMStoreFloat4x4(&matrix.View, XMMatrixTranspose(XMLoadFloat4x4(&g_Matrix.View)));
	XMStoreFloat4x4(&matrix.Projection, XMMatrixTranspose(XMLoadFloat4x4(&g_Matrix.Projection)));
	XMStoreFloat4x4(&matrix.ViewProjection, XMMatrixTranspose(XMLoadFloat4x4(&g_Matrix.ViewProjection)));
	XMStoreFloat4x4(&matrix.WorldViewProjection, XMMatrixTranspose(XMLoadFloat4x4(&g_Matrix.WorldViewProjection)));
	XMStoreFloat4x4(&matrix.AfterRotation, XMMatrixTranspose(XMLoadFloat4x4(&g_Matrix.AfterRotation)));
	GetDeviceContext()->UpdateSubresource(g_MatrixBuffer, 0, NULL, &matrix, 0, 0);
}
void SetViewBuffer(XMMATRIX* ViewMatrix) { XMStoreFloat4x4(&g_Matrix.View, *ViewMatrix); }
void SetProjectionBuffer(XMMATRIX* ProjectionMatrix) { XMStoreFloat4x4(&g_Matrix.Projection, *ProjectionMatrix); }
void SetAfterRotation(XMMATRIX* AfterRotationMatrix) { XMStoreFloat4x4(&g_Matrix.AfterRotation, *AfterRotationMatrix); }

// 定数バッファのセッター
void SetCameraBuffer(CAMERA *Camera) {
	GetDeviceContext()->UpdateSubresource(g_CameraBuffer, 0, NULL, Camera, 0, 0);
}
void SetMaterialBuffer(MATERIAL *Material) {
	GetDeviceContext()->UpdateSubresource(g_MaterialBuffer, 0, NULL, Material, 0, 0);
}
void SetAmbientLight(LIGHT_AMBIENT *pAmbientLight) {
	GetDeviceContext()->UpdateSubresource(g_AmbientLightBuffer, 0, NULL, pAmbientLight, 0, 0);
}
void SetDirectionalLight(LIGHT_DIRECTIONAL *pDirectionalLight) {
	GetDeviceContext()->UpdateSubresource(g_DirectionalLightBuffer, 0, NULL, pDirectionalLight, 0, 0);
}
void SetPointLight(LIGHT_POINT *pPointLight) {
	GetDeviceContext()->UpdateSubresource(g_PointLightBuffer, 0, NULL, pPointLight, 0, 0);
}
void SetSpotLight(LIGHT_SPOT* pSpotLight) {
	GetDeviceContext()->UpdateSubresource(g_SpotLightBuffer, 0, NULL, pSpotLight, 0, 0);
}
void SetLightNo(int lightNo) {
	LIGHT_NO light = { lightNo, 0.0f, 0.0f, 0.0f };
	GetDeviceContext()->UpdateSubresource(g_LightBuffer, 0, NULL, &light, 0, 0);
}
void SetFrameTime(int time) {
	CONSTANT constant = { time, 0.0f, 0.0f, 0.0f };
	GetDeviceContext()->UpdateSubresource(g_ConstantBuffer, 0, NULL, &constant, 0, 0);
}
void SetCurveBuffer(CURVE_BUFFER* curve) {
	GetDeviceContext()->UpdateSubresource(g_CurveBuffer, 0, NULL, curve, 0, 0);
}

// インスタンシング関連
ID3D11Buffer* GetInstanceBuffer(void) { return g_InstanceBuffer; }
void SetShaderInstanceingBillboard(XMFLOAT4X4 mtxView) {
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
}

// バッファ作成
void CreateBuffer(D3D11_BUFFER_DESC* pDesc, int slot, ID3D11Buffer** pBuffer, UINT size)
{
	pDesc->ByteWidth = size;
	g_D3DDevice->CreateBuffer(pDesc, NULL, pBuffer);
	g_ImmediateContext->VSSetConstantBuffers(slot, 1, pBuffer);
	g_ImmediateContext->GSSetConstantBuffers(slot, 1, pBuffer);
	g_ImmediateContext->PSSetConstantBuffers(slot, 1, pBuffer);
}

// 頂点シェーダー作成
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
// ピクセルシェーダー作成
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
// ジオメトリシェーダー作成
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

// シェーダー関連
void SetShaderDefault(void) {
	g_ImmediateContext->VSSetShader(g_VertexShader, NULL, 0);
	g_ImmediateContext->PSSetShader(g_PixelShader, NULL, 0);
}
void SetStencilWriteDL(void)
{
	SetCullingMode(CULL_MODE_NONE);
	g_ImmediateContext->VSSetShader(g_VertexShader, NULL, 0);
	g_ImmediateContext->GSSetShader(g_GeometryShaderDL, NULL, 0);
	g_ImmediateContext->PSSetShader(NULL, NULL, 0);
	g_ImmediateContext->OMSetDepthStencilState(g_StencilWrite, NULL);
	g_ImmediateContext->OMSetRenderTargets(0, NULL, g_DepthStencilView);
	// シャドウボリュームも描画してみる
	//g_ImmediateContext->PSSetShader(g_PixelShader, NULL, 0);
	//g_ImmediateContext->OMSetRenderTargets(1, &g_RenderTargetView, g_DepthStencilView);
}
void SetStencilWritePL(void)
{
	SetCullingMode(CULL_MODE_NONE);
	//g_ImmediateContext->VSSetShader(g_VertexShader, NULL, 0);
	g_ImmediateContext->VSSetShader(g_VertexShaderPlayer, NULL, 0);
	g_ImmediateContext->GSSetShader(g_GeometryShaderPL, NULL, 0);
	g_ImmediateContext->PSSetShader(NULL, NULL, 0);
	g_ImmediateContext->OMSetDepthStencilState(g_StencilWrite, NULL);
	g_ImmediateContext->OMSetRenderTargets(0, NULL, g_DepthStencilView);
	// シャドウボリュームも描画してみる
	//g_ImmediateContext->PSSetShader(g_PixelShader, NULL, 0);
	//g_ImmediateContext->OMSetRenderTargets(1, &g_RenderTargetView, g_DepthStencilView);
}
void SetStencilWriteSL(void)
{
	SetCullingMode(CULL_MODE_NONE);
	g_ImmediateContext->VSSetShader(g_VertexShader, NULL, 0);
	g_ImmediateContext->GSSetShader(g_GeometryShaderSL, NULL, 0);
	g_ImmediateContext->PSSetShader(NULL, NULL, 0);
	g_ImmediateContext->OMSetDepthStencilState(g_StencilWrite, NULL);
	g_ImmediateContext->OMSetRenderTargets(0, NULL, g_DepthStencilView);
	// シャドウボリュームも描画してみる
	//g_ImmediateContext->PSSetShader(g_PixelShader, NULL, 0);
	//g_ImmediateContext->OMSetRenderTargets(1, &g_RenderTargetView, g_DepthStencilView);
}
void SetStencilWriteLL(SHADER_TYPE shader)
{
	SetCullingMode(CULL_MODE_NONE);
	switch (shader)
	{
	case SHADER_TUBE:
	case SHADER_GIMMICK:
		g_ImmediateContext->VSSetShader(g_VertexShaderGimmick, NULL, 0);
		g_ImmediateContext->GSSetShader(g_GeometryShaderLLNonPlayer, NULL, 0);
		break;
	case SHADER_PLAYER:
		g_ImmediateContext->VSSetShader(g_VertexShaderPlayer, NULL, 0);
		g_ImmediateContext->GSSetShader(g_GeometryShaderLLPlayer, NULL, 0);
		break;
	}
	//g_ImmediateContext->GSSetShader(g_GeometryShaderLLPlayer, NULL, 0);
	g_ImmediateContext->PSSetShader(NULL, NULL, 0);
	g_ImmediateContext->OMSetDepthStencilState(g_StencilWrite, NULL);
	g_ImmediateContext->OMSetRenderTargets(0, NULL, g_DepthStencilView);

	// シャドウボリュームも描画してみる
	//g_ImmediateContext->PSSetShader(g_PixelShader, NULL, 0);
	//g_ImmediateContext->OMSetRenderTargets(1, &g_RenderTargetView, g_DepthStencilView);
	//g_ImmediateContext->OMSetRenderTargets(1, &g_RenderTargetViewWrite[g_CurrentTarget], g_DepthStencilView);
}

void SetStencilRead(void)
{
	SetCullingMode(CULL_MODE_BACK);
	g_ImmediateContext->VSSetShader(g_VertexShader, NULL, 0);
	g_ImmediateContext->GSSetShader(NULL, NULL, 0);
	g_ImmediateContext->PSSetShader(g_PixelShader, NULL, 0);
	g_ImmediateContext->OMSetDepthStencilState(g_StencilRead, NULL);
	g_ImmediateContext->OMSetRenderTargets(1, &g_RenderTargetViewWrite[g_CurrentTarget], g_DepthStencilView);
}
void SetStencilReadDL(void)
{
	SetCullingMode(CULL_MODE_BACK);
	g_ImmediateContext->VSSetShader(g_VertexShader, NULL, 0);
	g_ImmediateContext->GSSetShader(NULL, NULL, 0);
	g_ImmediateContext->PSSetShader(g_PixelShaderDL, NULL, 0);
	g_ImmediateContext->OMSetDepthStencilState(g_StencilRead, NULL);
	g_ImmediateContext->OMSetRenderTargets(1, &g_RenderTargetViewWrite[g_CurrentTarget], g_DepthStencilView);
}
void SetStencilReadPL(void)
{
	SetCullingMode(CULL_MODE_BACK);
	g_ImmediateContext->VSSetShader(g_VertexShader, NULL, 0);
	g_ImmediateContext->GSSetShader(NULL, NULL, 0);
	g_ImmediateContext->PSSetShader(g_PixelShaderPL, NULL, 0);
	g_ImmediateContext->OMSetDepthStencilState(g_StencilRead, NULL);
	g_ImmediateContext->OMSetRenderTargets(1, &g_RenderTargetViewWrite[g_CurrentTarget], g_DepthStencilView);
}
void SetStencilReadSL(void)
{
	SetCullingMode(CULL_MODE_BACK);
	g_ImmediateContext->VSSetShader(g_VertexShader, NULL, 0);
	g_ImmediateContext->GSSetShader(NULL, NULL, 0);
	g_ImmediateContext->PSSetShader(g_PixelShaderSL, NULL, 0);
	g_ImmediateContext->OMSetDepthStencilState(g_StencilRead, NULL);
	g_ImmediateContext->OMSetRenderTargets(1, &g_RenderTargetViewWrite[g_CurrentTarget], g_DepthStencilView);
}
void SetStencilReadLL(SHADER_TYPE shader)
{
	SetCullingMode(CULL_MODE_BACK);
	switch (shader)
	{
	case SHADER_TUBE:
		g_ImmediateContext->VSSetShader(g_VertexShaderTube, NULL, 0);
		break;
	case SHADER_GIMMICK:
		g_ImmediateContext->VSSetShader(g_VertexShaderGimmick, NULL, 0);
		break;
	case SHADER_PLAYER:
		g_ImmediateContext->VSSetShader(g_VertexShaderPlayer, NULL, 0);
		break;
	}
	g_ImmediateContext->GSSetShader(NULL, NULL, 0);
	g_ImmediateContext->PSSetShader(g_PixelShaderLL, NULL, 0);
	g_ImmediateContext->OMSetDepthStencilState(g_StencilRead, NULL);
	g_ImmediateContext->OMSetRenderTargets(1, &g_RenderTargetViewWrite[g_CurrentTarget], g_DepthStencilView);
}

void SetStencilNone(void)
{
	SetCullingMode(CULL_MODE_BACK);
	g_ImmediateContext->VSSetShader(g_VertexShader, NULL, 0);
	g_ImmediateContext->GSSetShader(NULL, NULL, 0);
	g_ImmediateContext->PSSetShader(g_PixelShader, NULL, 0);
	g_ImmediateContext->OMSetDepthStencilState(g_DepthStateEnable, NULL);
	g_ImmediateContext->OMSetRenderTargets(1, &g_RenderTargetViewWrite[g_CurrentTarget], g_DepthStencilView);
}
void SetStencilNoneAL(SHADER_TYPE shader)
{
	SetCullingMode(CULL_MODE_BACK);
	switch (shader)
	{
	case SHADER_TUBE:
		g_ImmediateContext->VSSetShader(g_VertexShaderTube, NULL, 0);
		break;
	case SHADER_GIMMICK:
		g_ImmediateContext->VSSetShader(g_VertexShaderGimmick, NULL, 0);
		break;
	case SHADER_PLAYER:
		g_ImmediateContext->VSSetShader(g_VertexShaderPlayer, NULL, 0);
		break;
	}
	g_ImmediateContext->GSSetShader(NULL, NULL, 0);
	g_ImmediateContext->PSSetShader(g_PixelShaderAL, NULL, 0);
	g_ImmediateContext->OMSetDepthStencilState(g_DepthStateEnable, NULL);
	g_ImmediateContext->OMSetRenderTargets(1, &g_RenderTargetViewWrite[g_CurrentTarget], g_DepthStencilView);
	//g_ImmediateContext->OMSetRenderTargets(1, &g_RenderTargetView, g_DepthStencilView);
}

void SetStencilNoneOnlyDepth(void)
{
	SetCullingMode(CULL_MODE_BACK);
	g_ImmediateContext->VSSetShader(g_VertexShader, NULL, 0);
	g_ImmediateContext->GSSetShader(NULL, NULL, 0);
	g_ImmediateContext->PSSetShader(NULL, NULL, 0);
	g_ImmediateContext->OMSetDepthStencilState(g_DepthStateEnable, NULL);
	g_ImmediateContext->OMSetRenderTargets(0, NULL, g_DepthStencilView);
}

void SetDrawOutline(float Scale, XMFLOAT4 Color)
{
	OUTLINE outline = { { Scale, 0.0f, 0.0f, 0.0f }, Color };
	GetDeviceContext()->UpdateSubresource(g_OutlineBuffer, 0, NULL, &outline, 0, 0);

	SetCullingMode(CULL_MODE_FRONT);
	g_ImmediateContext->VSSetShader(g_VertexShaderOutline, NULL, 0);
	g_ImmediateContext->GSSetShader(NULL, NULL, 0);
	g_ImmediateContext->PSSetShader(g_PixelShaderOutline, NULL, 0);
	g_ImmediateContext->OMSetDepthStencilState(g_DepthStateEnable, NULL);
	g_ImmediateContext->OMSetRenderTargets(1, &g_RenderTargetViewWrite[g_CurrentTarget], g_DepthStencilView);
}
void SetDrawFillBlack(SHADER_TYPE shader)
{
	OUTLINE outline = { { 0.0f, 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f, 1.0f} };
	GetDeviceContext()->UpdateSubresource(g_OutlineBuffer, 0, NULL, &outline, 0, 0);

	SetCullingMode(CULL_MODE_BACK);
	//g_ImmediateContext->VSSetShader(g_VertexShaderOutline, NULL, 0);
	switch (shader)
	{
	case SHADER_TUBE:
		g_ImmediateContext->VSSetShader(g_VertexShaderTube, NULL, 0);
		break;
	case SHADER_GIMMICK:
		g_ImmediateContext->VSSetShader(g_VertexShaderGimmick, NULL, 0);
		break;
	case SHADER_PLAYER:
		g_ImmediateContext->VSSetShader(g_VertexShaderPlayer, NULL, 0);
		break;
	}
	g_ImmediateContext->GSSetShader(NULL, NULL, 0);
	g_ImmediateContext->PSSetShader(g_PixelShaderOutline, NULL, 0);
	g_ImmediateContext->OMSetDepthStencilState(g_DepthStateEnable, NULL);
	g_ImmediateContext->OMSetRenderTargets(1, &g_RenderTargetViewWrite[g_CurrentTarget], g_DepthStencilView);
	//g_ImmediateContext->OMSetRenderTargets(1, &g_RenderTargetView, g_DepthStencilView);
}
void SetDrawFillBlackPlayer(void)
{
	OUTLINE outline = { { 0.0f, 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f, 1.0f} };
	GetDeviceContext()->UpdateSubresource(g_OutlineBuffer, 0, NULL, &outline, 0, 0);

	SetCullingMode(CULL_MODE_BACK);
	g_ImmediateContext->VSSetShader(g_VertexShaderOutline, NULL, 0);
	g_ImmediateContext->GSSetShader(NULL, NULL, 0);
	g_ImmediateContext->PSSetShader(g_PixelShaderOutline, NULL, 0);
	g_ImmediateContext->OMSetDepthStencilState(g_DepthStateEnable, NULL);
	g_ImmediateContext->OMSetRenderTargets(1, &g_RenderTargetViewWrite[g_CurrentTarget], g_DepthStencilView);
	//g_ImmediateContext->OMSetRenderTargets(1, &g_RenderTargetView, g_DepthStencilView);
}
void SetDrawNoLighting(void)
{
	SetCullingMode(CULL_MODE_BACK);
	g_ImmediateContext->VSSetShader(g_VertexShader, NULL, 0);
	g_ImmediateContext->GSSetShader(NULL, NULL, 0);
	g_ImmediateContext->PSSetShader(g_PixelShaderOnlyTex, NULL, 0);
	g_ImmediateContext->OMSetDepthStencilState(g_DepthStateEnable, NULL);
	//g_ImmediateContext->OMSetRenderTargets(1, &g_RenderTargetView, g_DepthStencilView);
	g_ImmediateContext->OMSetRenderTargets(1, &g_RenderTargetViewWrite[g_CurrentTarget], g_DepthStencilView);
}
void SetDrawTube(void)
{
	SetCullingMode(CULL_MODE_BACK);
	g_ImmediateContext->VSSetShader(g_VertexShaderTube, NULL, 0);
	g_ImmediateContext->GSSetShader(NULL, NULL, 0);
	//g_ImmediateContext->PSSetShader(g_PixelShaderPL, NULL, 0);
	//g_ImmediateContext->PSSetShader(g_PixelShaderAL, NULL, 0);
	g_ImmediateContext->PSSetShader(g_PixelShaderLL, NULL, 0);
	g_ImmediateContext->OMSetDepthStencilState(g_DepthStateEnable, NULL);
	//g_ImmediateContext->OMSetRenderTargets(1, &g_RenderTargetView, g_DepthStencilView);
	g_ImmediateContext->OMSetRenderTargets(1, &g_RenderTargetViewWrite[g_CurrentTarget], g_DepthStencilView);
}
void SetDrawGimmick(void)
{
	SetCullingMode(CULL_MODE_BACK);
	g_ImmediateContext->VSSetShader(g_VertexShaderGimmick, NULL, 0);
	g_ImmediateContext->GSSetShader(NULL, NULL, 0);
	//g_ImmediateContext->PSSetShader(g_PixelShaderOnlyTex, NULL, 0);
	//g_ImmediateContext->PSSetShader(g_PixelShaderPL, NULL, 0);
	g_ImmediateContext->PSSetShader(g_PixelShaderLL, NULL, 0);
	g_ImmediateContext->OMSetDepthStencilState(g_DepthStateEnable, NULL);
	//g_ImmediateContext->OMSetRenderTargets(1, &g_RenderTargetView, g_DepthStencilView);
	g_ImmediateContext->OMSetRenderTargets(1, &g_RenderTargetViewWrite[g_CurrentTarget], g_DepthStencilView);
}
void SetDrawPlayer(void)
{
	SetCullingMode(CULL_MODE_BACK);
	g_ImmediateContext->VSSetShader(g_VertexShaderPlayer, NULL, 0);
	g_ImmediateContext->GSSetShader(NULL, NULL, 0);
	g_ImmediateContext->PSSetShader(g_PixelShaderLL, NULL, 0);
	//g_ImmediateContext->PSSetShader(g_PixelShaderOnlyTex, NULL, 0);
	g_ImmediateContext->OMSetDepthStencilState(g_DepthStateEnable, NULL);
	//g_ImmediateContext->OMSetRenderTargets(1, &g_RenderTargetView, g_DepthStencilView);
	g_ImmediateContext->OMSetRenderTargets(1, &g_RenderTargetViewWrite[g_CurrentTarget], g_DepthStencilView);
}
void SetDrawLight(void)
{
	SetCullingMode(CULL_MODE_BACK);
	g_ImmediateContext->VSSetShader(g_VertexShaderTube, NULL, 0);
	g_ImmediateContext->GSSetShader(NULL, NULL, 0);
	g_ImmediateContext->PSSetShader(g_PixelShaderOnlyTex, NULL, 0);
	//g_ImmediateContext->PSSetShader(g_PixelShaderOnlyTex, NULL, 0);
	g_ImmediateContext->OMSetDepthStencilState(g_DepthStateEnable, NULL);
	//g_ImmediateContext->OMSetRenderTargets(1, &g_RenderTargetView, g_DepthStencilView);
	g_ImmediateContext->OMSetRenderTargets(1, &g_RenderTargetViewWrite[g_CurrentTarget], g_DepthStencilView);
}
void SetDrawFire(void)
{
	SetCullingMode(CULL_MODE_BACK);
	g_ImmediateContext->VSSetShader(g_VertexShaderPlayer, NULL, 0);
	g_ImmediateContext->GSSetShader(NULL, NULL, 0);
	g_ImmediateContext->PSSetShader(g_PixelShaderOnlyTex, NULL, 0);
	//g_ImmediateContext->PSSetShader(g_PixelShaderOnlyTex, NULL, 0);
	g_ImmediateContext->OMSetDepthStencilState(g_DepthStateEnable, NULL);
	//g_ImmediateContext->OMSetRenderTargets(1, &g_RenderTargetView, g_DepthStencilView);
	g_ImmediateContext->OMSetRenderTargets(1, &g_RenderTargetViewWrite[g_CurrentTarget], g_DepthStencilView);
}
void SetDraw2DTexture(void)
{
	g_ImmediateContext->VSSetShader(g_VertexShaderFilter, NULL, 0);
	g_ImmediateContext->GSSetShader(NULL, NULL, 0);
	g_ImmediateContext->PSSetShader(g_PixelShaderOnlyTex, NULL, 0);
	g_ImmediateContext->OMSetDepthStencilState(g_DepthStateDisable, NULL);
	g_ImmediateContext->OMSetRenderTargets(1, &g_RenderTargetView, g_DepthStencilView);
	//g_ImmediateContext->OMSetRenderTargets(1, &g_RenderTargetViewWrite[g_CurrentTarget], g_DepthStencilView);
}

void ApplyFilter(FILTER_MODE filter)
{
	// ターゲットの切り替え
	g_CurrentTarget = g_CurrentTarget ? 0 : 1;

	XMMATRIX f;
	for (int x = 0; x < 3; x++)
	{
		for (int y = 0; y < 3; y++)
		{
			f.r[y].m128_f32[x] = g_Filter[filter][y][x];
		}
	}
	GetDeviceContext()->UpdateSubresource(g_FilterBuffer, 0, NULL, &f, 0, 0);

	SetCullingMode(CULL_MODE_BACK);
	g_ImmediateContext->VSSetShader(g_VertexShaderFilter, NULL, 0);
	g_ImmediateContext->GSSetShader(NULL, NULL, 0);
	g_ImmediateContext->PSSetShader(g_PixelShaderFilter, NULL, 0);
	g_ImmediateContext->OMSetDepthStencilState(g_DepthStateDisable, NULL);
	g_ImmediateContext->OMSetRenderTargets(1, &g_RenderTargetViewWrite[g_CurrentTarget], g_DepthStencilView);

	DrawScreen(&g_WrittenTexture[g_CurrentResource]);

	// リソースの切り替え
	g_CurrentResource = g_CurrentResource ? 0 : 1;
}
void ApplyMosaic(int pixel_size)
{
	// ターゲットの切り替え
	g_CurrentTarget = g_CurrentTarget ? 0 : 1;

	MOSAIC mozaic = { pixel_size, 0.0f, 0.0f, 0.0f };
	GetDeviceContext()->UpdateSubresource(g_MosaicBuffer, 0, NULL, &mozaic, 0, 0);

	SetCullingMode(CULL_MODE_BACK);
	g_ImmediateContext->VSSetShader(g_VertexShaderFilter, NULL, 0);
	g_ImmediateContext->GSSetShader(NULL, NULL, 0);
	g_ImmediateContext->PSSetShader(g_PixelShaderMosaic, NULL, 0);
	//g_ImmediateContext->OMSetDepthStencilState(g_StencilRead, NULL);
	g_ImmediateContext->OMSetDepthStencilState(g_DepthStateDisable, NULL);
	g_ImmediateContext->OMSetRenderTargets(1, &g_RenderTargetViewWrite[g_CurrentTarget], g_DepthStencilView);

	DrawScreen(&g_WrittenTexture[g_CurrentResource]);

	// リソースの切り替え
	g_CurrentResource = g_CurrentResource ? 0 : 1;
}
void ApplyChromaticAberration(void)
{
	// ターゲットの切り替え
	g_CurrentTarget = g_CurrentTarget ? 0 : 1;

	SetCullingMode(CULL_MODE_BACK);
	g_ImmediateContext->VSSetShader(g_VertexShaderFilter, NULL, 0);
	g_ImmediateContext->GSSetShader(NULL, NULL, 0);
	g_ImmediateContext->PSSetShader(g_PixelShaderCA, NULL, 0);
	g_ImmediateContext->OMSetDepthStencilState(g_DepthStateDisable, NULL);
	g_ImmediateContext->OMSetRenderTargets(1, &g_RenderTargetViewWrite[g_CurrentTarget], g_DepthStencilView);

	DrawScreen(&g_WrittenTexture[g_CurrentResource]);

	// リソースの切り替え
	g_CurrentResource = g_CurrentResource ? 0 : 1;
}
void ApplyMonitoring(void)
{
	// ターゲットの切り替え
	g_CurrentTarget = g_CurrentTarget ? 0 : 1;

	SetCullingMode(CULL_MODE_BACK);
	g_ImmediateContext->VSSetShader(g_VertexShaderFilter, NULL, 0);
	g_ImmediateContext->GSSetShader(NULL, NULL, 0);
	g_ImmediateContext->PSSetShader(g_PixelShaderMonitoring, NULL, 0);
	g_ImmediateContext->OMSetDepthStencilState(g_DepthStateDisable, NULL);
	g_ImmediateContext->OMSetRenderTargets(1, &g_RenderTargetViewWrite[g_CurrentTarget], g_DepthStencilView);

	DrawScreen(&g_WrittenTexture[g_CurrentResource]);

	// リソースの切り替え
	g_CurrentResource = g_CurrentResource ? 0 : 1;
}
void ApplyOldGame(void)
{
	// ターゲットの切り替え
	g_CurrentTarget = g_CurrentTarget ? 0 : 1;

	SetCullingMode(CULL_MODE_BACK);
	g_ImmediateContext->VSSetShader(g_VertexShaderFilter, NULL, 0);
	g_ImmediateContext->GSSetShader(NULL, NULL, 0);
	g_ImmediateContext->PSSetShader(g_PixelShaderOldGame, NULL, 0);
	g_ImmediateContext->OMSetDepthStencilState(g_DepthStateDisable, NULL);
	g_ImmediateContext->OMSetRenderTargets(1, &g_RenderTargetViewWrite[g_CurrentTarget], g_DepthStencilView);

	DrawScreen(&g_WrittenTexture[g_CurrentResource]);

	// リソースの切り替え
	g_CurrentResource = g_CurrentResource ? 0 : 1;
}
void DrawTarget(void)
{
	SetCullingMode(CULL_MODE_BACK);
	g_ImmediateContext->VSSetShader(g_VertexShaderFilter, NULL, 0);
	g_ImmediateContext->GSSetShader(NULL, NULL, 0);
	g_ImmediateContext->PSSetShader(g_PixelShaderOnlyTex, NULL, 0);
	g_ImmediateContext->OMSetDepthStencilState(g_DepthStateDisable, NULL);
	g_ImmediateContext->OMSetRenderTargets(1, &g_RenderTargetView, g_DepthStencilView);

	DrawScreen(&g_WrittenTexture[g_CurrentResource]);

	g_CurrentTarget = 0;
	g_CurrentResource = 0;
}

void DrawScreen(ID3D11ShaderResourceView** pTexture)
{
	// マトリクス設定
	MATRIX matrix;
	XMMATRIX worldViewProjection = XMMatrixOrthographicOffCenterLH(0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f);
	XMStoreFloat4x4(&matrix.WorldViewProjection, XMMatrixTranspose(worldViewProjection));
	GetDeviceContext()->UpdateSubresource(g_MatrixBuffer, 0, NULL, &matrix, 0, 0);

	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_ScreenVertexBuffer, &stride, &offset);

	// プリミティブトポロジ設定
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// テクスチャ設定
	GetDeviceContext()->PSSetShaderResources(0, 1, pTexture);

	// ポリゴン描画
	GetDeviceContext()->Draw(4, 0);
}

void SetTargetNormalMap(BOOL bDrawDirect)
{
	float ClearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	g_ImmediateContext->ClearRenderTargetView(g_RenderTargetViewNormalMap, ClearColor);

	SetCullingMode(CULL_MODE_BACK);
	g_ImmediateContext->VSSetShader(g_VertexShaderNormalMap, NULL, 0);
	g_ImmediateContext->GSSetShader(NULL, NULL, 0);
	g_ImmediateContext->PSSetShader(g_PixelShaderNormalMap, NULL, 0);
	g_ImmediateContext->OMSetDepthStencilState(g_DepthStateEnable, NULL);
	if (bDrawDirect)
	{
		g_ImmediateContext->OMSetRenderTargets(1, &g_RenderTargetViewWrite[g_CurrentTarget], g_DepthStencilView);
	}
	else
	{
		g_ImmediateContext->OMSetRenderTargets(1, &g_RenderTargetViewNormalMap, g_DepthStencilView);
	}
}
void DrawNormalMap(void)
{
	SetCullingMode(CULL_MODE_BACK);
	g_ImmediateContext->VSSetShader(g_VertexShaderFilter, NULL, 0);
	g_ImmediateContext->GSSetShader(NULL, NULL, 0);
	g_ImmediateContext->PSSetShader(g_PixelShaderOnlyTex, NULL, 0);
	g_ImmediateContext->OMSetDepthStencilState(g_DepthStateDisable, NULL);
	g_ImmediateContext->OMSetRenderTargets(1, &g_RenderTargetView, g_DepthStencilView);

	DrawScreen(&g_NormalMapTexture);

	// ポリゴン描画
	GetDeviceContext()->Draw(4, 0);
}

void SetTargetDepthMap(BOOL bDrawDirect)
{
	float ClearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	g_ImmediateContext->ClearRenderTargetView(g_RenderTargetViewDepthMap, ClearColor);
	g_ImmediateContext->ClearDepthStencilView(g_DepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	SetCullingMode(CULL_MODE_BACK);
	g_ImmediateContext->VSSetShader(g_VertexShaderDepthMap, NULL, 0);
	g_ImmediateContext->GSSetShader(NULL, NULL, 0);
	g_ImmediateContext->PSSetShader(g_PixelShaderDepthMap, NULL, 0);
	g_ImmediateContext->OMSetDepthStencilState(g_DepthStateEnable, NULL);
	if (bDrawDirect)
	{
		g_ImmediateContext->OMSetRenderTargets(1, &g_RenderTargetViewWrite[g_CurrentTarget], g_DepthStencilView);
	}
	else
	{
		g_ImmediateContext->OMSetRenderTargets(1, &g_RenderTargetViewDepthMap, g_DepthStencilView);
	}

	//// マトリクス設定
	//D3DXMATRIX projection;
	//D3DXMatrixOrthoOffCenterLH(&projection, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f);
	//SetProjectionBuffer(&projection);
}
void DrawDepthMap(void)
{
	SetCullingMode(CULL_MODE_BACK);
	g_ImmediateContext->VSSetShader(g_VertexShaderFilter, NULL, 0);
	g_ImmediateContext->GSSetShader(NULL, NULL, 0);
	g_ImmediateContext->PSSetShader(g_PixelShaderOnlyTex, NULL, 0);
	g_ImmediateContext->OMSetDepthStencilState(g_DepthStateDisable, NULL);
	g_ImmediateContext->OMSetRenderTargets(1, &g_RenderTargetView, g_DepthStencilView);

	DrawScreen(&g_DepthMapTexture);

	// ポリゴン描画
	GetDeviceContext()->Draw(4, 0);
}

// Float3をFloat4に変換
XMFLOAT4 Float4(XMFLOAT3& f3) {
	return { f3.x, f3.y, f3.z, 0.0f };
}

// マトリクス計算
void MulMtxScl(XMMATRIX& mtxWorld, XMFLOAT3& scl) {
	mtxWorld = XMMatrixMultiply(mtxWorld, XMMatrixScaling(scl.x, scl.y, scl.z));
}
void MulMtxScl(XMMATRIX& mtxWorld, float x, float y, float z) {
	mtxWorld = XMMatrixMultiply(mtxWorld, XMMatrixScaling(x, y, z));
}
void MulMtxRot(XMMATRIX& mtxWorld, XMFLOAT3& rot) {
	mtxWorld = XMMatrixMultiply(mtxWorld, XMMatrixRotationRollPitchYaw(rot.x, rot.y, rot.z));
}
void MulMtxRot(XMMATRIX& mtxWorld, float x, float y, float z) {
	mtxWorld = XMMatrixMultiply(mtxWorld, XMMatrixRotationRollPitchYaw(x, y, z));
}
void MulMtxPos(XMMATRIX& mtxWorld, XMFLOAT3& pos) {
	mtxWorld = XMMatrixMultiply(mtxWorld, XMMatrixTranslation(pos.x, pos.y, pos.z));
}
void MulMtxPos(XMMATRIX& mtxWorld, float x, float y, float z) {
	mtxWorld = XMMatrixMultiply(mtxWorld, XMMatrixTranslation(x, y, z));
}

XMFLOAT4 GetBackGroundColor(void)
{
	return g_BackGroundColor;
}
void SetBackGroundColor(XMFLOAT4 color)
{
	g_BackGroundColor = color;
}

//=============================================================================
// バックバッファクリア
//=============================================================================
void Clear(void)
{
	float ClearColor[4] = {
		g_BackGroundColor.x,
		g_BackGroundColor.y,
		g_BackGroundColor.z,
		//g_BackGroundColor.w,
		0.0f,
	};
	//float ClearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	g_ImmediateContext->ClearRenderTargetView(g_RenderTargetView, ClearColor);
	g_ImmediateContext->ClearRenderTargetView(g_RenderTargetViewWrite[0], ClearColor);
	g_ImmediateContext->ClearRenderTargetView(g_RenderTargetViewWrite[1], ClearColor);
	g_ImmediateContext->ClearDepthStencilView(g_DepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}
void ClearStencil(void)
{
	g_ImmediateContext->ClearDepthStencilView(g_DepthStencilView, D3D11_CLEAR_STENCIL, 1.0f, 0);
}
void ClearWriteView(void)
{
	float ClearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	g_ImmediateContext->ClearRenderTargetView(g_RenderTargetViewWrite[0], ClearColor);
	g_ImmediateContext->ClearRenderTargetView(g_RenderTargetViewWrite[1], ClearColor);
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
