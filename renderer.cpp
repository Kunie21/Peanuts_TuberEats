//=============================================================================
//
// �����_�����O���� [renderer.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "renderer.h"

//�f�o�b�O�p��ʃe�L�X�g�o�͂�L���ɂ���
#define DEBUG_DISP_TEXTOUT
//�V�F�[�_�[�f�o�b�O�ݒ��L���ɂ���
//#define DEBUG_SHADER

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TOON_TEXTURE_MAX			(2)						// �e�N�X�`���̐�


//*********************************************************
// �\����
//*********************************************************

// �}�e���A���p�萔�o�b�t�@�\����
struct MATERIAL_CBUFFER
{
	XMFLOAT4	Ambient;
	XMFLOAT4	Diffuse;
	XMFLOAT4	Specular;
	XMFLOAT4	Emission;
	float		Shininess;
	int			noTexSampling;
	float		Dummy[2];				// 16byte���E�p
};

// ���C�g�p�t���O�\����
struct LIGHTFLAGS
{
	int			Type;		//���C�g�^�C�v�ienum LIGHT_TYPE�j
	int         OnOff;		//���C�g�̃I��or�I�t�X�C�b�`
	int			Dummy[2];
};

// ���C�g�p�萔�o�b�t�@�\����
struct LIGHT_CBUFFER
{
	XMFLOAT4	Direction[LIGHT_MAX];	// ���C�g�̕���
	XMFLOAT4	Position[LIGHT_MAX];	// ���C�g�̈ʒu
	XMFLOAT4	Diffuse[LIGHT_MAX];		// �g�U���̐F
	XMFLOAT4	Ambient[LIGHT_MAX];		// �����̐F
	XMFLOAT4	Attenuation[LIGHT_MAX];	// ������
	LIGHTFLAGS	Flags[LIGHT_MAX];		// ���C�g���
	int			Enable;					// ���C�e�B���O�L���E�����t���O
	int			Dummy[3];				// 16byte���E�p
};

// �t�H�O�p�萔�o�b�t�@�\����
struct FOG_CBUFFER
{
	XMFLOAT4	Fog;					// �t�H�O��
	XMFLOAT4	FogColor;				// �t�H�O�̐F
	int			Enable;					// �t�H�O�L���E�����t���O
	float		Dummy[3];				// 16byte���E�p
};

// �����p�o�b�t�@
struct FUCHI
{
	int			fuchi;
	int			fill[3];
};


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
static void SetLightBuffer(void);


//*****************************************************************************
// �O���[�o���ϐ�
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

// �C���X�^���V���O�`��p
static ID3D11VertexShader*		g_VertexShaderInstancingBillboard = NULL;
static ID3D11PixelShader*		g_PixelShaderNoLighting = NULL;
static ID3D11Buffer*			g_InstanceBuffer = NULL;
static ID3D11Buffer*			g_BillboardBuffer = NULL;

// �V���h�E�}�b�v�֘A 
static ID3D11PixelShader*		g_PixelShaderNonShadow = NULL;

static ID3D11RenderTargetView*	g_RenderTargetViewShadowMap = NULL;
static ID3D11Texture2D*			g_TextureShadowMap = NULL;
static ID3D11ShaderResourceView* g_ShaderResourceViewSM = NULL;
static ID3D11VertexShader*		g_VertexShaderShadowMap = NULL;
static ID3D11PixelShader*		g_PixelShaderShadowMap = NULL;
static ID3D11InputLayout*		g_VertexLayoutShadowMap = NULL;
static ID3D11Buffer*			g_LViewBuffer = NULL;
static ID3D11Buffer*			g_LProjectionBuffer = NULL;

// �h���C�����[�v�֘A
static ID3D11Buffer*			g_TimeBuffer = NULL;
static TIME				g_Time;
static ID3D11PixelShader*		g_PixelShaderWaterfall = NULL;
static ID3D11PixelShader*		g_PixelShaderFireStage = NULL;
static ID3D11PixelShader*		g_PixelShaderDarkStage = NULL;
static ID3D11PixelShader*		g_PixelShaderIceStage = NULL;
static ID3D11PixelShader*		g_PixelShaderSandStage = NULL;


