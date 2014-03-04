#pragma once

#include "KWMacros.h"

#include <d3d9.h>
#include <d3dx9.h>

class KWObject
{
public:
	SYNTHESIZE_READONLY(int, m_refCount, ReferenceCount);

public:
	KWObject(void);
	virtual ~KWObject(void);

	void Release(void);
	void Retain(void);
	void AutoRelease(void);
};

