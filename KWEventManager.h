#pragma once

#include "KWObject.h"
#include "KWMacros.h"
#include "KWIEventReceiver.h"



#include <vector>
using std::vector;


//simple event manager
class KWEventManager : public KWObject
{
private:
	vector<KWIEventReceiver*> m_receivers;

public:
	KWEventManager(void);
	~KWEventManager(void);

	static KWEventManager* Create();

	void AddEventReceiver(KWIEventReceiver* receiver);
	void RemoveEventReceiver(int index);

	void PostEvent(const KWEvent& event);

	LRESULT WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
	KWEvent GetMouseInputEvent(KWMouseInputType mouseInputType, LPARAM lParam);
	KWEvent GetKeyInputEvent(WPARAM wParam, LPARAM lParam);

private:
	KWMouseInputType m_previousMouseInputType;
};

