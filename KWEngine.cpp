#include "KWEngine.h"
#include "KWAutoReleasePool.h"

#include "KWMesh.h"
#include "KWTerrain.h"
#include "KWSkyBox.h"
#include "KWParticleExample.h"

#include "D3DUtility.h"
#include "KWGUI.h"
#include "KWAction.h"

static LPD3DXFONT g_pFpsFont = NULL;
static float g_fDeltaTime = 1/60.0f;
static KWEventManager* g_pEventManager = NULL;

KWEngine::KWEngine(void)
{
	m_hWnd = NULL;
	m_pDevice = NULL;
	m_screenWidth = 800;
	m_screenHeight = 600;
	m_bShowFps = true;
	SetUpdateInterval(1/60.0f);

	m_pRunningScene = NULL;
	m_pEventManager = NULL;
	m_pApplication = NULL;
}


KWEngine::~KWEngine(void)
{
	this->GameExit();
}


static LRESULT CALLBACK _WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)  
	{
	case WM_DESTROY:
		PostQuitMessage (0); 
		break;
	case WM_KEYDOWN:
		if(wParam == VK_ESCAPE)
			DestroyWindow(hwnd);
		break;
 	default:
		if(g_pEventManager)
 			return g_pEventManager->WindowProc(hwnd, message, wParam, lParam);
		else
			return DefWindowProc(hwnd, message, wParam, lParam);
	}
	return 0;
}

int KWEngine::WindowInit( HINSTANCE hInstance, int iCmdShow, float width, float height)
{
	m_screenWidth = width;
	m_screenHeight = height;

	WNDCLASS wndclass; 
	static TCHAR szClassName[ ] = TEXT("Keep Washing Machine");

	wndclass.hInstance = hInstance;
	wndclass.lpszClassName = szClassName;
	wndclass.lpfnWndProc = _WindowProcedure; 
	wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS; 
	wndclass.cbWndExtra = 0;
	wndclass.cbClsExtra = 0;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.lpszMenuName = NULL;

	if (!RegisterClass (&wndclass))
		return 0;

	RECT rc = {0, 0, (LONG)m_screenWidth, (LONG)m_screenHeight};
	m_rect = rc;
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, false);
	

	m_hWnd = CreateWindow(
		szClassName, 
		TEXT("Keep Washing Game Engine --copyright (c) 2013 XiaoXia 11331329"), 
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT, 
		rc.right - rc.left,   
		rc.bottom - rc.top,
		NULL,
		NULL,
		hInstance,
		NULL
		);

	//create event manager
	m_pEventManager = KWEventManager::Create();
	m_pEventManager->Retain();
	g_pEventManager = m_pEventManager;

	ShowWindow (m_hWnd, iCmdShow);
	UpdateWindow(m_hWnd);

	return this->Run();
}

int KWEngine::Run()
{
	this->GameInit();

	MSG msg; 
	LARGE_INTEGER nFreq;
	LARGE_INTEGER nLast;
	LARGE_INTEGER nNow;

	QueryPerformanceFrequency(&nFreq);
	QueryPerformanceCounter(&nLast);

	while(true)
	{
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if(msg.message == WM_QUIT)
				break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			// Get current time tick.
			QueryPerformanceCounter(&nNow);

			// If it's the time to draw next frame, draw it, else sleep a while.
			LONGLONG Interval = nNow.QuadPart - nLast.QuadPart;
			if (Interval > m_nAnimationInterval.QuadPart)
			{
				nLast.QuadPart = nNow.QuadPart;
				double dt = (double)Interval / nFreq.QuadPart;
				this->GameUpdate((float)dt);
				this->GameRender();
			}
			else
			{
				Sleep(0);
			}
		}
	}

	return msg.wParam;
}