// �g�D�[�������_�����O�֘A
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

void SetLViewMatrix(XMMATRIX *ViewMatrix)														// ��
{
	XMMATRIX view;
	view = *ViewMatrix;
	view = XMMatrixTranspose(view);

	GetDeviceContext()->UpdateSubresource(g_LViewBuffer, 0, NULL, &view, 0, 0);
}

void SetLProjectionMatrix(XMMATRIX *ProjectionMatrix)											// ��
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
	// �t���O���X�V����
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
	// �t���O���X�V����
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
// ����������
//=============================================================================
HRESULT InitRenderer(HINSTANCE hInstance, HWND hWnd, BOOL bWindow)
{
	HRESULT hr = S_OK;

	// �f�o�C�X�A�X���b�v�`�F�[���A�R���e�L�X�g����
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

	//�f�o�b�O�����o�͗p�ݒ�
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

	//�f�o�b�O�����o�͗p�ݒ�
#if defined(_DEBUG) && defined(DEBUG_DISP_TEXTOUT)
	hr = g_SwapChain->ResizeBuffers(0, SCREEN_WIDTH, SCREEN_HEIGHT, DXGI_FORMAT_UNKNOWN, DXGI_SWAP_CHAIN_FLAG_GDI_COMPATIBLE); // N.B. the GDI compatible flag
	if (FAILED(hr))
		return hr;
#endif

	// �����_�[�^�[�Q�b�g�r���[�����A�ݒ�
	ID3D11Texture2D* pBackBuffer = NULL;
	g_SwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), ( LPVOID* )&pBackBuffer );
	g_D3DDevice->CreateRenderTargetView( pBackBuffer, NULL, &g_RenderTargetView );
	pBackBuffer->Release();



	//�X�e���V���p�e�N�X�`���[�쐬
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

	//�X�e���V���^�[�Q�b�g�쐬
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
	ZeroMemory( &dsvd, sizeof(dsvd) );
	dsvd.Format			= td.Format;
	dsvd.ViewDimension	= D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvd.Flags			= 0;
	g_D3DDevice->CreateDepthStencilView( depthTexture, &dsvd, &g_DepthStencilView );


	g_ImmediateContext->OMSetRenderTargets( 1, &g_RenderTargetView, g_DepthStencilView );





	//====================================================================================�@��
	// �V���h�E�}�b�v�������ݗp�e�N�X�`��
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

	// �����_�[�^�[�Q�b�g�r���[�����A�ݒ�
	g_D3DDevice->CreateRenderTargetView(g_TextureShadowMap, NULL, &g_RenderTargetViewShadowMap);

	//�V�F�[�_�[���\�[�X�r���[�쐬
	D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
	ZeroMemory(&srvd, sizeof(srvd));
	srvd.Format = DXGI_FORMAT_R32_FLOAT;
	srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvd.Texture2D.MipLevels = 1;
	g_D3DDevice->CreateShaderResourceView(g_TextureShadowMap, &srvd, &g_ShaderResourceViewSM);

	//=======================================================================================





	// �r���[�|�[�g�ݒ�
	D3D11_VIEWPORT vp;
	vp.Width = (FLOAT)SCREEN_WIDTH;
	vp.Height = (FLOAT)SCREEN_HEIGHT;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	g_ImmediateContext->RSSetViewports( 1, &vp );



	// ���X�^���C�U�X�e�[�g�쐬
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

	// �J�����O���[�h�ݒ�iCCW�j
	SetCullingMode(CULL_MODE_BACK);



	// �u�����h�X�e�[�g�̍쐬
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

	// �A���t�@�u�����h�ݒ�
	SetBlendState(BLEND_MODE_ALPHABLEND);




	// �[�x�X�e���V���X�e�[�g�쐬
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	ZeroMemory( &depthStencilDesc, sizeof( depthStencilDesc ) );
	depthStencilDesc.DepthEnable = TRUE;
	depthStencilDesc.DepthWriteMask	= D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	depthStencilDesc.StencilEnable = FALSE;

	g_D3DDevice->CreateDepthStencilState( &depthStencilDesc, &g_DepthStateEnable );//�[�x�L���X�e�[�g

	//depthStencilDesc.DepthEnable = FALSE;
	depthStencilDesc.DepthWriteMask	= D3D11_DEPTH_WRITE_MASK_ZERO;
	g_D3DDevice->CreateDepthStencilState( &depthStencilDesc, &g_DepthStateDisable );//�[�x�����X�e�[�g

	// �[�x�X�e���V���X�e�[�g�ݒ�
	SetDepthEnable(true);



	// �T���v���[�X�e�[�g�ݒ�
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



	// ���_�V�F�[�_�R���p�C���E����
	ID3DBlob* pErrorBlob;
	ID3DBlob* pVSBlob = NULL;
	DWORD shFlag = D3DCOMPILE_ENABLE_STRICTNESS;

