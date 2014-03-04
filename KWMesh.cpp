#include "KWMesh.h"
#include "D3DUtility.h"
#include "KWEngine.h"

KWMesh::KWMesh(void)
{
	m_pTextureArray = NULL;
	m_pMaterialArray = NULL;
	m_pMesh = NULL;

	m_pDevice = NULL;
	m_materialNum = 0;
	m_wireframeEnable = false;
}


KWMesh::~KWMesh(void)
{
	this->ReleaseMesh();
}

KWMesh* KWMesh::Create()
{
	KWMesh* mesh = new KWMesh;
	mesh->m_pDevice = KWEngine::sharedInstance()->getDevice();
	mesh->AutoRelease();
	return mesh;
}

bool KWMesh::LoadMesh( const char* pFileName )
{
	this->ReleaseMesh();
	if (pFileName == NULL)
		return false;

	LPD3DXBUFFER pMtrlBuffer = NULL;
	DWORD dwNumMaterials;
	HRESULT hr = D3DXLoadMeshFromXA(pFileName, D3DXMESH_MANAGED, m_pDevice, &m_pAdjacency, &pMtrlBuffer, NULL, &dwNumMaterials, &m_pMesh);
	if(FAILED(hr)) return false;

	LPD3DXMATERIAL pMaterials = (LPD3DXMATERIAL)pMtrlBuffer->GetBufferPointer();

	m_pMaterialArray = new D3DMATERIAL9[dwNumMaterials];
	m_pTextureArray = new LPDIRECT3DTEXTURE9[dwNumMaterials];

	m_materialNum = dwNumMaterials;
	m_wireframeEnable = false;

	for (DWORD i = 0; i < dwNumMaterials; i++)
	{
		//load materials
		m_pMaterialArray[i] = pMaterials[i].MatD3D;
		m_pMaterialArray[i].Ambient = m_pMaterialArray[i].Diffuse;

		//load textures
		LPCSTR filename = pMaterials[i].pTextureFilename;
		
		if (filename)
		{
			hr = D3DXCreateTextureFromFileA(m_pDevice, filename, &m_pTextureArray[i]);
			if(FAILED(hr)) m_pTextureArray[i] = NULL;
		}
		else m_pTextureArray[i] = NULL;
		
	}

	if(pMtrlBuffer)
	{
		pMtrlBuffer->Release();
		pMtrlBuffer = NULL;
	}
	
	OptimizeMesh();

	
	return true;

}

bool KWMesh::LoadMesh( const KWBoundingShpere& shpere )
{
	HRESULT hr = D3DXCreateSphere(
		m_pDevice, 
		shpere.m_radius,
		30,
		30,
		&m_pMesh,
		&m_pAdjacency);

	m_wireframeEnable = true;
	m_materialNum = 1;

	m_pMaterialArray = new D3DMATERIAL9[1];
	m_pMaterialArray[0] = d3d::WHITE_MTRL;

	m_pTextureArray = new LPDIRECT3DTEXTURE9[1];
	m_pTextureArray[0] = NULL;

	if(FAILED(hr))
		return false;
	else return true;
}

bool KWMesh::LoadMesh( const KWBoundingBox& box )
{
	D3DXVECTOR3 delta = box.m_max - box.m_min;
	HRESULT hr = D3DXCreateBox(
		m_pDevice, 
		delta.x,
		delta.y,
		delta.z,
		&m_pMesh,
		&m_pAdjacency);

	m_wireframeEnable = true;
	m_materialNum = 1;

	m_pMaterialArray = new D3DMATERIAL9[1];
	m_pMaterialArray[0] = d3d::WHITE_MTRL;

	m_pTextureArray = new LPDIRECT3DTEXTURE9[1];
	m_pTextureArray[0] = NULL;

	if(FAILED(hr))
		return false;
	else return true;
}

void KWMesh::ReleaseMesh()
{
	for (int i=0; i<m_materialNum; ++i)
		COM_SAFE_RELEASE_NULL(m_pTextureArray[i]);

	KW_SAFE_DELETE_ARRAY(m_pTextureArray);
	KW_SAFE_DELETE_ARRAY(m_pMaterialArray);
	COM_SAFE_RELEASE_NULL(m_pMesh);

	m_materialNum = 0;
	m_wireframeEnable = false;
}

void KWMesh::Draw()
{
	if (m_wireframeEnable)
		m_pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	for (int i = 0; i < m_materialNum; i++)
	{
		m_pDevice->SetTexture(0, m_pTextureArray[i]);

		m_pDevice->SetMaterial(&m_pMaterialArray[i]);
		m_pMesh->DrawSubset(i);		
		
	}
	if (m_wireframeEnable)
		m_pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
}

KWBoundingShpere KWMesh::GetBoundingShpere()
{
	HRESULT hr = S_OK;
	
	BYTE* v = NULL;
	m_pMesh->LockVertexBuffer(0, (void**)&v);

	KWBoundingShpere boundingShpere;
	hr = D3DXComputeBoundingSphere(
		(D3DXVECTOR3*)v,
		m_pMesh->GetNumVertices(),
		m_pMesh->GetNumBytesPerVertex(),
		&boundingShpere.m_center,
		&boundingShpere.m_radius );

	m_pMesh->UnlockVertexBuffer();

	return boundingShpere;
}

void KWMesh::GetBoundingShpere( KWBoundingShpere& boundingShpere )
{
	HRESULT hr = S_OK;

	BYTE* v = NULL;
	m_pMesh->LockVertexBuffer(0, (void**)&v);

	hr = D3DXComputeBoundingSphere(
		(D3DXVECTOR3*)v,
		m_pMesh->GetNumVertices(),
		m_pMesh->GetNumBytesPerVertex(),
		&boundingShpere.m_center,
		&boundingShpere.m_radius );

	m_pMesh->UnlockVertexBuffer();
}

KWBoundingBox KWMesh::GetBoundingBox()
{
	unsigned char* v = NULL;

	m_pMesh->LockVertexBuffer(0, (void**)&v);

	KWBoundingBox boundingBox;
	D3DXComputeBoundingBox(
		(D3DXVECTOR3*)v,
		m_pMesh->GetNumVertices(),
		m_pMesh->GetNumBytesPerVertex(),
		&boundingBox.m_min,
		&boundingBox.m_max);

	m_pMesh->UnlockVertexBuffer();

	return boundingBox;
}

void KWMesh::GetBoundingBox( KWBoundingBox& boundingBox )
{
	unsigned char* v = NULL;

	m_pMesh->LockVertexBuffer(0, (void**)&v);

	D3DXComputeBoundingBox(
		(D3DXVECTOR3*)v,
		m_pMesh->GetNumVertices(),
		m_pMesh->GetNumBytesPerVertex(),
		&boundingBox.m_min,
		&boundingBox.m_max);

	m_pMesh->UnlockVertexBuffer();
}

void KWMesh::OptimizeMesh()
{
	if (m_pMesh == NULL) return;

	m_pMesh->OptimizeInplace(
		D3DXMESHOPT_COMPACT |
		D3DXMESHOPT_ATTRSORT |
		D3DXMESHOPT_VERTEXCACHE,
		(DWORD*)m_pAdjacency->GetBufferPointer(),
		NULL,
		NULL,
		NULL);
}


