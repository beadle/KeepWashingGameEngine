#include "KWActionManager.h"


KWActionManager::KWActionManager(void)
{
	m_pActionArray = KWArray::Create();
	m_pActionArray->Retain();
}


KWActionManager::~KWActionManager(void)
{
	KW_SAFE_RELEASE_NULL(m_pActionArray);
}

void KWActionManager::AddAction( KWAction* pAction )
{
	m_pActionArray->AddObject(pAction);
	pAction->StartWithTarget(m_pTarget);
}

void KWActionManager::RemoveFinishedActions()
{
	for (int i=0; i<m_pActionArray->Size();)
	{
		KWAction* pAction = (KWAction*)m_pActionArray->ObjectAtIndex(i);
		if (pAction->getFinished())
		{
			m_pActionArray->RemoveObjectAtIndex(i);
		}
		else ++i;
	}
}

void KWActionManager::Update( float dt )
{
	RemoveFinishedActions();
	for (int i=0; i<m_pActionArray->Size(); ++i)
	{
		KWAction* pAction = (KWAction*)m_pActionArray->ObjectAtIndex(i);
		pAction->Update(dt);
	}
}

KWActionManager* KWActionManager::Create( KWNode* target )
{
	KWActionManager* man = new KWActionManager;
	if (man && man->Init(target))
	{
		man->AutoRelease();
	}
	else
	{
		KW_SAFE_DELETE(man);
	}
	return man;
}

bool KWActionManager::Init( KWNode* target )
{
	m_pTarget = target;

	return true;
}

void KWActionManager::RemoveAllActions()
{
	m_pActionArray->Clear();
}