#if defined(_DEBUG) && defined(DEBUG_SHADER)
	shFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	// �f�t�H���g�o�[�e�b�N�X�V�F�[�_�[
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

	// ���̓��C�A�E�g����
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


	// �V���h�E�}�b�v�������ݗp�o�[�e�b�N�X�V�F�[�_�[
	pVSBlob = NULL;
	hr = D3DX11CompileFromFile("shader.hlsl", NULL, NULL, "VertexShaderShadowMap", "vs_4_0", shFlag, 0, NULL, &pVSBlob, &pErrorBlob, NULL);
	if (FAILED(hr))
	{
		MessageBox(NULL, (char*)pErrorBlob->GetBufferPointer(), "VS", MB_OK | MB_ICONERROR);
	}

	g_D3DDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), NULL, &g_VertexShaderShadowMap);

	// ���̓��C�A�E�g����
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

	// �s�N�Z���V�F�[�_�R���p�C���E����
	CreateShader("shader.hlsl", "PixelShaderPolygon", &g_PixelShader, shFlag);					// �f�t�H���g�s�N�Z���V�F�[�_�[
	CreateShader("shader.hlsl", "PixelShaderPolygonToon", &g_PixelShaderToon, shFlag);			// �g�D�[���s�N�Z���V�F�[�_�[
	CreateShader("shader.hlsl", "PixelShaderPolygonHoldToon", &g_PixelShaderHoldToon, shFlag);	// �g�D�[���s�N�Z���V�F�[�_�[
	CreateShader("shader.hlsl", "PixelShaderPolygonNonShadow", &g_PixelShaderNonShadow, shFlag);// �V���h�E�}�b�v��K�p�s�N�Z���V�F�[�_�[
	CreateShader("shader.hlsl", "PixelShaderShadowMap", &g_PixelShaderShadowMap, shFlag);		// �V���h�E�}�b�v�������ݗp�s�N�Z���V�F�[�_�[
	CreateShader("shader.hlsl", "PixelShaderWaterfall", &g_PixelShaderWaterfall, shFlag);		// ��p�s�N�Z���V�F�[�_�[
	CreateShader("shader.hlsl", "PixelShaderIceStage", &g_PixelShaderIceStage, shFlag);			// �X�X�e�[�W�p�s�N�Z���V�F�[�_�[
	CreateShader("shader.hlsl", "PixelShaderSandStage", &g_PixelShaderSandStage, shFlag);		// ���X�e�[�W�p�s�N�Z���V�F�[�_�[
	CreateShader("shader.hlsl", "PixelShaderFireStage", &g_PixelShaderFireStage, shFlag);		// ���X�e�[�W�p�s�N�Z���V�F�[�_�[
	CreateShader("shader.hlsl", "PixelShaderDarkStage", &g_PixelShaderDarkStage, shFlag);		// �ŃX�e�[�W�p�s�N�Z���V�F�[�_�[

	// �C���X�^���V���O�`��p�V�F�[�_�R���p�C���E����
	CreateShader("shader.hlsl", "VertexShaderInstancingBillboard", &g_VertexShaderInstancingBillboard, shFlag);
	CreateShader("shader.hlsl", "PixelShaderPolygonNoLighting", &g_PixelShaderNoLighting, shFlag);

	// �萔�o�b�t�@����
	D3D11_BUFFER_DESC hBufferDesc;
	hBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	hBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	hBufferDesc.CPUAccessFlags = 0;
	hBufferDesc.MiscFlags = 0;
	hBufferDesc.StructureByteStride = sizeof(float);

	CreateBuffer(&hBufferDesc, 0, &g_WVPMatrixBuffer, sizeof(WVP_MATRIX));		// �ϊ��}�g���N�X�Q
	CreateBuffer(&hBufferDesc, 1, &g_LViewBuffer, sizeof(XMMATRIX));			// ���C�g�r���[�}�g���N�X
	CreateBuffer(&hBufferDesc, 2, &g_LProjectionBuffer, sizeof(XMMATRIX));		// ���C�g�v���W�F�N�V�����}�g���N�X
	CreateBuffer(&hBufferDesc, 3, &g_MaterialBuffer, sizeof(MATERIAL_CBUFFER));	// �}�e���A�����
	CreateBuffer(&hBufferDesc, 4, &g_LightBuffer, sizeof(LIGHT_CBUFFER));		// ���C�g���
	CreateBuffer(&hBufferDesc, 5, &g_FogBuffer, sizeof(FOG_CBUFFER));			// �t�H�O���
	CreateBuffer(&hBufferDesc, 6, &g_FuchiBuffer, sizeof(FUCHI));				// �����
	CreateBuffer(&hBufferDesc, 7, &g_CameraBuffer, sizeof(XMFLOAT4));			// �J����
	CreateBuffer(&hBufferDesc, 8, &g_TimeBuffer, sizeof(TIME));					// ���ԁi�o�߃t���[���j���
	CreateBuffer(&hBufferDesc, 9, &g_FuchiColorBuffer, sizeof(XMFLOAT4));		// �����F
	CreateBuffer(&hBufferDesc,10, &g_BillboardBuffer, sizeof(XMMATRIX));		// �r���{�[�h�p
	
	// �C���X�^���X
	hBufferDesc.Usage = D3D11_USAGE_DYNAMIC;				// CPU�������݂�������
	hBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;	// CPU�������݂�������
	hBufferDesc.ByteWidth = sizeof(INSTANCE);
	g_D3DDevice->CreateBuffer(&hBufferDesc, NULL, &g_InstanceBuffer);
	g_ImmediateContext->VSSetConstantBuffers(11, 1, &g_InstanceBuffer);
	g_ImmediateContext->PSSetConstantBuffers(11, 1, &g_InstanceBuffer);


	// ���̓��C�A�E�g�ݒ�
	g_ImmediateContext->IASetInputLayout( g_VertexLayout );

	// �V�F�[�_�ݒ�
	g_ImmediateContext->VSSetShader( g_VertexShader, NULL, 0 );
	g_ImmediateContext->PSSetShader( g_PixelShader, NULL, 0 );

	//���C�g������
	ZeroMemory(&g_Light, sizeof(LIGHT_CBUFFER));
	g_Light.Direction[0] = XMFLOAT4(1.0f, -1.0f, 1.0f, 0.0f);
	g_Light.Diffuse[0] = XMFLOAT4(0.9f, 0.9f, 0.9f, 1.0f);
	g_Light.Ambient[0] = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	g_Light.Flags[0].Type = LIGHT_TYPE_DIRECTIONAL;
	SetLightBuffer();


	//�}�e���A��������
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	material.Ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);


	// �g�D�[�������_�����O�p�J���[���b�N�A�b�v�e�[�u���̍쐬
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
// �I������
//=============================================================================
void UninitRenderer(void)
{
	// �g�D�[�������_�����O�p�J���[���b�N�A�b�v�e�[�u���̉��
	for (int i = 0; i < TOON_TEXTURE_MAX; i++)
	{
		if (g_ShaderResourceViewToonMap[i])
		{
			g_ShaderResourceViewToonMap[i]->Release();
			g_ShaderResourceViewToonMap[i] = NULL;
		}
	}

	// �I�u�W�F�N�g���
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

	// �C���X�^���V���O�`��֘A
	if (g_VertexShaderInstancingBillboard)	g_VertexShaderInstancingBillboard->Release();
	if (g_PixelShaderNoLighting)	g_PixelShaderNoLighting->Release();
	if (g_InstanceBuffer)		g_InstanceBuffer->Release();
	if (g_BillboardBuffer)		g_BillboardBuffer->Release();
}



