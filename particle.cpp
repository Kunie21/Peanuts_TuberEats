//=============================================================================
//
// �p�[�e�B�N������ [particle.cpp]
// Author : ���]�đ�
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "input.h"
#include "camera.h"
#include "model.h"
#include "texture2d.h"
#include "particle.h"
#include "player.h"
#include "game.h"
#include "debugproc.h"
#include "stage.h"
#include "tube.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	EMITTER_MAX		(100)	// �G�~�b�^�[�ő吔
#define	PARTICLE_MAX	(25)	// �p�[�e�B�N���ő吔
#define	PARTICLE_SIZE	(50)	// �p�[�e�B�N�����T�C�Y
#define	PARTICLE_PUSH	(10)	// �t���[�����Ƃ̕��o��
#define	PARTICLE_SPD	(5.0f)	// ���o���x
//#define	PARTICLE_ACL	(0.98f)	// ���o�����x
#define	PARTICLE_ACL	(0.1f)	// ���o�����x
#define	PARTICLE_POS	(TUBE_RADIUS - 40.0f)

#define	EXP_POS	(TUBE_RADIUS - 80.0f)
#define	EXP_MAX			(10)	// �G�~�b�^�[�ő吔
#define	EXP_PUSH		(25)	// 

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT MakeVertexParticle(void);

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer		*g_VertexBuffer = NULL;		// ���_�o�b�t�@

static TEXTURE_LABEL g_Texture = TEXTURE_LABEL_PARTICLE;

static BOOL		g_Load = FALSE;

//*****************************************************************************
// �\���̒�`
//*****************************************************************************
// �p�[�e�B�N���ڍאݒ�
struct PARTICLE_DESC
{
	XMFLOAT4	m_col = { 1.0f, 1.0f, 1.0f, 1.0f };	// �F
	XMFLOAT3	m_pos = { 0.0f, 0.0f, 0.0f };	// �ʒu
	XMFLOAT3	m_vec = { 0.0f, 0.0f, 0.0f };	// ����
	float		m_spdmax = 0.0f;	// �ړ��ʍő�l
	float		m_spd = 0.0f;	// �ړ���
	float		m_acl = 0.0f;	// ���ʉ����x
	float		m_scl = 1.0f;	// ���ʃX�P�[��
	int			m_next = -1;	// ���ɉ����p�[�e�B�N���̃C���f�b�N�X
};

struct PARTICLE_SORT
{
	float	length = 0.0f;
	int		next = -1;
};

//*****************************************************************************
// �N���X��`
//*****************************************************************************
// �p�[�e�B�N���N���X
class PARTICLE
{
private:
	PARTICLE_DESC	m_desc;
	BOOL			m_bUse = FALSE;	// �g�p���Ă��邩�ǂ���

public:
	void Update(void) {
		m_desc.m_pos.x += m_desc.m_vec.x * m_desc.m_spd;
		m_desc.m_pos.y += m_desc.m_vec.y * m_desc.m_spd;
		m_desc.m_pos.z += m_desc.m_vec.z * m_desc.m_spd;
		float rate = m_desc.m_spd / m_desc.m_spdmax;
		m_desc.m_scl = PARTICLE_SIZE * (1.0f - rate);
		//m_desc.m_col = { m_desc.m_col.x * rate, m_desc.m_col.y * rate, m_desc.m_col.z * rate, 1.0f };
		//m_desc.m_col.w = m_desc.m_spd;
		//m_desc.m_spd *= m_desc.m_acl;
		m_desc.m_spd -= m_desc.m_acl;
		if(m_desc.m_spd < 0.5f) m_bUse = FALSE;
	}

	// �p�[�e�B�N������
	void Emit(PARTICLE_DESC& desc) {
		m_desc = desc;
		m_bUse = TRUE;
	}

	PARTICLE_DESC* GetDesc(void) { return &m_desc; }

	BOOL GetUse(void) const { return m_bUse; }
};

