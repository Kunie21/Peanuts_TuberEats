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
#define	TUBE_NUM		(2)	

//*****************************************************************************
// �\���̒�`
//*****************************************************************************
struct MESH_TUBE
{
	MATERIAL		material;					// �}�e���A��
	XMFLOAT3		pos;						// ���S���W
	XMFLOAT3		rot;						// ��]
	//XMFLOAT3		scl;						// �g�嗦
	int				nX, nZ;		// �u���b�N��
	int				nVertex;					// �����_��	
	int				nVertexIndex;			// ���C���f�b�N�X��
	int				nPolygon;				// ���|���S����
	float			sizeX, sizeZ;	// �u���b�N�T�C�Y

	ID3D11Buffer	*vertexBuffer;	// ���_�o�b�t�@
	ID3D11Buffer	*indexBuffer;	// �C���f�b�N�X�o�b�t�@
};

enum TUBE_BUFFER
{
	TUBE_BUFFER_FRONT = 0,
	TUBE_BUFFER_BACK,
	TUBE_BUFFER_MAX,
};

enum TUBE_TYPE
{
	TUBE_TYPE_STRAIGHT = 0,
	TUBE_TYPE_CURVE,
	TUBE_TYPE_MAX,
};

enum CURVE_TYPE
{
	CURVE_TYPE_RIGHT = 0,
	CURVE_TYPE_LEFT,
	CURVE_TYPE_UP,
	CURVE_TYPE_DOWN,
};

struct TUBE
{
	XMFLOAT3	pos;	// ���S���W
	XMFLOAT3	rot;	// ��]
	TUBE_TYPE	type;
};

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static MESH_TUBE g_MeshTube[TUBE_TYPE_MAX];
static TUBE g_Tube[TUBE_BUFFER_MAX];

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
static float	g_worldRot = 0.0f;

TUBE_TYPE testTubeArr[8];

