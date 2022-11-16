//=============================================================================
//
// �����_�����O���� [renderer.cpp]
// Author : ���] �đ�
//
//=============================================================================
#include "main.h"
#include "renderer.h"

// �f�o�b�O�p��ʃe�L�X�g�o�͂�L���ɂ���
#define DEBUG_DISP_TEXTOUT
// �V�F�[�_�[�f�o�b�O�ݒ��L���ɂ���
//#define DEBUG_SHADER

//*****************************************************************************
// �\����
//*****************************************************************************
float g_Filter[FILTER_MODE_NUM][3][3] = {
	{// �t�B���^�Ȃ�
		{ 0.0f, 0.0f, 0.0f},
		{ 0.0f, 1.0f, 0.0f},
		{ 0.0f, 0.0f, 0.0f},
	},
	{// ���ω��t�B���^
		{ 1.0f / 9.0f, 1.0f / 9.0f, 1.0f / 9.0f},
		{ 1.0f / 9.0f, 1.0f / 9.0f, 1.0f / 9.0f},
		{ 1.0f / 9.0f, 1.0f / 9.0f, 1.0f / 9.0f},
	},
	{// �K�E�V�A���t�B���^
		{ 1.0f / 16.0f, 2.0f / 16.0f, 1.0f / 16.0f},
		{ 2.0f / 16.0f, 4.0f / 16.0f, 2.0f / 16.0f},
		{ 1.0f / 16.0f, 2.0f / 16.0f, 1.0f / 16.0f},
	},
	{// �N�s���t�B���^��
		{ -1.0f / 3.0f, -1.0f / 3.0f, -1.0f / 3.0f},
		{ -1.0f / 3.0f, 11.0f / 3.0f, -1.0f / 3.0f},
		{ -1.0f / 3.0f, -1.0f / 3.0f, -1.0f / 3.0f},
	},
	{// �N�s���t�B���^��
		{ -1.0f, -1.0f, -1.0f},
		{ -1.0f,  9.0f, -1.0f},
		{ -1.0f, -1.0f, -1.0f},
	},
	{// ���v���V�A���t�B���^
		{ 1.0f, 1.0f, 1.0f},
		{ 1.0f,-8.0f, 1.0f},
		{ 1.0f, 1.0f, 1.0f},
	},
	{// ���v���V�A���t�B���^�J���[
		{ 1.0f, 1.0f, 1.0f},
		{ 1.0f,-7.0f, 1.0f},
		{ 1.0f, 1.0f, 1.0f},
	},
	{// �v�����[�E�B�b�g�t�B���^��
		{ -1.0f, 0.0f, 1.0f},
		{ -1.0f, 0.0f, 1.0f},
		{ -1.0f, 0.0f, 1.0f},
	},
	{// �v�����[�E�B�b�g�t�B���^�c
		{ 1.0f, 1.0f, 1.0f},
		{ 0.0f, 0.0f, 0.0f},
		{-1.0f,-1.0f,-1.0f},
	},
	{// �\�[�x���t�B���^��
		{ -1.0f, 0.0f, 1.0f},
		{ -2.0f, 0.0f, 2.0f},
		{ -1.0f, 0.0f, 1.0f},
	},
	{// �\�[�x���t�B���^��
		{ 1.0f, 2.0f, 1.0f},
		{ 0.0f, 0.0f, 0.0f},
		{-1.0f,-2.0f,-1.0f},
	},
	//{// �΂߂ڂ���
	//	{ 1.0f / 3.0f, 0.0f, 0.0f},
	//	{ 0.0f, 1.0f / 3.0f, 0.0f},
	//	{ 0.0f, 0.0f, 1.0f / 3.0f},
	//},
};

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static D3D_FEATURE_LEVEL       g_FeatureLevel = D3D_FEATURE_LEVEL_11_0;

// �W�����u
static ID3D11Device*           g_D3DDevice = NULL;
static ID3D11DeviceContext*    g_ImmediateContext = NULL;
static IDXGISwapChain*         g_SwapChain = NULL;
static ID3D11RenderTargetView* g_RenderTargetView = NULL;
static ID3D11DepthStencilView* g_DepthStencilView = NULL;

// �m�[�}���}�b�v�p
static ID3D11RenderTargetView*	g_RenderTargetViewNormalMap = NULL;
static ID3D11ShaderResourceView*g_NormalMapTexture = NULL;
static ID3D11VertexShader*		g_VSNormalMap = NULL;
static ID3D11PixelShader*		g_PSNormalMap = NULL;

// �f�v�X�}�b�v�p
static ID3D11RenderTargetView*	g_RenderTargetViewDepthMap = NULL;
static ID3D11ShaderResourceView*g_DepthMapTexture = NULL;
static ID3D11VertexShader*		g_VSDepthMap = NULL;
static ID3D11PixelShader*		g_PSDepthMap = NULL;

// �t�B���^�[�p
static ID3D11VertexShader*		g_VSFilter = NULL;
static ID3D11PixelShader*		g_PSFilter = NULL;
static ID3D11PixelShader*		g_PSOnlyTex = NULL;
static ID3D11PixelShader*		g_PSMosaic = NULL;
static ID3D11PixelShader*		g_PSCA = NULL;
static ID3D11PixelShader*		g_PSMonitoring = NULL;
static ID3D11PixelShader*		g_PSOldGame = NULL;

// �t�B���^�[�K�p�p�����_�[�^�[�Q�b�g
static ID3D11RenderTargetView*	g_RenderTargetViewWrite[2] = { NULL, NULL };
static ID3D11ShaderResourceView*g_WrittenTexture[2] = { NULL, NULL };
static int						g_CurrentTarget = 0;
static int						g_CurrentResource = 0;

// ���C�g�p�����_�[�^�[�Q�b�g
static ID3D11RenderTargetView*	g_RenderTargetViewLight[2] = { NULL, NULL };
static ID3D11ShaderResourceView*g_LightTexture[2] = { NULL, NULL };
static int						g_CurrentTargetLight = 0;
static int						g_CurrentResourceLight = 0;

// ���̑��V�F�[�_�[
static ID3D11VertexShader*		g_VS = NULL;
static ID3D11VertexShader*		g_VSOutline = NULL;
static ID3D11VertexShader*		g_VSOutlineInstancing = NULL;
static ID3D11VertexShader*		g_VSTube = NULL;
static ID3D11VertexShader*		g_VSGimmick = NULL;
static ID3D11VertexShader*		g_VSPlayer = NULL;
static ID3D11VertexShader*		g_VSEX = NULL;
static ID3D11PixelShader*		g_PS = NULL;
static ID3D11PixelShader*		g_PSOutline = NULL;
static ID3D11PixelShader*		g_PSAL = NULL;
static ID3D11PixelShader*		g_PSDL = NULL;
static ID3D11PixelShader*		g_PSPL = NULL;
static ID3D11PixelShader*		g_PSSL = NULL;
static ID3D11PixelShader*		g_PSLL = NULL;
static ID3D11PixelShader*		g_PSLight = NULL;
static ID3D11GeometryShader*	g_GSDL = NULL;
static ID3D11GeometryShader*	g_GSPL = NULL;
static ID3D11GeometryShader*	g_GSSL = NULL;
static ID3D11GeometryShader*	g_GSLLPlayer = NULL;
static ID3D11GeometryShader*	g_GSLLNonPlayer = NULL;
static ID3D11GeometryShader*	g_GSEX = NULL;

// �C���v�b�g���C�A�E�g
static ID3D11InputLayout*		g_VertexLayout = NULL;

// �萔�o�b�t�@
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

// �X�N���[���̒��_�o�b�t�@
static ID3D11Buffer*			g_ScreenVertexBuffer = NULL;

// �e�\����
static MATRIX					g_Matrix;
static XMFLOAT4					g_LightVector;
static XMFLOAT4					g_CameraPosition;
static XMFLOAT4					g_BackGroundColor = { 0.8f, 0.9f, 1.0f, 1.0f };
//static XMFLOAT4					g_BackGroundColor = { 0.0f, 0.0f, 0.0f, 1.0f };

// �f�v�X�X�e���V���ݒ�
static ID3D11DepthStencilState* g_DepthStateEnable;
static ID3D11DepthStencilState* g_DepthStateDisable;
static ID3D11DepthStencilState* g_StencilWrite;
static ID3D11DepthStencilState* g_StencilRead;

// �u�����h�ݒ�
static ID3D11BlendState*		g_BlendStateNone;
static ID3D11BlendState*		g_BlendStateAlphaBlend;
static ID3D11BlendState*		g_BlendStateAdd;
static ID3D11BlendState*		g_BlendStateSubtract;
static BLEND_MODE				g_BlendStateParam;

