#include "KWGUIControl.h"
#include "KWGUI.h"
#include "D3DUtility.h"
#include "KWGeometry.h"


KWGUIControl::KWGUIControl(void)
{
	m_pGUI = NULL;
	m_type = GCT_STATIC_FONT;
	m_state = GCT_STATE_NORMAL;
	m_id = -1;

	m_pFont = NULL;
	m_textColor = d3d::WHITE;

	m_pVertexBuffer = NULL;
	m_xPos = m_yPos = 0;
	m_width = 50.0f;
	m_height = 25.0f;

	m_background = NULL;
	m_upTex = m_overTex = m_downTex = NULL;
}


KWGUIControl::~KWGUIControl(void)
{
	this->ReleaseControl();
}


bool KWGUIControl::Init( KWGUI* pGUI )
{
	if (!pGUI) return false;

	m_pGUI = pGUI;
	m_id = pGUI->GetNextID();
	m_state = GCT_STATE_NORMAL;
	
	return true;
}

bool KWGUIControl::ContainPoint( const D3DXVECTOR2& point )
{
	D3DXVECTOR2 delta = point - D3DXVECTOR2(m_xPos, m_yPos);
	if (delta.x > 0 && delta.x < m_width &&
		delta.y > 0 && delta.y < m_height)
	{
		return true;
	}
	return false;
}

void KWGUIControl::Draw()
{

}

void KWGUIControl::LoadControl()
{

}

void KWGUIControl::ReleaseControl()
{
	COM_SAFE_RELEASE_NULL(m_pFont);
	COM_SAFE_RELEASE_NULL(m_pVertexBuffer);
	COM_SAFE_RELEASE_NULL(m_background);
	COM_SAFE_RELEASE_NULL(m_upTex);
	COM_SAFE_RELEASE_NULL(m_overTex);
	COM_SAFE_RELEASE_NULL(m_downTex);
}

void KWGUIControl::SetPosition( const D3DXVECTOR2& pos )
{
	m_xPos = pos.x;
	m_yPos = pos.y;
}

void KWGUIControl::SetContentSize(const D3DXVECTOR2& size)
{
	m_width = size.x;
	m_height = size.y;
}

bool KWGUIControl::IsSelected( const D3DXVECTOR2& pos )
{
	KWRect rect;
	rect.left = m_xPos; 
	rect.top = m_yPos;
	rect.right = rect.left + m_width;
	rect.bottom = rect.top + m_height;
	return rect.ContainPoint(pos);
}

//static text
KWGUIStaticText* KWGUIStaticText::Create( KWGUI* pGUI, const char* pText )
{
	KWGUIStaticText* text = new KWGUIStaticText;
	if (text && text->Init(pGUI, pText))
	{
		text->AutoRelease();
	}
	else
	{
		KW_SAFE_DELETE(text);
	}
	return text;
}

bool KWGUIStaticText::Init( KWGUI* pGUI, const char* pText )
{
	KWGUIControl::Init(pGUI);
	m_type = GCT_STATIC_FONT;
	m_pText = pText;

	return true;
}

void KWGUIStaticText::Draw()
{
	RECT fontPosition = { 0, 0, (long)m_pGUI->getWindowWidth(), (long)m_pGUI->getWindowHeight() };

	fontPosition.left = m_xPos;
	fontPosition.top = m_yPos;

	m_pFont->DrawTextA(NULL, m_pText.c_str(), -1, &fontPosition, DT_LEFT, m_textColor);
}

void KWGUIStaticText::LoadControl()
{
	D3DXCreateFont(m_pGUI->getDevice(), 30, 0, 0, 1, FALSE, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, 0, TEXT("微软雅黑"), &m_pFont);
}


//picture box control
KWGUIPictureBox* KWGUIPictureBox::Create( KWGUI* pGUI, const char* upTex )
{
	KWGUIPictureBox* picBox = new KWGUIPictureBox;
	if (picBox && picBox->Init(pGUI, upTex))
	{
		picBox->AutoRelease();
	}
	else
	{
		KW_SAFE_DELETE(picBox);
	}
	return picBox;
}

