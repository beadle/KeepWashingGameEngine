#include "KWApplication.h"
#include "KWEngine.h"
#include "TestScene.h"


KWApplication::KWApplication(void)
{
}


KWApplication::~KWApplication(void)
{
}

KWApplication* KWApplication::Create()
{
	KWApplication* app = new KWApplication;
	app->AutoRelease();
	return app;
}

void KWApplication::OnApplicationEnter()
{
	KWEngine* engine = KWEngine::sharedInstance();

	//fps
	engine->SetUpdateInterval(1/60.0f);
	
	//running scene
	TestScene* scene = TestScene::Create(engine->getDevice());
	//KWScene* scene = KWScene::Create(engine->getDevice());
	engine->ReplaceScene(scene);
}

void KWApplication::OnApplicationPause()
{

}

void KWApplication::OnApplicationExit()
{

}
