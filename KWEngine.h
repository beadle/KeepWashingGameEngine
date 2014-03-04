#pragma once

#include <d3d9.h>
#include <d3dx9.h>

#include "KWMacros.h"
#include "MySingleton.h"
#include "KWNode.h"
#include "KWLightManager.h"
#include "KWCamera.h"
#include "KWScene.h"
#include "KWEventManager.h"
#include "KWApplication.h"


class KWEngine : public MySingleton<KWEngine>
{
public:
	SYNTHESIZE_READONLY(float, m_screenWidth, ScreenWidth);
	SYNTHESIZE_READONLY(float, m_screenHeight, ScreenHeight);

	SYNTHESIZE(bool, m_bShowFps, ShowFps);

	SYNTHESIZE_READONLY(HWND, m_hWnd, HWND);
	SYNTHESIZE_READONLY(RECT, m_rect, RECT);

	SYNTHESIZE_READONLY(LPDIRECT3DDEVICE9, m_pDevice, Device);

	SYNTHESIZE_READONLY(KWScene*, m_pRunningScene, RunningScene);
	SYNTHESIZE_READONLY(KWEventManager*, m_pEventManager, EventManager);
	SYNTHESIZE_RETAIN(KWApplication*, m_pApplication, Application);

public:
	KWEngine(void);
	~KWEngine(void);

	int WindowInit(HINSTANCE hInstance, int iCmdShow, float width, float height);

	void SetUpdateInterval(float dt);
	D3DXVECTOR2 GetWindowSize() { return D3DXVECTOR2(m_screenWidth, m_screenHeight); }

	//running scene
	void ReplaceScene(KWScene* scene);

protected:
	int Run();

	bool GameInit();
	void GameUpdate(float dt);
	void GameRender();
	void GameExit();

	void Test();

protected:
	LARGE_INTEGER       m_nAnimationInterval;

};

