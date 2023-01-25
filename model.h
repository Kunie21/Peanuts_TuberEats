//=============================================================================
//
// モデルの処理 [model.h]
// Author :
//
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"
#include "texture2d.h"

//*********************************************************
// 構造体
//*********************************************************

// マテリアル構造体

#define MODEL_MAX_MATERIAL		(16)		// １モデルのMaxマテリアル数

struct DX11_MODEL_MATERIAL
{
	MATERIAL					Material;
	ID3D11ShaderResourceView	*Texture;
};

// 描画サブセット構造体
struct DX11_SUBSET
{
	unsigned short	StartIndex;
	unsigned short	IndexNum;
	DX11_MODEL_MATERIAL	Material;
};

struct DX11_MODEL
{
	ID3D11Buffer*	VertexBuffer;
	ID3D11Buffer*	IndexBuffer;

	DX11_SUBSET		*SubsetArray;
	unsigned short	SubsetNum;
};

enum MODEL_LABEL {
	MODEL_GATE = 0,
	MODEL_ICE,
	MODEL_RING,
	MODEL_MISSILE1,
	MODEL_MISSILE2,
	MODEL_FIRE,
	MODEL_ROCKET1,
	MODEL_ROCKET2,
	MODEL_ROCKET3,
	MODEL_ROCKET4,
	//MODEL_ROCKET5,
	MODEL_STAGE,
	MODEL_EARTH,
	MODEL_SUSHI01,
	MODEL_SUSHI02,
	//MODEL_SUSHI03,
	MODEL_LOLLIPOP01,
	MODEL_LOLLIPOP02,
	MODEL_RAMEN,
	MODEL_DONUT01,
	//MODEL_DONUT02,
	MODEL_MAX,
};
void InitModel(void);
void UninitModel(void);
BOOL LoadModelKernel(void);

struct MODEL_DATA
{
	MODEL_LABEL	model;
	//DX11_MODEL	model;
	SRT			srt;
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void LoadModel(char *FileName, DX11_MODEL *Model);
void UnloadModel(DX11_MODEL *Model);

void DrawModel(MODEL_LABEL* model, XMMATRIX* mtx, MATERIAL* pMaterial = NULL);
void DrawModel(MODEL_LABEL* model, SRT* srt, MATERIAL* pMaterial = NULL);
void DrawModel(MODEL_LABEL* model, MATERIAL* pMaterial = NULL);

void DrawModel(MODEL_LABEL* model, XMMATRIX* mtx, TEXTURE_LABEL pTexture, MATERIAL* pMaterial = NULL);
void DrawModel(MODEL_LABEL* model, SRT* srt, TEXTURE_LABEL pTexture, MATERIAL* pMaterial = NULL);
void DrawModel(MODEL_LABEL* model, TEXTURE_LABEL pTexture, MATERIAL* pMaterial = NULL);

//void DrawModel(MODEL_LABEL *model, XMMATRIX* mtx, ID3D11ShaderResourceView** pTexture = NULL, MATERIAL* pMaterial = NULL);
//void DrawModel(MODEL_LABEL *model, SRT* srt, ID3D11ShaderResourceView** pTexture = NULL, MATERIAL* pMaterial = NULL);
//void DrawModel(MODEL_LABEL *model, ID3D11ShaderResourceView** pTexture = NULL, MATERIAL* pMaterial = NULL);
void DrawModelInstanced(MODEL_LABEL* model, int instanceCount, MATERIAL* pMaterial = NULL);

void DrawModel(DX11_MODEL *Model, XMMATRIX* mtx, ID3D11ShaderResourceView** Texture, MATERIAL* pMaterial);
void DrawModel(DX11_MODEL *Model, SRT* srt, ID3D11ShaderResourceView** Texture, MATERIAL* pMaterial);
void DrawModel(DX11_MODEL *Model, ID3D11ShaderResourceView** Texture, MATERIAL* pMaterial);
void DrawModelInstanced(DX11_MODEL *Model, int instanceCount, MATERIAL* pMaterial);

// モデルのマテリアルのディフューズを取得する。Max16個分にしてある
void GetModelDiffuse(DX11_MODEL *Model, XMFLOAT4 *diffuse);

// モデルの指定マテリアルのディフューズをセットする。
void SetModelDiffuse(DX11_MODEL *Model, int mno, XMFLOAT4 diffuse);


