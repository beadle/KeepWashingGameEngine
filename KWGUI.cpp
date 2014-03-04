#include "KWGUI.h"



KWGUI::KWGUI(void)
{
	m_pDevice = NULL;
	m_pControls = NULL;
	m_selectedControl = NULL;
}


KWGUI::~KWGUI(void)
{
	this->ReleaseGUI();
}

KWGUI* KWGUI::Create( LPDIRECT3DDEVICE9 pDevice, const D3DXVECTOR2& windowSize )
{
	KWGUI* gui = new KWGUI;
	if (gui && gui->Init(pDevice, windowSize))
	{
		gui->AutoRelease();
	}
	else
	{
		KW_SAFE_DELETE(gui);
	}
	return gui;
}

bool KWGUI::Init( LPDIRECT3DDEVICE9 pDevice, const D3DXVECTOR2& windowSize )
{
	m_pDevice = pDevice;

	m_pControls = KWArray::Create();
	m_pControls->Retain();

	m_windowWidth = windowSize.x;
	m_windowHeight = windowSize.y;

	return true;
}

KWGUIControl* KWGUI::GetControl( int id )
{
	for (int i=0; i<m_pControls->Size(); ++i)
	{
		KWGUIControl* control = (KWGUIControl*)m_pControls->ObjectAtIndex(i);
		if (control->getControlID() == id)
		{
			return control;
		}
	}
	return NULL;
}

void KWGUI::Draw()
{
	for (int i=0; i<m_pControls->Size(); ++i)
	{
		KWGUIControl* control = (KWGUIControl*)m_pControls->ObjectAtIndex(i);
		control->Draw();
	}

}

void KWGUI::OnEvent( const KWEvent& event )
{
	switch(event.eventType)
	{
	case EET_MOUSE_INPUT_ENENT:
		KWGUIControl* pickingControl = GetPickingControl(event.mouseInput.point);
		switch(event.mouseInput.type)
		{
		case EMIT_MOUSE_MOVE:
			if (pickingControl != m_selectedControl)
			{
				if(m_selectedControl)
					m_selectedControl->UnHighLight();
				if (pickingControl)
					pickingControl->HighLight();
				m_selectedControl = pickingControl;
			}
			break;
		case EMIT_LMOUSE_PRESS_DOWN:
			if (pickingControl)
			{
				pickingControl->PressDown();
			}
			break;
		case EMIT_LMOUSE_PRESS_UP:
			if (pickingControl)
			{
				pickingControl->UnHighLight();
			}
			break;
		}
		break;
	}
}

void KWGUI::LoadGUI()
{

}

void KWGUI::ReleaseGUI()
{
	KW_SAFE_RELEASE_NULL(m_pControls);
}

void KWGUI::AddControl( KWGUIControl* control )
{
	m_pControls->AddObject(control);
}

KWGUIControl* KWGUI::GetPickingControl( const D3DXVECTOR2& point )
{
	for (int i=0; i<m_pControls->Size(); ++i)
	{
		KWGUIControl* control = (KWGUIControl*)m_pControls->ObjectAtIndex(i);
		if (control->IsSelected(point))
			return control;
	}
	return NULL;
}
