#pragma once

#include <d3d9.h>
#include <d3dx9.h>
#include <string>

#include "KWObject.h"

using std::string;

// FVF�������͵Ľṹ��
struct GUIVERTEX
{
	float x, y, z, rhw;
	unsigned long color;
	float tu, tv;
};
#define D3DFVF_GUI (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1)

typedef enum
{
	GCT_BUTTON,
	GCT_PICTURE_BOX,
	GCT_STATIC_FONT,
}KWGUIControlType;

typedef enum
{
	GCT_STATE_NORMAL,
	GCT_STATE_HIGHLIGHT,
	GCT_STATE_DOWN,
}KWGUIControlState;

class KWGUI;

//GUI�ؼ���
class KWGUIControl : public KWObject
{
public:
	SYNTHESIZE_READONLY(KWGUI*, m_pGUI, GUI);
	SYNTHESIZE_READONLY(KWGUIControlType, m_type, ControlType);	//�ؼ�����
	SYNTHESIZE_READONLY(KWGUIControlState, m_state, ControlState);	//�ؼ�״̬
	SYNTHESIZE_READONLY(int, m_id, ControlID);					//�ؼ�ID

	LPD3DXFONT	m_pFont;			//�ı�ָ��
	unsigned long m_textColor;		//�ؼ���ɫ
	string m_pText;					//��������

	LPDIRECT3DVERTEXBUFFER9	m_pVertexBuffer;

	float m_xPos, m_yPos;		//�ؼ�����ʼλ��
	float m_width, m_height;	//�ؼ��Ŀ�Ⱥ͸߶�
	
	LPDIRECT3DTEXTURE9 m_background;					// �ؼ����������ͼ��
	LPDIRECT3DTEXTURE9 m_upTex, m_downTex, m_overTex;   // ��Ű�ť���𣬰��º���꾭��ʱ��3������ͼ

public:
	KWGUIControl(void);
	virtual ~KWGUIControl(void);

	virtual bool Init(KWGUI* pGUI);

	virtual bool ContainPoint(const D3DXVECTOR2& point);

	virtual void PressDown() { m_state = GCT_STATE_DOWN; };
	virtual void HighLight() { m_state = GCT_STATE_HIGHLIGHT; }
	virtual void UnHighLight() { m_state = GCT_STATE_NORMAL; }

	virtual void Draw();
	virtual void LoadControl();
	virtual void ReleaseControl();

	virtual bool IsSelected(const D3DXVECTOR2& pos);

	virtual void SetPosition(const D3DXVECTOR2& pos);
	virtual D3DXVECTOR2 GetPosition() { return D3DXVECTOR2(m_xPos, m_yPos); }
	virtual void SetContentSize(const D3DXVECTOR2& size);
	virtual D3DXVECTOR2 GetContentSize() { return D3DXVECTOR2(m_width, m_height); }
};

class KWGUIStaticText : public KWGUIControl
{
public:
	static KWGUIStaticText* Create(KWGUI* pGUI, const char* pText);
	virtual bool Init(KWGUI* pGUI, const char* pText);

	virtual void Draw();
	virtual void LoadControl();

	virtual bool IsSelected(const D3DXVECTOR2& pos) { return false; }
};


class KWGUIPictureBox : public KWGUIControl
{
public:
	static KWGUIPictureBox* Create(KWGUI* pGUI, const char* upTex);
	virtual bool Init(KWGUI* pGUI, const char* upTex);

	virtual void Draw();
	virtual void LoadControl();

	virtual bool IsSelected(const D3DXVECTOR2& pos) { return false; }
};


class KWGUIButton : public KWGUIPictureBox
{
public:
	static KWGUIButton* Create(KWGUI* pGUI, const char* upTex, const char* downTex, const char* overTex);
	virtual bool Init(KWGUI* pGUI, const char* upTex, const char* downTex, const char* overTex);

	virtual void Draw();

	virtual bool IsSelected(const D3DXVECTOR2& pos) { return KWGUIControl::IsSelected(pos); }
};


