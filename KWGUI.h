#pragma once

#include "KWGUIControl.h"
#include "KWArray.h"
#include "KWIEventReceiver.h"

class KWGUI : public KWObject, public KWIEventReceiver
{
public:
	SYNTHESIZE_READONLY(KWArray*, m_pControls, Controls);

public:
	SYNTHESIZE_READONLY(LPDIRECT3DDEVICE9, m_pDevice, Device);
	SYNTHESIZE_READONLY(float, m_windowWidth, WindowWidth);
	SYNTHESIZE_READONLY(float, m_windowHeight, WindowHeight);
	SYNTHESIZE_READONLY(KWGUIControl*, m_selectedControl, SelectedControl);

public:
	KWGUI(void);
	~KWGUI(void);

	static KWGUI* Create(LPDIRECT3DDEVICE9 pDevice, const D3DXVECTOR2& windowSize);
	virtual bool Init(LPDIRECT3DDEVICE9 pDevice, const D3DXVECTOR2& windowSize);

	int GetNextID() { static int id_counter = 0; return id_counter++; }
	KWGUIControl* GetControl(int id);
	KWGUIControl* GetPickingControl(const D3DXVECTOR2& point);

	virtual void LoadGUI();
	virtual void ReleaseGUI();
	void AddControl(KWGUIControl* control);

	virtual void Draw();
	virtual void OnEvent(const KWEvent& event);
};