// �G�~�b�^�[�N���X
class EMITTER
{
private:
	PARTICLE		m_aPtc[PARTICLE_MAX];		// �p�[�e�B�N���z��
	PARTICLE_DESC	m_dscPtc;					// �p�[�e�B�N���ݒ�
	PARTICLE_SORT	m_aSort[PARTICLE_MAX];		// �p�[�e�B�N�����ёւ�
	int				m_indexTop = 0;					// �p�[�e�B�N�����ёւ�
	int				m_zPosNo = 0;
	float			m_rot = 0.0f;				// ���o�p�x
	float			m_vol = 0.0f;				// ���o�p�x��
	int				m_pushNum = PARTICLE_PUSH;	// �t���[�����̍쐬��
	BOOL			m_bUse = FALSE;				// �g�p��

public:
	EMITTER() {
		m_dscPtc.m_vec = { cosf(m_rot), sinf(m_rot), 0.0f };
		m_dscPtc.m_scl = PARTICLE_SIZE;
		m_dscPtc.m_spd = PARTICLE_SPD;
		m_dscPtc.m_spdmax = m_dscPtc.m_spd;
		m_dscPtc.m_acl = PARTICLE_ACL;
		m_dscPtc.m_col = { 3.0f, 2.0f, 2.0f, 1.0f };
		m_vol = XM_PIDIV2 * 0.5f;	// 30�x
	}

	// �G�~�b�^�[�ݒu
	virtual BOOL Set(int zPosNo, int rotPosNo) {
		if (GetUse()) return FALSE;

		SetZPosNo(zPosNo);
		SetRot(rotPosNo);
		SetPos(PARTICLE_POS);

		SetUse(TRUE);

		return TRUE;
	}

	virtual void Update(void) {
		if (!GetUse()) return;

		// �p�[�e�B�N���X�V�Ɣ�������
		int count = 0;
		for (int p = 0; p < PARTICLE_MAX; p++) {
			if (!UpdateParticle(p)) {
				if (count < m_pushNum) {
					SetRandDesc();
					m_aPtc[p].Emit(m_dscPtc);
					count++;
				}
			}
		}

		// �p�[�e�B�N�����ёւ�
		SortParticle();
	}

	// �����_���ɃX�e�[�^�X�����߂�
	virtual void SetRandDesc(void) {
		PARTICLE_DESC* dsc = GetDesc();
		dsc->m_spd = dsc->m_spdmax * (float)(rand() % 30) * 0.033333f;
		float rand_xy = m_vol * ((float)(rand() % 30) * 0.033333f - 0.5f);
		float rand_z = m_vol * ((float)(rand() % 30) * 0.033333f - 0.5f);
		dsc->m_vec = { cosf(m_rot + rand_xy), sinf(m_rot + rand_xy), sinf(rand_z) };
	}

	// �p�[�e�B�N�����ёւ�
	void SortParticle(void) {
		for (int p = 0; p < PARTICLE_MAX; p++) m_aSort[p].next = -1;
		int prv, cur; m_indexTop = 0;
		for (int p = 1; p < PARTICLE_MAX; p++) {
			prv = -1;
			cur = m_indexTop;
			while (m_aPtc[p].GetDesc()->m_pos.z < m_aPtc[cur].GetDesc()->m_pos.z)
			{
				prv = cur;
				cur = m_aSort[cur].next;
				if (cur == -1) {
					m_aSort[prv].next = p;
					break;
				}
			}
			m_aSort[p].next = cur;
			if (prv < 0) {
				m_indexTop = p;
			}
			else {
				m_aSort[prv].next = p;
			}
		}
	}

	PARTICLE* GetParticle(int index) { return &m_aPtc[index]; }

	int GetZPosNo(void) { return m_zPosNo; }
	float GetRot(void) { return m_rot; }
	int GetIndexTop(void) { return m_indexTop; }
	int GetIndexNext(int index) { return m_aSort[index].next; }
	BOOL GetUse(void) { return m_bUse; }
	BOOL GetPtcUse(int index) { return m_aPtc[index].GetUse(); }
	PARTICLE_DESC* GetDesc(void) { return &m_dscPtc; }

