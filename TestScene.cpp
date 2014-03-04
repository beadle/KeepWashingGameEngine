#include "TestScene.h"
#include "KWEngine.h"
#include "KWMath.h"
#include "KWUserNode.h"

const float CAMERA_MOVE_UNIT = 50.0f;
const float CAMERA_ROTATE_UNIT = 0.005f;

static POINT g_InitMousePos;

#include <MMSystem.h>

TestScene::TestScene(void)
{
	m_gui = NULL;
	m_octTree = NULL;
}


TestScene::~TestScene(void)
{
	KW_SAFE_RELEASE(m_gui);
	KW_SAFE_RELEASE(m_octTree);
}


TestScene* TestScene::Create( LPDIRECT3DDEVICE9 pDevice )
{
	TestScene* scene = new TestScene;
	if (scene && scene->Init(pDevice))
	{
		scene->AutoRelease();
	}
	else
	{
		KW_SAFE_DELETE(scene);
	}
	return scene;
}

void TestScene::Update( float dt )
{
	KWScene::Update(dt);

	this->UpdateCamera(dt);
	this->UpdateLight(dt);
}

void TestScene::OnEnter()
{
	//configure
	KWEngine* engine = KWEngine::sharedInstance();
	g_InitMousePos.x = engine->getScreenWidth() / 2;
	g_InitMousePos.y = engine->getScreenHeight() / 2;
	ClientToScreen(engine->getHWND(), &g_InitMousePos);
	SetCursorPos(g_InitMousePos.x, g_InitMousePos.y);

	//oct tree
	m_octTree = KWOctTree::Create();
	m_octTree->Retain();

	//1.light
	KWLight* light1 = KWLight::Create();
	light1->light = d3d::InitPointLight(D3DXVECTOR3(0, 400.0f, -400.0f), d3d::WHITE*0.05f);
	m_lightManager->AddLight(light1);

	//2.camera
	KWCamera* camera = KWFpsCamera::Create(D3DXVECTOR3(0, 0, -40.0f), D3DXVECTOR3(0, 2.0f, -20.0f), D3DX_PI/3, -D3DX_PI/3);
	this->setCamera(camera);

	//3.terrain
	KWTerrain* terrain = KWTerrain::Create();
	terrain->LoadPlaneTerrain("resources/ground_bg.png", 500, 0);
	this->SetTerrain(terrain);
	this->AddChild(terrain);

	//gui
	this->InitGUI();

	//mesh2
	KWNode* shafaNode = CreateMeshNode("resources/shafa.X");
	shafaNode->SetTranslation(D3DXVECTOR3(50, 0, 20));
	shafaNode->SetScaling(0.8f);
	PlaceOnTerrain(shafaNode);
	this->AddChild(shafaNode);

	//TV
	KWNode* tvNode = CreateMeshNode("resources/tv.X");
	tvNode->SetScaling(0.4f);
	tvNode->SetTranslation(D3DXVECTOR3(-50, 1, 80));
	tvNode->SetRotationY(-D3DX_PI/6);
	shafaNode->AddChild(tvNode);
	m_tv = tvNode;

	//chair
	KWNode* chairNode = CreateMeshNode("resources/chair.X");
	chairNode->SetScaling(0.5f);
	chairNode->SetTranslation(D3DXVECTOR3(-40, 0, 0));
	PlaceOnTerrain(chairNode);
	this->AddChild(chairNode);

	//computer table
	KWNode* tableNode = CreateMeshNode("resources/table.X");
	tableNode->SetScaling(0.2f);
	tableNode->SetTranslation(D3DXVECTOR3(-40, 0, 20));
	PlaceOnTerrain(tableNode);
	this->AddChild(tableNode);

	KWNode* computerNode = CreateMeshNode("resources/computer.X");
	computerNode->SetTranslation(D3DXVECTOR3(0, computerNode->GetBoundingBox().GetHeight()/2
		+tableNode->GetBoundingBox().GetHeight()/3, 0));
	tableNode->AddChild(computerNode);
	m_computer = computerNode;

// 	KWNode* boundNode = KWNode::Create();
// 	KWMesh* boundMesh = KWMesh::Create();
// 	boundMesh->LoadMesh(computerNode->GetBoundingBox());
// 	boundNode->setMesh(boundMesh);
// 	computerNode->AddChild(boundNode);

	//air condictional
	KWNode* air = CreateMeshNode("resources/air_condictional.X");
	air->SetScaling(0.15f);
	air->SetRotationY(-0.5f);
	air->SetTranslation(D3DXVECTOR3(-80, 0, 50));
	PlaceOnTerrain(air);
	this->AddChild(air);

//	PlaySound("background_music.wav", NULL, SND_FILENAME | SND_ASYNC);

//	m_pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	//wall and floor and ceiling
	float halfWidth = 200.0f, halfHeight = 50.0f;
	KWUserNode* roomNode1 = KWUserNode::Create();
	roomNode1->LoadRectangleNode("resources/room_whole_004.jpg", halfWidth, halfHeight);
	roomNode1->SetTranslation(D3DXVECTOR3(0, halfHeight, halfWidth));

 	KWUserNode* roomNode2 = KWUserNode::Create();
 	roomNode2->LoadRectangleNode("resources/room_whole_001.jpg", halfWidth, halfHeight);
 	roomNode2->SetTranslation(D3DXVECTOR3(halfWidth, halfHeight, 0));
 	roomNode2->SetRotationY(D3DX_PI/2);

	KWUserNode* roomNode3 = KWUserNode::Create();
	roomNode3->LoadRectangleNode("resources/room_whole_002.jpg", halfWidth, halfHeight);
	roomNode3->SetTranslation(D3DXVECTOR3(0, halfHeight, -halfWidth));
	roomNode3->SetRotationY(D3DX_PI);

	KWUserNode* roomNode4 = KWUserNode::Create();
	roomNode4->LoadRectangleNode("resources/room_whole_003.jpg", halfWidth, halfHeight);
	roomNode4->SetTranslation(D3DXVECTOR3(-halfWidth, halfHeight, 0));
	roomNode4->SetRotationY(-D3DX_PI/2);

	KWUserNode* roomNode5 = KWUserNode::Create();
	roomNode5->LoadRectangleNode("resources/wall_bg.png", halfWidth, halfWidth);
	roomNode5->SetTranslation(D3DXVECTOR3(0, 2*halfHeight, 0));
	roomNode5->SetRotationX(-D3DX_PI/2);

	this->AddChild(roomNode1);
	this->AddChild(roomNode2);
	this->AddChild(roomNode3);
	this->AddChild(roomNode4);
	this->AddChild(roomNode5);
	
	
}