// ���X�^���C�U�ݒ�
static ID3D11RasterizerState*	g_RasterStateCullOff;
static ID3D11RasterizerState*	g_RasterStateCullCW;
static ID3D11RasterizerState*	g_RasterStateCullCCW;

// �C���X�^���V���O�`��p
static ID3D11VertexShader*		g_VSInstancing = NULL;
static ID3D11VertexShader*		g_VSInstancingTexture = NULL;
static ID3D11PixelShader*		g_PSInstancingOnlyTex = NULL;
static ID3D11Buffer*			g_InstanceBuffer = NULL;
static ID3D11Buffer*			g_BillboardBuffer = NULL;

//*****************************************************************************
// �I������
//*****************************************************************************
void UninitRenderer(void)
{
	// �m�[�}���}�b�v�p
	if (g_RenderTargetViewNormalMap)	g_RenderTargetViewNormalMap->Release();
	if (g_NormalMapTexture)		g_NormalMapTexture->Release();
	if (g_VSNormalMap)			g_VSNormalMap->Release();
	if (g_PSNormalMap)			g_PSNormalMap->Release();

	// �f�v�X�}�b�v�p
	if (g_RenderTargetViewDepthMap)	g_RenderTargetViewDepthMap->Release();
	if (g_DepthMapTexture)		g_DepthMapTexture->Release();
	if (g_VSDepthMap)			g_VSDepthMap->Release();
	if (g_PSDepthMap)			g_PSDepthMap->Release();

	// �t�B���^�[�p�V�F�[�_�[
	if (g_VSFilter)				g_VSFilter->Release();
	if (g_PSFilter)				g_PSFilter->Release();
	if (g_PSOnlyTex)			g_PSOnlyTex->Release();
	if (g_PSMosaic)				g_PSMosaic->Release();
	if (g_PSCA)					g_PSCA->Release();
	if (g_PSMonitoring)			g_PSMonitoring->Release();
	if (g_PSOldGame)			g_PSOldGame->Release();

	// �t�B���^�[�K�p�p�����_�[�^�[�Q�b�g
	if (g_RenderTargetViewWrite[0])	g_RenderTargetViewWrite[0]->Release();
	if (g_RenderTargetViewWrite[1])	g_RenderTargetViewWrite[1]->Release();
	if (g_WrittenTexture[0])	g_WrittenTexture[0]->Release();
	if (g_WrittenTexture[1])	g_WrittenTexture[1]->Release();

	// ���C�g�p�����_�[�^�[�Q�b�g
	if (g_RenderTargetViewLight[0]) g_RenderTargetViewLight[0]->Release();
	if (g_RenderTargetViewLight[1]) g_RenderTargetViewLight[1]->Release();
	if (g_LightTexture[0])			g_LightTexture[0]->Release();
	if (g_LightTexture[1])			g_LightTexture[1]->Release();

	// ���̑��V�F�[�_�[
	if (g_VS)					g_VS->Release();
	if (g_VSOutline)			g_VSOutline->Release();
	if (g_VSOutlineInstancing)	g_VSOutlineInstancing->Release();
	if (g_VSTube)				g_VSTube->Release();
	if (g_VSGimmick)			g_VSGimmick->Release();
	if (g_VSPlayer)				g_VSPlayer->Release();
	if (g_VSEX)					g_VSEX->Release();
	if (g_PS)					g_PS->Release();
	if (g_PSOutline)			g_PSOutline->Release();
	if (g_PSAL)					g_PSAL->Release();
	if (g_PSDL)					g_PSDL->Release();
	if (g_PSPL)					g_PSPL->Release();
	if (g_PSSL)					g_PSSL->Release();
	if (g_PSLL)					g_PSLL->Release();
	if (g_PSLight)				g_PSLight->Release();
	if (g_GSDL)					g_GSDL->Release();
	if (g_GSPL)					g_GSPL->Release();
	if (g_GSSL)					g_GSSL->Release();
	if (g_GSLLPlayer)			g_GSLLPlayer->Release();
	if (g_GSLLNonPlayer)		g_GSLLNonPlayer->Release();
	if (g_GSEX)					g_GSEX->Release();

	// ���_���C�A�E�g
	if (g_VertexLayout)			g_VertexLayout->Release();

	// �萔�o�b�t�@
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

	// �X�N���[���̒��_�o�b�t�@
	if (g_ScreenVertexBuffer)	g_ScreenVertexBuffer->Release();

	// �f�v�X�X�e���V���ݒ�
	if (g_DepthStateEnable)		g_DepthStateEnable->Release();
	if (g_DepthStateDisable)	g_DepthStateDisable->Release();
	if (g_StencilWrite)			g_StencilWrite->Release();
	if (g_StencilRead)			g_StencilRead->Release();

	// �u�����h�ݒ�
	if (g_BlendStateNone)		g_BlendStateNone->Release();
	if (g_BlendStateAlphaBlend)	g_BlendStateAlphaBlend->Release();
	if (g_BlendStateAdd)		g_BlendStateAdd->Release();
	if (g_BlendStateSubtract)	g_BlendStateSubtract->Release();

	// ���X�^���C�U�ݒ�
	if (g_RasterStateCullOff)	g_RasterStateCullOff->Release();
	if (g_RasterStateCullCW)	g_RasterStateCullCW->Release();
	if (g_RasterStateCullCCW)	g_RasterStateCullCCW->Release();

	// �C���X�^���V���O�`��֘A
	if (g_VSInstancing)			g_VSInstancing->Release();
	if (g_VSInstancingTexture)	g_VSInstancingTexture->Release();
	if (g_PSInstancingOnlyTex)	g_PSInstancingOnlyTex->Release();
	if (g_InstanceBuffer)		g_InstanceBuffer->Release();
	if (g_BillboardBuffer)		g_BillboardBuffer->Release();

	// ��{���u
	if (g_ImmediateContext)		g_ImmediateContext->ClearState();
	if (g_RenderTargetView)		g_RenderTargetView->Release();
	if (g_SwapChain)			g_SwapChain->Release();
	if (g_ImmediateContext)		g_ImmediateContext->Release();
	if (g_D3DDevice)			g_D3DDevice->Release();
}

