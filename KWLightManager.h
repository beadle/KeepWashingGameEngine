#pragma once

#include "KWObject.h"
#include "KWArray.h"

class KWLight : public KWObject
{
public:
	SYNTHESIZE_READONLY(int, m_lightIndex, LightIndex);
	
	D3DLIGHT9 light;
	
public:
	KWLight(void);
	static KWLight* Create();
	
protected:
	void createLight();
};

class KWLightManager : public KWObject
{
public:
	SYNTHESIZE_READONLY(KWArray*, m_pLightArray, LightArray);
	SYNTHESIZE_READONLY(LPDIRECT3DDEVICE9, m_pDevice, Device);

public:
	KWLightManager(void);
	~KWLightManager(void);

	static KWLightManager* Create(LPDIRECT3DDEVICE9 pDevice);

	void AddLight(KWLight* light);
	KWLight* GetLight(int index);
	void RemoveLight(int index);
	void EnableLight(int index, bool enable=true);
	

	void SetLightingEnable(bool enable);
	void SetSpecularEnable(bool enable);
};

