//=============================================================================
//
// �����蔻�菈�� [collision.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "collision.h"

//=============================================================================
// ���ɂ�铖���蔻�菈��
// ��]�͍l�����Ȃ�
// �߂�l�F�������Ă���TRUE
//=============================================================================
int CollisionSphere(XMFLOAT3 pos1, float radius1, XMFLOAT3 pos2, float radius2)
{
	int ans = -1;	// �O���Z�b�g���Ă���

	// ����v�Z
	XMVECTOR v_1 = XMLoadFloat3(&pos1);
	XMVECTOR v_2 = XMLoadFloat3(&pos2);
	XMVECTOR v_temp = v_2 - v_1;

	// ����
	float length = 0.0f;
	float radius = radius1 + radius2;

	// �i�[
	XMStoreFloat(&length, XMVector3Length(v_temp));

	// �o�E���f�B���O�{�b�N�X(BB)�̏���
	if (radius > length)
	{
		// �����������̏���
		ans = (int)((1.0f - (length - 7.0f) / (radius - 7.0f)) * 5.9f);
	}

	return ans;
}

//=============================================================================
// BB�ɂ�铖���蔻�菈��
// ��]�͍l�����Ȃ�
// �߂�l�F�������Ă���TRUE
//=============================================================================
BOOL CollisionBB(XMFLOAT3 mpos, float mw, float mh,
	XMFLOAT3 ypos, float yw, float yh)
{
	BOOL ans = FALSE;	// �O���Z�b�g���Ă���

	// ���W�����S�_�Ȃ̂Ōv�Z���₷�������ɂ��Ă���
	mw /= 2;
	mh /= 2;
	yw /= 2;
	yh /= 2;

	// �o�E���f�B���O�{�b�N�X(BB)�̏���
	if ((mpos.x + mw > ypos.x - yw) &&
		(mpos.x - mw < ypos.x + yw) &&
		(mpos.y + mh > ypos.y - yh) &&
		(mpos.y - mh < ypos.y + yh))
	{
		// �����������̏���
		ans = TRUE;
	}

	return ans;
}

//=============================================================================
// BC�ɂ�铖���蔻�菈��
// �T�C�Y�͔��a
// �߂�l�F�������Ă���TRUE
//=============================================================================
BOOL CollisionBC(XMFLOAT3 pos1, XMFLOAT3 pos2, float r1, float r2)
{
	BOOL ans = FALSE;						// �O���Z�b�g���Ă���

	float len = (r1 + r2) * (r1 + r2);		// ���a��2�悵����
	XMVECTOR temp = XMLoadFloat3(&pos1) - XMLoadFloat3(&pos2);
	temp = XMVector3LengthSq(temp);			// 2�_�Ԃ̋����i2�悵�����j
	float lenSq = 0.0f;
	XMStoreFloat(&lenSq, temp);

	// ���a��2�悵������苗�����Z���H
	if (len > lenSq)
	{
		ans = TRUE;	// �������Ă���
	}

	return ans;
}


//=============================================================================
// ���(dot)
//=============================================================================
float dotProduct(XMVECTOR *v1, XMVECTOR *v2)
{
#if 0
	float ans = v1->x * v2->x + v1->y * v2->y + v1->z * v2->z;
#else
	// �_�C���N�g�w�ł́A�A�A
	XMVECTOR temp = XMVector3Dot(*v1, *v2);
	float ans = 0.0f;
	XMStoreFloat(&ans, temp);
#endif

	return(ans);
}


//=============================================================================
// �O��(cross)
//=============================================================================
void crossProduct(XMVECTOR *ret, XMVECTOR *v1, XMVECTOR *v2)
{
#if 0
	ret->x = v1->y * v2->z - v1->z * v2->y;
	ret->y = v1->z * v2->x - v1->x * v2->z;
	ret->z = v1->x * v2->y - v1->y * v2->x;
#else
	// �_�C���N�g�w�ł́A�A�A
	*ret = XMVector3Cross(*v1, *v2);
#endif

}


