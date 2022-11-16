//=============================================================================
//
// 2D�摜�`�揈�� [2Dtexture.cpp]
// Author : ���] �đ�
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "texture2d.h"
#include "camera.h"

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer*				g_VertexBuffer = NULL;		// ���_���
static BOOL							g_Load = FALSE;

static INSTANCE						g_Instance;
static int							g_InstenceCount = 0;
static ID3D11ShaderResourceView**	g_pTexture[INSTANCE_MAX];	// �e�N�X�`�����

//#define NO_INSTANCING	// �C���X�^���V���O���Ȃ��o�[�W����

//=============================================================================
// ����������
//=============================================================================
HRESULT InitTexture2D(void)
{
	// ���_�o�b�t�@����
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);

	//���_�o�b�t�@�̒��g�𖄂߂�
	D3D11_MAPPED_SUBRESOURCE msr;
	GetDeviceContext()->Map(g_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

	VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

	// ���_���W�̐ݒ�
	vertex[0].Position = XMFLOAT3(-0.5f, -0.5f, 0.0f);
	vertex[1].Position = XMFLOAT3(0.5f, -0.5f, 0.0f);
	vertex[2].Position = XMFLOAT3(-0.5f, 0.5f, 0.0f);
	vertex[3].Position = XMFLOAT3(0.5f, 0.5f, 0.0f);

	// �@���̐ݒ�
	vertex[0].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
	vertex[1].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
	vertex[2].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
	vertex[3].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);

	// ���ˌ��̐ݒ�
	vertex[0].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[1].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[2].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[3].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	// �e�N�X�`�����W�̐ݒ�
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

	// ���_���W�̐ݒ�
	vertex[0].Position = XMFLOAT3(-0.5f, -0.5f, 0.0f);
	vertex[1].Position = XMFLOAT3(0.5f, -0.5f, 0.0f);
	vertex[2].Position = XMFLOAT3(-0.5f, 0.5f, 0.0f);
	vertex[3].Position = XMFLOAT3(0.5f, 0.5f, 0.0f);

	// �@���̐ݒ�
	vertex[0].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
	vertex[1].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
	vertex[2].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
	vertex[3].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);

	// ���ˌ��̐ݒ�
	vertex[0].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[1].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[2].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[3].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	// �e�N�X�`�����W�̐ݒ�
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

	// ���_���W�̐ݒ�
	vertex[0].Position = XMFLOAT3(-0.5f, -0.5f, 0.0f);
	vertex[1].Position = XMFLOAT3(0.5f, -0.5f, 0.0f);
	vertex[2].Position = XMFLOAT3(-0.5f, 0.5f, 0.0f);
	vertex[3].Position = XMFLOAT3(0.5f, 0.5f, 0.0f);

	// �@���̐ݒ�
	vertex[0].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
	vertex[1].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
	vertex[2].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
	vertex[3].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);

	// ���ˌ��̐ݒ�
	vertex[0].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[1].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[2].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[3].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	// �e�N�X�`�����W�̐ݒ�
	vertex[0].TexCoord = { 0.0f, 0.0f };
	vertex[1].TexCoord = { 1.0f, 0.0f };
	vertex[2].TexCoord = { 0.0f, 1.0f };
	vertex[3].TexCoord = { 1.0f, 1.0f };

	GetDeviceContext()->Unmap(g_VertexBuffer, 0);
}

