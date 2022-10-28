//=============================================================================
//
// レンダリング処理 [renderer.h]
// Author : 國江 翔太
//
//=============================================================================
#pragma once


//*********************************************************
// マクロ定義
//*********************************************************
#define LIGHT_MAX		(5)
#define LIGHT_DIRECTIONAL_MAX	5
#define LIGHT_POINT_MAX			10
#define LIGHT_SPOT_MAX			10

//*********************************************************
// 列挙子
//*********************************************************
enum FILTER_MODE {	// フィルターモード
	FILTER_MODE_NONE,			// フィルタなし
	FILTER_MODE_AVERAGING,		// 平均化フィルタ
	FILTER_MODE_GAUSSIAN,		// ガウシアンフィルタ
	FILTER_MODE_SHARPNING,		// 鮮鋭化フィルタ弱
	FILTER_MODE_SHARPNING_HIGH,	// 鮮鋭化フィルタ強
	FILTER_MODE_LAPLACIAN,		// ラプラシアンフィルタ
	FILTER_MODE_LAPLACIAN_COLOR,// ラプラシアンフィルタカラー
	FILTER_MODE_PREWITT_X,		// プリューウィットフィルタ横
	FILTER_MODE_PREWITT_Y,		// プリューウィットフィルタ縦
	FILTER_MODE_SOBEL_X,		// ソーベルフィルタ横
	FILTER_MODE_SOBEL_Y,		// ソーベルフィルタ縦

	FILTER_MODE_NUM
};
enum BLEND_MODE {		// ブレンドモード
	BLEND_MODE_NONE,		//ブレンド無し
	BLEND_MODE_ALPHABLEND,	//αブレンド
	BLEND_MODE_ADD,			//加算ブレンド
	BLEND_MODE_SUBTRACT,	//減算ブレンド

	BLEDD_MODE_NUM
};
enum CULL_MODE {		// カリングモード
	CULL_MODE_NONE,			//カリング無し
	CULL_MODE_FRONT,		//表のポリゴンを描画しない(CW)
	CULL_MODE_BACK,			//裏のポリゴンを描画しない(CCW)

	CULL_MODE_NUM
};
enum POSITION_TYPE {	// 位置の種類
	POSITION_RELATIVE = 0,	// 相対位置
	POSITION_ABSOLUTE,		// 絶対位置
};
enum CENTER_TYPE {	// 中心の種類
	CENTER_LEFTTOP = 0,	// 左上を指すとき
	CENTER_CENTER,		// 中心を指すとき
};
enum HORIZONTAL_POSITION {	// 水平位置の種類
	HORIZONTAL_LEFT = 0,	// 左寄せ
	HORIZONTAL_CENTER,		// 中央寄せ
	HORIZONTAL_RIGHT,		// 右寄せ
};
enum VERTICAL_POSITION {	// 垂直位置の種類
	VERTICAL_TOP = 0,		// 上寄せ
	VERTICAL_MIDDLE,		// 中央寄せ
	VERTICAL_BOTTOM,		// 下寄せ
};