//=============================================================================
// ���C�L���X�g
// p0, p1, p2�@�|���S���̂R���_
// pos0 �n�_
// pos1 �I�_
// hit�@��_�̕ԋp�p
// normal �@���x�N�g���̕ԋp�p
// �������Ă���ꍇ�ATRUE��Ԃ�
//=============================================================================
BOOL RayCast(XMFLOAT3 xp0, XMFLOAT3 xp1, XMFLOAT3 xp2, XMFLOAT3 xpos0, XMFLOAT3 xpos1, XMFLOAT3 *hit, XMFLOAT3 *normal)
{
	XMVECTOR	p0   = XMLoadFloat3(&xp0);
	XMVECTOR	p1   = XMLoadFloat3(&xp1);
	XMVECTOR	p2   = XMLoadFloat3(&xp2);
	XMVECTOR	pos0 = XMLoadFloat3(&xpos0);
	XMVECTOR	pos1 = XMLoadFloat3(&xpos1);

	XMVECTOR	nor;	// �|���S���̖@��
	XMVECTOR	vec1;
	XMVECTOR	vec2;
	float		d1, d2;

	{	// �|���S���̊O�ς�Ƃ��Ė@������߂�(���̏����͑Ώۂ��Œ蕨�Ȃ�\��Init()�ōs���Ă����Ɨǂ�)
		vec1 = p1 - p0;
		vec2 = p2 - p0;
		crossProduct(&nor, &vec2, &vec1);
		nor = XMVector3Normalize(nor);		// �v�Z���₷���悤�ɖ@����m�[�}���C�Y���Ă���(���̃x�N�g���̒�����P�ɂ��Ă���)
		XMStoreFloat3(normal, nor);			// ���߂��@�������Ă���
	}

	// �|���S�����ʂƐ����̓�ςƂ��ďՓ˂��Ă���\���𒲂ׂ�i�s�p�Ȃ�{�A�݊p�Ȃ�[�A���p�Ȃ�O�j
	vec1 = pos0 - p0;
	vec2 = pos1 - p0;
	{	// ���߂��|���S���̖@���ƂQ�̃x�N�g���i�����̗��[�ƃ|���S����̔C�ӂ̓_�j�̓�ςƂ��ďՓ˂��Ă���\���𒲂ׂ�
		d1 = dotProduct(&vec1, &nor);
		d2 = dotProduct(&vec2, &nor);
		if (((d1 * d2) > 0.0f) || (d1 == 0 && d2 == 0))
		{
			// �������Ă���\���͖���
			return(FALSE);
		}
	}


	{	// �|���S���Ɛ����̌�_����߂�
		d1 = (float)fabs(d1);	// ��Βl����߂Ă���
		d2 = (float)fabs(d2);	// ��Βl����߂Ă���
		float a = d1 / (d1 + d2);							// �����

		XMVECTOR	vec3 = (1 - a) * vec1 + a * vec2;		// p0�����_�ւ̃x�N�g��
		XMVECTOR	p3 = p0 + vec3;							// ��_
		XMStoreFloat3(hit, p3);								// ���߂���_�����Ă���

		{	// ���߂���_���|���S���̒��ɂ��邩���ׂ�

			// �|���S���̊e�ӂ̃x�N�g��
			XMVECTOR	v1 = p1 - p0;
			XMVECTOR	v2 = p2 - p1;
			XMVECTOR	v3 = p0 - p2;

			// �e���_�ƌ�_�Ƃ̃x�N�g��
			XMVECTOR	v4 = p3 - p1;
			XMVECTOR	v5 = p3 - p2;
			XMVECTOR	v6 = p3 - p0;

			// �O�ςŊe�ӂ̖@������߂āA�|���S���̖@���Ƃ̓�ς�Ƃ��ĕ�����`�F�b�N����
			XMVECTOR	n1, n2, n3;

			crossProduct(&n1, &v4, &v1);
			if (dotProduct(&n1, &nor) < 0.0f) return(FALSE);	// �������Ă��Ȃ�

			crossProduct(&n2, &v5, &v2);
			if (dotProduct(&n2, &nor) < 0.0f) return(FALSE);	// �������Ă��Ȃ�
			
			crossProduct(&n3, &v6, &v3);
			if (dotProduct(&n3, &nor) < 0.0f) return(FALSE);	// �������Ă��Ȃ�
		}
	}

	return(TRUE);	// �������Ă���I(hit�ɂ͓������Ă����_�������Ă���Bnormal�ɂ͖@���������Ă���)
}


int CollisionLineSegment(XMFLOAT3 pos, XMFLOAT3 start, XMFLOAT3 end, float range)
{
	int ans = -1;	// �O���Z�b�g���Ă���

	// ����v�Z
	XMVECTOR v_pos = XMLoadFloat3(&pos) - XMLoadFloat3(&start);
	XMVECTOR v_line = XMLoadFloat3(&end) - XMLoadFloat3(&start);

	float line_length;
	XMStoreFloat(&line_length, XMVector3Length(v_line));

	float t = dotProduct(&XMVector3Normalize(v_line), &v_pos) / line_length;
	XMVECTOR v_col = v_line * t - v_pos;

	float col_length;
	if (t < 0)
	{
		XMStoreFloat(&col_length, XMVector3Length(v_pos));
	}
	else if (t > 1)
	{
		XMStoreFloat(&col_length, XMVector3Length(XMLoadFloat3(&pos) - XMLoadFloat3(&end)));
	}
	else
	{
		XMStoreFloat(&col_length, XMVector3Length(v_col));
	}

	if (range > col_length)
	{
		// �����������̏���
		ans = (int)((1.0f - (col_length - 7.0f) / (range - 7.0f)) * 5.9f);
	}

	return ans;
}