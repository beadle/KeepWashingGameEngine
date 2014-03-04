#pragma once

#include "KWCamera.h"
#include "KWNode.h"
#include "KWTerrain.h"
#include "KWMesh.h"
#include "KWSkyBox.h"
#include "KWLightManager.h"

#include "KWIEventReceiver.h"

class KWScene : public KWNode, public KWIEventReceiver
{
public:
	//scene elements
	SYNTHESIZE_READONLY(KWLightManager*, m_lightManager, LightManager);
	SYNTHESIZE_RETAIN(KWCamera*, m_camera, Camera);

	//special nodes
	SYNTHESIZE_READONLY(KWTerrain*, m_terrain, Terrain);
	SYNTHESIZE_READONLY(KWSkyBox*, m_skyBox, SkyBox);

public:
	KWScene(void);
	virtual ~KWScene(void);

	static KWScene* Create(LPDIRECT3DDEVICE9 pDevice);
	virtual bool Init(LPDIRECT3DDEVICE9 pDevice);

	void SetTerrain(KWTerrain* terrain);
	void SetSkyBox(KWSkyBox* skyBox);

	void PlaceOnTerrain(KWNode* node);
	KWNode* CreateMeshNode(const char* pFileName);

	virtual void Draw();
	virtual void Update(float dt);

	virtual void OnEnter() {};
	virtual void OnExit() {};

	virtual void OnEvent(const KWEvent& event);

};

