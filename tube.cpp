//=============================================================================
//
// �p�C�v���C���ǂ̏��� [tube.cpp]
// Author : ���] �đ�
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "tube.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	TUBE_RADIUS		(200.0f)	// ���a
#define	MESH_NUM_X		(8)			// �f�ʂ̊p��
#define	MESH_SIZE		(TUBE_RADIUS * 2.0f * XM_PI / MESH_NUM_X)	// �}�X��1�ӂ̒���
#define	MESH_NUM_Z		(100)		// ���s��
#define	TUBE_NUM		(2)	

//*****************************************************************************
// �\���̒�`
//*****************************************************************************
typedef struct
{
	MATERIAL		material;					// �}�e���A��
	XMFLOAT3		pos;						// ���S���W
	XMFLOAT3		rot;						// ��]
	//XMFLOAT3		scl;						// �g�嗦
	int				nNumBlockX, nNumBlockZ;		// �u���b�N��
	int				nNumVertex;					// �����_��	
	int				nNumVertexIndex;			// ���C���f�b�N�X��
	int				nNumPolygon;				// ���|���S����
	float			fBlockSizeX, fBlockSizeZ;	// �u���b�N�T�C�Y

	ID3D11Buffer	*vertexBuffer;	// ���_�o�b�t�@
	ID3D11Buffer	*indexBuffer;	// �C���f�b�N�X�o�b�t�@

} MESH_TUBE;

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static MESH_TUBE g_MeshTube[TUBE_NUM];

// �e�N�X�`���Ǘ�
enum
{
	TEXTURE_NOMAL = 0,
	TEXTURE_MAX,
};
// static TEXTURE2D_DESC	g_td[TEXTURE_MAX];
static ID3D11ShaderResourceView*	g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����
static char*	g_TextureName[TEXTURE_MAX] = {
	"data/TEXTURE/r.tif",
};

static BOOL		g_Load = FALSE;
static float	g_time;
static int		g_frontTube = 0;
static int		g_backTube = 1;

