#pragma once

#include "KWNode.h"

class KWTerrain : public KWNode
{
public:
	typedef enum
	{
		TerrainTypeNormal,
		TerrainTypePlane,
	}TerrainType;


private:
	float* m_heights;

	LPDIRECT3DVERTEXBUFFER9 m_pVertexBuffer;
	LPDIRECT3DINDEXBUFFER9 m_pIndexBuffer;
	LPDIRECT3DTEXTURE9 m_pTexture;

	int m_iRowNum;
	float m_fVertexInterval;
	float m_fHeightScale;

	TerrainType m_terrainType;

public:
	KWTerrain(void);
	~KWTerrain(void);

	static KWTerrain* Create();

	HRESULT LoadTerrain(const char* heightmap, const char* textureFilename
		, float vertexInterval, float heightScale);
	HRESULT LoadPlaneTerrain(const char* textureFilename, float vertexInterval, float height);
	void ReleaseTerrain();

	float GetHeight(float x, float z);

	virtual void Draw();
	virtual void Update(float dt);
	

};