void TestScene::UpdateCamera( float dt )
{
	//update fps camera
	KWCamera* camera = this->getCamera();
// 	POINT curMousePos;
// 	GetCursorPos(&curMousePos);
// 	int deltaX = curMousePos.x - g_InitMousePos.x;
// 	int deltaY = curMousePos.y - g_InitMousePos.y;
// 	SetCursorPos(g_InitMousePos.x, g_InitMousePos.y);
// 
// 	if(deltaY != 0)
// 	{
// 		camera->Pitch(deltaY * CAMERA_ROTATE_UNIT);
// 	}
// 	if(deltaX != 0)
// 	{
// 		camera->Yaw(deltaX * CAMERA_ROTATE_UNIT);
// 	}
	if(GetAsyncKeyState('W') & 0x8000)
	{
		camera->Walk(CAMERA_MOVE_UNIT * dt);
	}

	if(GetAsyncKeyState('S') & 0x8000)
	{
		camera->Walk(-CAMERA_MOVE_UNIT * dt);
	}

	if(GetAsyncKeyState('A') & 0x8000)
	{
		camera->Strafe(-CAMERA_MOVE_UNIT * dt);
	}

	if(GetAsyncKeyState('D') & 0x8000)
	{
		camera->Strafe(CAMERA_MOVE_UNIT * dt);
	}

	if(GetAsyncKeyState('J') & 0x8000)
	{
		camera->Yaw(-5.0f*CAMERA_ROTATE_UNIT);
	}
	if(GetAsyncKeyState('L') & 0x8000)
	{
		camera->Yaw(5.0f*CAMERA_ROTATE_UNIT);
	}
	if(GetAsyncKeyState('I') & 0x8000)
	{
		camera->Pitch(-CAMERA_ROTATE_UNIT);
	}
	if(GetAsyncKeyState('K') & 0x8000)
	{
		camera->Pitch(CAMERA_ROTATE_UNIT);
	}

	static float originHeight = 20.0f;
	D3DXVECTOR3 pos = camera->GetPosition();
	if (m_terrain)
	{
		pos.y = m_terrain->GetHeight(pos.x, pos.z) + originHeight;
	}
	else
	{
		pos.y = originHeight;
	}
	camera->SetPosition(pos);

}