	BOOL UpdateParticle(int index){
		if (m_aPtc[index].GetUse()) {
			m_aPtc[index].Update();
			return TRUE;
		}
		return FALSE;
	}
	void EmitParticle(int index) { return m_aPtc[index].Emit(m_dscPtc); }
	void SetZPosNo(int zPosNo) { m_zPosNo = zPosNo; }
	void SetRot(int rotPosNo) { m_rot = GetRotPos(rotPosNo) + XM_PI; }
	void SetUse(BOOL use) { m_bUse = use; }
	void SetPos(float height) {
		m_dscPtc.m_pos = { height * cosf(m_rot - XM_PI), height * sinf(m_rot - XM_PI), 0.0f };
	}
};

class EMITTER_EXP :public EMITTER
{
public:
	EMITTER_EXP() {
		GetDesc()->m_col = { 1.0f, 2.0f, 3.0f, 1.0f };
	}

	// �G�~�b�^�[�ݒu
	BOOL Set(int zPosNo, int rotPosNo) override {
		if (GetUse()) return FALSE;

		SetZPosNo(zPosNo);
		SetRot(rotPosNo);
		SetPos(EXP_POS);

		// �p�[�e�B�N����������
		int count = 0;
		for (int p = 0; p < PARTICLE_MAX; p++) {
			if (!GetPtcUse(p)) {
				if (count < EXP_PUSH) {
					SetRandDesc();
					EmitParticle(p);
					count++;
				}
			}
		}

		SetUse(TRUE);

		return TRUE;
	}

	void Update(void) override {
		if (!GetUse()) return;

		int count = 0;
		for (int p = 0; p < PARTICLE_MAX; p++) {
			if (UpdateParticle(p)) {
				count++;
			}
		}
		if (!count) { SetUse(FALSE); return; }	// �p�[�e�B�N�����Ȃ��Ȃ���

		// �p�[�e�B�N�����ёւ�
		SortParticle();
	}

	// �����_���ɃX�e�[�^�X�����߂�
	void SetRandDesc(void) override {
		PARTICLE_DESC* dsc = GetDesc();
		dsc->m_spd = dsc->m_spdmax * (float)(rand() % 30) * 0.033333f;
		float rand_xy = XM_2PI * (float)(rand() % 30) * 0.033333f;
		float rand_z = XM_2PI * (float)(rand() % 30) * 0.033333f;
		dsc->m_vec = { cosf(GetRot() + rand_xy), sinf(GetRot() + rand_xy), sinf(rand_z) };
	}
};

// �G�~�b�^�[�Ǘ��N���X
class EMITTER_MANAGER
{
private:
	EMITTER* m_aEmt;
	int		m_nEmt;

public:
	EMITTER_MANAGER(EMITTER* aEmt, int nEmt) :m_aEmt(aEmt), m_nEmt(nEmt) {}

	void Update(void) {
		for (int e = 0; e < m_nEmt; e++) {
			m_aEmt[e].Update();
		}
	}

