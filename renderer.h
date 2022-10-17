//=============================================================================
//
// レンダリング処理 [renderer.h]
// Author : 
//
//=============================================================================
#pragma once


//*********************************************************
// マクロ定義
//*********************************************************
#define LIGHT_MAX		(5)

enum LIGHT_TYPE
{
	LIGHT_TYPE_NONE,		//ライト無し
	LIGHT_TYPE_DIRECTIONAL,	//ディレクショナルライト
	LIGHT_TYPE_POINT,		//ポイントライト

	LIGHT_TYPE_NUM
};

enum BLEND_MODE
{
	BLEND_MODE_NONE,		//ブレンド無し
	BLEND_MODE_ALPHABLEND,	//αブレンド
	BLEND_MODE_ADD,			//加算ブレンド
	BLEND_MODE_SUBTRACT,	//減算ブレンド

	BLEDD_MODE_NUM
};

enum CULL_MODE
{
	CULL_MODE_NONE,			//カリング無し
	CULL_MODE_FRONT,		//表のポリゴンを描画しない(CW)
	CULL_MODE_BACK,			//裏のポリゴンを描画しない(CCW)

	CULL_MODE_NUM
};

enum SHADER_MODE
{
	SHADER_MODE_DEFAULT,
	SHADER_MODE_TOON,
	SHADER_MODE_HOLD_TOON,
	SHADER_MODE_NON_SHADOW,
	SHADER_MODE_SHADOWMAPDRAW,
	SHADER_MODE_WATERFALL,
	SHADER_MODE_ICESTAGE,
	SHADER_MODE_SANDSTAGE,
	SHADER_MODE_FIRESTAGE,
	SHADER_MODE_DARKSTAGE,

	SHADER_MODE_NUM
};

// インスタンシング構造体
struct INSTANCE
{
	XMFLOAT4 pos[1024];
	XMFLOAT4 rot[1024];
	XMFLOAT4 scl[1024];
	XMFLOAT4 col[1024];
};

// UV座標構造体
struct UV_POSITION
{
	float u = 0.0f;
	float v = 0.0f;
	float uw = 1.0f;
	float vh = 1.0f;
};

// 位置の種類
enum POSITION_TYPE
{
	POSITION_RELATIVE = 0,	// 相対位置
	POSITION_ABSOLUTE,		// 絶対位置
};

// 中心の種類
enum CENTER_TYPE
{
	CENTER_LEFTTOP = 0,	// 左上を指すとき
	CENTER_CENTER,		// 中心を指すとき
};

// 水平位置
enum HORIZONTAL_POSITION
{
	HORIZONTAL_LEFT = 0,	// 左寄せ
	HORIZONTAL_CENTER,		// 中央寄せ
	HORIZONTAL_RIGHT,		// 右寄せ
};

// 垂直位置
enum VERTICAL_POSITION
{
	VERTICAL_TOP = 0,		// 上寄せ
	VERTICAL_MIDDLE,		// 中央寄せ
	VERTICAL_BOTTOM,		// 下寄せ
};

//*********************************************************
// 構造体
//*********************************************************

// 頂点構造体
struct VERTEX_3D
{
    XMFLOAT3	Position;
    XMFLOAT3	Normal;
    XMFLOAT4	Diffuse;
    XMFLOAT2	TexCoord;
};

// マトリクス構造体
struct WVP_MATRIX
{
	XMFLOAT4X4	World;
	XMFLOAT4X4	View;
	XMFLOAT4X4	Projection;
	XMFLOAT4X4	WorldViewProjection;
	XMFLOAT4X4	ViewProjection;
};

// マテリアル構造体
struct MATERIAL
{
	XMFLOAT4	Ambient;
	XMFLOAT4	Diffuse;
	XMFLOAT4	Specular;
	XMFLOAT4	Emission;
	float		Shininess;
	int			noTexSampling;
};

// ライト構造体
struct LIGHT {
	XMFLOAT3	Direction;	// ライトの方向
	XMFLOAT3	Position;	// ライトの位置
	XMFLOAT4	Diffuse;	// 拡散光の色
	XMFLOAT4	Ambient;	// 環境光の色
	float		Attenuation;// 減衰率
	int			Type;		// ライト種別・有効フラグ
	int			Enable;		// ライト種別・有効フラグ
};

// フォグ構造体
struct FOG {
	float		FogStart;	// フォグの開始距離
	float		FogEnd;		// フォグの最大距離
	XMFLOAT4	FogColor;	// フォグの色
};

//　時間経過用構造体
struct TIME
{
	int Time;
	int Dummy[3];
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitRenderer(HINSTANCE hInstance, HWND hWnd, BOOL bWindow);
void UninitRenderer(void);

void Clear(void);
void Present(void);

ID3D11Device *GetDevice( void );
ID3D11DeviceContext *GetDeviceContext( void );

void SetDepthEnable( BOOL Enable );
void SetBlendState(BLEND_MODE bm);
void SetCullingMode(CULL_MODE cm);
void SetAlphaTestEnable(BOOL flag);

void SetWorldViewProjection2D( void );
void SetWorldMatrix( XMMATRIX *WorldMatrix );
void SetViewMatrix( XMMATRIX *ViewMatrix );
void SetProjectionMatrix( XMMATRIX *ProjectionMatrix );
void SetLViewMatrix(XMMATRIX *ViewMatrix);
void SetLProjectionMatrix(XMMATRIX *ProjectionMatrix);

void SetMaterial( MATERIAL material );

void SetLightEnable(BOOL flag);
void SetLight(int index, LIGHT* light);

void SetFogEnable(BOOL flag);
void SetFog(FOG* fog);

void DebugTextOut(char* text, int x, int y);

void SetFuchi(int flag);
void SetShaderCamera(XMFLOAT3 pos);
void SetShaderFuchiColor(XMFLOAT4 color);

void SetTime(int time);

void SetShaderMode(int mode);
void LimitRenderTargetViewShadowMap(BOOL mode);
void SetShaderResourceShadowMap(BOOL mode);
void ClearOnlyDepthStencil(void);


void SetBackColor(XMFLOAT4 color);
void Resize(long winW, long winH);

void CreateBuffer(D3D11_BUFFER_DESC* pDesc, int slot, ID3D11Buffer** pBuffer, UINT size);
void CreateShader(const char* pFileName, const char* pEntryPoint, ID3D11VertexShader** pVS, DWORD shFlag);
void CreateShader(const char* pFileName, const char* pEntryPoint, ID3D11PixelShader** pPS, DWORD shFlag);
void CreateShader(const char* pFileName, const char* pEntryPoint, ID3D11GeometryShader** pGS, DWORD shFlag);

ID3D11Buffer * GetInstanceBuffer(void);
void SetShaderInstanceingBillboard(XMFLOAT4X4 mtxView);
void SetShaderDefault(void);

XMFLOAT4 Float4(XMFLOAT3 f3);