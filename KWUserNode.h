#pragma once

#include "KWNode.h"

class KWUserNode : public KWNode
{
public:
	LPDIRECT3DVERTEXBUFFER9	m_pVertexBuffer;
	LPDIRECT3DINDEXBUFFER9	m_pIndexBuffer;
	LPDIRECT3DTEXTURE9		m_pTexture;

	int m_vertexCount;
public:
	KWUserNode(void);
	~KWUserNode(void);

	static KWUserNode* Create();

	void LoadRectangleNode(const char* textureName, float hw, float hh);
	void ReleaseNode();

	KWBoundingBox GetBoundingBox();

	void Draw();
};