//*****************************************************************************
// ����������
//*****************************************************************************
HRESULT InitRenderer(HINSTANCE hInstance, HWND hWnd, BOOL bWindow)
{
	HRESULT hr = S_OK;

	// �f�o�C�X�A�X���b�v�`�F�[���A�R���e�L�X�g����
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

		//�f�o�b�O�����o�͗p�ݒ�
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

		//�f�o�b�O�����o�͗p�ݒ�
#if defined(_DEBUG) && defined(DEBUG_DISP_TEXTOUT)
		hr = g_SwapChain->ResizeBuffers(0, SCREEN_WIDTH, SCREEN_HEIGHT, DXGI_FORMAT_UNKNOWN, DXGI_SWAP_CHAIN_FLAG_GDI_COMPATIBLE); // N.B. the GDI compatible flag
		if (FAILED(hr))
			return hr;
#endif
	}

	// �����_�[�^�[�Q�b�g�r���[�����A�ݒ�
	{
		ID3D11Texture2D* pBackBuffer = NULL;
		g_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
		g_D3DDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_RenderTargetView);
		pBackBuffer->Release();
	}

	//�X�e���V���p�e�N�X�`���[�쐬
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

		//�X�e���V���^�[�Q�b�g�쐬
		D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
		ZeroMemory(&dsvd, sizeof(dsvd));
		dsvd.Format = td.Format;
		dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		dsvd.Flags = 0;
		g_D3DDevice->CreateDepthStencilView(depthTexture, &dsvd, &g_DepthStencilView);

		g_ImmediateContext->OMSetRenderTargets(1, &g_RenderTargetView, g_DepthStencilView);
	}

	// �������ݗp�e�N�X�`���쐬
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

		// �����_�[�^�[�Q�b�g�r���[�����A�ݒ�
		g_D3DDevice->CreateRenderTargetView(writeTexture, NULL, &g_RenderTargetViewWrite[i]);

		//�V�F�[�_�[���\�[�X�r���[�쐬
		D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
		ZeroMemory(&srvd, sizeof(srvd));
		srvd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvd.Texture2D.MipLevels = 1;
		g_D3DDevice->CreateShaderResourceView(writeTexture, &srvd, &g_WrittenTexture[i]);
	}

	// �������ݗp�e�N�X�`���쐬
	for (int i = 0; i < 2; i++)
	{
		ID3D11Texture2D* lightTexture = NULL;
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
		g_D3DDevice->CreateTexture2D(&td, NULL, &lightTexture);

		// �����_�[�^�[�Q�b�g�r���[�����A�ݒ�
		g_D3DDevice->CreateRenderTargetView(lightTexture, NULL, &g_RenderTargetViewLight[i]);

		//�V�F�[�_�[���\�[�X�r���[�쐬
		D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
		ZeroMemory(&srvd, sizeof(srvd));
		srvd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvd.Texture2D.MipLevels = 1;
		g_D3DDevice->CreateShaderResourceView(lightTexture, &srvd, &g_LightTexture[i]);
	}

	// �m�[�}���}�b�v�p�e�N�X�`���쐬
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

		// �����_�[�^�[�Q�b�g�r���[�����A�ݒ�
		g_D3DDevice->CreateRenderTargetView(normalTexture, NULL, &g_RenderTargetViewNormalMap);

		//�V�F�[�_�[���\�[�X�r���[�쐬
		D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
		ZeroMemory(&srvd, sizeof(srvd));
		srvd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvd.Texture2D.MipLevels = 1;
		g_D3DDevice->CreateShaderResourceView(normalTexture, &srvd, &g_NormalMapTexture);
	}

	// �f�v�X�}�b�v�p�e�N�X�`���쐬
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

		// �����_�[�^�[�Q�b�g�r���[�����A�ݒ�
		g_D3DDevice->CreateRenderTargetView(Texture, NULL, &g_RenderTargetViewDepthMap);

		//�V�F�[�_�[���\�[�X�r���[�쐬
		D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
		ZeroMemory(&srvd, sizeof(srvd));
		srvd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvd.Texture2D.MipLevels = 1;
		g_D3DDevice->CreateShaderResourceView(Texture, &srvd, &g_DepthMapTexture);
	}

	// �r���[�|�[�g�ݒ�
	{
		D3D11_VIEWPORT vp;
		vp.Width = (FLOAT)SCREEN_WIDTH;
		vp.Height = (FLOAT)SCREEN_HEIGHT;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;

		// �����ݒ�
		g_ImmediateContext->RSSetViewports(1, &vp);
	}

	// ���X�^���C�U�X�e�[�g�쐬
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

		// �����ݒ�
		SetCullingMode(CULL_MODE_BACK);
	}

	// �u�����h�X�e�[�g�̍쐬
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

		// �����ݒ�
		SetBlendState(BLEND_MODE_ALPHABLEND);
	}

	// �[�x�X�e���V���X�e�[�g�쐬
	{
		D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
		ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
		depthStencilDesc.DepthEnable = TRUE;
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		depthStencilDesc.StencilEnable = FALSE;

		g_D3DDevice->CreateDepthStencilState(&depthStencilDesc, &g_DepthStateEnable);//�[�x�L���X�e�[�g

		depthStencilDesc.DepthEnable = FALSE;
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		//depthStencilDesc.DepthFunc = D3D11_COMPARISON_NEVER;
		g_D3DDevice->CreateDepthStencilState(&depthStencilDesc, &g_DepthStateDisable);//�[�x�����X�e�[�g

		ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
		depthStencilDesc.DepthEnable = TRUE;
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		depthStencilDesc.StencilEnable = TRUE;
		depthStencilDesc.StencilReadMask = 0xff;
		depthStencilDesc.StencilWriteMask = 0xff;
		depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;	// �X�e���V���e�X�g���ɍs��
		depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_INCR;
		depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;	// �X�e���V���e�X�g���ɍs��
		depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_DECR;
		depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		g_D3DDevice->CreateDepthStencilState(&depthStencilDesc, &g_StencilWrite);

		ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
		depthStencilDesc.DepthEnable = TRUE;
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		depthStencilDesc.StencilEnable = TRUE;					// �X�e���V���o�b�t�@�������ݐ錾
		depthStencilDesc.StencilReadMask = 0xff;				// �X�e���V���Q�ƒl
		depthStencilDesc.StencilWriteMask = 0xff;			// �X�e���V���}�X�N
		depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;	// �X�e���V���e�X�g���ɍs��
		depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_NOT_EQUAL;	// �X�e���V���e�X�g���ɍs��
		depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
		depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		g_D3DDevice->CreateDepthStencilState(&depthStencilDesc, &g_StencilRead);

		g_ImmediateContext->OMSetDepthStencilState(g_DepthStateEnable, NULL);

		// �����ݒ�
		SetDepthEnable(true);
	}

	// �T���v���[�X�e�[�g�ݒ�
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

		// �����ݒ�
		g_ImmediateContext->PSSetSamplers(0, 1, &samplerState);
	}

	// �V�F�[�_�[�̃R���p�C���E����
	{
		DWORD shFlag = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(_DEBUG) && defined(DEBUG_SHADER)
		shFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

		// ���_�V�F�[�_
		ID3DBlob* pErrorBlob;
		ID3DBlob* pVSBlob = NULL;
		hr = D3DX11CompileFromFile("shader.hlsl", NULL, NULL, "VSPolygon", "vs_4_0", shFlag, 0, NULL, &pVSBlob, &pErrorBlob, NULL);
		if (FAILED(hr)) {
			MessageBox(NULL, (char*)pErrorBlob->GetBufferPointer(), "VS", MB_OK | MB_ICONERROR);
		}
		g_D3DDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), NULL, &g_VS);

		// ���̓��C�A�E�g����
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

		// �����ݒ�
		g_ImmediateContext->IASetInputLayout(g_VertexLayout);

		// ���̑��̒��_�V�F�[�_�R���p�C���E����
		CreateShader("shader.hlsl", "VSOutline", &g_VSOutline, shFlag);
		CreateShader("shader.hlsl", "VSOutlineInstancing", &g_VSOutlineInstancing, shFlag);
		CreateShader("shader.hlsl", "VSTube", &g_VSTube, shFlag);
		CreateShader("shader.hlsl", "VSGimmick", &g_VSGimmick, shFlag);
		CreateShader("shader.hlsl", "VSPlayer", &g_VSPlayer, shFlag);
		CreateShader("shader.hlsl", "VSEX", &g_VSEX, shFlag);
		CreateShader("shader_postEffect.hlsl", "VSFilter", &g_VSFilter, shFlag);
		CreateShader("shader_postEffect.hlsl", "VSNormalMap", &g_VSNormalMap, shFlag);
		CreateShader("shader_postEffect.hlsl", "VSDepthMap", &g_VSDepthMap, shFlag);

		// �W�I���g���V�F�[�_�R���p�C���E����
		CreateShader("shader.hlsl", "GSDL", &g_GSDL, shFlag);
		CreateShader("shader.hlsl", "GSPL", &g_GSPL, shFlag);
		CreateShader("shader.hlsl", "GSSL", &g_GSSL, shFlag);
		CreateShader("shader.hlsl", "GSLLPlayer", &g_GSLLPlayer, shFlag);
		CreateShader("shader.hlsl", "GSLLNonPlayer", &g_GSLLNonPlayer, shFlag);
		CreateShader("shader.hlsl", "GSEX", &g_GSEX, shFlag);

		// �s�N�Z���V�F�[�_�R���p�C���E����
		CreateShader("shader.hlsl", "PSPolygon", &g_PS, shFlag);
		CreateShader("shader.hlsl", "PSAL", &g_PSAL, shFlag);
		CreateShader("shader.hlsl", "PSDL", &g_PSDL, shFlag);
		CreateShader("shader.hlsl", "PSPL", &g_PSPL, shFlag);
		CreateShader("shader.hlsl", "PSSL", &g_PSSL, shFlag);
		CreateShader("shader.hlsl", "PSLL", &g_PSLL, shFlag);
		CreateShader("shader.hlsl", "PSOutline", &g_PSOutline, shFlag);
		CreateShader("shader_postEffect.hlsl", "PSFilter", &g_PSFilter, shFlag);
		CreateShader("shader_postEffect.hlsl", "PSLight", &g_PSLight, shFlag);
		CreateShader("shader_postEffect.hlsl", "PSOnlyTex", &g_PSOnlyTex, shFlag);
		CreateShader("shader_postEffect.hlsl", "PSMosaic", &g_PSMosaic, shFlag);
		CreateShader("shader_postEffect.hlsl", "PSNormalMap", &g_PSNormalMap, shFlag);
		CreateShader("shader_postEffect.hlsl", "PSDepthMap", &g_PSDepthMap, shFlag);
		CreateShader("shader_postEffect.hlsl", "PSCA", &g_PSCA, shFlag);
		CreateShader("shader_postEffect.hlsl", "PSMonitoring", &g_PSMonitoring, shFlag);
		CreateShader("shader_postEffect.hlsl", "PSOldGame", &g_PSOldGame, shFlag);

		// �C���X�^���V���O�`��p�V�F�[�_�R���p�C���E����
		CreateShader("shader.hlsl", "VSInstancing", &g_VSInstancing, shFlag);
		CreateShader("shader.hlsl", "VSInstancingTexture", &g_VSInstancingTexture, shFlag);
		CreateShader("shader.hlsl", "PSInstancingOnlyTex", &g_PSInstancingOnlyTex, shFlag);

		// �����ݒ�
		g_ImmediateContext->VSSetShader(g_VS, NULL, 0);
		g_ImmediateContext->PSSetShader(g_PS, NULL, 0);
	}

	// �萔�o�b�t�@����
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
		//CreateBuffer(&hBufferDesc, 10, &g_MosaicBuffer, sizeof(MOSAIC));
		CreateBuffer(&hBufferDesc, 10, &g_CurveBuffer, sizeof(CURVE_BUFFER));
		//CreateBuffer(&hBufferDesc, 11, &g_ConstantBuffer, sizeof(CONSTANT));

		// �C���X�^���X
		hBufferDesc.Usage = D3D11_USAGE_DYNAMIC;				// CPU�������݂������
		hBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;	// CPU�������݂������
		hBufferDesc.ByteWidth = sizeof(INSTANCE);
		g_D3DDevice->CreateBuffer(&hBufferDesc, NULL, &g_InstanceBuffer);
		g_ImmediateContext->VSSetConstantBuffers(11, 1, &g_InstanceBuffer);
		g_ImmediateContext->PSSetConstantBuffers(11, 1, &g_InstanceBuffer);
	}

	// �X�N���[���p���_�o�b�t�@�쐬
	{
		// ���_�o�b�t�@����
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DYNAMIC;
		bd.ByteWidth = sizeof(VERTEX_3D) * 4;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		GetDevice()->CreateBuffer(&bd, NULL, &g_ScreenVertexBuffer);

		//���_�o�b�t�@�̒��g�𖄂߂�
		D3D11_MAPPED_SUBRESOURCE msr;
		GetDeviceContext()->Map(g_ScreenVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

		VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

		// ���_���W�̐ݒ�
		vertex[0].Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
		vertex[1].Position = XMFLOAT3(SCREEN_WIDTH, 0.0f, 0.0f);
		vertex[2].Position = XMFLOAT3(0.0f, SCREEN_HEIGHT, 0.0f);
		vertex[3].Position = XMFLOAT3(SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f);

		// �@���x�N�g���̐ݒ�
		vertex[0].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
		vertex[1].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
		vertex[2].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
		vertex[3].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);

		// �g�U���̐ݒ�
		vertex[0].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[1].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[2].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[3].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		// �e�N�X�`�����W�̐ݒ�
		vertex[0].TexCoord = XMFLOAT2(0.0f, 0.0f);
		vertex[1].TexCoord = XMFLOAT2(1.0f, 0.0f);
		vertex[2].TexCoord = XMFLOAT2(0.0f, 1.0f);
		vertex[3].TexCoord = XMFLOAT2(1.0f, 1.0f);

		GetDeviceContext()->Unmap(g_ScreenVertexBuffer, 0);
	}

	return S_OK;
}