//=============================================================================
// �I������
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
// �X�V����
//=============================================================================
void UpdateTexture2D(void)
{

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawTexture2D(TEXTURE2D_DESC* td, BOOL bShadow, BOOL bUV)
{
#ifdef NO_INSTANCING
	// UV���W�̍Đݒ�
	if (bUV) { SetUVTexture2D(&td->uv_pos); };
#endif

	// �ʒu�̌v�Z
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

	// �}�e���A���ݒ�
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = td->col;
	// �e���ݒ肳��Ă���ꍇ
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

	// �e�N�X�`���ݒ�
	GetDeviceContext()->PSSetShaderResources(0, 1, td->tex);

	SetDepthEnable(FALSE);

	// ���_�o�b�t�@�ݒ�
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// �v���~�e�B�u�g�|���W�ݒ�
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// 2D�}�g���N�X�ݒ�
	SetViewBuffer(&XMMatrixIdentity());
	SetProjectionBuffer(&XMMatrixOrthographicOffCenterLH(0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f));


	// ���[���h�}�g���N�X�̏�����
	XMMATRIX mtxWorld = XMMatrixIdentity();

	// �@S�g�嗦�̔��f
	mtxWorld = XMMatrixMultiply(mtxWorld, XMMatrixScaling(
		td->size.x * td->scl.x,
		td->size.y * td->scl.y,
		1.0f
	));

	// �AR��]�̔��f
	mtxWorld = XMMatrixMultiply(mtxWorld, XMMatrixRotationRollPitchYaw(
		0.0f,
		0.0f,
		td->rot
	));

	// �BT�ʒu�̔��f
	mtxWorld = XMMatrixMultiply(mtxWorld, XMMatrixTranslation(
		pos.x,
		pos.y,
		0.0f
	));

	// ���[���h�}�g���N�X��Z�b�g
	SetWorldBuffer(&mtxWorld);

	// �|���S���̕`��
	GetDeviceContext()->Draw(4, 0);

	SetDepthEnable(TRUE);

#else
	// �e�N�X�`���ݒ�
	g_pTexture[g_InstenceCount] = td->tex;

	// �C���X�^���X����o�^
	g_Instance.scl[g_InstenceCount] = { td->size.x * td->scl.x, td->size.y * td->scl.y, 1.0f , 0.0f };
	g_Instance.rot[g_InstenceCount] = { 0.0f, 0.0f, td->rot, 0.0f };
	g_Instance.pos[g_InstenceCount] = { pos.x, pos.y, 0.0f, 0.0f };
	g_Instance.col[g_InstenceCount] = material.Diffuse;
	g_Instance.txc[g_InstenceCount] = { td->uv_pos.u, td->uv_pos.v, td->uv_pos.uw, td->uv_pos.vh };

	// �C���X�^���X����X�V
	g_InstenceCount++;
#endif

	// �e���ݒ肳��Ă���ꍇ
	if (bShadow) { DrawTexture2D(td); }	// �{�̂�`�悷��

#ifdef NO_INSTANCING
	// UV���W�̃��Z�b�g
	if (bUV) { ResetUVTexture2D(); };
#endif
}


void DrawTexture2DAll(BOOL bInterrupt)
{
	// �C���X�^���X����o�^
	D3D11_MAPPED_SUBRESOURCE msr;
	GetDeviceContext()->Map(GetInstanceBuffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
	*(INSTANCE*)msr.pData = g_Instance;
	GetDeviceContext()->Unmap(GetInstanceBuffer(), 0);
	
	// �e�N�X�`���ݒ�
	for (int i = 0; i < g_InstenceCount; i++) { GetDeviceContext()->PSSetShaderResources(i + 1, 1, g_pTexture[i]); }

	// ���_�o�b�t�@�ݒ�
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// �v���~�e�B�u�g�|���W�ݒ�
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// 2D�}�g���N�X�ݒ�
	SetViewBuffer(&XMMatrixIdentity());
	SetProjectionBuffer(&XMMatrixOrthographicOffCenterLH(0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 0.0f, 1.0f));
	SetWorldBuffer(&XMMatrixIdentity());

	// �C���X�^���V���O�`��ݒ�
	SetShaderInstanceingOnlyTex(bInterrupt);
	
	// �C���X�^���V���O�`��
	GetDeviceContext()->DrawInstanced(4, g_InstenceCount, 0, 0);

	// �C���X�^���X����X�V
	g_InstenceCount = 0;

	// �r���[�E�v���W�F�N�V�����}�g���N�X����ɖ߂�
	SetCamera();
}