	void DrawParticle(void) {

		// �f�o�C�X�擾
		ID3D11DeviceContext* device = GetDeviceContext();

		// ���_�o�b�t�@�ݒ�
		UINT stride = sizeof(VERTEX_3D);
		UINT offset = 0;
		device->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

		// �v���~�e�B�u�g�|���W�ݒ�
		device->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

		// �e�N�X�`���ݒ�
		device->PSSetShaderResources(0, 1, GetTexture(g_Texture));
		
		// ���[���h�}�g���b�N�X�̐ݒ�
		SetWorldBuffer(&XMMatrixIdentity());
		SetMaterialBuffer(GetDefaultMaterial());

		// �C���X�^���V���O�`��
		SetShaderInstanceingBillboard();

		// �C���X�^���X�o�b�t�@�ւ̃|�C���^���擾
		D3D11_MAPPED_SUBRESOURCE msr;
		device->Map(GetInstanceBuffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
		INSTANCE* pInstance = (INSTANCE*)msr.pData;

		int instanceCount = 0;
		int instNo = 0;
		PARTICLE_DESC* desc = NULL;
		float zPos = 0.0f;
		float rate = 0.0f;
		for (int e = 0; e < m_nEmt; e++)
		{
			if (!m_aEmt[e].GetUse()) continue;
			zPos = GetZPos(m_aEmt[e].GetZPosNo());
			if (zPos < DRAW_DIST_NEAR || DRAW_DIST_FAR < zPos) continue;
			
			int p = m_aEmt[e].GetIndexTop();
			while (p != -1) {
				if (m_aEmt[e].GetParticle(p)->GetUse())
				{
					desc = m_aEmt[e].GetParticle(p)->GetDesc();

					instNo = instanceCount % INSTANCE_MAX;
					pInstance->pos[instNo] = { desc->m_pos.x, desc->m_pos.y, desc->m_pos.z + zPos, 0.0f };
					pInstance->rot[instNo] = { 0.0f, 0.0f, 0.0f, 0.0f };
					pInstance->scl[instNo] = { desc->m_scl, desc->m_scl, desc->m_scl, 0.0f };
					rate = desc->m_spd / desc->m_spdmax;
					pInstance->col[instNo] = { desc->m_col.x * rate, desc->m_col.y * rate, desc->m_col.z * rate, 1.0f };

					instanceCount++;

					if (instanceCount % INSTANCE_MAX == 0)
					{
						device->Unmap(GetInstanceBuffer(), 0);
						device->DrawInstanced(4, INSTANCE_MAX, 0, 0);

						if (instanceCount < m_nEmt * PARTICLE_MAX)
						{
							device->Map(GetInstanceBuffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
							pInstance = (INSTANCE*)msr.pData;
						}
					}
				}
				p = m_aEmt[e].GetIndexNext(p);
			}
		}
		instNo = instanceCount % INSTANCE_MAX;
		if (instNo != 0)
		{
			device->Unmap(GetInstanceBuffer(), 0);
			//device->DrawInstanced(4, instNo + 1, 0, 0);
			device->DrawInstanced(4, instNo, 0, 0);
		}
	}

	// �G�~�b�^�[�ݒu�Ǘ�
	//BOOL SetEmitter(XMFLOAT3* pos, float rot) {
	//	for (int e = 0; e < EMITTER_MAX; e++) {
	//		if (m_aEmt[e].Set(pos, rot)) return TRUE;
	//	}
	//	return FALSE;
	//}
	BOOL SetEmitter(int zPosNo, int rotPosNo) {
		for (int e = 0; e < m_nEmt; e++) {
			if (m_aEmt[e].Set(zPosNo, rotPosNo)) return TRUE;
		}
		return FALSE;
	}

	//EMITTER* GetEmitter(int index) { return &m_aEmt[index]; }
};

EMITTER g_aEmt[EMITTER_MAX];
EMITTER_MANAGER g_EM(g_aEmt, EMITTER_MAX);

EMITTER_EXP g_aEmtExp[EXP_MAX];
EMITTER_MANAGER g_EMExp(g_aEmtExp, EXP_MAX);

//=============================================================================
// ����������
//=============================================================================
HRESULT InitParticle(void)
{
	// ���_���̍쐬
	MakeVertexParticle();

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitParticle(void)
{
	if (g_Load == FALSE) return;

	// ���_�o�b�t�@�̉��
	if (g_VertexBuffer != NULL)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	g_Load = FALSE;
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateParticle(void)
{
	g_EM.Update();
	g_EMExp.Update();
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawParticle(void)
{
	g_EM.DrawParticle();
	g_EMExp.DrawParticle();
}

BOOL SetEmitter(int zPosNo, int rotPosNo) {
	return g_EM.SetEmitter(zPosNo, rotPosNo);
}

BOOL SetEmitterExp(int zPosNo, int rotPosNo) {
	return g_EMExp.SetEmitter(zPosNo, rotPosNo);
}

//=============================================================================
// ���_���̍쐬
//=============================================================================
HRESULT MakeVertexParticle(void)
{
	// ���_�o�b�t�@����
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);

	{//���_�o�b�t�@�̒��g�𖄂߂�
		D3D11_MAPPED_SUBRESOURCE msr;
		GetDeviceContext()->Map(g_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

		VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

		// ���_���W�̐ݒ�
		vertex[0].Position = XMFLOAT3(-0.5f, 0.5f, 0.0f);
		vertex[1].Position = XMFLOAT3(0.5f, 0.5f, 0.0f);
		vertex[2].Position = XMFLOAT3(-0.5f, -0.5f, 0.0f);
		vertex[3].Position = XMFLOAT3(0.5f, -0.5f, 0.0f);

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

	return S_OK;
}
