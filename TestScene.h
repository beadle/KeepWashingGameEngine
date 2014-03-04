#pragma once

#include "KWScene.h"
#include "KWGUI.h"
#include "KWOctTree.h"

class TestScene : public KWScene
{
public:
	SYNTHESIZE_READONLY(KWGUI*, m_gui, GUI);
	SYNTHESIZE_READONLY(KWNode*, m_computer, Computer);
	SYNTHESIZE_READONLY(KWNode*, m_tv, TV);
	SYNTHESIZE_READONLY(KWOctTree*, m_octTree, OctTree);

public:
	TestScene(void);
	~TestScene(void);

	static TestScene* Create(LPDIRECT3DDEVICE9 pDevice);
	void Update(float dt);
	void OnEnter();
	void Draw();
	void InitGUI();
	void OnEvent(const KWEvent& event);

protected:
	void UpdateCamera(float dt);
	void UpdateLight(float dt);
};

