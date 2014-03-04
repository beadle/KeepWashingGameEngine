#include "KWAutoReleasePool.h"

static KWAutoReleasePool g_AutoReleasePool;

KWAutoReleasePool::KWAutoReleasePool(void)
{
	m_pManagedObjectArray.clear();
}


KWAutoReleasePool::~KWAutoReleasePool(void)
{
	this->ExcuteAutoRelease();
}

KWAutoReleasePool* KWAutoReleasePool::sharedInstance()
{
	return &g_AutoReleasePool;
}

void KWAutoReleasePool::ExcuteAutoRelease()
{
	for (vector<KWObject*>::iterator it = m_pManagedObjectArray.begin(); 
		it != m_pManagedObjectArray.end(); ++it)
	{
		(*it)->Release();
	}
	m_pManagedObjectArray.clear();
}

void KWAutoReleasePool::RemoveObject( KWObject* pObject )
{
	for (vector<KWObject*>::iterator it = m_pManagedObjectArray.begin(); 
		it != m_pManagedObjectArray.end(); ++it)
	{
		if((*it) == pObject)
		{
			m_pManagedObjectArray.erase(it);
			break;
		}
	}
}

void KWAutoReleasePool::AddObject( KWObject* pObject )
{
	m_pManagedObjectArray.push_back(pObject);
}

