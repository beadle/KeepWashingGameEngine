#include "KWTerrain.h"
#include "KWEngine.h"


KWTerrain::KWTerrain(void)
{
	m_pTexture = NULL;
	m_pIndexBuffer = NULL;
	m_pVertexBuffer = NULL;
	m_heights = NULL;
	m_fHeightScale = 4.0f;
	m_fVertexInterval = 0.1f;
	m_iRowNum = 0;
	m_isOctTreeNode = false;
}


KWTerrain::~KWTerrain(void)
{
	ReleaseTerrain();
}

KWTerrain* KWTerrain::Create()
{
	KWTerrain* terrain = new KWTerrain;
	terrain->m_pDevice = KWEngine::sharedInstance()->getDevice();
	terrain->AutoRelease();
	return terrain;
}

HRESULT KWTerrain::LoadTerrain( const char* heightmapName, const char* textureName , 
	float vertexInterval, float heightScale )
{
	ReleaseTerrain();

	m_terrainType = TerrainTypeNormal;
	
	m_fVertexInterval = vertexInterval;
	m_fHeightScale = heightScale;

	FILE* fp = fopen(heightmapName, "rb");
	if(!fp) return E_FAIL;

	//获取高度图的大小(单位是字节)
	fseek(fp, 0, SEEK_END);
	int fileSize = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	int iVertexNum = fileSize;
	m_iRowNum = (int)sqrt((double)fileSize);
	unsigned char* ucHeights = new unsigned char[iVertexNum];
	m_heights = new float[iVertexNum];

	//读取fileSize个字节的高度信息
	fread(ucHeights, iVertexNum, 1, fp);
	fclose(fp);

	//乘以高度缩放系数
	int i, j;
	for (i = 0; i < iVertexNum; i++)
		m_heights[i] = ucHeights[i] * m_fHeightScale;

	delete [] ucHeights;

	//创建顶点缓存
	m_pDevice->CreateVertexBuffer(
		iVertexNum * sizeof(KWVertex), 
		D3DUSAGE_WRITEONLY, 
		KWVertex::FVF, 
		D3DPOOL_DEFAULT, 
		&m_pVertexBuffer, 
		NULL);

	//创建索引缓存，一共有 (m_iRowNum - 1) * (m_iRowNum - 1)个正方形
	m_pDevice->CreateIndexBuffer(
		sizeof(WORD) * (m_iRowNum - 1) * (m_iRowNum - 1) * 6,
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX16,
		D3DPOOL_DEFAULT,
		&m_pIndexBuffer,
		NULL);


	KWVertex* vertices;
	m_pVertexBuffer->Lock(0, 0, (void**)&vertices, 0);

	//以地形的中点为中心（本地坐标原点）
	float fStartZ = (m_iRowNum - 1) * m_fVertexInterval * 0.5f;
	float fStartX = -fStartZ;


	//填充顶点缓存
	for (i = 0; i < m_iRowNum; i++)
	{
		for (j = 0; j < m_iRowNum; j++)
		{
			vertices[i * m_iRowNum + j] = KWVertex(
				fStartX + j * m_fVertexInterval,
				m_heights[i * m_iRowNum + j],
				fStartZ - i * m_fVertexInterval,
				(float)j / (m_iRowNum - 1) * 10,		//重复绘制10*10次地形纹理
				(float)i / (m_iRowNum - 1) * 10
				);
		}
	}

	m_pVertexBuffer->Unlock();


	WORD* indices;
	int index = 0;
	m_pIndexBuffer->Lock(0, 0, (void**)&indices, 0);

	//填充索引缓存
	for (i = 0; i < m_iRowNum - 1; i++)
	{
		for (j = 0; j < m_iRowNum - 1; j++)
		{
			indices[index++] = i * m_iRowNum + j;
			indices[index++] = i * m_iRowNum + j + 1;
			indices[index++] = (i + 1) * m_iRowNum + j;

			indices[index++] = i * m_iRowNum + j + 1;
			indices[index++] = (i + 1) * m_iRowNum + j + 1;
			indices[index++] = (i + 1) * m_iRowNum + j;
		}
	}

	m_pIndexBuffer->Unlock();

	//创建地形纹理
	D3DXCreateTextureFromFileA(m_pDevice, textureName, &m_pTexture);

	return S_OK;
}

