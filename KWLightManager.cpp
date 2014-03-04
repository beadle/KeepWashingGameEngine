#include "KWLightManager.h"
#include "D3DUtility.h"

KWLight::KWLight( void )
{
	static int lightCounter = 0;
	m_lightIndex = lightCounter++;
}

KWLight* KWLight::Create()
{
	KWLight* light = new KWLight;
	light->createLight();
	light->AutoRelease();
	return light;
}

void KWLight::createLight()
{
	::ZeroMemory(&light,sizeof(light));  
	D3DXCOLOR color = /*d3d::WHITE*/D3DXCOLOR(0.1f, 0.1f, 0.1f, 1.0f);
	light = d3d::InitDirectionalLight(D3DXVECTOR3(0, -1.0f, 1.0f), color);
}


/************************************************************************/
//Class LightManager
/************************************************************************/
KWLightManager::KWLightManager(void)
{
	m_pLightArray = KWArray::Create();
	m_pLightArray->Retain();

	m_pDevice = NULL;
}


KWLightManager::~KWLightManager(void)
{
	KW_SAFE_RELEASE_NULL(m_pLightArray);
}

KWLightManager* KWLightManager::Create( LPDIRECT3DDEVICE9 pDevice )
{
	KWLightManager* lightMan = new KWLightManager;
	lightMan->m_pDevice = pDevice;
	lightMan->AutoRelease();

	float comp = 0.9f;
	pDevice->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_COLORVALUE(comp, comp, comp, 1.0f));

	lightMan->SetLightingEnable(true);
	lightMan->SetSpecularEnable(false);

	return lightMan;
}

void KWLightManager::AddLight( KWLight* light )
{
	this->RemoveLight(light->getLightIndex());
	m_pLightArray->AddObject(light);

	this->EnableLight(light->getLightIndex(), true);
}

KWLight* KWLightManager::GetLight( int index )
{
	for (int i=0; i<m_pLightArray->Size(); ++i)
	{
		KWLight* light = (KWLight*)m_pLightArray->ObjectAtIndex(i);
		if (light->getLightIndex() == index)
		{
			return light;
		}
	}
	return NULL;
}

void KWLightManager::RemoveLight( int index )
{
	KWLight* light = GetLight(index);
	this->EnableLight(index, false);
	m_pLightArray->RemoveObject(light);
}

void KWLightManager::EnableLight( int index, bool enable/*=true*/ )
{
	KWLight* light = GetLight(index);
	if (enable == true)
	{
		m_pDevice->SetLight(index, &light->light);
	}
	m_pDevice->LightEnable(index, enable);
}

void KWLightManager::SetLightingEnable( bool enable )
{
	m_pDevice->SetRenderState(D3DRS_LIGHTING, enable);
}

void KWLightManager::SetSpecularEnable(bool enable)
{
	m_pDevice->SetRenderState(D3DRS_SPECULARENABLE, enable);
}