bool KWEngine::GameInit()
{
	srand(timeGetTime());

	LPDIRECT3D9 d3d9 = Direct3DCreate9(D3D_SDK_VERSION);

	D3DCAPS9 d3dcap;
	d3d9->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &d3dcap);

	DWORD vp;
	if( d3dcap.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT )
		vp = D3DCREATE_HARDWARE_VERTEXPROCESSING;
	else
		vp = D3DCREATE_SOFTWARE_VERTEXPROCESSING;

	D3DPRESENT_PARAMETERS d3dpp; 
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.BackBufferWidth            = (UINT)m_screenWidth;
	d3dpp.BackBufferHeight           = (UINT)m_screenHeight;
	d3dpp.BackBufferFormat           = D3DFMT_A8R8G8B8;
	d3dpp.BackBufferCount            = 1;
	d3dpp.MultiSampleType            = D3DMULTISAMPLE_NONE;
	d3dpp.MultiSampleQuality         = 0;
	d3dpp.SwapEffect                 = D3DSWAPEFFECT_DISCARD; 
	d3dpp.hDeviceWindow              = m_hWnd;
	d3dpp.Windowed                   = true;
	d3dpp.EnableAutoDepthStencil     = true; 
	d3dpp.AutoDepthStencilFormat     = D3DFMT_D24S8;
	d3dpp.Flags                      = 0;
	d3dpp.FullScreen_RefreshRateInHz = 0;
	d3dpp.PresentationInterval       = D3DPRESENT_INTERVAL_IMMEDIATE;

	HRESULT hr;
	hr = d3d9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, 
		m_hWnd, vp, &d3dpp, &m_pDevice);

	COM_SAFE_RELEASE(d3d9);

	//set projection matrix
	D3DXMATRIX matProj; 
	D3DXMatrixPerspectiveFovLH(
		&matProj, 
		D3DX_PI / 4.0f,                 
		(float)m_screenWidth / (float)m_screenHeight,    
		1.0f, 
		1000.0f
		); 
//	D3DXMatrixOrthoLH(&matProj, 960, 640, 1.0f, 1000.0f);
	m_pDevice->SetTransform(D3DTS_PROJECTION, &matProj);

	D3DXCreateFont(m_pDevice, 30, 0, 0, 1, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
		DEFAULT_QUALITY, 0, TEXT("微软雅黑"), &g_pFpsFont);

//	m_pDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );  

// 	//test
// 	this->Test();
	KWApplication* app = KWApplication::Create();
	this->setApplication(app);
	app->OnApplicationEnter();

	if(FAILED(hr))
		return false;
	else return true;
}

void KWEngine::GameExit()
{
	m_pApplication->OnApplicationExit();

	KW_SAFE_RELEASE_NULL(m_pRunningScene);
	KW_SAFE_RELEASE_NULL(m_pEventManager);
	KW_SAFE_RELEASE_NULL(m_pApplication);
	COM_SAFE_RELEASE_NULL(m_pDevice);
	COM_SAFE_RELEASE_NULL(g_pFpsFont);
}

void KWEngine::GameUpdate( float dt )
{
	KWAutoReleasePool::sharedInstance()->ExcuteAutoRelease();

	m_pRunningScene->Update(dt);

// 	for (int i=0; i<m_renderNodes->Size(); ++i)
// 	{
// 		KWNode* node = (KWNode*)m_renderNodes->ObjectAtIndex(i);
// 		node->Update(dt);
// 	}
	
// 	if(GetAsyncKeyState('W') & 0x8000)
// 	{
// 		m_camera->Walk(CAMERA_MOVE_UNIT * dt);
// 	}
// 
// 	if(GetAsyncKeyState('S') & 0x8000)
// 	{
// 		m_camera->Walk(-CAMERA_MOVE_UNIT * dt);
// 	}
// 
// 	if(GetAsyncKeyState('A') & 0x8000)
// 	{
// 		m_camera->Strafe(-CAMERA_MOVE_UNIT * dt);
// 	}
// 
// 	if(GetAsyncKeyState('D') & 0x8000)
// 	{
// 		m_camera->Strafe(CAMERA_MOVE_UNIT * dt);
// 	}
// 
// 	if(GetAsyncKeyState('R') & 0x8000)
// 	{
// 		m_camera->Fly(CAMERA_MOVE_UNIT * dt);
// 	}
// 
// 	if(GetAsyncKeyState('F') & 0x8000)
// 	{
// 		m_camera->Fly(-CAMERA_MOVE_UNIT * dt);
// 	}
// 
// 	if(GetAsyncKeyState(VK_UP) & 0x8000)
// 	{
// 		m_camera->Pitch(-CAMERA_ROTATE_UNIT * dt);
// 	}
// 
// 	if(GetAsyncKeyState(VK_DOWN) & 0x8000)
// 	{
// 		m_camera->Pitch(CAMERA_ROTATE_UNIT * dt);
// 	}
// 
// 	if(GetAsyncKeyState(VK_LEFT) & 0x8000)
// 	{
// 		m_camera->Yaw(-CAMERA_ROTATE_UNIT * dt);
// 	}
// 
// 	if(GetAsyncKeyState(VK_RIGHT) & 0x8000)
// 	{
// 		m_camera->Yaw(CAMERA_ROTATE_UNIT * dt);
// 	}
// 
// 	if(GetAsyncKeyState('N') & 0x8000)
// 	{
// 		m_camera->Roll(CAMERA_ROTATE_UNIT * dt);
// 	}
// 
// 	if(GetAsyncKeyState('M') & 0x8000)
// 	{
// 		m_camera->Roll(-CAMERA_ROTATE_UNIT * dt);
// 	}

}

