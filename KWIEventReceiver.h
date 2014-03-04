#pragma once

//enevt types
typedef enum 
{
	EET_GUI_ENENT,
	EET_MOUSE_INPUT_ENENT,
	EET_KEY_INPUY_ENENT,
	EET_USER_TYPE,
}KWEventType;

typedef enum
{
	EMIT_LMOUSE_PRESS_DOWN,
	EMIT_LMOUSE_RRESS_MOVE,
	EMIT_LMOUSE_PRESS_UP,

	EMIT_RMOUSE_PRESS_DOWN,
	EMIT_RMOUSE_PRESS_MOVE,
	EMIT_RMOUSE_PRESS_UP,

	EMIT_MOUSE_MOVE,
}KWMouseInputType;


//simple event struct
struct KWEvent
{
	KWEventType eventType;

	struct KWMouseInput
	{
		KWMouseInputType type;
		D3DXVECTOR2 point;
	};

	struct KWKeyInput
	{
		char key;
		unsigned int key_state;
	};

	struct KWMouseInput mouseInput;
	struct KWKeyInput keyInput;
};

//simple event receiver
class KWIEventReceiver
{
public:
	SYNTHESIZE_READONLY(int, m_receiverIndex, ReceiverIndex);

public:
	virtual void OnEvent(const KWEvent& event) = 0;

	friend class KWEventManager;
};