//*****************************************************************************
// �e�폈��
//*****************************************************************************
ID3D11Device* GetDevice(void) { return g_D3DDevice; }
ID3D11DeviceContext* GetDeviceContext(void) { return g_ImmediateContext; }
void Present(void) { g_SwapChain->Present(0, 0); }

// �[�x�e�X�g�̃Z�b�^�[
void SetDepthEnable(BOOL Enable) {
	if (Enable)
		g_ImmediateContext->OMSetDepthStencilState(g_DepthStateEnable, NULL);
	else
		g_ImmediateContext->OMSetDepthStencilState(g_DepthStateDisable, NULL);
}

// �u�����h�̃Z�b�^�[
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

// �J�����O�̃Z�b�^�[
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

// �}�g���N�X�o�b�t�@�̃Z�b�^�[
void SetWorldViewProjection2D(void) {
	MATRIX matrix;
	XMMATRIX worldViewProjection = XMMatrixOrthographicOffCenterLH(0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f);
	XMStoreFloat4x4(&matrix.WorldViewProjection, XMMatrixTranspose(worldViewProjection));
	GetDeviceContext()->UpdateSubresource(g_MatrixBuffer, 0, NULL, &matrix, 0, 0);
}
void SetWorldBuffer(SRT& srt) {
	XMMATRIX mtxWorld = XMMatrixIdentity();		// ���[���h�}�g���b�N�X�̏�����
	MulMtxScl(mtxWorld, srt.scl);	// �X�P�[���𔽉f
	MulMtxRot(mtxWorld, srt.rot);	// ��]�𔽉f�F�S�̂̊p�x
	MulMtxPos(mtxWorld, srt.pos);	// �ړ��𔽉f
	SetWorldBuffer(&mtxWorld);		// ���[���h�}�g���b�N�X�̐ݒ�
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

// �萔�o�b�t�@�̃Z�b�^�[
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
	//CONSTANT constant = { time, 0.0f, 0.0f, 0.0f };
	//GetDeviceContext()->UpdateSubresource(g_ConstantBuffer, 0, NULL, &constant, 0, 0);
}
void SetCurveBuffer(CURVE_BUFFER* curve) {
	GetDeviceContext()->UpdateSubresource(g_CurveBuffer, 0, NULL, curve, 0, 0);
}

// �C���X�^���V���O�֘A
ID3D11Buffer* GetInstanceBuffer(void) { return g_InstanceBuffer; }
void SetShaderInstanceingOnlyTex(BOOL bInterrupt) {
	//SetBlendState(BLEND_MODE_ALPHABLEND);
	g_ImmediateContext->VSSetShader(g_VSInstancingTexture, NULL, 0);
	g_ImmediateContext->GSSetShader(NULL, NULL, 0);
	g_ImmediateContext->PSSetShader(g_PSInstancingOnlyTex, NULL, 0);
	g_ImmediateContext->OMSetDepthStencilState(g_DepthStateDisable, NULL);
	if(bInterrupt) g_ImmediateContext->OMSetRenderTargets(1, &g_RenderTargetViewWrite[g_CurrentTarget], NULL);
	else g_ImmediateContext->OMSetRenderTargets(1, &g_RenderTargetView, NULL);
}
//void SetShaderInstanceingBillbooard(XMFLOAT4X4 mtxView) {
//	XMMATRIX invView = XMMatrixIdentity();
//
//	invView.r[0].m128_f32[0] = mtxView.m[0][0];
//	invView.r[0].m128_f32[1] = mtxView.m[1][0];
//	invView.r[0].m128_f32[2] = mtxView.m[2][0];
//
//	invView.r[1].m128_f32[0] = mtxView.m[0][1];
//	invView.r[1].m128_f32[1] = mtxView.m[1][1];
//	invView.r[1].m128_f32[2] = mtxView.m[2][1];
//
//	invView.r[2].m128_f32[0] = mtxView.m[0][2];
//	invView.r[2].m128_f32[1] = mtxView.m[1][2];
//	invView.r[2].m128_f32[2] = mtxView.m[2][2];
//
//	invView = XMMatrixTranspose(invView);
//
//	GetDeviceContext()->UpdateSubresource(g_BillboardBuffer, 0, NULL, &invView, 0, 0);
//
//	g_ImmediateContext->VSSetShader(g_VSInstancing, NULL, 0);
//	g_ImmediateContext->PSSetShader(g_PSOnlyTex, NULL, 0);
//}

// �o�b�t�@�쐬
void CreateBuffer(D3D11_BUFFER_DESC* pDesc, int slot, ID3D11Buffer** pBuffer, UINT size)
{
	pDesc->ByteWidth = size;
	g_D3DDevice->CreateBuffer(pDesc, NULL, pBuffer);
	g_ImmediateContext->VSSetConstantBuffers(slot, 1, pBuffer);
	g_ImmediateContext->GSSetConstantBuffers(slot, 1, pBuffer);
	g_ImmediateContext->PSSetConstantBuffers(slot, 1, pBuffer);
}

// ���_�V�F�[�_�[�쐬
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
// �s�N�Z���V�F�[�_�[�쐬
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
// �W�I���g���V�F�[�_�[�쐬
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