//=============================================================================
// ����������
//=============================================================================
HRESULT InitTube(void)
{
	for (int i = 0; i < 8; i++)
	{
		//testTubeArr[i] = (TUBE_TYPE)(i % 2);
		testTubeArr[i] = TUBE_TYPE_STRAIGHT;
	}

	// �e�N�X�`������
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		D3DX11CreateShaderResourceViewFromFile(GetDevice(), g_TextureName[i], NULL, NULL, &g_Texture[i], NULL);
	}

	// �����p�C�v
	{
		// �|�W�V�����ݒ�
		g_MeshTube[TUBE_TYPE_STRAIGHT].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_MeshTube[TUBE_TYPE_STRAIGHT].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);

		// �}�e���A�����̏�����
		ZeroMemory(&g_MeshTube[TUBE_TYPE_STRAIGHT].material, sizeof(g_MeshTube[TUBE_TYPE_STRAIGHT].material));
		g_MeshTube[TUBE_TYPE_STRAIGHT].material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		// �u���b�N���̐ݒ�
		g_MeshTube[TUBE_TYPE_STRAIGHT].nX = MESH_NUM_X;
		g_MeshTube[TUBE_TYPE_STRAIGHT].nZ = MESH_NUM_Z;

		// ���_���̐ݒ�
		g_MeshTube[TUBE_TYPE_STRAIGHT].nVertex = (MESH_NUM_X + 1) * (MESH_NUM_Z + 1);

		// �C���f�b�N�X���̐ݒ�
		g_MeshTube[TUBE_TYPE_STRAIGHT].nVertexIndex = (MESH_NUM_X + 1) * 2 * MESH_NUM_Z + (MESH_NUM_Z - 1) * 2;

		// �|���S�����̐ݒ�
		g_MeshTube[TUBE_TYPE_STRAIGHT].nPolygon = MESH_NUM_X * MESH_NUM_Z * 2 + (MESH_NUM_Z - 1) * 4;

		// �u���b�N�T�C�Y�̐ݒ�
		g_MeshTube[TUBE_TYPE_STRAIGHT].sizeX = MESH_SIZE;
		g_MeshTube[TUBE_TYPE_STRAIGHT].sizeZ = MESH_SIZE;

		// ���_�o�b�t�@����
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DYNAMIC;
		bd.ByteWidth = sizeof(VERTEX_3D) * g_MeshTube[TUBE_TYPE_STRAIGHT].nVertex;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		GetDevice()->CreateBuffer(&bd, NULL, &g_MeshTube[TUBE_TYPE_STRAIGHT].vertexBuffer);

		// �C���f�b�N�X�o�b�t�@����
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DYNAMIC;
		bd.ByteWidth = sizeof(unsigned short) * g_MeshTube[TUBE_TYPE_STRAIGHT].nVertexIndex;
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		GetDevice()->CreateBuffer(&bd, NULL, &g_MeshTube[TUBE_TYPE_STRAIGHT].indexBuffer);

		{//���_�o�b�t�@�̒��g�𖄂߂�
#if 0
			const float texSizeX = 1.0f / g_nX;
			const float texSizeZ = 1.0f / g_nZ;
#else
			const float texSizeX = 1.0f;
			const float texSizeZ = 1.0f;
#endif

			// ���_�o�b�t�@�ւ̃|�C���^��擾
			D3D11_MAPPED_SUBRESOURCE msr;
			GetDeviceContext()->Map(g_MeshTube[TUBE_TYPE_STRAIGHT].vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

			VERTEX_3D* pVtx = (VERTEX_3D*)msr.pData;

			float drad = XM_2PI / g_MeshTube[TUBE_TYPE_STRAIGHT].nX;	// �P�ʊp�x
			float halfNumBlockX = (float)(g_MeshTube[TUBE_TYPE_STRAIGHT].nX / 2);	// X���u���b�N���̔����̒l

			for (int nz = 0; nz < (g_MeshTube[TUBE_TYPE_STRAIGHT].nZ + 1); nz++)
			{
				for (int nx = 0; nx < (g_MeshTube[TUBE_TYPE_STRAIGHT].nX + 1); nx++)
				{
					// ���_���W�̐ݒ�
					pVtx[nz * (g_MeshTube[TUBE_TYPE_STRAIGHT].nX + 1) + nx].Position.x = TUBE_RADIUS * cosf(nx * drad);
					pVtx[nz * (g_MeshTube[TUBE_TYPE_STRAIGHT].nX + 1) + nx].Position.y = TUBE_RADIUS * sinf(nx * drad);
					pVtx[nz * (g_MeshTube[TUBE_TYPE_STRAIGHT].nX + 1) + nx].Position.z = 0.5f * g_MeshTube[TUBE_TYPE_STRAIGHT].nZ * g_MeshTube[TUBE_TYPE_STRAIGHT].sizeZ - nz * g_MeshTube[TUBE_TYPE_STRAIGHT].sizeZ;

					// �@���̐ݒ�
					pVtx[nz * (g_MeshTube[TUBE_TYPE_STRAIGHT].nX + 1) + nx].Normal = XMFLOAT3(cosf(nx * drad + XM_PI), sinf(nx * drad + XM_PI), 0.0f);

					// �g�U���̐ݒ�
					pVtx[nz * (g_MeshTube[TUBE_TYPE_STRAIGHT].nX + 1) + nx].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

					// �e�N�X�`�����W�̐ݒ�
					pVtx[nz * (g_MeshTube[TUBE_TYPE_STRAIGHT].nX + 1) + nx].TexCoord.x = texSizeX * nx;
					pVtx[nz * (g_MeshTube[TUBE_TYPE_STRAIGHT].nX + 1) + nx].TexCoord.y = texSizeZ * nz;

					// �e�N�X�`�����W�̒���
					//�i�~���̃e�N�X�`�����W��r�؂ꂳ���Ȃ����߁AX���u���b�N���̔����ɓ��B��������W�̒l��܂�Ԃ��B�j
					if (nx > halfNumBlockX)
					{
						pVtx[nz * (g_MeshTube[TUBE_TYPE_STRAIGHT].nX + 1) + nx].TexCoord.x = halfNumBlockX + (halfNumBlockX - nx);
					}
				}
			}

			GetDeviceContext()->Unmap(g_MeshTube[TUBE_TYPE_STRAIGHT].vertexBuffer, 0);
		}

		{//�C���f�b�N�X�o�b�t�@�̒��g�𖄂߂�
			// �C���f�b�N�X�o�b�t�@�̃|�C���^��擾
			D3D11_MAPPED_SUBRESOURCE msr;
			GetDeviceContext()->Map(g_MeshTube[TUBE_TYPE_STRAIGHT].indexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

			unsigned short *pIdx = (unsigned short*)msr.pData;

			int nCntIdx = 0;
			for (int nz = 0; nz < g_MeshTube[TUBE_TYPE_STRAIGHT].nZ; nz++)
			{
				if (nz > 0)
				{// �k�ރ|���S���̂��߂̃_�u��̐ݒ�
					pIdx[nCntIdx] = (nz + 1) * (g_MeshTube[TUBE_TYPE_STRAIGHT].nX + 1);
					nCntIdx++;
				}

				for (int nx = 0; nx < (g_MeshTube[TUBE_TYPE_STRAIGHT].nX + 1); nx++)
				{
					pIdx[nCntIdx] = (nz + 1) * (g_MeshTube[TUBE_TYPE_STRAIGHT].nX + 1) + nx;
					nCntIdx++;
					pIdx[nCntIdx] = nz * (g_MeshTube[TUBE_TYPE_STRAIGHT].nX + 1) + nx;
					nCntIdx++;
				}

				if (nz < (g_MeshTube[TUBE_TYPE_STRAIGHT].nZ - 1))
				{// �k�ރ|���S���̂��߂̃_�u��̐ݒ�
					pIdx[nCntIdx] = nz * (g_MeshTube[TUBE_TYPE_STRAIGHT].nX + 1) + g_MeshTube[TUBE_TYPE_STRAIGHT].nX;
					nCntIdx++;
				}
			}

			GetDeviceContext()->Unmap(g_MeshTube[TUBE_TYPE_STRAIGHT].indexBuffer, 0);
		}
	}

	// �J�[�u�p�C�v
	{
		// �|�W�V�����ݒ�
		g_MeshTube[TUBE_TYPE_CURVE].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_MeshTube[TUBE_TYPE_CURVE].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);

		// �}�e���A�����̏�����
		ZeroMemory(&g_MeshTube[TUBE_TYPE_CURVE].material, sizeof(g_MeshTube[TUBE_TYPE_CURVE].material));
		g_MeshTube[TUBE_TYPE_CURVE].material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		// �u���b�N���̐ݒ�
		g_MeshTube[TUBE_TYPE_CURVE].nX = MESH_NUM_X;
		g_MeshTube[TUBE_TYPE_CURVE].nZ = MESH_NUM_Z;

		// ���_���̐ݒ�
		g_MeshTube[TUBE_TYPE_CURVE].nVertex = (MESH_NUM_X + 1) * (MESH_NUM_Z + 1);

		// �C���f�b�N�X���̐ݒ�
		g_MeshTube[TUBE_TYPE_CURVE].nVertexIndex = (MESH_NUM_X + 1) * 2 * MESH_NUM_Z + (MESH_NUM_Z - 1) * 2;

		// �|���S�����̐ݒ�
		g_MeshTube[TUBE_TYPE_CURVE].nPolygon = MESH_NUM_X * MESH_NUM_Z * 2 + (MESH_NUM_Z - 1) * 4;

		// �u���b�N�T�C�Y�̐ݒ�
		g_MeshTube[TUBE_TYPE_CURVE].sizeX = MESH_SIZE;
		g_MeshTube[TUBE_TYPE_CURVE].sizeZ = MESH_SIZE;

		// ���_�o�b�t�@����
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DYNAMIC;
		bd.ByteWidth = sizeof(VERTEX_3D) * g_MeshTube[TUBE_TYPE_CURVE].nVertex;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		GetDevice()->CreateBuffer(&bd, NULL, &g_MeshTube[TUBE_TYPE_CURVE].vertexBuffer);

		// �C���f�b�N�X�o�b�t�@����
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DYNAMIC;
		bd.ByteWidth = sizeof(unsigned short) * g_MeshTube[TUBE_TYPE_CURVE].nVertexIndex;
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		GetDevice()->CreateBuffer(&bd, NULL, &g_MeshTube[TUBE_TYPE_CURVE].indexBuffer);

		{//���_�o�b�t�@�̒��g�𖄂߂�

			const float texSizeX = 1.0f;
			const float texSizeZ = 1.0f;

			// ���_�o�b�t�@�ւ̃|�C���^��擾
			D3D11_MAPPED_SUBRESOURCE msr;
			GetDeviceContext()->Map(g_MeshTube[TUBE_TYPE_CURVE].vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

			VERTEX_3D* pVtx = (VERTEX_3D*)msr.pData;

			float drad = XM_2PI / g_MeshTube[TUBE_TYPE_CURVE].nX;	// �P�ʊp�x
			float halfNumBlockX = (float)(g_MeshTube[TUBE_TYPE_CURVE].nX / 2);	// X���u���b�N���̔����̒l

			for (int nz = 0; nz < (g_MeshTube[TUBE_TYPE_CURVE].nZ + 1); nz++)
			{
				for (int nx = 0; nx < (g_MeshTube[TUBE_TYPE_CURVE].nX + 1); nx++)
				{
					// �J�[�u�̔��a
					float rad = CURVE_RADIUS + TUBE_RADIUS * cosf(nx * drad);
					float agl = XM_PIDIV2 * (float)nz / (float)g_MeshTube[TUBE_TYPE_CURVE].nZ - XM_PIDIV2;	// -XM_PIDIV2�ŉE�J�[�u

					// ���_���W�̐ݒ�
					pVtx[nz * (g_MeshTube[TUBE_TYPE_CURVE].nX + 1) + nx].Position.x = rad * sinf(agl) + CURVE_CENTER;
					pVtx[nz * (g_MeshTube[TUBE_TYPE_CURVE].nX + 1) + nx].Position.y = TUBE_RADIUS * sinf(nx * drad);
					pVtx[nz * (g_MeshTube[TUBE_TYPE_CURVE].nX + 1) + nx].Position.z = rad * cosf(agl) - CURVE_CENTER;

					// �@���̐ݒ�
					pVtx[nz * (g_MeshTube[TUBE_TYPE_CURVE].nX + 1) + nx].Normal = XMFLOAT3(cosf(nx * drad + XM_PI), sinf(nx * drad + XM_PI), 0.0f);

					// �g�U���̐ݒ�
					pVtx[nz * (g_MeshTube[TUBE_TYPE_CURVE].nX + 1) + nx].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

					// �e�N�X�`�����W�̐ݒ�
					pVtx[nz * (g_MeshTube[TUBE_TYPE_CURVE].nX + 1) + nx].TexCoord.x = texSizeX * nx;
					pVtx[nz * (g_MeshTube[TUBE_TYPE_CURVE].nX + 1) + nx].TexCoord.y = texSizeZ * nz;

					// �e�N�X�`�����W�̒���
					//�i�~���̃e�N�X�`�����W��r�؂ꂳ���Ȃ����߁AX���u���b�N���̔����ɓ��B��������W�̒l��܂�Ԃ��B�j
					if (nx > halfNumBlockX)
					{
						pVtx[nz * (g_MeshTube[TUBE_TYPE_CURVE].nX + 1) + nx].TexCoord.x = halfNumBlockX + (halfNumBlockX - nx);
					}
				}
			}

			GetDeviceContext()->Unmap(g_MeshTube[TUBE_TYPE_CURVE].vertexBuffer, 0);
		}

		{//�C���f�b�N�X�o�b�t�@�̒��g�𖄂߂�
			// �C���f�b�N�X�o�b�t�@�̃|�C���^��擾
			D3D11_MAPPED_SUBRESOURCE msr;
			GetDeviceContext()->Map(g_MeshTube[TUBE_TYPE_CURVE].indexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

			unsigned short *pIdx = (unsigned short*)msr.pData;

			int nCntIdx = 0;
			for (int nz = 0; nz < g_MeshTube[TUBE_TYPE_CURVE].nZ; nz++)
			{
				if (nz > 0)
				{// �k�ރ|���S���̂��߂̃_�u��̐ݒ�
					pIdx[nCntIdx] = (nz + 1) * (g_MeshTube[TUBE_TYPE_CURVE].nX + 1);
					nCntIdx++;
				}

				for (int nx = 0; nx < (g_MeshTube[TUBE_TYPE_CURVE].nX + 1); nx++)
				{
					pIdx[nCntIdx] = (nz + 1) * (g_MeshTube[TUBE_TYPE_CURVE].nX + 1) + nx;
					nCntIdx++;
					pIdx[nCntIdx] = nz * (g_MeshTube[TUBE_TYPE_CURVE].nX + 1) + nx;
					nCntIdx++;
				}

				if (nz < (g_MeshTube[TUBE_TYPE_CURVE].nZ - 1))
				{// �k�ރ|���S���̂��߂̃_�u��̐ݒ�
					pIdx[nCntIdx] = nz * (g_MeshTube[TUBE_TYPE_CURVE].nX + 1) + g_MeshTube[TUBE_TYPE_CURVE].nX;
					nCntIdx++;
				}
			}

			GetDeviceContext()->Unmap(g_MeshTube[TUBE_TYPE_CURVE].indexBuffer, 0);
		}
	}

	g_time = 0.0f;

	//SetTubeCurveRight(0);
	//SetTubeCurveRight(1);
	g_Tube[TUBE_BUFFER_FRONT].type = testTubeArr[0];
	SetNextTube();
	
	//g_Tube[TUBE_BUFFER_BACK].rot.y = -XM_PIDIV2;
	//g_Tube[TUBE_BUFFER_BACK].pos.x = CURVE_RADIUS + CURVE_RADIUS * cosf(XM_PIDIV4);
	//g_Tube[TUBE_BUFFER_BACK].pos.z = CURVE_RADIUS * sinf(XM_PIDIV4);

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitTube(void)
{
	if (g_Load == FALSE) return;

	for (int i = 0; i < TUBE_TYPE_MAX; i++)
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

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawTube(void)
{
	for (int i = 0; i < TUBE_BUFFER_MAX; i++)
	{
		// ���_�o�b�t�@�ݒ�
		UINT stride = sizeof(VERTEX_3D);
		UINT offset = 0;
		GetDeviceContext()->IASetVertexBuffers(0, 1, &g_MeshTube[g_Tube[i].type].vertexBuffer, &stride, &offset);

		// �C���f�b�N�X�o�b�t�@�ݒ�
		GetDeviceContext()->IASetIndexBuffer(g_MeshTube[g_Tube[i].type].indexBuffer, DXGI_FORMAT_R16_UINT, 0);

		// �v���~�e�B�u�g�|���W�ݒ�
		GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

		// �}�e���A���ݒ�
		SetMaterialBuffer(&g_MeshTube[g_Tube[i].type].material);

		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[TEXTURE_NOMAL]);


		XMMATRIX mtxRot, mtxTranslate, mtxWorld;

		// ���[���h�}�g���b�N�X�̏�����
		mtxWorld = XMMatrixIdentity();

		// ��]�𔽉f
		mtxRot = XMMatrixRotationRollPitchYaw(g_Tube[i].rot.x, g_Tube[i].rot.y, g_Tube[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// �ړ��𔽉f
		mtxTranslate = XMMatrixTranslation(g_Tube[i].pos.x, g_Tube[i].pos.y, g_Tube[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ���E�̉�]�𔽉f
		//mtxRot = XMMatrixRotationRollPitchYaw(0.0f, 0.0f, g_worldRot);
		//SetAfterRotation(&mtxRot);
		//mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// ���[���h�}�g���b�N�X�̐ݒ�
		SetWorldBuffer(&mtxWorld);

		// �|���S���̕`��
		GetDeviceContext()->DrawIndexed(g_MeshTube[g_Tube[i].type].nVertexIndex, 0, 0);
	}
}

void RotateTube(float rot)
{
	//for (int i = 0; i < TUBE_NUM; i++)
	//{
	//	g_MeshTube[i].rot.z += rot;
	//}

	g_worldRot += rot;
}

void MoveTube(float move)
{
	for (int i = 0; i < TUBE_BUFFER_MAX; i++)
	{
		//g_MeshTube[i].pos.z -= move;
		//if (g_MeshTube[i].pos.z <= -TUBE_SIZE * 0.5f)
		//{
		//	g_MeshTube[i].pos.z += 2.0f * TUBE_SIZE;
		//}
		switch (g_Tube[TUBE_BUFFER_FRONT].type)
		{
		case TUBE_TYPE_STRAIGHT:
			g_Tube[i].pos.z -= move;
			break;
		case TUBE_TYPE_CURVE:
			g_Tube[i].rot.y -= XM_PIDIV2 * move / TUBE_SIZE;
			float agl = -g_Tube[i].rot.y + XM_PIDIV2 + XM_PIDIV4;
			g_Tube[i].pos.x = CURVE_RADIUS + CURVE_RADIUS * cosf(agl);
			g_Tube[i].pos.z = CURVE_RADIUS * sinf(agl);
			break;
		}
	}

	switch (g_Tube[TUBE_BUFFER_FRONT].type)
	{
	case TUBE_TYPE_STRAIGHT:
		if (g_Tube[TUBE_BUFFER_FRONT].pos.z <= -TUBE_SIZE * 0.5f)
		{
			PresentTube();
		}
		break;
	case TUBE_TYPE_CURVE:
		if (g_Tube[TUBE_BUFFER_FRONT].rot.y <= -XM_PIDIV2)
		{
			PresentTube();
		}
		break;
	}
}
void TestCurveTube(float move)
{
	float agl;
	for (int i = 0; i < 2; i++)
	{
		g_MeshTube[i].rot.y -= XM_PIDIV2 * move / TUBE_SIZE;
		agl = -g_MeshTube[i].rot.y + XM_PIDIV2 + XM_PIDIV4;
		g_MeshTube[i].pos.x = CURVE_RADIUS + CURVE_RADIUS * cosf(agl);
		g_MeshTube[i].pos.z = CURVE_RADIUS * sinf(agl);
		if (g_MeshTube[i].rot.y <= -XM_PIDIV2)
		{
			g_MeshTube[i].rot.y += XM_PI;
			agl -= XM_PI;
			g_MeshTube[i].pos.x = CURVE_RADIUS + CURVE_RADIUS * cosf(agl);
			g_MeshTube[i].pos.z = CURVE_RADIUS * sinf(agl);
		}
	}
}

float GetTubeRotation(void)
{
	return g_MeshTube[0].rot.z;
	//return g_worldRot;
}

void PresentTube(void)
{
	g_Tube[TUBE_BUFFER_FRONT] = g_Tube[TUBE_BUFFER_BACK];
	SetNextTube();
}
void SetNextTube(void)
{
	static int now = 0;
	switch (testTubeArr[now])
	{
	case TUBE_TYPE_STRAIGHT:
		switch (testTubeArr[now + 1])
		{
		case TUBE_TYPE_STRAIGHT:
			g_Tube[TUBE_BUFFER_BACK].type = TUBE_TYPE_STRAIGHT;
			g_Tube[TUBE_BUFFER_BACK].rot.y = 0.0f;
			g_Tube[TUBE_BUFFER_BACK].pos.x = 0.0f;
			g_Tube[TUBE_BUFFER_BACK].pos.y = 0.0f;
			g_Tube[TUBE_BUFFER_BACK].pos.z = TUBE_SIZE;
			break;
		case TUBE_TYPE_CURVE:
			g_Tube[TUBE_BUFFER_BACK].type = TUBE_TYPE_CURVE;
			g_Tube[TUBE_BUFFER_BACK].rot.y = 0.0f;
			g_Tube[TUBE_BUFFER_BACK].pos.x = CURVE_RADIUS + CURVE_RADIUS * cosf(XM_PIDIV4 + XM_PIDIV2);
			g_Tube[TUBE_BUFFER_BACK].pos.y = 0.0f;
			g_Tube[TUBE_BUFFER_BACK].pos.z = TUBE_SIZE * 0.5f + CURVE_RADIUS * sinf(XM_PIDIV4 + XM_PIDIV2);
			break;
		}
		break;
	case TUBE_TYPE_CURVE:
		switch (testTubeArr[now + 1])
		{
		case TUBE_TYPE_STRAIGHT:
			g_Tube[TUBE_BUFFER_BACK].type = TUBE_TYPE_STRAIGHT;
			g_Tube[TUBE_BUFFER_BACK].rot.y = XM_PIDIV2;
			g_Tube[TUBE_BUFFER_BACK].pos.x = TUBE_SIZE;
			g_Tube[TUBE_BUFFER_BACK].pos.y = 0.0f;
			g_Tube[TUBE_BUFFER_BACK].pos.z = TUBE_SIZE * 0.5f;
			break;
		case TUBE_TYPE_CURVE:
			g_Tube[TUBE_BUFFER_BACK].type = TUBE_TYPE_CURVE;
			g_Tube[TUBE_BUFFER_BACK].rot.y = -XM_PIDIV2;
			g_Tube[TUBE_BUFFER_BACK].pos.x = CURVE_RADIUS + CURVE_RADIUS * cosf(XM_PIDIV4);
			g_Tube[TUBE_BUFFER_BACK].pos.y = 0.0f;
			g_Tube[TUBE_BUFFER_BACK].pos.z = CURVE_RADIUS * sinf(XM_PIDIV4);
			break;
		}
		break;
	}
	now++;
}

void SetTubeCurveRight(int tube)
{
	int i = tube;

	//���_�o�b�t�@�����������
	const float texSizeX = 1.0f;
	const float texSizeZ = 1.0f;

	// ���_�o�b�t�@�ւ̃|�C���^��擾
	D3D11_MAPPED_SUBRESOURCE msr;
	GetDeviceContext()->Map(g_MeshTube[i].vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

	VERTEX_3D* pVtx = (VERTEX_3D*)msr.pData;

	float drad = XM_2PI / g_MeshTube[i].nX;	// �P�ʊp�x
	float halfNumBlockX = (float)(g_MeshTube[i].nX / 2);	// X���u���b�N���̔����̒l

	for (int nz = 0; nz < (g_MeshTube[i].nZ + 1); nz++)
	{
		for (int nx = 0; nx < (g_MeshTube[i].nX + 1); nx++)
		{
			// �J�[�u�̔��a
			float rad = CURVE_RADIUS + TUBE_RADIUS * cosf(nx * drad);
			float agl = XM_PIDIV2 * (float)nz / (float)g_MeshTube[i].nZ - XM_PIDIV2;	// -XM_PIDIV2�ŉE�J�[�u

			// ���_���W�̐ݒ�
			pVtx[nz * (g_MeshTube[i].nX + 1) + nx].Position.x = rad * sinf(agl) + CURVE_CENTER;
			pVtx[nz * (g_MeshTube[i].nX + 1) + nx].Position.y = TUBE_RADIUS * sinf(nx * drad);
			pVtx[nz * (g_MeshTube[i].nX + 1) + nx].Position.z = rad * cosf(agl) - CURVE_CENTER;

			// �@���̐ݒ�
			pVtx[nz * (g_MeshTube[i].nX + 1) + nx].Normal = XMFLOAT3(cosf(nx * drad + XM_PI), sinf(nx * drad + XM_PI), 0.0f);

			// �g�U���̐ݒ�
			pVtx[nz * (g_MeshTube[i].nX + 1) + nx].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

			// �e�N�X�`�����W�̐ݒ�
			pVtx[nz * (g_MeshTube[i].nX + 1) + nx].TexCoord.x = texSizeX * nx;
			pVtx[nz * (g_MeshTube[i].nX + 1) + nx].TexCoord.y = texSizeZ * nz;

			// �e�N�X�`�����W�̒���
			//�i�~���̃e�N�X�`�����W��r�؂ꂳ���Ȃ����߁AX���u���b�N���̔����ɓ��B��������W�̒l��܂�Ԃ��B�j
			if (nx > halfNumBlockX)
			{
				pVtx[nz * (g_MeshTube[i].nX + 1) + nx].TexCoord.x = halfNumBlockX + (halfNumBlockX - nx);
			}
		}
	}

	GetDeviceContext()->Unmap(g_MeshTube[i].vertexBuffer, 0);
}