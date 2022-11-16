//=============================================================================
//
// �����_�����O���� [renderer.h]
// Author : ���] �đ�
//
//=============================================================================
#pragma once


//*********************************************************
// �}�N����`
//*********************************************************
#define LIGHT_MAX		(5)
#define LIGHT_DIRECTIONAL_MAX	5
#define LIGHT_POINT_MAX			10
#define LIGHT_SPOT_MAX			10

//*********************************************************
// �񋓎q
//*********************************************************
enum FILTER_MODE {	// �t�B���^�[���[�h
	FILTER_MODE_NONE,			// �t�B���^�Ȃ�
	FILTER_MODE_AVERAGING,		// ���ω��t�B���^
	FILTER_MODE_GAUSSIAN,		// �K�E�V�A���t�B���^
	FILTER_MODE_SHARPNING,		// �N�s���t�B���^��
	FILTER_MODE_SHARPNING_HIGH,	// �N�s���t�B���^��
	FILTER_MODE_LAPLACIAN,		// ���v���V�A���t�B���^
	FILTER_MODE_LAPLACIAN_COLOR,// ���v���V�A���t�B���^�J���[
	FILTER_MODE_PREWITT_X,		// �v�����[�E�B�b�g�t�B���^��
	FILTER_MODE_PREWITT_Y,		// �v�����[�E�B�b�g�t�B���^�c
	FILTER_MODE_SOBEL_X,		// �\�[�x���t�B���^��
	FILTER_MODE_SOBEL_Y,		// �\�[�x���t�B���^�c

	FILTER_MODE_NUM
};
enum BLEND_MODE {		// �u�����h���[�h
	BLEND_MODE_NONE,		//�u�����h����
	BLEND_MODE_ALPHABLEND,	//���u�����h
	BLEND_MODE_ADD,			//���Z�u�����h
	BLEND_MODE_SUBTRACT,	//���Z�u�����h

	BLEDD_MODE_NUM
};
enum CULL_MODE {		// �J�����O���[�h
	CULL_MODE_NONE,			//�J�����O����
	CULL_MODE_FRONT,		//�\�̃|���S����`�悵�Ȃ�(CW)
	CULL_MODE_BACK,			//���̃|���S����`�悵�Ȃ�(CCW)

	CULL_MODE_NUM
};
enum POSITION_TYPE {	// �ʒu�̎��
	POSITION_RELATIVE = 0,	// ���Έʒu�i��ʒ��S��0,0�Ƃ���Position�𐔂���j
	POSITION_ABSOLUTE,		// ��Έʒu�i��ʍ����0,0�Ƃ���Position�𐔂���j
};
enum CENTER_TYPE {		// ���S�̎��
	CENTER_LEFTTOP = 0,		// Position���e�N�X�`���̍����w���Ƃ�
	CENTER_CENTER,			// Position���e�N�X�`���̒��S��w���Ƃ�
};
enum HORIZONTAL_POSITION {	// �����ʒu�̎��
	HORIZONTAL_LEFT = 0,	// ����
	HORIZONTAL_CENTER,		// ������
	HORIZONTAL_RIGHT,		// �E��
};
enum VERTICAL_POSITION {	// �����ʒu�̎��
	VERTICAL_TOP = 0,		// ���
	VERTICAL_MIDDLE,		// ������
	VERTICAL_BOTTOM,		// ����
};
enum SHADER_TYPE {	// �V�F�[�_�[�̎��
	SHADER_TUBE = 0,	// �p�C�v�p�i�J�[�u���e�N�X�`�����ړ�����j
	SHADER_GIMMICK,		// �M�~�b�N�p�i�J�[�u�j
	SHADER_PLAYER,		// �v���C���[�p�i���ɖ����j
};