HRESULT KWTerrain::LoadPlaneTerrain( const char* textureFilename, float vertexInterval, float height )
{
	ReleaseTerrain();

	m_fVertexInterval = vertexInterval;
	m_terrainType = TerrainTypePlane;
	//创建地形纹理
	HRESULT hr = D3DXCreateTextureFromFileA(m_pDevice, textureFilename, &m_pTexture);

	int iVertexNum = 4;
	m_iRowNum = 2;

	m_heights = new float[4];
	for(int i=0; i<iVertexNum; ++i)
		m_heights[i] = height;

	//创建顶点缓存
	m_pDevice->CreateVertexBuffer(
		iVertexNum * sizeof(KWVertex), 
		D3DUSAGE_WRITEONLY, 
		KWVertex::FVF, 
		D3DPOOL_DEFAULT, 
		&m_pVertexBuffer, 
		NULL);

	KWVertex* vertices;
	m_pVertexBuffer->Lock(0, 0, (void**)&vertices, 0);

	float halfSize = m_fVertexInterval / 2;
	vertices[0] = KWVertex(-halfSize, height, halfSize, 0.0f, 0.0f);
	vertices[1] = KWVertex(halfSize, height, halfSize, 1.0f, 0.0f);
	vertices[2] = KWVertex(halfSize, height, -halfSize, 1.0f, 1.0f);
	vertices[3] = KWVertex(-halfSize, height, -halfSize, 0.0f, 1.0f);
	m_pVertexBuffer->Unlock();


	//创建索引缓存，一共有 (m_iRowNum - 1) * (m_iRowNum - 1)个正方形
	m_pDevice->CreateIndexBuffer(
		sizeof(WORD) * (m_iRowNum - 1) * (m_iRowNum - 1) * 6,
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

	return S_OK;
}

void KWTerrain::ReleaseTerrain()
{
	COM_SAFE_RELEASE_NULL(m_pTexture);
	COM_SAFE_RELEASE_NULL(m_pIndexBuffer);
	COM_SAFE_RELEASE_NULL(m_pVertexBuffer);
	KW_SAFE_DELETE_ARRAY(m_heights);
	m_fHeightScale = 4.0f;
	m_fVertexInterval = 0.1f;
	m_iRowNum = 0;
}

float KWTerrain::GetHeight( float x, float z )
{
	if (m_terrainType == TerrainTypePlane)
	{
		return m_heights[0];
	}

	float terrainSize = (m_iRowNum - 1) * m_fVertexInterval;

	//地形的本地坐标原点是地形中心位置
	x = terrainSize * 0.5f + x;
	z = terrainSize * 0.5f - z;

	x /= m_fVertexInterval;
	z /= m_fVertexInterval;

	int i = (int)z;
	int j = (int)x;

	float A = m_heights[i * m_iRowNum + j];
	float B = m_heights[i * m_iRowNum + j + 1];
	float C = m_heights[(i + 1) * m_iRowNum + j];
	float D = m_heights[(i + 1) * m_iRowNum + j + 1];

	float dx = x - j;
	float dz = z - i;
	float height;

	//由三角形的三个顶点经过插值得到当前点的高度
	if(dx + dz < 1.0f) 
	{
		height = A + (B - A) * dx + (C - A) * dz;	
	}
	else
	{
		height = D + (B - D) * (1.0f - dz) + (C - D) * (1.0f - dx); 
	}
	return height;
}

void KWTerrain::Draw()
{
	KWNode::Draw();

	m_pDevice->SetStreamSource(0, m_pVertexBuffer, 0, sizeof(KWVertex));
	m_pDevice->SetIndices(m_pIndexBuffer);
	m_pDevice->SetFVF(KWVertex::FVF);
	m_pDevice->SetTexture(0, m_pTexture);

//	m_pDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
//	m_pDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);

	m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 
		m_iRowNum * m_iRowNum, 0, 
		(m_iRowNum - 1) * (m_iRowNum - 1) * 2);
}


void KWTerrain::Update( float dt )
{
}
