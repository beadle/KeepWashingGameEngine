#include "KWSkyBox.h"
#include "KWEngine.h"

KWSkyBox::KWSkyBox(void)
{
	m_pVertexBuffer = NULL;
	ZeroMemory(m_pTextures, sizeof(m_pTextures));
	m_isOctTreeNode = false;
}


KWSkyBox::~KWSkyBox(void)
{
	ReleaseSkyBox();
}

KWSkyBox* KWSkyBox::Create()
{
	KWSkyBox* skyBox = new KWSkyBox;
	skyBox->m_pDevice = KWEngine::sharedInstance()->getDevice();
	skyBox->AutoRelease();
	return skyBox;
}

HRESULT KWSkyBox::LoadSkyBox( LPCSTR szTextureNames[], float fSize )
{
	ReleaseSkyBox();
	for (int i = 0; i < 5; i++)
	{
		HRESULT hr = D3DXCreateTextureFromFileA(m_pDevice, szTextureNames[i], &m_pTextures[i]);
		if (FAILED(hr))
			return S_FALSE;
	}

	m_pDevice->CreateVertexBuffer(
		sizeof(KWVertex) * 20, 
		D3DUSAGE_WRITEONLY, 
		KWVertex::FVF, 
		D3DPOOL_DEFAULT, 
		&m_pVertexBuffer, 
		NULL);

	float fHalfSize = fSize * 0.5f;
	m_fSize = fSize;

	KWVertex* vertices;
	m_pVertexBuffer->Lock(0, 0, (void**)&vertices, 0);

	//font
	vertices[0] = KWVertex(-fHalfSize, fHalfSize, fHalfSize, 0.0f, 0.0f);
	vertices[1] = KWVertex(fHalfSize, fHalfSize, fHalfSize, 1.0f, 0.0f);
	vertices[2] = KWVertex(fHalfSize, -fHalfSize, fHalfSize, 1.0f, 1.0f);
	vertices[3] = KWVertex(-fHalfSize, -fHalfSize, fHalfSize, 0.0f, 1.0f);

	//back
	vertices[4] = KWVertex(fHalfSize, fHalfSize, -fHalfSize, 0.0f, 0.0f);
	vertices[5] = KWVertex(-fHalfSize, fHalfSize, -fHalfSize, 1.0f, 0.0f);
	vertices[6] = KWVertex(-fHalfSize, -fHalfSize, -fHalfSize, 1.0f, 1.0f);
	vertices[7] = KWVertex(fHalfSize, -fHalfSize, -fHalfSize, 0.0f, 1.0f);

	//left
	vertices[8] = KWVertex(-fHalfSize, fHalfSize, -fHalfSize, 0.0f, 0.0f);
	vertices[9] = KWVertex(-fHalfSize, fHalfSize, fHalfSize, 1.0f, 0.0f);
	vertices[10] = KWVertex(-fHalfSize, -fHalfSize, fHalfSize, 1.0f, 1.0f);
	vertices[11] = KWVertex(-fHalfSize, -fHalfSize, -fHalfSize, 0.0f, 1.0f);

	//right
	vertices[12] = KWVertex(fHalfSize, fHalfSize, fHalfSize, 0.0f, 0.0f);
	vertices[13] = KWVertex(fHalfSize, fHalfSize, -fHalfSize, 1.0f, 0.0f);
	vertices[14] = KWVertex(fHalfSize, -fHalfSize, -fHalfSize, 1.0f, 1.0f);
	vertices[15] = KWVertex(fHalfSize, -fHalfSize, fHalfSize, 0.0f, 1.0f);

	//up
	vertices[16] = KWVertex(fHalfSize, fHalfSize, -fHalfSize, 1.0f, 0.0f);
	vertices[17] = KWVertex(fHalfSize, fHalfSize, fHalfSize, 1.0f, 1.0f);
	vertices[18] = KWVertex(-fHalfSize, fHalfSize, fHalfSize, 0.0f, 1.0f);
	vertices[19] = KWVertex(-fHalfSize, fHalfSize, -fHalfSize, 0.0f, 0.0f);

	m_pVertexBuffer->Unlock();


	return S_OK;
}

void KWSkyBox::ReleaseSkyBox()
{
	for (int i=0; i<5; ++i)
		COM_SAFE_RELEASE_NULL(m_pTextures[i]);

	COM_SAFE_RELEASE_NULL(m_pVertexBuffer);
}

void KWSkyBox::Draw()
{
	KWNode::Draw();

	m_pDevice->SetStreamSource(0, m_pVertexBuffer, 0, sizeof(KWVertex));
	m_pDevice->SetFVF(KWVertex::FVF);
	m_pDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	m_pDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

	for (int i = 0; i < 5; i++)
	{
		m_pDevice->SetTexture(0, m_pTextures[i]);
		m_pDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, i * 4, 2);
	}

	m_pDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	m_pDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
}

void KWSkyBox::Update( float dt )
{
	KWScene* scene = (KWScene*)m_pFatherNode;
	KWCamera* camera = scene->getCamera();
	D3DXVECTOR3 pos = camera->GetPosition();
	pos.y = m_position.y;
	this->SetTranslation(pos);
}
