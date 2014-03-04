#include "KWUserNode.h"
#include "KWEngine.h"


KWUserNode::KWUserNode(void)
{
	m_pIndexBuffer = NULL;
	m_pVertexBuffer = NULL;
	m_pTexture = NULL;
}


KWUserNode::~KWUserNode(void)
{
	ReleaseNode();
}

KWUserNode* KWUserNode::Create()
{
	KWUserNode* userNode = new KWUserNode;
	userNode->m_pDevice = KWEngine::sharedInstance()->getDevice();
	userNode->AutoRelease();
	return userNode;
}

void KWUserNode::LoadRectangleNode( const char* textureName, float hw, float hh )
{
	ReleaseNode();

	HRESULT hr = D3DXCreateTextureFromFileA(m_pDevice, textureName, &m_pTexture);

	//´´½¨¶¥µã»º´æ
	m_vertexCount = 4;
	m_pDevice->CreateVertexBuffer(
		m_vertexCount * sizeof(KWVertex), 
		D3DUSAGE_WRITEONLY, 
		KWVertex::FVF, 
		D3DPOOL_DEFAULT, 
		&m_pVertexBuffer, 
		NULL);

	KWVertex* vertices;
	m_pVertexBuffer->Lock(0, 0, (void**)&vertices, 0);

	vertices[0] = KWVertex(-hw, hh, 0, 0.0f, 0.0f);
	vertices[1] = KWVertex(hw, hh, 0, 1.0f, 0.0f);
	vertices[2] = KWVertex(hw, -hh, 0, 1.0f, 1.0f);
	vertices[3] = KWVertex(-hw, -hh, 0, 0.0f, 1.0f);
	m_pVertexBuffer->Unlock();

	m_pDevice->CreateIndexBuffer(
		sizeof(WORD) * 6,
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX16,
		D3DPOOL_DEFAULT,
		&m_pIndexBuffer,
		NULL);

	WORD* indices;
	int index = 0;
	m_pIndexBuffer->Lock(0, 0, (void**)&indices, 0);

	indices[0] = 0; indices[1] = 1; indices[2] = 2;
	indices[3] = 0; indices[4] = 2; indices[5] = 3;

	m_pIndexBuffer->Unlock();

}

void KWUserNode::ReleaseNode()
{
	COM_SAFE_RELEASE_NULL(m_pIndexBuffer);
	COM_SAFE_RELEASE_NULL(m_pVertexBuffer);
	COM_SAFE_RELEASE_NULL(m_pTexture);
}

void KWUserNode::Draw()
{
	KWNode::Draw();

// 	if(!m_shouldDraw) return;
// 	if(m_hadDraw) return;
// 	m_hadDraw = true;
	m_pDevice->SetStreamSource(0, m_pVertexBuffer, 0, sizeof(KWVertex));
	m_pDevice->SetIndices(m_pIndexBuffer);
	m_pDevice->SetFVF(KWVertex::FVF);
	m_pDevice->SetTexture(0, m_pTexture);
	m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_vertexCount, 0, 2);
}

KWBoundingBox KWUserNode::GetBoundingBox()
{
	KWBoundingBox box;
	bool init = false;
	KWVertex* vertices = NULL;
	m_pVertexBuffer->Lock(0, 0, (void**)&vertices, 0);
	for (int i=0; i<m_vertexCount; ++i)
	{
		D3DXVECTOR3 vertex(vertices[i].x, vertices[i].y, vertices[i].z);
		if (!init)
		{
			init = true;
			box.Reset(vertex);
		}
		else
			box.AddInternalPoint(vertex);
	}
	m_pVertexBuffer->Unlock();
	return box;
}