//=============================================================================
// ����������
//=============================================================================
HRESULT InitTube(void)
{
	// �e�N�X�`������
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		D3DX11CreateShaderResourceViewFromFile(GetDevice(), g_TextureName[i], NULL, NULL, &g_Texture[i], NULL);
	}

	for (int i = 0; i < TUBE_NUM; i++)
	{
		// �|�W�V�����ݒ�
		g_MeshTube[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_MeshTube[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);

		// �}�e���A�����̏�����
		ZeroMemory(&g_MeshTube[i].material, sizeof(g_MeshTube[i].material));
		g_MeshTube[i].material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		// �u���b�N���̐ݒ�
		g_MeshTube[i].nNumBlockX = MESH_NUM_X;
		g_MeshTube[i].nNumBlockZ = MESH_NUM_Z;

		// ���_���̐ݒ�
		g_MeshTube[i].nNumVertex = (MESH_NUM_X + 1) * (MESH_NUM_Z + 1);

		// �C���f�b�N�X���̐ݒ�
		g_MeshTube[i].nNumVertexIndex = (MESH_NUM_X + 1) * 2 * MESH_NUM_Z + (MESH_NUM_Z - 1) * 2;

		// �|���S�����̐ݒ�
		g_MeshTube[i].nNumPolygon = MESH_NUM_X * MESH_NUM_Z * 2 + (MESH_NUM_Z - 1) * 4;

		// �u���b�N�T�C�Y�̐ݒ�
		g_MeshTube[i].fBlockSizeX = MESH_SIZE;
		g_MeshTube[i].fBlockSizeZ = MESH_SIZE;

		// ���_�o�b�t�@����
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DYNAMIC;
		bd.ByteWidth = sizeof(VERTEX_3D) * g_MeshTube[i].nNumVertex;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		GetDevice()->CreateBuffer(&bd, NULL, &g_MeshTube[i].vertexBuffer);

		// �C���f�b�N�X�o�b�t�@����
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DYNAMIC;
		bd.ByteWidth = sizeof(unsigned short) * g_MeshTube[i].nNumVertexIndex;
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		GetDevice()->CreateBuffer(&bd, NULL, &g_MeshTube[i].indexBuffer);

		{//���_�o�b�t�@�̒��g�𖄂߂�
#if 0
			const float texSizeX = 1.0f / g_nNumBlockX;
			const float texSizeZ = 1.0f / g_nNumBlockZ;
#else
			const float texSizeX = 1.0f;
			const float texSizeZ = 1.0f;
#endif

			// ���_�o�b�t�@�ւ̃|�C���^���擾
			D3D11_MAPPED_SUBRESOURCE msr;
			GetDeviceContext()->Map(g_MeshTube[i].vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

			VERTEX_3D* pVtx = (VERTEX_3D*)msr.pData;

			float drad = XM_2PI / g_MeshTube[i].nNumBlockX;	// �P�ʊp�x
			float halfNumBlockX = (float)(g_MeshTube[i].nNumBlockX / 2);	// X���u���b�N���̔����̒l

			for (int nCntVtxY = 0; nCntVtxY < (g_MeshTube[i].nNumBlockZ + 1); nCntVtxY++)
			{
				for (int nCntVtxX = 0; nCntVtxX < (g_MeshTube[i].nNumBlockX + 1); nCntVtxX++)
				{
					// ���_���W�̐ݒ�
					pVtx[nCntVtxY * (g_MeshTube[i].nNumBlockX + 1) + nCntVtxX].Position.x = TUBE_RADIUS * cosf(nCntVtxX * drad);
					pVtx[nCntVtxY * (g_MeshTube[i].nNumBlockX + 1) + nCntVtxX].Position.y = TUBE_RADIUS * sinf(nCntVtxX * drad);
					pVtx[nCntVtxY * (g_MeshTube[i].nNumBlockX + 1) + nCntVtxX].Position.z = g_MeshTube[i].nNumBlockZ * g_MeshTube[i].fBlockSizeZ - nCntVtxY * g_MeshTube[i].fBlockSizeZ;

					// �@���̐ݒ�
					pVtx[nCntVtxY * (g_MeshTube[i].nNumBlockX + 1) + nCntVtxX].Normal = XMFLOAT3(cosf(nCntVtxX * drad + XM_PI), sinf(nCntVtxX * drad + XM_PI), 0.0f);

					// �g�U���̐ݒ�
					pVtx[nCntVtxY * (g_MeshTube[i].nNumBlockX + 1) + nCntVtxX].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

					// �e�N�X�`�����W�̐ݒ�
					pVtx[nCntVtxY * (g_MeshTube[i].nNumBlockX + 1) + nCntVtxX].TexCoord.x = texSizeX * nCntVtxX;
					pVtx[nCntVtxY * (g_MeshTube[i].nNumBlockX + 1) + nCntVtxX].TexCoord.y = texSizeZ * nCntVtxY;

					//// ���_���W�����ڂ߂�
					//int closeVertex = 2;
					//if (nCntVtxY < closeVertex)
					//{
					//	// �@���̐ݒ�
					//	pVtx[nCntVtxY * (g_MeshTube[i].nNumBlockX + 1) + nCntVtxX].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);

					//	// ���_�̐ݒ�
					//	pVtx[nCntVtxY * (g_MeshTube[i].nNumBlockX + 1) + nCntVtxX].Position.x = nCntVtxY * (1 / closeVertex) * TUBE_RADIUS * cosf(nCntVtxX * drad);
					//	pVtx[nCntVtxY * (g_MeshTube[i].nNumBlockX + 1) + nCntVtxX].Position.y = nCntVtxY * (1 / closeVertex) * TUBE_RADIUS * sinf(nCntVtxX * drad);
					//	pVtx[nCntVtxY * (g_MeshTube[i].nNumBlockX + 1) + nCntVtxX].Position.z = g_MeshTube[i].nNumBlockZ * g_MeshTube[i].fBlockSizeZ - (closeVertex * g_MeshTube[i].fBlockSizeZ);
					//}

					// �e�N�X�`�����W�̒���
					//�i�~���̃e�N�X�`�����W��r�؂ꂳ���Ȃ����߁AX���u���b�N���̔����ɓ��B��������W�̒l��܂�Ԃ��B�j
					if (nCntVtxX > halfNumBlockX)
					{
						pVtx[nCntVtxY * (g_MeshTube[i].nNumBlockX + 1) + nCntVtxX].TexCoord.x = halfNumBlockX + (halfNumBlockX - nCntVtxX);
					}
				}
			}

			GetDeviceContext()->Unmap(g_MeshTube[i].vertexBuffer, 0);
		}

		{//�C���f�b�N�X�o�b�t�@�̒��g�𖄂߂�
			// �C���f�b�N�X�o�b�t�@�̃|�C���^���擾
			D3D11_MAPPED_SUBRESOURCE msr;
			GetDeviceContext()->Map(g_MeshTube[i].indexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

			unsigned short *pIdx = (unsigned short*)msr.pData;

			int nCntIdx = 0;
			for (int nCntVtxY = 0; nCntVtxY < g_MeshTube[i].nNumBlockZ; nCntVtxY++)
			{
				if (nCntVtxY > 0)
				{// �k�ރ|���S���̂��߂̃_�u��̐ݒ�
					pIdx[nCntIdx] = (nCntVtxY + 1) * (g_MeshTube[i].nNumBlockX + 1);
					nCntIdx++;
				}

				for (int nCntVtxX = 0; nCntVtxX < (g_MeshTube[i].nNumBlockX + 1); nCntVtxX++)
				{
					pIdx[nCntIdx] = (nCntVtxY + 1) * (g_MeshTube[i].nNumBlockX + 1) + nCntVtxX;
					nCntIdx++;
					pIdx[nCntIdx] = nCntVtxY * (g_MeshTube[i].nNumBlockX + 1) + nCntVtxX;
					nCntIdx++;
				}

				if (nCntVtxY < (g_MeshTube[i].nNumBlockZ - 1))
				{// �k�ރ|���S���̂��߂̃_�u��̐ݒ�
					pIdx[nCntIdx] = nCntVtxY * (g_MeshTube[i].nNumBlockX + 1) + g_MeshTube[i].nNumBlockX;
					nCntIdx++;
				}
			}

			GetDeviceContext()->Unmap(g_MeshTube[i].indexBuffer, 0);
		}
	}

	g_MeshTube[g_backTube].pos.z = g_MeshTube[0].fBlockSizeZ * g_MeshTube[0].nNumBlockZ;

	g_time = 0.0f;

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitTube(void)
{
	if (g_Load == FALSE) return;

	for (int i = 0; i < TUBE_NUM; i++)
	{
		// ���_�o�b�t�@�̉��
		if (g_MeshTube[i].vertexBuffer)
		{
			g_MeshTube[i].vertexBuffer->Release();
			g_MeshTube[i].vertexBuffer = NULL;
		}

		// �C���f�b�N�X�o�b�t�@�̉��
		if (g_MeshTube[i].indexBuffer)
		{
			g_MeshTube[i].indexBuffer->Release();
			g_MeshTube[i].indexBuffer = NULL;
		}
	}

	// �e�N�X�`���̉��
	for (int i = 0; i < TEXTURE_MAX; i++)
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
// �X�V����
//=============================================================================
void UpdateTube(void)
{
	for (int i = 0; i < TUBE_NUM; i++)
	{
		g_MeshTube[i].pos.z -= 40.0f;
		if (g_MeshTube[i].pos.z < -g_MeshTube[i].fBlockSizeZ * g_MeshTube[i].nNumBlockZ)
		{
			g_MeshTube[i].pos.z = g_MeshTube[i].fBlockSizeZ * g_MeshTube[i].nNumBlockZ;
		}
	}
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawTube(void)
{
	SetLightEnable(FALSE);
	for (int i = 0; i < TUBE_NUM; i++)
	{
		// ���_�o�b�t�@�ݒ�
		UINT stride = sizeof(VERTEX_3D);
		UINT offset = 0;
		GetDeviceContext()->IASetVertexBuffers(0, 1, &g_MeshTube[i].vertexBuffer, &stride, &offset);

		// �C���f�b�N�X�o�b�t�@�ݒ�
		GetDeviceContext()->IASetIndexBuffer(g_MeshTube[i].indexBuffer, DXGI_FORMAT_R16_UINT, 0);

		// �v���~�e�B�u�g�|���W�ݒ�
		GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

		// �}�e���A���ݒ�
		SetMaterial(g_MeshTube[i].material);

		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[TEXTURE_NOMAL]);


		XMMATRIX mtxRot, mtxTranslate, mtxWorld;

		// ���[���h�}�g���b�N�X�̏�����
		mtxWorld = XMMatrixIdentity();

		// ��]�𔽉f
		mtxRot = XMMatrixRotationRollPitchYaw(g_MeshTube[i].rot.x, g_MeshTube[i].rot.y, g_MeshTube[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// �ړ��𔽉f
		mtxTranslate = XMMatrixTranslation(g_MeshTube[i].pos.x, g_MeshTube[i].pos.y, g_MeshTube[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ���[���h�}�g���b�N�X�̐ݒ�
		SetWorldMatrix(&mtxWorld);

		// �|���S���̕`��
		GetDeviceContext()->DrawIndexed(g_MeshTube[i].nNumVertexIndex, 0, 0);
	}
	SetLightEnable(TRUE);
}

void TubeRotation(float rot)
{
	for (int i = 0; i < TUBE_NUM; i++)
	{
		g_MeshTube[i].rot.z += rot;
	}
}