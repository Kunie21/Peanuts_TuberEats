//=============================================================================
//
// ���C�g���� [light.cpp]
// Author : ���] �đ�
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "light.h"

#include "input.h"
#include "debugproc.h"

//=============================================================================
// �O���[�o���ϐ�
//=============================================================================
LIGHT_AMBIENT		g_Ambient;
LIGHT_DIRECTIONAL	g_Directional[LIGHT_DIRECTIONAL_MAX];
LIGHT_POINT			g_Point[LIGHT_POINT_MAX];
LIGHT_SPOT			g_Spot[LIGHT_SPOT_MAX];

//=============================================================================
// ����������
//=============================================================================
void InitLight(void)
{
	g_Ambient.Color = { 1.0f, 1.0f, 1.0f, 1.0f };
	g_Ambient.Intensity = 0.75f; // 0.2f;
	g_Ambient.Use = TRUE;
	SetAmbientLight(&g_Ambient);

	for (int i = 0; i < LIGHT_DIRECTIONAL_MAX; i++)
	{
		g_Directional[i].Use = FALSE;
	}
	//g_Directional[0].Color = { 1.0f, 1.0f, 1.0f, 1.0f };
	//g_Directional[0].Direction = { 0.3f, -0.5f, 1.0f, 1.0f };
	//g_Directional[0].Intensity = 1.0f;
	//g_Directional[0].Use = TRUE;
	//g_Directional[1].Color = { 1.0f, 1.0f, 1.0f, 1.0f };
	//g_Directional[1].Direction = { 1.0f, -1.0f, 1.0f, 1.0f };
	//g_Directional[1].Intensity = 0.5f;
	//g_Directional[1].Use = TRUE;
	SetDirectionalLight(g_Directional);

	for (int i = 0; i < LIGHT_POINT_MAX; i++)
	{
		g_Point[i].Use = FALSE;
	}
	g_Point[0].Color = { 1.0f, 1.0f, 1.0f, 0.0f };
	g_Point[0].Position = { 0.0f, 180.0f, 700.0f, 1.0f };
	g_Point[0].Intensity = 1.0f;
	g_Point[0].Attenuation = 2000.0f;
	g_Point[0].Use = TRUE;
	SetPointLight(g_Point);

	for (int i = 0; i < LIGHT_SPOT_MAX; i++)
	{
		g_Spot[i].Use = FALSE;
	}
	//g_Spot[0].Color = { 1.0f, 1.0f, 1.0f, 1.0f };
	//g_Spot[0].Position = { 150.0f, 300.0f, -1000.0f, 1.0f };
	//g_Spot[0].Direction = { -0.2f, -0.3f, 1.0f, 1.0f };
	//g_Spot[0].Intensity = 1.0f;
	//g_Spot[0].Attenuation = 3000.0f;
	//g_Spot[0].Angle = XM_PIDIV4 * 0.5f;
	//g_Spot[0].Use = TRUE;
	//g_Spot[1].Color = { 1.0f, 1.0f, 1.0f, 1.0f };
	//g_Spot[1].Position = { -200.0f, 200.0f, -200.0f, 1.0f };
	//g_Spot[1].Direction = { 0.5f, -0.5f, 1.0f, 1.0f };
	//g_Spot[1].Intensity = 1.0f;
	//g_Spot[1].Attenuation = 1000.0f;
	//g_Spot[1].Angle = XM_PIDIV4 * 0.5f;
	//g_Spot[1].Use = FALSE;
	SetSpotLight(g_Spot);
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateLight(void)
{
	if (GetKeyboardPress(DIK_6)) {
		g_Point[0].Position.y += 10.0f;
		SetPointLight(g_Point);
	}
	if (GetKeyboardPress(DIK_7)) {
		g_Point[0].Position.y -= 10.0f;
		SetPointLight(g_Point);
	}
	if (GetKeyboardPress(DIK_8)) {
		g_Point[0].Position.x += 10.0f;
		SetPointLight(g_Point);
	}
	if (GetKeyboardPress(DIK_9)) {
		g_Point[0].Position.x -= 10.0f;
		SetPointLight(g_Point);
	}
#ifdef _DEBUG
	PrintDebugProc("lightposy:%f\n", g_Point[0].Position.y);
	PrintDebugProc("lightposx:%f\n", g_Point[0].Position.x);
#endif
}

//=============================================================================
// ���̑��̏���
//=============================================================================
BOOL GetALUse(void)
{
	if (g_Ambient.Use)
		return TRUE;
	return FALSE;
}
BOOL GetDLUse(int lightNo)
{
	if (lightNo < LIGHT_DIRECTIONAL_MAX && g_Directional[lightNo].Use)
		return TRUE;
	return FALSE;
}
BOOL GetPLUse(int lightNo)
{
	if (lightNo < LIGHT_POINT_MAX && g_Point[lightNo].Use)
		return TRUE;
	return FALSE;
}
BOOL GetSLUse(int lightNo)
{
	if (lightNo < LIGHT_SPOT_MAX && g_Spot[lightNo].Use)
		return TRUE;
	return FALSE;
}
BOOL GetPLAUse(void)
{
	if (g_Point[0].Color.w == 1.0f)
		return TRUE;
	return FALSE;
}