//=============================================================================
// �o�b�N�o�b�t�@�N���A
//=============================================================================
//float g_ClearColor[4] = { 0.2f, 0.5f, 1.0f, 1.0f };
float g_ClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
void Clear(void)
{
	// �o�b�N�o�b�t�@�N���A
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
// �v���[���g
//=============================================================================
void Present(void)
{
	g_SwapChain->Present( 0, 0 );
}


//=============================================================================
// �f�o�b�O�p�e�L�X�g�o��
//=============================================================================
void DebugTextOut(char* text, int x, int y)
{
#if defined(_DEBUG) && defined(DEBUG_DISP_TEXTOUT)
	HRESULT hr;

	//�o�b�N�o�b�t�@����T�[�t�F�X���擾����
	IDXGISurface1* pBackSurface = NULL;
	hr = g_SwapChain->GetBuffer(0, __uuidof(IDXGISurface1), (void**)&pBackSurface);

	if (SUCCEEDED(hr))
	{
		//�擾�����T�[�t�F�X����f�o�C�X�R���e�L�X�g���擾����
		HDC hdc;
		hr = pBackSurface->GetDC(FALSE, &hdc);

		if (SUCCEEDED(hr))
		{
			//�����F�𔒂ɕύX
			SetTextColor(hdc, RGB(255, 255, 255));

			//�w�i�𓧖��ɕύX
			SetBkMode(hdc, TRANSPARENT);

			RECT rect;
			rect.left = 0;
			rect.top = 0;
			rect.right = SCREEN_WIDTH;
			rect.bottom = SCREEN_HEIGHT;

			//�e�L�X�g�o��
			DrawText(hdc, text, (int)strlen(text), &rect, DT_LEFT);

			//�f�o�C�X�R���e�L�X�g���������
			pBackSurface->ReleaseDC(NULL);
		}
		//�T�[�t�F�X���������
		pBackSurface->Release();

		//�����_�����O�^�[�Q�b�g�����Z�b�g�����̂ŃZ�b�g���Ȃ���
		g_ImmediateContext->OMSetRenderTargets(1, &g_RenderTargetView, g_DepthStencilView);
	}
#endif
}


//=============================================================================
// �V�F�[�_�[���[�h�̃Z�b�g
//=============================================================================
void SetShaderMode(int mode)
{
	switch (mode)
	{
	case SHADER_MODE_DEFAULT:
		// ���̓��C�A�E�g�ݒ�
		g_ImmediateContext->IASetInputLayout(g_VertexLayout);

		//�V�F�[�_�[��ʏ�̂��̂ɃZ�b�g
		g_ImmediateContext->VSSetShader(g_VertexShader, NULL, 0);
		g_ImmediateContext->PSSetShader(g_PixelShader, NULL, 0);

		break;

	case SHADER_MODE_TOON:
		// ���̓��C�A�E�g�ݒ�
		g_ImmediateContext->IASetInputLayout(g_VertexLayout);

		//�V�F�[�_�[��ʏ�̂��̂ɃZ�b�g
		g_ImmediateContext->VSSetShader(g_VertexShader, NULL, 0);
		g_ImmediateContext->PSSetShader(g_PixelShaderToon, NULL, 0);

		break;

	case SHADER_MODE_HOLD_TOON:
		// ���̓��C�A�E�g�ݒ�
		g_ImmediateContext->IASetInputLayout(g_VertexLayout);

		//�V�F�[�_�[��ʏ�̂��̂ɃZ�b�g
		g_ImmediateContext->VSSetShader(g_VertexShader, NULL, 0);
		g_ImmediateContext->PSSetShader(g_PixelShaderHoldToon, NULL, 0);

		break;

	case SHADER_MODE_NON_SHADOW:
		// ���̓��C�A�E�g�ݒ�
		g_ImmediateContext->IASetInputLayout(g_VertexLayout);

		//�V�F�[�_�[��ʏ�̂��̂ɃZ�b�g
		g_ImmediateContext->VSSetShader(g_VertexShader, NULL, 0);
		g_ImmediateContext->PSSetShader(g_PixelShaderNonShadow, NULL, 0);

		break;

	case SHADER_MODE_SHADOWMAPDRAW:
		// ���̓��C�A�E�g�ݒ�
		g_ImmediateContext->IASetInputLayout(g_VertexLayoutShadowMap);

		//�V�F�[�_�[��Z�o�b�t�@�������ݗp�̂��̂ɃZ�b�g
		g_ImmediateContext->VSSetShader(g_VertexShaderShadowMap, NULL, 0);
		g_ImmediateContext->PSSetShader(g_PixelShaderShadowMap, NULL, 0);

		break;

	case SHADER_MODE_WATERFALL:
		// ���̓��C�A�E�g�ݒ�
		g_ImmediateContext->IASetInputLayout(g_VertexLayout);

		//�V�F�[�_�[���p�̂��̂ɃZ�b�g
		g_ImmediateContext->VSSetShader(g_VertexShader, NULL, 0);
		g_ImmediateContext->PSSetShader(g_PixelShaderWaterfall, NULL, 0);
		break;

	case SHADER_MODE_FIRESTAGE:
		// ���̓��C�A�E�g�ݒ�
		g_ImmediateContext->IASetInputLayout(g_VertexLayout);

		//�V�F�[�_�[���p�̂��̂ɃZ�b�g
		g_ImmediateContext->VSSetShader(g_VertexShader, NULL, 0);
		g_ImmediateContext->PSSetShader(g_PixelShaderFireStage, NULL, 0);
		break;

	case SHADER_MODE_DARKSTAGE:
		// ���̓��C�A�E�g�ݒ�
		g_ImmediateContext->IASetInputLayout(g_VertexLayout);

		//�V�F�[�_�[���p�̂��̂ɃZ�b�g
		g_ImmediateContext->VSSetShader(g_VertexShader, NULL, 0);
		g_ImmediateContext->PSSetShader(g_PixelShaderDarkStage, NULL, 0);
		break;

	case SHADER_MODE_ICESTAGE:
		// ���̓��C�A�E�g�ݒ�
		g_ImmediateContext->IASetInputLayout(g_VertexLayout);

		//�V�F�[�_�[���p�̂��̂ɃZ�b�g
		g_ImmediateContext->VSSetShader(g_VertexShader, NULL, 0);
		g_ImmediateContext->PSSetShader(g_PixelShaderIceStage, NULL, 0);
		break;

	case SHADER_MODE_SANDSTAGE:
		// ���̓��C�A�E�g�ݒ�
		g_ImmediateContext->IASetInputLayout(g_VertexLayout);

		//�V�F�[�_�[���p�̂��̂ɃZ�b�g
		g_ImmediateContext->VSSetShader(g_VertexShader, NULL, 0);
		g_ImmediateContext->PSSetShader(g_PixelShaderSandStage, NULL, 0);
		break;
	}
}


//=============================================================================
// �����_�\�^�[�Q�b�g�̕ύX
//=============================================================================
void LimitRenderTargetViewShadowMap(BOOL mode)
{
	if (mode == TRUE)
	{
		// �����_�\�^�[�Q�b�g���V���h�E�}�b�v�������ݗp�̂��̂ɃZ�b�g
		g_ImmediateContext->OMSetRenderTargets(1, &g_RenderTargetViewShadowMap, g_DepthStencilView);
	}
	else
	{
		// �����_�\�^�[�Q�b�g���f�t�H���g�ɃZ�b�g
		g_ImmediateContext->OMSetRenderTargets(1, &g_RenderTargetView, g_DepthStencilView);
	}
}


//=============================================================================
// �V�F�[�_�[���\�[�X�Ƃ��ẴV���h�E�}�b�v�̃Z�b�g�ƃN���A
//=============================================================================
void SetShaderResourceShadowMap(BOOL mode)
{
	if (mode == TRUE)
	{
		// �V�F�[�_�[���\�[�X�Ƃ��ăV���h�E�}�b�v�������ݗp�e�N�X�`����(���W�X�^t1��)�Z�b�g
		GetDeviceContext()->PSSetShaderResources(1, 1, &g_ShaderResourceViewSM);
	}
	else
	{
		// �V�F�[�_�[���\�[�X���N���A
		ID3D11ShaderResourceView* clearsrv = NULL;
		GetDeviceContext()->PSSetShaderResources(1, 1, &clearsrv);
	}
}


//=============================================================================
// �f�v�X�X�e���V���r���[�̃N���A
//=============================================================================
void ClearOnlyDepthStencil(void)
{
	g_ImmediateContext->ClearDepthStencilView(g_DepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);	//�[�x�X�e���V���r���[�N���A
}




//=============================================================================
// �E�B���h�E�̃T�C�Y�ύX�ɑΉ�
//=============================================================================
void Resize(long winW, long winH)
{
	g_ImmediateContext->OMSetRenderTargets(0, NULL, NULL);
	g_RenderTargetView->Release();
	g_RenderTargetViewShadowMap->Release();
	g_DepthStencilView->Release();
	g_SwapChain->ResizeBuffers(1, winW, winH, DXGI_FORMAT_R8G8B8A8_UNORM, 0);

	// �o�b�N�o�b�t�@�̃����_�[�^�[�Q�b�g�r���[�Đݒ�
	ID3D11Texture2D* pBackBuffer = NULL;
	g_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);	// 0�Ԗڂ̃o�b�N�o�b�t�@��2D�Ƃ��Ď擾
	g_D3DDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_RenderTargetView);	// ���̃o�b�N�o�b�t�@�������_�[�^�[�Q�b�g�r���[�ɐݒ肵�A�C���^�[�t�F�[�X���󂯎��
	pBackBuffer->Release();

	// �[�x�E�X�e���V���e�N�X�`�������\�[�X�Ƃ��č쐬
	ID3D11Texture2D* depthTexture;
	D3D11_TEXTURE2D_DESC td;
	ZeroMemory(&td, sizeof(td));
	td.Width = winW;									// �o�b�t�@�̉���
	td.Height = winH;									// �o�b�t�@�̏c��
	td.MipLevels = 1;									// �~�j�}�b�v�̃��x�����w��i1�̓~�j�}�b�v�Ȃ��j
	td.ArraySize = 1;									// �e�N�X�`���z��̃T�C�Y���w��
	td.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;			// �e�N�X�`���̃t�H�[�}�b�g���w��iDepth24bit�AStencil8bit�A0.0~1.0�j
	td.SampleDesc.Count = 1;							// �}���`�T���v�����O�̐ݒ���w��
	td.SampleDesc.Quality = 0;							// �}���`�T���v�����O�̐ݒ���w��
	td.Usage = D3D11_USAGE_DEFAULT;						// �e�N�X�`���̎g�p���@���w��
	td.BindFlags = D3D11_BIND_DEPTH_STENCIL;			// Bind�̐ݒ���w��
	td.CPUAccessFlags = 0;								// �e�N�X�`���ւ�CPU�̃A�N�Z�X������ݒ�i0�ł����j
	td.MiscFlags = 0;									// �e�N�X�`���I�v�V�����̃t���O�i0�ł����j
	g_D3DDevice->CreateTexture2D(&td, NULL, &depthTexture);

	// �쐬�����e�N�X�`����[�x�E�X�e���V���r���[�Ƃ��Đݒ�
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
	ZeroMemory(&dsvd, sizeof(dsvd));
	dsvd.Format = td.Format;							// View�̃t�H�[�}�b�g���w��
	dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;	// �e�N�X�`���̎�ނ��w��
	dsvd.Flags = 0;										// �ǂݎ���p�t���O
	g_D3DDevice->CreateDepthStencilView(depthTexture, &dsvd, &g_DepthStencilView);

	g_ImmediateContext->OMSetRenderTargets(1, &g_RenderTargetView, g_DepthStencilView);
	
	//==============
	// �V���h�E�}�b�v�������ݗp�e�N�X�`��
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

	// �����_�[�^�[�Q�b�g�r���[�����A�ݒ�
	g_D3DDevice->CreateRenderTargetView(g_TextureShadowMap, NULL, &g_RenderTargetViewShadowMap);

	//�V�F�[�_�[���\�[�X�r���[�쐬
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
	g_ImmediateContext->RSSetViewports(1, vp);			// ���X�^���C�U�Ŏg�p�i�����͔z��̒����j
}

