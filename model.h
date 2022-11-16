//=============================================================================
//
// モデルの処理 [model.h]
// Author :
//
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"

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

struct MODEL_DATA
{
	DX11_MODEL	model;
	SRT			srt;
};



//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void LoadModel( char *FileName, DX11_MODEL *Model );
void UnloadModel( DX11_MODEL *Model );
void DrawModel(DX11_MODEL *Model, XMMATRIX* mtx, ID3D11ShaderResourceView** Texture = NULL, MATERIAL* pMaterial = NULL);
void DrawModel(DX11_MODEL *Model, SRT* srt, ID3D11ShaderResourceView** Texture = NULL, MATERIAL* pMaterial = NULL);
void DrawModel(DX11_MODEL *Model, ID3D11ShaderResourceView** Texture = NULL, MATERIAL* pMaterial = NULL);
void DrawModelInstanced(DX11_MODEL *Model, int instanceCount, MATERIAL* pMaterial = NULL);

// モデルのマテリアルのディフューズを取得する。Max16個分にしてある
void GetModelDiffuse(DX11_MODEL *Model, XMFLOAT4 *diffuse);

// モデルの指定マテリアルのディフューズをセットする。
void SetModelDiffuse(DX11_MODEL *Model, int mno, XMFLOAT4 diffuse);