void TestScene::UpdateLight( float dt )
{
	//update light position
	KWLight* light = m_lightManager->GetLight(0);
	D3DXVECTOR3 pos = light->light.Position;

	D3DXMATRIX matRotate;
	D3DXMatrixRotationY(&matRotate, dt*2.0f);
	D3DXVec3TransformCoord(&pos, &pos, &matRotate);
	light->light.Position = pos;
	m_lightManager->EnableLight(light->getLightIndex());
}

void TestScene::InitGUI()
{
	D3DXVECTOR2 windowSize = KWEngine::sharedInstance()->GetWindowSize();
	m_gui = KWGUI::Create(m_pDevice, windowSize);
	m_gui->Retain();
	
	KWGUIStaticText* introduce = KWGUIStaticText::Create(m_gui, "室内可交互场景 Create By 11331329 肖夏");
	introduce->SetPosition(D3DXVECTOR2(20, 0));
	introduce->LoadControl();
	m_gui->AddControl(introduce);

	KWGUIStaticText* operatingIntroduce = KWGUIStaticText::Create(m_gui, "按ADWS以及JLIK键控制视角左右上下移动");
	operatingIntroduce->SetPosition(D3DXVECTOR2(20, 30));
	operatingIntroduce->LoadControl();
	m_gui->AddControl(operatingIntroduce);

	KWGUIStaticText* mouseIntroduce = KWGUIStaticText::Create(m_gui, "鼠标点击电脑或者电视可以交互");
	mouseIntroduce->SetPosition(D3DXVECTOR2(20, 60));
	mouseIntroduce->LoadControl();
	m_gui->AddControl(mouseIntroduce);
}

void TestScene::Draw()
{
 	ResetDrawTag();
//  	m_octTree->ConstructTree(m_pChildrenNodes, 1024.0f);
//  	m_octTree->TagVisibleNodes(d3d::GetViewPyramid());
	KWScene::Draw();
//	m_octTree->ReleaseTree();
	m_gui->Draw();
}

void TestScene::OnEvent( const KWEvent& event )
{
	if (event.eventType == EET_MOUSE_INPUT_ENENT)
	{
		if (event.mouseInput.type == EMIT_LMOUSE_PRESS_DOWN)
		{
			KWRay pickingRay = d3d::GetPickingRay(d3d::ConvertToDXCoord(event.mouseInput.point));
			if (m_computer->IsPicking(pickingRay))
			{
				system("START resources/my_video.avi");
			}
			if (m_tv->IsPicking(pickingRay))
			{
				PlaySound("resources/background_music.wav", NULL, SND_FILENAME | SND_ASYNC);
			}
		}
	}
}
