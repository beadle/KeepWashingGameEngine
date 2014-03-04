#pragma once

#include "KWMacros.h"
#include "KWObject.h"
#include "KWGeometry.h"
#include "D3DUtility.h"

#include <d3d9.h>
#include <d3dx9.h>

class KWMesh : public KWObject
{
public:
	SYNTHESIZE_READONLY(int, m_materialNum, materialNum);
	SYNTHESIZE_READONLY(D3DMATERIAL9*, m_pMaterialArray, MaterialArray);
	SYNTHESIZE_READONLY(LPDIRECT3DTEXTURE9*, m_pTextureArray, TextureArray);
	SYNTHESIZE(bool, m_wireframeEnable, WireframeEnable);

protected:
	LPD3DXMESH m_pMesh;
	LPD3DXBUFFER m_pAdjacency;
	LPDIRECT3DDEVICE9 m_pDevice;

public:
	KWMesh(void);
	virtual ~KWMesh(void);

	static KWMesh* Create();

	bool LoadMesh(const char* pFileName);
	bool LoadMesh(const KWBoundingShpere& shpere);
	bool LoadMesh(const KWBoundingBox& box);
	void ReleaseMesh();

	KWBoundingShpere GetBoundingShpere();
	void GetBoundingShpere(KWBoundingShpere& boundingShpere);

	KWBoundingBox GetBoundingBox();
	void GetBoundingBox(KWBoundingBox& boundingBox);

	void OptimizeMesh();

	virtual void Draw();
};

