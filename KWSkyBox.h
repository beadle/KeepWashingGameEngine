#pragma once

#include "KWNode.h"

class KWSkyBox : public KWNode
{
private:
	LPDIRECT3DTEXTURE9 m_pTextures[5];
	LPDIRECT3DVERTEXBUFFER9 m_pVertexBuffer;

	SYNTHESIZE_READONLY(float, m_fSize, Size);

public:
	KWSkyBox(void);
	~KWSkyBox(void);

	static KWSkyBox* Create();

	HRESULT LoadSkyBox(LPCSTR szTextureNames[], float fSize);
	void ReleaseSkyBox();

	virtual void Draw();
	virtual void Update(float dt);
};

