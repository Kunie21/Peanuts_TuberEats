//=============================================================================
//
// �����_�����O���� [renderer.h]
// Author : 
//
//=============================================================================
#pragma once


//*********************************************************
// �}�N����`
//*********************************************************
#define LIGHT_MAX		(5)

enum LIGHT_TYPE
{
	LIGHT_TYPE_NONE,		//���C�g����
	LIGHT_TYPE_DIRECTIONAL,	//�f�B���N�V���i�����C�g
	LIGHT_TYPE_POINT,		//�|�C���g���C�g

	LIGHT_TYPE_NUM
};

enum BLEND_MODE
{
	BLEND_MODE_NONE,		//�u�����h����
	BLEND_MODE_ALPHABLEND,	//���u�����h
	BLEND_MODE_ADD,			//���Z�u�����h
	BLEND_MODE_SUBTRACT,	//���Z�u�����h

	BLEDD_MODE_NUM
};

enum CULL_MODE
{
	CULL_MODE_NONE,			//�J�����O����
	CULL_MODE_FRONT,		//�\�̃|���S����`�悵�Ȃ�(CW)
	CULL_MODE_BACK,			//���̃|���S����`�悵�Ȃ�(CCW)

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

// �C���X�^���V���O�\����
struct INSTANCE
{
	XMFLOAT4 pos[1024];
	XMFLOAT4 rot[1024];
	XMFLOAT4 scl[1024];
	XMFLOAT4 col[1024];
};

// UV���W�\����
struct UV_POSITION
{
	float u = 0.0f;
	float v = 0.0f;
	float uw = 1.0f;
	float vh = 1.0f;
};

// �ʒu�̎��
enum POSITION_TYPE
{
	POSITION_RELATIVE = 0,	// ���Έʒu
	POSITION_ABSOLUTE,		// ��Έʒu
};

// ���S�̎��
enum CENTER_TYPE
{
	CENTER_LEFTTOP = 0,	// ������w���Ƃ�
	CENTER_CENTER,		// ���S���w���Ƃ�
};

// �����ʒu
enum HORIZONTAL_POSITION
{
	HORIZONTAL_LEFT = 0,	// ����
	HORIZONTAL_CENTER,		// ������
	HORIZONTAL_RIGHT,		// �E��
};

// �����ʒu
enum VERTICAL_POSITION
{
	VERTICAL_TOP = 0,		// ���
	VERTICAL_MIDDLE,		// ������
	VERTICAL_BOTTOM,		// ����
};

//*********************************************************
// �\����
//*********************************************************

// ���_�\����
struct VERTEX_3D
{
    XMFLOAT3	Position;
    XMFLOAT3	Normal;
    XMFLOAT4	Diffuse;
    XMFLOAT2	TexCoord;
};

// �}�g���N�X�\����
struct WVP_MATRIX
{
	XMFLOAT4X4	World;
	XMFLOAT4X4	View;
	XMFLOAT4X4	Projection;
	XMFLOAT4X4	WorldViewProjection;
	XMFLOAT4X4	ViewProjection;
};

// �}�e���A���\����
struct MATERIAL
{
	XMFLOAT4	Ambient;
	XMFLOAT4	Diffuse;
	XMFLOAT4	Specular;
	XMFLOAT4	Emission;
	float		Shininess;
	int			noTexSampling;
};

// ���C�g�\����
struct LIGHT {
	XMFLOAT3	Direction;	// ���C�g�̕���
	XMFLOAT3	Position;	// ���C�g�̈ʒu
	XMFLOAT4	Diffuse;	// �g�U���̐F
	XMFLOAT4	Ambient;	// �����̐F
	float		Attenuation;// ������
	int			Type;		// ���C�g��ʁE�L���t���O
	int			Enable;		// ���C�g��ʁE�L���t���O
};

// �t�H�O�\����
struct FOG {
	float		FogStart;	// �t�H�O�̊J�n����
	float		FogEnd;		// �t�H�O�̍ő勗��
	XMFLOAT4	FogColor;	// �t�H�O�̐F
};

//�@���Ԍo�ߗp�\����
struct TIME
{
	int Time;
	int Dummy[3];
};

//*****************************************************************************
// �v���g�^�C�v�錾
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