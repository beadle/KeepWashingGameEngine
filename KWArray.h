#pragma once

#include "KWObject.h"

#include <vector>

using std::vector;


class KWArray : public KWObject
{
private:
	vector<KWObject*> m_pObjects;

	typedef vector<KWObject*>::iterator ObjectIterator;

public:
	KWArray(void);
	virtual ~KWArray(void);

	static KWArray* Create();

	void AddObject(KWObject* pObject);

	void RemoveObject(KWObject* pObject);
	void RemoveObjectAtIndex(int index);
	void Clear();

	int Size() { return m_pObjects.size(); }

	KWObject* ObjectAtIndex(int index);
};

