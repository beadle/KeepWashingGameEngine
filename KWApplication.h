#pragma once

#include "KWObject.h"

class KWApplication : public KWObject
{
public:
	KWApplication(void);
	~KWApplication(void);

	static KWApplication* Create();

	virtual void OnApplicationEnter();
	virtual void OnApplicationPause();
	virtual void OnApplicationExit();
};

