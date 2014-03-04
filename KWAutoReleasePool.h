#pragma once

#include "KWObject.h"
#include <vector>

using std::vector;

class KWAutoReleasePool
{
public:
	vector<KWObject*> m_pManagedObjectArray;

public:
	KWAutoReleasePool(void);
	~KWAutoReleasePool(void);

	static KWAutoReleasePool* sharedInstance();

	void ExcuteAutoRelease();

	void RemoveObject(KWObject* pObject);
	void AddObject(KWObject* pObject);
};