bool KWGUIPictureBox::Init( KWGUI* pGUI, const char* upTex )
{
	KWGUIControl::Init(pGUI);
	m_type = GCT_PICTURE_BOX;

	//load up texture
	if(!upTex) return false;

	HRESULT hr = S_OK;
	hr = D3DXCreateTextureFromFile(pGUI->getDevice(), upTex, &m_upTex);
	if (FAILED(hr)) return false;

	//pre set size
	D3DSURFACE_DESC desc;
	m_upTex->GetLevelDesc(0, &desc);
	m_width = desc.Width;
	m_height = desc.Height;

	return true;
}

void KWGUIPictureBox::Draw()
{
	LPDIRECT3DDEVICE9 device = m_pGUI->getDevice();

	// 设置纹理的alpha透明选项
	device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	bool isSetTexture = false;
	if(m_state == GCT_STATE_NORMAL && m_upTex) 
	{
		device->SetTexture(0, m_upTex);
		isSetTexture = true;
	}
	if(m_state == GCT_STATE_DOWN && m_downTex)
	{
		device->SetTexture(0, m_downTex);
		isSetTexture = true;
	}
	if(m_state == GCT_STATE_HIGHLIGHT && m_overTex)
	{
		device->SetTexture(0, m_overTex);
		isSetTexture = true;
	}

	if (!isSetTexture) device->SetTexture(0, m_upTex);

	device->SetStreamSource(0, m_pVertexBuffer, 0, sizeof(GUIVERTEX));
	device->SetFVF(D3DFVF_GUI);
	device->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

	device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
}

void KWGUIPictureBox::LoadControl()
{
	float w = m_width;
	float h = m_height;

	float x = m_xPos;
	float y = m_yPos;

	GUIVERTEX obj[] =
	{
		{w + x, 0 + y, 0.0f, 1, d3d::WHITE, 1.0f, 0.0f},
		{w + x, h + y, 0.0f, 1, d3d::WHITE, 1.0f, 1.0f},
		{0 + x, 0 + y, 0.0f, 1, d3d::WHITE, 0.0f, 0.0f},
		{0 + x, h + y, 0.0f, 1, d3d::WHITE, 0.0f, 1.0f},
	};

	LPDIRECT3DDEVICE9 pDevice = m_pGUI->getDevice();
	HRESULT hr = pDevice->CreateVertexBuffer(sizeof(obj), 
		0, D3DFVF_GUI, D3DPOOL_DEFAULT, &m_pVertexBuffer, NULL);
	if (FAILED(hr)) return;

	//fill vertex buffer
	void * v;
	hr = m_pVertexBuffer->Lock(0, sizeof(obj), (void**)&v, 0);
	if (FAILED(hr)) return;
	memcpy(v, obj, sizeof(obj));
	m_pVertexBuffer->Unlock();
}


//button control
KWGUIButton* KWGUIButton::Create( KWGUI* pGUI, const char* upTex, const char* downTex, const char* overTex )
{
	KWGUIButton* button = new KWGUIButton;
	if (button && button->Init(pGUI, upTex, downTex, overTex))
	{
		button->AutoRelease();
	}
	else
	{
		KW_SAFE_DELETE(button);
	}
	return button;
}

bool KWGUIButton::Init( KWGUI* pGUI, const char* upTex, const char* downTex, const char* overTex )
{
	bool ret = KWGUIPictureBox::Init(pGUI, upTex);
	m_type = GCT_BUTTON;

	//load all textures
	if(!ret) return false;

	HRESULT hr = S_OK;
	if (downTex)
	{
		hr = D3DXCreateTextureFromFile(pGUI->getDevice(), downTex, &m_downTex);
		if (FAILED(hr)) return false;
	}

	if (overTex)
	{
		hr = D3DXCreateTextureFromFile(pGUI->getDevice(), overTex, &m_overTex);
		if (FAILED(hr)) return false;
	}

	return true;
}

void KWGUIButton::Draw()
{
	KWGUIPictureBox::Draw();
}