void KWEngine::GameRender()
{
	m_pDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0); 
	m_pDevice->BeginScene();

	m_pRunningScene->Draw();

	if (m_bShowFps)
	{
		static char FpsString[15];
		sprintf(FpsString, "FPS: %.2f", 1.0f / g_fDeltaTime);
		g_pFpsFont->DrawTextA(0, FpsString, -1, &m_rect, DT_RIGHT | DT_TOP, d3d::WHITE);
	}

	m_pDevice->EndScene();
	m_pDevice->Present(NULL, NULL, NULL, NULL);
}

void KWEngine::Test()
{
	//scene test
	m_pRunningScene = KWScene::Create(m_pDevice);
	m_pRunningScene->Retain();

	//light test
	KWLight* light1 = KWLight::Create();
	m_pRunningScene->getLightManager()->AddLight(light1);

	//camera test
//	m_camera = KWFreeCamera::Create(D3DXVECTOR3(0, 0, -100.0f), D3DXVECTOR3(0, 0.0f, 0.0f), D3DXVECTOR3(0, 1.0f, 0));
	KWCamera* camera = KWFpsCamera::Create(D3DXVECTOR3(0, 0, -100.0f), D3DXVECTOR3(0, 2.0f, 0.0f), D3DX_PI / 3, -D3DX_PI / 3);
	m_pRunningScene->setCamera(camera);

	//mesh test
	KWMesh* mesh = KWMesh::Create();
	mesh->LoadMesh("hero.X");
	KWNode* meshNode = KWNode::Create();
	meshNode->setMesh(mesh);
	meshNode->SetScaling(0.3f);

	//terrain test
	KWTerrain* terrain = KWTerrain::Create();
	terrain->LoadTerrain("terrain-heightmap.raw", "grass_1024.jpg", 5.0f, 0.1f);

	//skyBox test
	KWSkyBox* skyBox = KWSkyBox::Create();
	const char* skyTextureNames[] = {
		"skybox/bleached_front.jpg",
		"skybox/bleached_back.jpg",
		"skybox/bleached_left.jpg",
		"skybox/bleached_right.jpg",
		"skybox/bleached_top.jpg"
	};
	skyBox->LoadSkyBox(skyTextureNames, 700.0f);

	//particle system test
	KWBoundingBox boundingBox;
	boundingBox.m_min = D3DXVECTOR3(-200, 0, -200);
	boundingBox.m_max = D3DXVECTOR3(200, 100, 200);
	KWParticleSnow* snow = KWParticleSnow::Create(m_pDevice, boundingBox, 500);

	D3DXVECTOR3 pos(50, 50, 50);
	KWParticleFire* fire = KWParticleFire::Create(m_pDevice, pos, 2000);

//	m_pRunningScene->SetTerrain(terrain);
//	m_pRunningScene->SetSkyBox(skyBox);
	m_pRunningScene->AddChild(meshNode);
//	m_pRunningScene->AddChild(fire);
//	m_pRunningScene->AddChild(snow);


	//action test
	KWActionMoveTo* moveTo = KWActionMoveTo::Create(D3DXVECTOR3(200, 200, 200), 20);
	meshNode->RunAction(moveTo);
	
	//gui test
// 	g_gui = KWGUI::Create(m_pDevice, D3DXVECTOR2(m_screenWidth, m_screenHeight));
// 	g_gui->Retain();
// 
// 	KWGUIPictureBox* background = KWGUIPictureBox::Create(g_gui, "GameMedia/maingui.jpg");
// 	background->SetContentSize(D3DXVECTOR2(m_screenWidth, m_screenHeight));
// 	background->SetPosition(D3DXVECTOR2(0, 0));
// 	background->LoadControl();
// 	g_gui->AddControl(background);
// 
// 	KWGUIButton* button = KWGUIButton::Create(g_gui, "GameMedia/startDown.png", "GameMedia/startUp.png", "GameMedia/startOver.png");
// 	button->LoadControl();
// 	g_gui->AddControl(button);
// 	m_pEventManager->AddEventReceiver(g_gui);
	
}

void KWEngine::SetUpdateInterval( float dt )
{
	LARGE_INTEGER nFreq;
	QueryPerformanceFrequency(&nFreq);
	m_nAnimationInterval.QuadPart = (LONGLONG)(dt * nFreq.QuadPart);
}

void KWEngine::ReplaceScene( KWScene* scene )
{
	KW_SAFE_RETAIN(scene);

	if (m_pRunningScene)
	{
		m_pRunningScene->OnExit();
		KW_SAFE_RELEASE(m_pRunningScene);
	}
	m_pRunningScene = scene;
	if(m_pRunningScene)
		m_pRunningScene->OnEnter();
}