//*********************************************************
// 構造体
//*********************************************************
struct INSTANCE {		// インスタンシングバッファ用構造体
	XMFLOAT4 pos[1024];
	XMFLOAT4 rot[1024];
	XMFLOAT4 scl[1024];
	XMFLOAT4 col[1024];
};
struct VERTEX_3D {		// 頂点バッファ用構造体
    XMFLOAT3	Position;
    XMFLOAT3	Normal;
    XMFLOAT4	Diffuse;
    XMFLOAT2	TexCoord;
};
struct MATRIX {		// マトリクスバッファ用構造体
	XMFLOAT4X4	World;
	XMFLOAT4X4	View;
	XMFLOAT4X4	Projection;
	XMFLOAT4X4	WorldViewProjection;
	XMFLOAT4X4	ViewProjection;
	XMFLOAT4X4	AfterRotation;
};
struct CAMERA {			// カメラバッファ用構造体
	XMFLOAT4	Position;
	XMFLOAT4	ViewVolume;
};
struct MATERIAL {		// マテリアルバッファ用構造体
	XMFLOAT4	Ambient;
	XMFLOAT4	Diffuse;
	XMFLOAT4	Specular;
	XMFLOAT4	Emission;
	float		Shininess;
	int			noTexSampling;
	float		Dummy[2];		//16bit境界用
};
struct LIGHT {			// ライトバッファ用構造体
	XMFLOAT4	Direction;	// ライトの方向
	XMFLOAT4	Position;	// ライトの位置
	XMFLOAT4	Diffuse;	// 拡散光の色
	XMFLOAT4	Ambient;	// 環境光の色
	float		Attenuation;// 減衰率
	int			Type;		// ライト種別・有効フラグ
	int			Enable;		// ライト種別・有効フラグ
	float		Dummy;			//16bit境界用
};
struct LIGHT_AMBIENT {	// アンビエントライトバッファ用構造体
	XMFLOAT4	Color;			// 色
	float		Intensity;		// 強度
	int			Use;			// 使用
	float		Dummy[2];
};
struct LIGHT_DIRECTIONAL {	// ディレクショナルライトバッファ用構造体
	XMFLOAT4	Color;			// 色
	XMFLOAT4	Direction;		// 向き
	float		Intensity;		// 強度
	int			Use;			// 使用
	float		Dummy[2];
};
struct LIGHT_POINT {		// ポイントライトバッファ用構造体
	XMFLOAT4	Color;			// 色
	XMFLOAT4	Position;		// 位置
	float		Intensity;		// 強度
	float		Attenuation;	// 減衰距離
	int			Use;			// 使用
	float		Dummy;
};
struct LIGHT_SPOT {		// スポットライトバッファ用構造体
	XMFLOAT4	Color;			// 色
	XMFLOAT4	Position;		// 位置
	XMFLOAT4	Direction;		// 向き
	float		Intensity;		// 強度
	float		Attenuation;	// 減衰距離
	float		Angle;			// 角度
	int			Use;			// 使用
};
struct LIGHT_NO {		// ライト番号バッファ用構造体
	int			lightNo;
	float		Dummy[3];
};
struct OUTLINE {		// アウトライン処理バッファ用構造体
	XMFLOAT4	Scale;
	XMFLOAT4	Color;
};
struct MOSAIC {			// モザイク処理バッファ用構造体
	int			Size;
	float		Dummy[3];
};
struct CURVE_BUFFER {			// カーブ処理バッファ用構造体
	XMFLOAT2	Angle = { 0.0f, 0.0f };
	float		TexPos = 0.0f;
	float		Dummy;
};
struct CONSTANT {		// 定数バッファ用構造体
	int			Time;
	float		Dummy[3];
};
struct UV_POSITION {	// UV座標構造体
	float u = 0.0f;
	float v = 0.0f;
	float uw = 1.0f;
	float vh = 1.0f;
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitRenderer(HINSTANCE hInstance, HWND hWnd, BOOL bWindow);
void UninitRenderer(void);

void Clear(void);
void Present(void);

ID3D11Device *GetDevice(void);
ID3D11DeviceContext *GetDeviceContext(void);

void SetDepthEnable(BOOL Enable);
void SetBlendState(BLEND_MODE bm);
void SetCullingMode(CULL_MODE cm);

void SetAmbientLight(LIGHT_AMBIENT* pAmbientLight);
void SetDirectionalLight(LIGHT_DIRECTIONAL* pDirectionalLight);
void SetPointLight(LIGHT_POINT* pPointLight);
void SetSpotLight(LIGHT_SPOT* pSpotLight);
void SetLightNo(int lightNo);

void SetFrameTime(int time);

void SetWorldViewProjection2D(void);
void SetWorldBuffer(XMMATRIX* WorldMatrix);
void SetViewBuffer(XMMATRIX* ViewMatrix);
void SetProjectionBuffer(XMMATRIX* ProjectionMatrix);
void SetCameraBuffer(CAMERA* Camera);
void SetMaterialBuffer(MATERIAL* Material);
void SetCurveBuffer(CURVE_BUFFER* curve);
void SetAfterRotation(XMMATRIX* AfterRotationMatrix);


XMFLOAT4 GetBackGroundColor(void);
void SetBackGroundColor(XMFLOAT4 color);

//void SetStencilWrite(void);
void SetStencilWriteDL(void);
void SetStencilWritePL(void);
void SetStencilWriteSL(void);
void SetStencilRead(void);
void SetStencilReadDL(void);
void SetStencilReadPL(void);
void SetStencilReadSL(void);
void SetStencilNone(void);
void SetStencilNoneAL(void);
void SetStencilNoneOnlyDepth(void);
void SetDrawOutline(float Scale, XMFLOAT4 Color = { 0.0f, 0.0f, 0.0f, 1.0f });
void SetDrawFillBlack(void);
void SetDrawNoLighting(void);
void SetDrawTube(void);
void SetDrawGimmick(void);
void SetDraw2DTexture(void);
//void SetDrawPreOutline(void);
//void SetDrawPostOutline(void);
//void DrawExpOutline(void);
//void DrawOutline(void);

void ClearStencil(void);
void ClearWriteView(void);

void ApplyFilter(FILTER_MODE filter);
void ApplyMosaic(int pixel_size);
void ApplyChromaticAberration(void);
void ApplyMonitoring(void);
void ApplyOldGame(void);

void DrawTarget(void);
void DrawScreen(ID3D11ShaderResourceView** pTexture);

void SetTargetNormalMap(BOOL bDrawDirect = FALSE);
void DrawNormalMap(void);
void SetTargetDepthMap(BOOL bDrawDirect = FALSE);
void DrawDepthMap(void);

void DebugTextOut(char* text, int x, int y);

//void SetBackColor(XMFLOAT4 color);
//void Resize(long winW, long winH);

void CreateBuffer(D3D11_BUFFER_DESC* pDesc, int slot, ID3D11Buffer** pBuffer, UINT size);
void CreateShader(const char* pFileName, const char* pEntryPoint, ID3D11VertexShader** pVS, DWORD shFlag);
void CreateShader(const char* pFileName, const char* pEntryPoint, ID3D11PixelShader** pPS, DWORD shFlag);
void CreateShader(const char* pFileName, const char* pEntryPoint, ID3D11GeometryShader** pGS, DWORD shFlag);

ID3D11Buffer* GetInstanceBuffer(void);
void SetShaderInstanceingBillboard(XMFLOAT4X4 mtxView);
void SetShaderDefault(void);

XMFLOAT4 Float4(XMFLOAT3* f3);