//*********************************************************
// �\����
//*********************************************************
#define INSTANCE_MAX 100
struct INSTANCE {		// �C���X�^���V���O�o�b�t�@�p�\����
	XMFLOAT4 pos[INSTANCE_MAX];
	XMFLOAT4 rot[INSTANCE_MAX];
	XMFLOAT4 scl[INSTANCE_MAX];
	XMFLOAT4 col[INSTANCE_MAX];
	XMFLOAT4 txc[INSTANCE_MAX];
};
struct VERTEX_3D {		// ���_�o�b�t�@�p�\����
	XMFLOAT3	Position;
	XMFLOAT3	Normal;
	XMFLOAT4	Diffuse;
	XMFLOAT2	TexCoord;
};
struct MATRIX {		// �}�g���N�X�o�b�t�@�p�\����
	XMFLOAT4X4	World;
	XMFLOAT4X4	View;
	XMFLOAT4X4	Projection;
	XMFLOAT4X4	WorldViewProjection;
	XMFLOAT4X4	ViewProjection;
	XMFLOAT4X4	AfterRotation;
};
struct CAMERA {			// �J�����o�b�t�@�p�\����
	XMFLOAT4	Position;
	XMFLOAT4	ViewVolume;
};
struct MATERIAL {		// �}�e���A���o�b�t�@�p�\����
	XMFLOAT4	Ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	XMFLOAT4	Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	XMFLOAT4	Specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	XMFLOAT4	Emission = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	float		Shininess = 10.0f;
	int			noTexSampling = 0;
	float		Dummy[2];		//16bit���E�p
};
struct LIGHT {			// ���C�g�o�b�t�@�p�\����
	XMFLOAT4	Direction;	// ���C�g�̕���
	XMFLOAT4	Position;	// ���C�g�̈ʒu
	XMFLOAT4	Diffuse;	// �g�U���̐F
	XMFLOAT4	Ambient;	// �����̐F
	float		Attenuation;// ������
	int			Type;		// ���C�g��ʁE�L���t���O
	int			Enable;		// ���C�g��ʁE�L���t���O
	float		Dummy;			//16bit���E�p
};
struct LIGHT_AMBIENT {	// �A���r�G���g���C�g�o�b�t�@�p�\����
	XMFLOAT4	Color;			// �F
	float		Intensity;		// ���x
	int			Use;			// �g�p
	float		Dummy[2];
};
struct LIGHT_DIRECTIONAL {	// �f�B���N�V���i�����C�g�o�b�t�@�p�\����
	XMFLOAT4	Color;			// �F
	XMFLOAT4	Direction;		// ����
	float		Intensity;		// ���x
	int			Use;			// �g�p
	float		Dummy[2];
};
struct LIGHT_POINT {		// �|�C���g���C�g�o�b�t�@�p�\����
	XMFLOAT4	Color;			// �F
	XMFLOAT4	Position;		// �ʒu
	float		Intensity;		// ���x
	float		Attenuation;	// ��������
	int			Use;			// �g�p
	float		Dummy;
};
struct LIGHT_SPOT {		// �X�|�b�g���C�g�o�b�t�@�p�\����
	XMFLOAT4	Color;			// �F
	XMFLOAT4	Position;		// �ʒu
	XMFLOAT4	Direction;		// ����
	float		Intensity;		// ���x
	float		Attenuation;	// ��������
	float		Angle;			// �p�x
	int			Use;			// �g�p
};
struct LIGHT_NO {		// ���C�g�ԍ��o�b�t�@�p�\����
	int			lightNo;
	float		Dummy[3];
};
struct OUTLINE {		// �A�E�g���C�������o�b�t�@�p�\����
	XMFLOAT4	Scale;
	XMFLOAT4	Color;
};
struct MOSAIC {			// ���U�C�N�����o�b�t�@�p�\����
	int			Size;
	float		Dummy[3];
};
struct CURVE_BUFFER {			// �J�[�u�����o�b�t�@�p�\����
	XMFLOAT2	Angle = { 0.0f, 0.0f };
	float		TexPos = 0.0f;
	float		Dummy;
};
struct CONSTANT {		// �萔�o�b�t�@�p�\����
	int			Time;
	float		Dummy[3];
};
struct UV_POSITION {	// UV���W�\����
	float u = 0.0f;
	float v = 0.0f;
	float uw = 1.0f;
	float vh = 1.0f;
};
struct SRT {	// ScaleRotTrans�\����
	XMFLOAT3 pos = { 1.0f, 1.0f, 1.0f };
	XMFLOAT3 rot = { 0.0f, 0.0f, 0.0f };
	XMFLOAT3 scl = { 0.0f, 0.0f, 0.0f };
};

//*****************************************************************************
// �v���g�^�C�v�錾
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
void SetWorldBuffer(SRT& srt);
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
void SetStencilWriteLL(SHADER_TYPE shader);
void SetStencilRead(void);
void SetStencilReadDL(void);
void SetStencilReadPL(void);
void SetStencilReadSL(void);
void SetStencilReadLL(SHADER_TYPE shader);
void SetStencilReadLLGimmick(void);
void SetStencilNone(void);
void SetStencilNoneAL(SHADER_TYPE shader);
void SetStencilNoneOnlyDepth(void);
void SetDrawOutline(float Scale, XMFLOAT4 Color = { 0.0f, 0.0f, 0.0f, 1.0f });
void SetDrawFillBlack(SHADER_TYPE shader);
void SetDrawFillBlackPlayer(void);
void SetDrawNoLighting(void);
void SetDrawTube(void);
void SetDrawGimmick(void);
void SetDrawPlayer(void);
void SetDrawTubeLight(void);
void SetDrawFire(void);
void SetDrawMissileFire(void);
void SetDraw2DTexture(void);
//void SetDrawPreOutline(void);
//void SetDrawPostOutline(void);
//void DrawExpOutline(void);
//void DrawOutline(void);

void ClearStencil(void);
void ClearDepth(void);
void ClearWriteView(void);

void SetDrawLight(void);
void SetDrawInstancingOnlyTex(void);
void ApplyLightToTarget(void);

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
void SetShaderInstanceingOnlyTex(BOOL bInterrupt = FALSE);
//void SetShaderInstanceingBillboard(XMFLOAT4X4 mtxView);
void SetShaderDefault(void);

XMFLOAT4 Float4(XMFLOAT3& f3);

void MulMtxScl(XMMATRIX& mtxWorld, XMFLOAT3& scl);
void MulMtxScl(XMMATRIX& mtxWorld, float x, float y, float z);
void MulMtxRot(XMMATRIX& mtxWorld, XMFLOAT3& rot);
void MulMtxRot(XMMATRIX& mtxWorld, float x, float y, float z);
void MulMtxPos(XMMATRIX& mtxWorld, XMFLOAT3& pos);
void MulMtxPos(XMMATRIX& mtxWorld, float x, float y, float z);
