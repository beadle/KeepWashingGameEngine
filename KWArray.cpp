#include "KWArray.h"


KWArray::KWArray(void)
{
	m_pObjects.clear();
}


KWArray::~KWArray(void)
{
	this->Clear();
}

void KWArray::AddObject( KWObject* pObject )
{
	m_pObjects.push_back(pObject);
	pObject->Retain();
}

void KWArray::RemoveObject( KWObject* pObject )
{
	for (int i=0; i<m_pObjects.size(); ++i)
	{
		if (m_pObjects[i] == pObject)
		{
			RemoveObjectAtIndex(i);
			break;
		}
	}
}

void KWArray::RemoveObjectAtIndex( int index )
{
	if (index >= m_pObjects.size() || index < 0)
		return;
	m_pObjects[index]->Release();
	m_pObjects.erase(m_pObjects.begin() + index);
}

KWObject* KWArray::ObjectAtIndex( int index )
{
	return m_pObjects[index];
}

void KWArray::Clear()
{
	for (ObjectIterator it = m_pObjects.begin(); it != m_pObjects.end(); ++it)
	{
		(*it)->Release();
	}
	m_pObjects.clear();
}

KWArray* KWArray::Create()
{
	KWArray* arr = new KWArray;
	arr->AutoRelease();
	return arr;
}