// �V�F�[�_�[�֘A
void SetShaderDefault(void) {
	g_ImmediateContext->VSSetShader(g_VS, NULL, 0);
	g_ImmediateContext->PSSetShader(g_PS, NULL, 0);
}
void SetStencilWriteDL(void)
{
	SetCullingMode(CULL_MODE_NONE);
	g_ImmediateContext->VSSetShader(g_VS, NULL, 0);
	g_ImmediateContext->GSSetShader(g_GSDL, NULL, 0);
	g_ImmediateContext->PSSetShader(NULL, NULL, 0);
	g_ImmediateContext->OMSetDepthStencilState(g_StencilWrite, NULL);
	g_ImmediateContext->OMSetRenderTargets(0, NULL, g_DepthStencilView);
	// �V���h�E�{�����[����`�悵�Ă݂�
	//g_ImmediateContext->PSSetShader(g_PS, NULL, 0);
	//g_ImmediateContext->OMSetRenderTargets(1, &g_RenderTargetView, g_DepthStencilView);
}
void SetStencilWritePL(void)
{
	SetCullingMode(CULL_MODE_NONE);
	//g_ImmediateContext->VSSetShader(g_VS, NULL, 0);
	g_ImmediateContext->VSSetShader(g_VSPlayer, NULL, 0);
	g_ImmediateContext->GSSetShader(g_GSPL, NULL, 0);
	g_ImmediateContext->PSSetShader(NULL, NULL, 0);
	g_ImmediateContext->OMSetDepthStencilState(g_StencilWrite, NULL);
	g_ImmediateContext->OMSetRenderTargets(0, NULL, g_DepthStencilView);
	// �V���h�E�{�����[����`�悵�Ă݂�
	//g_ImmediateContext->PSSetShader(g_PS, NULL, 0);
	//g_ImmediateContext->OMSetRenderTargets(1, &g_RenderTargetView, g_DepthStencilView);
}
void SetStencilWriteSL(void)
{
	SetCullingMode(CULL_MODE_NONE);
	g_ImmediateContext->VSSetShader(g_VS, NULL, 0);
	g_ImmediateContext->GSSetShader(g_GSSL, NULL, 0);
	g_ImmediateContext->PSSetShader(NULL, NULL, 0);
	g_ImmediateContext->OMSetDepthStencilState(g_StencilWrite, NULL);
	g_ImmediateContext->OMSetRenderTargets(0, NULL, g_DepthStencilView);
	// �V���h�E�{�����[����`�悵�Ă݂�
	//g_ImmediateContext->PSSetShader(g_PS, NULL, 0);
	//g_ImmediateContext->OMSetRenderTargets(1, &g_RenderTargetView, g_DepthStencilView);
}
void SetStencilWriteLL(SHADER_TYPE shader)
{
	SetCullingMode(CULL_MODE_NONE);
	switch (shader)
	{
	case SHADER_TUBE:
	case SHADER_GIMMICK:
		g_ImmediateContext->VSSetShader(g_VSGimmick, NULL, 0);
		g_ImmediateContext->GSSetShader(g_GSLLNonPlayer, NULL, 0);
		break;
	case SHADER_PLAYER:
		g_ImmediateContext->VSSetShader(g_VSPlayer, NULL, 0);
		g_ImmediateContext->GSSetShader(g_GSLLPlayer, NULL, 0);
		break;
	}
	//g_ImmediateContext->GSSetShader(g_GSLLPlayer, NULL, 0);
	g_ImmediateContext->PSSetShader(NULL, NULL, 0);
	g_ImmediateContext->OMSetDepthStencilState(g_StencilWrite, NULL);
	g_ImmediateContext->OMSetRenderTargets(0, NULL, g_DepthStencilView);

	// �V���h�E�{�����[����`�悵�Ă݂�
	//g_ImmediateContext->PSSetShader(g_PS, NULL, 0);
	//g_ImmediateContext->OMSetRenderTargets(1, &g_RenderTargetView, g_DepthStencilView);
	//g_ImmediateContext->OMSetRenderTargets(1, &g_RenderTargetViewWrite[g_CurrentTarget], g_DepthStencilView);
}

void SetStencilRead(void)
{
	SetCullingMode(CULL_MODE_BACK);
	g_ImmediateContext->VSSetShader(g_VS, NULL, 0);
	g_ImmediateContext->GSSetShader(NULL, NULL, 0);
	g_ImmediateContext->PSSetShader(g_PS, NULL, 0);
	g_ImmediateContext->OMSetDepthStencilState(g_StencilRead, NULL);
	g_ImmediateContext->OMSetRenderTargets(1, &g_RenderTargetViewWrite[g_CurrentTarget], g_DepthStencilView);
}
void SetStencilReadDL(void)
{
	SetCullingMode(CULL_MODE_BACK);
	g_ImmediateContext->VSSetShader(g_VS, NULL, 0);
	g_ImmediateContext->GSSetShader(NULL, NULL, 0);
	g_ImmediateContext->PSSetShader(g_PSDL, NULL, 0);
	g_ImmediateContext->OMSetDepthStencilState(g_StencilRead, NULL);
	g_ImmediateContext->OMSetRenderTargets(1, &g_RenderTargetViewWrite[g_CurrentTarget], g_DepthStencilView);
}
void SetStencilReadPL(void)
{
	SetCullingMode(CULL_MODE_BACK);
	g_ImmediateContext->VSSetShader(g_VS, NULL, 0);
	g_ImmediateContext->GSSetShader(NULL, NULL, 0);
	g_ImmediateContext->PSSetShader(g_PSPL, NULL, 0);
	g_ImmediateContext->OMSetDepthStencilState(g_StencilRead, NULL);
	g_ImmediateContext->OMSetRenderTargets(1, &g_RenderTargetViewWrite[g_CurrentTarget], g_DepthStencilView);
}
void SetStencilReadSL(void)
{
	SetCullingMode(CULL_MODE_BACK);
	g_ImmediateContext->VSSetShader(g_VS, NULL, 0);
	g_ImmediateContext->GSSetShader(NULL, NULL, 0);
	g_ImmediateContext->PSSetShader(g_PSSL, NULL, 0);
	g_ImmediateContext->OMSetDepthStencilState(g_StencilRead, NULL);
	g_ImmediateContext->OMSetRenderTargets(1, &g_RenderTargetViewWrite[g_CurrentTarget], g_DepthStencilView);
}
void SetStencilReadLL(SHADER_TYPE shader)
{
	SetCullingMode(CULL_MODE_BACK);
	switch (shader)
	{
	case SHADER_TUBE:
		g_ImmediateContext->VSSetShader(g_VSTube, NULL, 0);
		break;
	case SHADER_GIMMICK:
		g_ImmediateContext->VSSetShader(g_VSGimmick, NULL, 0);
		break;
	case SHADER_PLAYER:
		g_ImmediateContext->VSSetShader(g_VSPlayer, NULL, 0);
		break;
	}
	g_ImmediateContext->GSSetShader(NULL, NULL, 0);
	g_ImmediateContext->PSSetShader(g_PSLL, NULL, 0);
	//g_ImmediateContext->PSSetShader(g_PSOnlyTex, NULL, 0);
	//g_ImmediateContext->OMSetDepthStencilState(g_StencilRead, NULL);
	g_ImmediateContext->OMSetDepthStencilState(g_DepthStateEnable, NULL);
	g_ImmediateContext->OMSetRenderTargets(1, &g_RenderTargetViewWrite[g_CurrentTarget], g_DepthStencilView);
}
void SetStencilReadLLGimmick(void)
{
	SetCullingMode(CULL_MODE_BACK);
	g_ImmediateContext->VSSetShader(g_VSInstancing, NULL, 0);
	//g_ImmediateContext->GSSetShader(NULL, NULL, 0);
	//g_ImmediateContext->VSSetShader(g_VSEX, NULL, 0);
	g_ImmediateContext->GSSetShader(g_GSEX, NULL, 0);
	g_ImmediateContext->PSSetShader(g_PSLL, NULL, 0);
	//g_ImmediateContext->PSSetShader(g_PSOnlyTex, NULL, 0);
	//g_ImmediateContext->OMSetDepthStencilState(g_StencilRead, NULL);
	g_ImmediateContext->OMSetDepthStencilState(g_DepthStateEnable, NULL);
	g_ImmediateContext->OMSetRenderTargets(1, &g_RenderTargetViewWrite[g_CurrentTarget], g_DepthStencilView);
}

