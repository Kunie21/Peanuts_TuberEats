//=============================================================================
//
// ���[�h���� [load.cpp]
// Author : ���] �đ�
//
//=============================================================================
#include "main.h"
#include "game.h"
#include "renderer.h"
#include "texture2d.h"
#include "debugproc.h"
#include "camera.h"
#include "input.h"
#include "sound.h"
#include "fade.h"
#include "light.h"
#include "load.h"

#define LOAD_SUM 122	// ���[�h���郂�m�̐�

#define LOAD_Y					(110.0f)	// 
#define LOADMAP_Y				(0.0f)	// 
#define LOADMAP_ICON_SCALE		(0.18f)		// 

#define LOAD_ANIM_NUM	(12)
#define LOAD_ANIM_SPAN	(6)

/*******************************************************************************
* �O���[�o���ϐ�
*******************************************************************************/
static int		g_LoadSum = 0;
static int		g_LoadPoint = 0;
static float	g_LoadPalam = 0.0f;
static int		g_LoadAnimTime = 0;
static HANDLE	g_hThread = NULL;

// �e�N�X�`���Ǘ�
enum
{
	TEXTURE_NOWLOADING = 0,
	TEXTURE_NOWLOADING2,
	TEXTURE_LOAD_ANIM,
	TEXTURE_BAR_EMPTY,
	TEXTURE_BAR_FULL,
	TEXTURE_BG,
	TEXTURE_MAX,
};
static TEXTURE2D_DESC	g_td[TEXTURE_MAX];
static ID3D11ShaderResourceView*	g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����
static char*	g_TextureName[TEXTURE_MAX] = {
	"data/TEXTURE/nowloading.png",
	"data/TEXTURE/nowloading2.png",
	"data/TEXTURE/frog_jump_animation.png",
	"data/TEXTURE/empty_bar.png",
	"data/TEXTURE/full_bar.png",
	"data/TEXTURE/loadbg.png",
};

// ���[�h�p�X���b�h
DWORD WINAPI ThreadFuncLoad(LPVOID pParam)
{
	while (TRUE) UpdateLoad();
	return 0;
}

void InitLoad(void)
{
	// �e�N�X�`������
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		D3DX11CreateShaderResourceViewFromFile(GetDevice(), g_TextureName[i], NULL, NULL, &g_Texture[i], NULL);
		g_td[i].tex = &g_Texture[i];
	}

	// �ڍאݒ�
	g_td[TEXTURE_LOAD_ANIM].size = { 175.0f * 0.5f, 300.0f * 0.5f };
	g_td[TEXTURE_LOAD_ANIM].pos.y = LOAD_Y - 190.0f;
	g_td[TEXTURE_LOAD_ANIM].uv_pos = { 0.0f, 0.0f, 1.0f / (float)LOAD_ANIM_NUM };

	g_td[TEXTURE_NOWLOADING2].size = { 150.0f, 32.0f * 1.5f };
	g_td[TEXTURE_NOWLOADING2].pos.y = LOADMAP_Y + 70.0f + LOAD_Y;
	g_td[TEXTURE_NOWLOADING2].uv_pos = { 0.0f, 0.0f, 1.0f, 1.0f / (float)LOAD_MAX };

	g_td[TEXTURE_BAR_EMPTY].size = { 800.0f, 55.0f };
	g_td[TEXTURE_BAR_EMPTY].pos.y = LOAD_Y;
	g_td[TEXTURE_BAR_FULL].size = { 800.0f, 55.0f };
	g_td[TEXTURE_BAR_FULL].pos.y = LOAD_Y;


	// �y����̂͐�ɓǂݍ��񂶂Ⴄ
	//InitMap();

	// �ʂ̃X���b�h��쐬
	g_hThread = CreateThread(NULL, 0, ThreadFuncLoad, NULL, 0, NULL);
}

void UpdateLoad(void)
{
	if (CheckFadeIn() && g_LoadPoint < LOAD_MAX)
	{
		if (LoadingNow()) g_LoadPoint++;
		if (g_LoadPoint == LOAD_MAX)
		{
			CloseHandle(g_hThread);	// �X���b�h�����
			SetFade(FADE_OUT, MODE_GAME);
		}
	}
}

BOOL LoadingNow(void)
{
	//switch (g_LoadPoint)
	//{
	//case LOAD_GAMETEXTURE:
	//	return LoadGameTextureKernel(&g_LoadPalam, &g_LoadSum);

	//case LOAD_SOUND:
	//	return LoadSoundKernel(&g_LoadPalam, &g_LoadSum);

	//case LOAD_OBJECT:
	//	return LoadObjectKernel(&g_LoadPalam, &g_LoadSum);

	//case LOAD_SKYDOME:
	//	//return LoadSkydomeKernel(&g_LoadPalam, &g_LoadSum);
	//	return TRUE;

	//case LOAD_WALL:
	//	InitWall();
	//	break;

	//case LOAD_HOLD:
	//	return LoadHoldKernel(&g_LoadPalam, &g_LoadSum);

	//case LOAD_EXHOLD:
	//	InitExHold();
	//	break;

	//case LOAD_PARTICLE:
	//	InitParticle();
	//	break;

	//case LOAD_STARPARTICLE:
	//	InitStarParticle();
	//	break;

	//case LOAD_EFFECT:
	//	InitEffect();
	//	break;

	//case LOAD_ARROW:
	//	InitArrow();
	//	break;

	//case LOAD_PLAYER:
	//	return LoadPlayerKernel(&g_LoadPalam, &g_LoadSum);
	//}
	g_LoadSum++;
	return TRUE;
}

void DrawLoad(void)
{
	// �w�i
	{
		DrawTexture2D(&g_td[TEXTURE_BG]);
	}

	// Animation
	{
		float anim = (float)((int)(g_LoadAnimTime++ / LOAD_ANIM_SPAN) % LOAD_ANIM_NUM) / (float)LOAD_ANIM_NUM;
		g_td[TEXTURE_LOAD_ANIM].uv_pos.u = anim;
		DrawTexture2D(&g_td[TEXTURE_LOAD_ANIM], TRUE, TRUE);
	}

	// NowLoading2
	{
		static float time = 0.0f;
		time += 0.05f;
		g_td[TEXTURE_NOWLOADING2].uv_pos.v = (float)g_LoadPoint / (float)LOAD_MAX;
		g_td[TEXTURE_NOWLOADING2].col.w = 1.1f + sinf(time);
		DrawTexture2D(&g_td[TEXTURE_NOWLOADING2], TRUE, TRUE);
	}

	// �o�[
	{
		DrawTexture2D(&g_td[TEXTURE_BAR_EMPTY], TRUE);

		float rate = (float)g_LoadSum / (float)LOAD_SUM;
		g_td[TEXTURE_BAR_FULL].size.x = 800.0f * rate;
		g_td[TEXTURE_BAR_FULL].pos.x = -800.0f * 0.5f * (1.0f - rate);
		g_td[TEXTURE_BAR_FULL].uv_pos.uw = rate;
		DrawTexture2D(&g_td[TEXTURE_BAR_FULL], FALSE, TRUE);
	}

#ifdef _DEBUG	// �f�o�b�O����\������
	PrintDebugProc("LoadPoint:%f\n", (float)g_LoadPoint + g_LoadPalam);
	PrintDebugProc("LoadRate:%f\n", (float)g_LoadSum / (float)LOAD_SUM);
#endif
}
