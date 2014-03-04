#include "KWObject.h"
#include "KWAutoReleasePool.h"

KWObject::KWObject(void) : m_refCount(1)
{
}


KWObject::~KWObject(void)
{
}

void KWObject::Release(void)
{
	--m_refCount;

	if (m_refCount == 0)
	{
		delete this;
	}
}

void KWObject::Retain(void)
{
	++m_refCount;
}

void KWObject::AutoRelease(void)
{
	KWAutoReleasePool::sharedInstance()->AddObject(this);
}