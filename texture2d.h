//=============================================================================
//
// 2D�摜�`�揈�� [texture2D.h]
// Author : ���] �đ�
//
//=============================================================================
#pragma once

// 2D�摜�`��p�f�[�^
struct TEXTURE2D_DESC
{
	UV_POSITION uv_pos = { 0.0f, 0.0f, 1.0f, 1.0f };	// UV���W
	XMFLOAT4 col = { 1.0f, 1.0f, 1.0f, 1.0f };			// �F
	XMFLOAT4 sd_col = { 0.0f, 0.0f, 0.0f, 0.5f };		// �e�F
	XMFLOAT2 sd_pos = { 5.0f, 5.0f };					// �e�ʒu�i���炷�ʁj
	XMFLOAT2 size = { SCREEN_WIDTH, SCREEN_HEIGHT };	// �T�C�Y
	XMFLOAT2 pos = { 0.0f, 0.0f };						// �ʒu
	XMFLOAT2 scl = { 1.0f, 1.0f };						// �g�嗦
	float rot = 0.0f;									// ��]
	CENTER_TYPE ctrType = CENTER_CENTER;				// ���S�̎��
	POSITION_TYPE posType = POSITION_RELATIVE;			// �ʒu�̎��
	ID3D11ShaderResourceView** tex = NULL ;				// �e�N�X�`��
};

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitTexture2D(void);
void UninitTexture2D(void);
void UpdateTexture2D(void);
void DrawTexture2D(TEXTURE2D_DESC* td, BOOL bShadow = FALSE, BOOL bUV = FALSE);
void SetUVTexture2D(UV_POSITION* uv);
void ResetUVTexture2D(void);
void DrawTexture2DAll(BOOL bInterrupt = FALSE);	// TRUE�Ŋ��荞�݁i�r���ŕ`�悷��j