void SetStencilNone(void)
{
	SetCullingMode(CULL_MODE_BACK);
	g_ImmediateContext->VSSetShader(g_VS, NULL, 0);
	g_ImmediateContext->GSSetShader(NULL, NULL, 0);
	g_ImmediateContext->PSSetShader(g_PS, NULL, 0);
	g_ImmediateContext->OMSetDepthStencilState(g_DepthStateEnable, NULL);
	g_ImmediateContext->OMSetRenderTargets(1, &g_RenderTargetViewWrite[g_CurrentTarget], g_DepthStencilView);
}
void SetStencilNoneAL(SHADER_TYPE shader)
{
	SetCullingMode(CULL_MODE_BACK);
	switch (shader)
	{
	case SHADER_TUBE:
		g_ImmediateContext->VSSetShader(g_VSTube, NULL, 0);
		break;
	case SHADER_GIMMICK:
		g_ImmediateContext->VSSetShader(g_VSGimmick, NULL, 0);
		break;
	case SHADER_PLAYER:
		g_ImmediateContext->VSSetShader(g_VSPlayer, NULL, 0);
		break;
	}
	g_ImmediateContext->GSSetShader(NULL, NULL, 0);
	g_ImmediateContext->PSSetShader(g_PSAL, NULL, 0);
	g_ImmediateContext->OMSetDepthStencilState(g_DepthStateEnable, NULL);
	g_ImmediateContext->OMSetRenderTargets(1, &g_RenderTargetViewWrite[g_CurrentTarget], g_DepthStencilView);
	//g_ImmediateContext->OMSetRenderTargets(1, &g_RenderTargetView, g_DepthStencilView);
}

void SetStencilNoneOnlyDepth(void)
{
	SetCullingMode(CULL_MODE_BACK);
	g_ImmediateContext->VSSetShader(g_VS, NULL, 0);
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
	g_ImmediateContext->VSSetShader(g_VSOutlineInstancing, NULL, 0);
	g_ImmediateContext->GSSetShader(NULL, NULL, 0);
	g_ImmediateContext->PSSetShader(g_PSOutline, NULL, 0);
	g_ImmediateContext->OMSetDepthStencilState(g_DepthStateEnable, NULL);
	g_ImmediateContext->OMSetRenderTargets(1, &g_RenderTargetViewWrite[g_CurrentTarget], g_DepthStencilView);
}
void SetDrawFillBlack(SHADER_TYPE shader)
{
	OUTLINE outline = { { 0.0f, 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f, 1.0f} };
	GetDeviceContext()->UpdateSubresource(g_OutlineBuffer, 0, NULL, &outline, 0, 0);

	SetCullingMode(CULL_MODE_BACK);
	//g_ImmediateContext->VSSetShader(g_VSOutline, NULL, 0);
	switch (shader)
	{
	case SHADER_TUBE:
		g_ImmediateContext->VSSetShader(g_VSTube, NULL, 0);
		break;
	case SHADER_GIMMICK:
		g_ImmediateContext->VSSetShader(g_VSGimmick, NULL, 0);
		break;
	case SHADER_PLAYER:
		g_ImmediateContext->VSSetShader(g_VSPlayer, NULL, 0);
		break;
	}
	g_ImmediateContext->GSSetShader(NULL, NULL, 0);
	g_ImmediateContext->PSSetShader(g_PSOutline, NULL, 0);
	g_ImmediateContext->OMSetDepthStencilState(g_DepthStateEnable, NULL);
	g_ImmediateContext->OMSetRenderTargets(1, &g_RenderTargetViewWrite[g_CurrentTarget], g_DepthStencilView);
	//g_ImmediateContext->OMSetRenderTargets(1, &g_RenderTargetView, g_DepthStencilView);
}
void SetDrawFillBlackPlayer(void)
{
	OUTLINE outline = { { 0.0f, 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f, 1.0f} };
	GetDeviceContext()->UpdateSubresource(g_OutlineBuffer, 0, NULL, &outline, 0, 0);

	SetCullingMode(CULL_MODE_BACK);
	g_ImmediateContext->VSSetShader(g_VSOutline, NULL, 0);
	g_ImmediateContext->GSSetShader(NULL, NULL, 0);
	g_ImmediateContext->PSSetShader(g_PSOutline, NULL, 0);
	g_ImmediateContext->OMSetDepthStencilState(g_DepthStateEnable, NULL);
	g_ImmediateContext->OMSetRenderTargets(1, &g_RenderTargetViewWrite[g_CurrentTarget], g_DepthStencilView);
	//g_ImmediateContext->OMSetRenderTargets(1, &g_RenderTargetView, g_DepthStencilView);
}
void SetDrawNoLighting(void)
{
	SetCullingMode(CULL_MODE_BACK);
	g_ImmediateContext->VSSetShader(g_VS, NULL, 0);
	g_ImmediateContext->GSSetShader(NULL, NULL, 0);
	g_ImmediateContext->PSSetShader(g_PSOnlyTex, NULL, 0);
	g_ImmediateContext->OMSetDepthStencilState(g_DepthStateEnable, NULL);
	//g_ImmediateContext->OMSetRenderTargets(1, &g_RenderTargetView, g_DepthStencilView);
	g_ImmediateContext->OMSetRenderTargets(1, &g_RenderTargetViewWrite[g_CurrentTarget], g_DepthStencilView);
}
void SetDrawTube(void)
{
	SetCullingMode(CULL_MODE_BACK);
	g_ImmediateContext->VSSetShader(g_VSTube, NULL, 0);
	g_ImmediateContext->GSSetShader(NULL, NULL, 0);
	//g_ImmediateContext->PSSetShader(g_PSPL, NULL, 0);
	//g_ImmediateContext->PSSetShader(g_PSAL, NULL, 0);
	g_ImmediateContext->PSSetShader(g_PSLL, NULL, 0);
	g_ImmediateContext->OMSetDepthStencilState(g_DepthStateEnable, NULL);
	//g_ImmediateContext->OMSetRenderTargets(1, &g_RenderTargetView, g_DepthStencilView);
	g_ImmediateContext->OMSetRenderTargets(1, &g_RenderTargetViewWrite[g_CurrentTarget], g_DepthStencilView);
}
void SetDrawGimmick(void)
{
	SetCullingMode(CULL_MODE_BACK);
	g_ImmediateContext->VSSetShader(g_VSGimmick, NULL, 0);
	g_ImmediateContext->GSSetShader(NULL, NULL, 0);
	//g_ImmediateContext->PSSetShader(g_PSOnlyTex, NULL, 0);
	//g_ImmediateContext->PSSetShader(g_PSPL, NULL, 0);
	g_ImmediateContext->PSSetShader(g_PSLL, NULL, 0);
	g_ImmediateContext->OMSetDepthStencilState(g_DepthStateEnable, NULL);
	//g_ImmediateContext->OMSetRenderTargets(1, &g_RenderTargetView, g_DepthStencilView);
	g_ImmediateContext->OMSetRenderTargets(1, &g_RenderTargetViewWrite[g_CurrentTarget], g_DepthStencilView);
}
void SetDrawPlayer(void)
{
	SetCullingMode(CULL_MODE_BACK);
	g_ImmediateContext->VSSetShader(g_VSPlayer, NULL, 0);
	g_ImmediateContext->GSSetShader(NULL, NULL, 0);
	g_ImmediateContext->PSSetShader(g_PSLL, NULL, 0);
	//g_ImmediateContext->PSSetShader(g_PSOnlyTex, NULL, 0);
	g_ImmediateContext->OMSetDepthStencilState(g_DepthStateEnable, NULL);
	//g_ImmediateContext->OMSetRenderTargets(1, &g_RenderTargetView, g_DepthStencilView);
	g_ImmediateContext->OMSetRenderTargets(1, &g_RenderTargetViewWrite[g_CurrentTarget], g_DepthStencilView);
}
void SetDrawTubeLight(void)
{
	SetCullingMode(CULL_MODE_BACK);
	g_ImmediateContext->VSSetShader(g_VSTube, NULL, 0);
	g_ImmediateContext->GSSetShader(NULL, NULL, 0);
	g_ImmediateContext->PSSetShader(g_PSOnlyTex, NULL, 0);
	//g_ImmediateContext->PSSetShader(g_PSOnlyTex, NULL, 0);
	g_ImmediateContext->OMSetDepthStencilState(g_DepthStateEnable, NULL);
	//g_ImmediateContext->OMSetRenderTargets(1, &g_RenderTargetView, g_DepthStencilView);
	g_ImmediateContext->OMSetRenderTargets(1, &g_RenderTargetViewWrite[g_CurrentTarget], g_DepthStencilView);
}
void SetDrawFire(void)
{
	SetCullingMode(CULL_MODE_BACK);
	g_ImmediateContext->VSSetShader(g_VSPlayer, NULL, 0);
	g_ImmediateContext->GSSetShader(NULL, NULL, 0);
	g_ImmediateContext->PSSetShader(g_PSOnlyTex, NULL, 0);
	//g_ImmediateContext->PSSetShader(g_PSOnlyTex, NULL, 0);
	g_ImmediateContext->OMSetDepthStencilState(g_DepthStateEnable, NULL);
	//g_ImmediateContext->OMSetRenderTargets(1, &g_RenderTargetView, g_DepthStencilView);
	g_ImmediateContext->OMSetRenderTargets(1, &g_RenderTargetViewWrite[g_CurrentTarget], g_DepthStencilView);
}
void SetDrawMissileFire(void)
{
	SetCullingMode(CULL_MODE_BACK);
	g_ImmediateContext->VSSetShader(g_VSInstancing, NULL, 0);
	g_ImmediateContext->GSSetShader(NULL, NULL, 0);
	g_ImmediateContext->PSSetShader(g_PSOnlyTex, NULL, 0);
	g_ImmediateContext->OMSetDepthStencilState(g_DepthStateEnable, NULL);
	g_ImmediateContext->OMSetRenderTargets(1, &g_RenderTargetViewWrite[g_CurrentTarget], g_DepthStencilView);
}
void SetDrawLight(void)
{
	SetCullingMode(CULL_MODE_BACK);
	g_ImmediateContext->VSSetShader(g_VSGimmick, NULL, 0);
	g_ImmediateContext->GSSetShader(NULL, NULL, 0);
	g_ImmediateContext->PSSetShader(g_PSOnlyTex, NULL, 0);
	g_ImmediateContext->OMSetDepthStencilState(g_DepthStateEnable, NULL);
	g_ImmediateContext->OMSetRenderTargets(1, &g_RenderTargetViewLight[g_CurrentTargetLight], g_DepthStencilView);
	//g_ImmediateContext->OMSetRenderTargets(1, &g_RenderTargetViewWrite[g_CurrentTarget], g_DepthStencilView);
}
void SetDrawInstancingOnlyTex(void)
{
	SetCullingMode(CULL_MODE_BACK);
	g_ImmediateContext->VSSetShader(g_VSInstancing, NULL, 0);
	g_ImmediateContext->GSSetShader(NULL, NULL, 0);
	g_ImmediateContext->PSSetShader(g_PSOnlyTex, NULL, 0);
	g_ImmediateContext->OMSetDepthStencilState(g_DepthStateEnable, NULL);
	g_ImmediateContext->OMSetRenderTargets(1, &g_RenderTargetViewLight[g_CurrentTargetLight], g_DepthStencilView);
	//g_ImmediateContext->OMSetRenderTargets(1, &g_RenderTargetViewWrite[g_CurrentTarget], g_DepthStencilView);
}
void SetDraw2DTexture(void)
{
	g_ImmediateContext->VSSetShader(g_VSFilter, NULL, 0);
	g_ImmediateContext->GSSetShader(NULL, NULL, 0);
	g_ImmediateContext->PSSetShader(g_PSOnlyTex, NULL, 0);
	g_ImmediateContext->OMSetDepthStencilState(g_DepthStateDisable, NULL);
	//g_ImmediateContext->OMSetRenderTargets(1, &g_RenderTargetView, NULL);
	g_ImmediateContext->OMSetRenderTargets(1, &g_RenderTargetViewWrite[g_CurrentTarget], NULL);
}

