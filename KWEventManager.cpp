#include "KWEventManager.h"
#include "KWEngine.h"


KWEventManager::KWEventManager(void)
{
}


KWEventManager::~KWEventManager(void)
{
}

KWEventManager* KWEventManager::Create()
{
	KWEventManager* man = new KWEventManager;
	if (man)
	{
		man->AutoRelease();
	}
	else
	{
		KW_SAFE_DELETE(man);
	}
	return man;
}

void KWEventManager::AddEventReceiver( KWIEventReceiver* receiver )
{
	static int IndexCounter = 0;
	receiver->m_receiverIndex = IndexCounter++;

	m_receivers.push_back(receiver);
}

void KWEventManager::RemoveEventReceiver( int index )
{
	for (int i=0; i<m_receivers.size(); ++i)
	{
		KWIEventReceiver* receiver = m_receivers[i];
		if (receiver->getReceiverIndex() == index)
		{
			m_receivers.erase(m_receivers.begin() + i);
			break;
		}
	}
}

void KWEventManager::PostEvent( const KWEvent& event )
{
	for (int i=0; i<m_receivers.size(); ++i)
	{
		KWIEventReceiver* receiver = m_receivers[i];
		receiver->OnEvent(event);
	}
}

LRESULT KWEventManager::WindowProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	struct KWEvent event;
	bool bProcessed = false;

	switch(message)
	{
	//mouse events
	case WM_LBUTTONDOWN:
		event = GetMouseInputEvent(EMIT_LMOUSE_PRESS_DOWN, lParam);
		m_previousMouseInputType = EMIT_LMOUSE_PRESS_DOWN;
		bProcessed = true;
		break;
	case WM_LBUTTONUP:
		event = GetMouseInputEvent(EMIT_LMOUSE_PRESS_UP, lParam);
		m_previousMouseInputType = EMIT_LMOUSE_PRESS_UP;
		bProcessed = true;
		break;
	case WM_RBUTTONDOWN:
		event = GetMouseInputEvent(EMIT_RMOUSE_PRESS_DOWN, lParam);
		m_previousMouseInputType = EMIT_LMOUSE_PRESS_DOWN;
		bProcessed = true;
		break;
	case WM_RBUTTONUP:
		event = GetMouseInputEvent(EMIT_RMOUSE_PRESS_UP, lParam);
		m_previousMouseInputType = EMIT_RMOUSE_PRESS_UP;
		bProcessed = true;
		break;
	case WM_MOUSEMOVE:
// 		if (m_previousMouseInputType == EMIT_LMOUSE_PRESS_DOWN)
// 		{
// 			event = GetMouseInputEvent(EMIT_LMOUSE_RRESS_MOVE, lParam);
// 		}
// 		else if (m_previousMouseInputType == EMIT_RMOUSE_PRESS_DOWN)
// 		{
// 			event = GetMouseInputEvent(EMIT_RMOUSE_PRESS_MOVE, lParam);
// 		}
// 		else
// 		{
// 			event = GetMouseInputEvent(EMIT_MOUSE_MOVE, lParam);
// 		}
		event = GetMouseInputEvent(EMIT_MOUSE_MOVE, lParam);
		bProcessed = true;
		break;
	//key events
	case WM_KEYDOWN:
		event = GetKeyInputEvent(wParam, lParam);
		bProcessed = true;
		break;
	case WM_KEYUP:
		event = GetKeyInputEvent(wParam, lParam);
		bProcessed = true;
		break;
	default:
		bProcessed = false;
		return DefWindowProc(hwnd, message, wParam, lParam);
	}

	if (bProcessed)
		this->PostEvent(event);

	return 0;
}

KWEvent KWEventManager::GetMouseInputEvent( KWMouseInputType mouseInputType, LPARAM lParam )
{
	KWEvent event;
	event.eventType = EET_MOUSE_INPUT_ENENT;
	event.mouseInput.type = mouseInputType;
	event.mouseInput.point.x = (float)((short)LOWORD(lParam));
	float y = (float)((short)HIWORD(lParam));
	event.mouseInput.point.y = y;
	return event;
}

KWEvent KWEventManager::GetKeyInputEvent( WPARAM wParam, LPARAM lParam )
{
	KWEvent event;
	event.eventType = EET_KEY_INPUY_ENENT;
	event.keyInput.key = wParam;
	event.keyInput.key_state = lParam;
	return event;
}
