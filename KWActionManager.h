#pragma once

#include "KWAction.h"
#include "KWArray.h"

class KWNode;

class KWActionManager : public KWObject
{
public:
	SYNTHESIZE_READONLY(KWArray*, m_pActionArray, ActionArray);
	SYNTHESIZE_READONLY(KWNode*, m_pTarget, Target);

public:
	KWActionManager(void);
	~KWActionManager(void);

	static KWActionManager* Create(KWNode* target);
	bool Init(KWNode* target);

	void AddAction(KWAction* pAction);
	void RemoveAllActions();

	void Update(float dt);

protected:
	void RemoveFinishedActions();
};