void ApplyLightToTarget(void)	// ���Z��������
{
	// �^�[�Q�b�g�̐؂�ւ�
	g_CurrentTargetLight = g_CurrentTargetLight ? 0 : 1;

	XMMATRIX f;
	for (int x = 0; x < 3; x++) {
		for (int y = 0; y < 3; y++) {
			f.r[y].m128_f32[x] = g_Filter[FILTER_MODE_AVERAGING][y][x];
		}
	}
	GetDeviceContext()->UpdateSubresource(g_FilterBuffer, 0, NULL, &f, 0, 0);

	SetCullingMode(CULL_MODE_BACK);
	SetBlendState(BLEND_MODE_NONE);
	g_ImmediateContext->VSSetShader(g_VSFilter, NULL, 0);
	g_ImmediateContext->GSSetShader(NULL, NULL, 0);
	g_ImmediateContext->PSSetShader(g_PSLight, NULL, 0);
	//g_ImmediateContext->PSSetShader(g_PSOnlyTex, NULL, 0);
	//g_ImmediateContext->PSSetShader(g_PSLight, NULL, 0);
	g_ImmediateContext->OMSetDepthStencilState(g_DepthStateDisable, NULL);
	g_ImmediateContext->OMSetRenderTargets(1, &g_RenderTargetViewLight[g_CurrentTargetLight], NULL);

	DrawScreen(&g_LightTexture[g_CurrentResourceLight]);

	// ���\�[�X�̐؂�ւ�
	g_CurrentResourceLight = g_CurrentResourceLight ? 0 : 1;

	SetBlendState(BLEND_MODE_ADD);
	g_ImmediateContext->PSSetShader(g_PSOnlyTex, NULL, 0);
	g_ImmediateContext->OMSetRenderTargets(1, &g_RenderTargetViewWrite[g_CurrentTarget], NULL);

	DrawScreen(&g_LightTexture[g_CurrentResourceLight]);

}
void ApplyFilter(FILTER_MODE filter)
{
	// �^�[�Q�b�g�̐؂�ւ�
	g_CurrentTarget = g_CurrentTarget ? 0 : 1;

	XMMATRIX f;
	for (int x = 0; x < 3; x++) {
		for (int y = 0; y < 3; y++) {
			f.r[y].m128_f32[x] = g_Filter[filter][y][x];
		}
	}
	GetDeviceContext()->UpdateSubresource(g_FilterBuffer, 0, NULL, &f, 0, 0);

	SetCullingMode(CULL_MODE_BACK);
	g_ImmediateContext->VSSetShader(g_VSFilter, NULL, 0);
	g_ImmediateContext->GSSetShader(NULL, NULL, 0);
	g_ImmediateContext->PSSetShader(g_PSFilter, NULL, 0);
	g_ImmediateContext->OMSetDepthStencilState(g_DepthStateDisable, NULL);
	g_ImmediateContext->OMSetRenderTargets(1, &g_RenderTargetViewWrite[g_CurrentTarget], g_DepthStencilView);

	DrawScreen(&g_WrittenTexture[g_CurrentResource]);

	// ���\�[�X�̐؂�ւ�
	g_CurrentResource = g_CurrentResource ? 0 : 1;
}
void ApplyMosaic(int pixel_size)
{
	// �^�[�Q�b�g�̐؂�ւ�
	g_CurrentTarget = g_CurrentTarget ? 0 : 1;

	MOSAIC mozaic = { pixel_size, 0.0f, 0.0f, 0.0f };
	GetDeviceContext()->UpdateSubresource(g_MosaicBuffer, 0, NULL, &mozaic, 0, 0);

	SetCullingMode(CULL_MODE_BACK);
	g_ImmediateContext->VSSetShader(g_VSFilter, NULL, 0);
	g_ImmediateContext->GSSetShader(NULL, NULL, 0);
	g_ImmediateContext->PSSetShader(g_PSMosaic, NULL, 0);
	//g_ImmediateContext->OMSetDepthStencilState(g_StencilRead, NULL);
	g_ImmediateContext->OMSetDepthStencilState(g_DepthStateDisable, NULL);
	g_ImmediateContext->OMSetRenderTargets(1, &g_RenderTargetViewWrite[g_CurrentTarget], g_DepthStencilView);

	DrawScreen(&g_WrittenTexture[g_CurrentResource]);

	// ���\�[�X�̐؂�ւ�
	g_CurrentResource = g_CurrentResource ? 0 : 1;
}
void ApplyChromaticAberration(void)
{
	// �^�[�Q�b�g�̐؂�ւ�
	g_CurrentTarget = g_CurrentTarget ? 0 : 1;

	SetCullingMode(CULL_MODE_BACK);
	g_ImmediateContext->VSSetShader(g_VSFilter, NULL, 0);
	g_ImmediateContext->GSSetShader(NULL, NULL, 0);
	g_ImmediateContext->PSSetShader(g_PSCA, NULL, 0);
	g_ImmediateContext->OMSetDepthStencilState(g_DepthStateDisable, NULL);
	g_ImmediateContext->OMSetRenderTargets(1, &g_RenderTargetViewWrite[g_CurrentTarget], g_DepthStencilView);

	DrawScreen(&g_WrittenTexture[g_CurrentResource]);

	// ���\�[�X�̐؂�ւ�
	g_CurrentResource = g_CurrentResource ? 0 : 1;
}
void ApplyMonitoring(void)
{
	// �^�[�Q�b�g�̐؂�ւ�
	g_CurrentTarget = g_CurrentTarget ? 0 : 1;

	SetCullingMode(CULL_MODE_BACK);
	g_ImmediateContext->VSSetShader(g_VSFilter, NULL, 0);
	g_ImmediateContext->GSSetShader(NULL, NULL, 0);
	g_ImmediateContext->PSSetShader(g_PSMonitoring, NULL, 0);
	g_ImmediateContext->OMSetDepthStencilState(g_DepthStateDisable, NULL);
	g_ImmediateContext->OMSetRenderTargets(1, &g_RenderTargetViewWrite[g_CurrentTarget], g_DepthStencilView);

	DrawScreen(&g_WrittenTexture[g_CurrentResource]);

	// ���\�[�X�̐؂�ւ�
	g_CurrentResource = g_CurrentResource ? 0 : 1;
}
void ApplyOldGame(void)
{
	// �^�[�Q�b�g�̐؂�ւ�
	g_CurrentTarget = g_CurrentTarget ? 0 : 1;

	SetCullingMode(CULL_MODE_BACK);
	g_ImmediateContext->VSSetShader(g_VSFilter, NULL, 0);
	g_ImmediateContext->GSSetShader(NULL, NULL, 0);
	g_ImmediateContext->PSSetShader(g_PSOldGame, NULL, 0);
	g_ImmediateContext->OMSetDepthStencilState(g_DepthStateDisable, NULL);
	g_ImmediateContext->OMSetRenderTargets(1, &g_RenderTargetViewWrite[g_CurrentTarget], g_DepthStencilView);

	DrawScreen(&g_WrittenTexture[g_CurrentResource]);

	// ���\�[�X�̐؂�ւ�
	g_CurrentResource = g_CurrentResource ? 0 : 1;
}
void DrawTarget(void)
{
	SetBlendState(BLEND_MODE_ALPHABLEND);
	SetCullingMode(CULL_MODE_BACK);
	g_ImmediateContext->VSSetShader(g_VSFilter, NULL, 0);
	g_ImmediateContext->GSSetShader(NULL, NULL, 0);
	g_ImmediateContext->PSSetShader(g_PSOnlyTex, NULL, 0);
	g_ImmediateContext->OMSetDepthStencilState(g_DepthStateDisable, NULL);
	g_ImmediateContext->OMSetRenderTargets(1, &g_RenderTargetView, NULL);

	DrawScreen(&g_WrittenTexture[g_CurrentResource]);

	g_CurrentTarget = 0;
	g_CurrentResource = 0;
}

