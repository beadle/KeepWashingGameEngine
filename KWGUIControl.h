#pragma once

#include <d3d9.h>
#include <d3dx9.h>
#include <string>

#include "KWObject.h"

using std::string;

// FVF灵活顶点类型的结构体
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

//GUI控件类
class KWGUIControl : public KWObject
{
public:
	SYNTHESIZE_READONLY(KWGUI*, m_pGUI, GUI);
	SYNTHESIZE_READONLY(KWGUIControlType, m_type, ControlType);	//控件类型
	SYNTHESIZE_READONLY(KWGUIControlState, m_state, ControlState);	//控件状态
	SYNTHESIZE_READONLY(int, m_id, ControlID);					//控件ID

	LPD3DXFONT	m_pFont;			//文本指针
	unsigned long m_textColor;		//控件颜色
	string m_pText;					//文字内容

	LPDIRECT3DVERTEXBUFFER9	m_pVertexBuffer;

	float m_xPos, m_yPos;		//控件的起始位置
	float m_width, m_height;	//控件的宽度和高度
	
	LPDIRECT3DTEXTURE9 m_background;					// 控件背景的填充图像
	LPDIRECT3DTEXTURE9 m_upTex, m_downTex, m_overTex;   // 存放按钮弹起，按下和鼠标经过时的3张纹理图

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


