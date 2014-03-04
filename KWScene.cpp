#include "KWScene.h"
#include "KWEngine.h"
#include "D3DUtility.h"

KWScene::KWScene(void)
{
	m_terrain = NULL;
	m_camera = NULL;
	m_lightManager = NULL;
	m_skyBox = NULL;
}

KWScene::~KWScene(void)
{
	KW_SAFE_RELEASE(m_lightManager);
	KW_SAFE_RELEASE(m_camera);
	KW_SAFE_RELEASE(m_terrain);
	KW_SAFE_RELEASE(m_skyBox);
}


KWScene* KWScene::Create( LPDIRECT3DDEVICE9 pDevice )
{
	KWScene* scene = new KWScene;
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

bool KWScene::Init( LPDIRECT3DDEVICE9 pDevice )
{
	m_pDevice = pDevice;

	m_lightManager = KWLightManager::Create(pDevice);
	m_lightManager->Retain();

	KWFpsCamera* camera = KWFpsCamera::Create(D3DXVECTOR3(0, 0, -10.0f), D3DXVECTOR3(0, 2.0f, 0.0f), D3DX_PI / 3, -D3DX_PI / 3);
	setCamera(camera);

	KWEngine::sharedInstance()->getEventManager()->AddEventReceiver(this);

	return true;
}

void KWScene::Draw()
{
	KWNode::Draw();
}

void KWScene::Update( float dt )
{
	KWNode::Update(dt);

	if (m_terrain) m_terrain->Update(dt);
	if (m_skyBox) m_skyBox->Update(dt);

	if (m_camera)
		m_pDevice->SetTransform(D3DTS_VIEW, &(m_camera->GetViewMatrix()));
}

void KWScene::SetTerrain( KWTerrain* terrain )
{
	KW_SAFE_RETAIN(terrain);
	KW_SAFE_RELEASE(m_terrain);
	m_terrain = terrain;
}

void KWScene::SetSkyBox( KWSkyBox* skyBox )
{
	KW_SAFE_RETAIN(skyBox);
	KW_SAFE_RELEASE(m_skyBox);
	m_skyBox = skyBox;
}

void KWScene::OnEvent( const KWEvent& event )
{
	switch(event.eventType)
	{
	case EET_MOUSE_INPUT_ENENT:
// 		if (event.mouseInput.type == EMIT_LMOUSE_PRESS_DOWN)
// 		{
// 			KWRay pickingRay = d3d::GetPickingRay(d3d::ConvertToDXCoord(event.mouseInput.point));
// 			KWNode* pickingNode = this->GetPickingNode(pickingRay);
// 
// 			if (pickingNode)
// 			{
// 				static bool open = false;
// 				open = (bool)(1 - open);
// 				if (open)
// 					pickingNode->SetScaling(1.5f);
// 				else pickingNode->SetScaling(1.0f);
// 			}
// 				
// 		}
		break;
	}
}

void KWScene::PlaceOnTerrain( KWNode* node )
{
	if(!m_terrain) return;

	D3DXVECTOR3 pos = node->getPosition();
	float height = node->GetWorldBoundingBox().GetHeight() / 2 + m_terrain->GetHeight(pos.x, pos.z);
	node->SetTranslation(D3DXVECTOR3(pos.x, height, pos.z));
}

KWNode* KWScene::CreateMeshNode(const char* pFileName)
{
	KWNode* meshNode = KWNode::Create();
	KWMesh* mesh = KWMesh::Create();
	mesh->LoadMesh(pFileName);
	meshNode->setMesh(mesh);
	return meshNode;
}