void DrawScreen(ID3D11ShaderResourceView** pTexture)
{
	// �}�g���N�X�ݒ�
	MATRIX matrix;
	XMMATRIX worldViewProjection = XMMatrixOrthographicOffCenterLH(0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f);
	XMStoreFloat4x4(&matrix.WorldViewProjection, XMMatrixTranspose(worldViewProjection));
	GetDeviceContext()->UpdateSubresource(g_MatrixBuffer, 0, NULL, &matrix, 0, 0);

	// ���_�o�b�t�@�ݒ�
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_ScreenVertexBuffer, &stride, &offset);

	// �v���~�e�B�u�g�|���W�ݒ�
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// �e�N�X�`���ݒ�
	GetDeviceContext()->PSSetShaderResources(0, 1, pTexture);

	// �|���S���`��
	GetDeviceContext()->Draw(4, 0);
}

void SetTargetNormalMap(BOOL bDrawDirect)
{
	float ClearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	g_ImmediateContext->ClearRenderTargetView(g_RenderTargetViewNormalMap, ClearColor);

	SetCullingMode(CULL_MODE_BACK);
	g_ImmediateContext->VSSetShader(g_VSNormalMap, NULL, 0);
	g_ImmediateContext->GSSetShader(NULL, NULL, 0);
	g_ImmediateContext->PSSetShader(g_PSNormalMap, NULL, 0);
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
	g_ImmediateContext->VSSetShader(g_VSFilter, NULL, 0);
	g_ImmediateContext->GSSetShader(NULL, NULL, 0);
	g_ImmediateContext->PSSetShader(g_PSOnlyTex, NULL, 0);
	g_ImmediateContext->OMSetDepthStencilState(g_DepthStateDisable, NULL);
	g_ImmediateContext->OMSetRenderTargets(1, &g_RenderTargetView, g_DepthStencilView);

	DrawScreen(&g_NormalMapTexture);

	// �|���S���`��
	GetDeviceContext()->Draw(4, 0);
}

void SetTargetDepthMap(BOOL bDrawDirect)
{
	float ClearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	g_ImmediateContext->ClearRenderTargetView(g_RenderTargetViewDepthMap, ClearColor);
	g_ImmediateContext->ClearDepthStencilView(g_DepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	SetCullingMode(CULL_MODE_BACK);
	g_ImmediateContext->VSSetShader(g_VSDepthMap, NULL, 0);
	g_ImmediateContext->GSSetShader(NULL, NULL, 0);
	g_ImmediateContext->PSSetShader(g_PSDepthMap, NULL, 0);
	g_ImmediateContext->OMSetDepthStencilState(g_DepthStateEnable, NULL);
	if (bDrawDirect)
	{
		g_ImmediateContext->OMSetRenderTargets(1, &g_RenderTargetViewWrite[g_CurrentTarget], g_DepthStencilView);
	}
	else
	{
		g_ImmediateContext->OMSetRenderTargets(1, &g_RenderTargetViewDepthMap, g_DepthStencilView);
	}

	//// �}�g���N�X�ݒ�
	//D3DXMATRIX projection;
	//D3DXMatrixOrthoOffCenterLH(&projection, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f);
	//SetProjectionBuffer(&projection);
}
void DrawDepthMap(void)
{
	SetCullingMode(CULL_MODE_BACK);
	g_ImmediateContext->VSSetShader(g_VSFilter, NULL, 0);
	g_ImmediateContext->GSSetShader(NULL, NULL, 0);
	g_ImmediateContext->PSSetShader(g_PSOnlyTex, NULL, 0);
	g_ImmediateContext->OMSetDepthStencilState(g_DepthStateDisable, NULL);
	g_ImmediateContext->OMSetRenderTargets(1, &g_RenderTargetView, g_DepthStencilView);

	DrawScreen(&g_DepthMapTexture);

	// �|���S���`��
	GetDeviceContext()->Draw(4, 0);
}

// Float3��Float4�ɕϊ�
XMFLOAT4 Float4(XMFLOAT3& f3) {
	return { f3.x, f3.y, f3.z, 0.0f };
}

// �}�g���N�X�v�Z
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
// �o�b�N�o�b�t�@�N���A
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
	//g_ImmediateContext->ClearRenderTargetView(g_RenderTargetViewLight, ClearColor);
	//g_ImmediateContext->ClearRenderTargetView(g_RenderTargetViewLight[0], ClearColor);
	//g_ImmediateContext->ClearRenderTargetView(g_RenderTargetViewLight[1], ClearColor);
	g_ImmediateContext->ClearDepthStencilView(g_DepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}
void ClearStencil(void)
{
	g_ImmediateContext->ClearDepthStencilView(g_DepthStencilView, D3D11_CLEAR_STENCIL, 1.0f, 0);
}
void ClearDepth(void)
{
	g_ImmediateContext->ClearDepthStencilView(g_DepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}
void ClearWriteView(void)
{
	float ClearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	g_ImmediateContext->ClearRenderTargetView(g_RenderTargetViewWrite[0], ClearColor);
	g_ImmediateContext->ClearRenderTargetView(g_RenderTargetViewWrite[1], ClearColor);
}

//=============================================================================
// �f�o�b�O�p�e�L�X�g�o��
//=============================================================================
void DebugTextOut(char* text, int x, int y)
{
#if defined(_DEBUG) && defined(DEBUG_DISP_TEXTOUT)
	HRESULT hr;

	//�o�b�N�o�b�t�@����T�[�t�F�X��擾����
	IDXGISurface1* pBackSurface = NULL;
	hr = g_SwapChain->GetBuffer(0, __uuidof(IDXGISurface1), (void**)&pBackSurface);

	if (SUCCEEDED(hr))
	{
		//�擾�����T�[�t�F�X����f�o�C�X�R���e�L�X�g��擾����
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

			//�f�o�C�X�R���e�L�X�g��������
			pBackSurface->ReleaseDC(NULL);
		}
		//�T�[�t�F�X��������
		pBackSurface->Release();

		//�����_�����O�^�[�Q�b�g�����Z�b�g�����̂ŃZ�b�g���Ȃ���
		g_ImmediateContext->OMSetRenderTargets(1, &g_RenderTargetView, g_DepthStencilView);
	}
#endif
}
