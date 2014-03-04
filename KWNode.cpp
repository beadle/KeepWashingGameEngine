#include "KWNode.h"
#include "KWEngine.h"

KWNode::KWNode(void)
{
	ZeroMemory(m_position, sizeof(D3DXVECTOR3));
	ZeroMemory(m_rotation, sizeof(D3DXVECTOR3));
	m_scaling = D3DXVECTOR3(1.0f, 1.0f, 1.0f);

	m_pDevice = NULL;
	m_pFatherNode = NULL;
	m_pChildrenNodes = KWArray::Create();
	m_pChildrenNodes->Retain();
	m_pMesh = NULL;

	m_pActionManager = KWActionManager::Create(this);
	m_pActionManager->Retain();

//	m_isOctTreeNode = true;
	m_isOctTreeNode = false;
	CalculateNodeToParentTransform();
}


KWNode::~KWNode(void)
{
	KW_SAFE_RELEASE(m_pChildrenNodes);
	KW_SAFE_RELEASE(m_pActionManager);
	KW_SAFE_RELEASE(m_pMesh);
}

KWNode* KWNode::Create()
{
	KWNode* node = new KWNode;
	node->m_pDevice = KWEngine::sharedInstance()->getDevice();
	node->AutoRelease();
	return node;
}

void KWNode::Update( float dt )
{
	//Update all its children
	for (int i=0; i<m_pChildrenNodes->Size(); ++i)
	{
		KWNode* childNode = (KWNode*)m_pChildrenNodes->ObjectAtIndex(i);
		childNode->Update(dt);
	}

	//Update itself
	m_pActionManager->Update(dt);
}

void KWNode::Draw()
{
	//draw all its children
	for (int i=0; i<m_pChildrenNodes->Size(); ++i)
	{
		KWNode* childNode = (KWNode*)m_pChildrenNodes->ObjectAtIndex(i);
		childNode->Draw();
	}

	if(!m_shouldDraw) return;
	if(m_hadDraw) return;
	//draw itself
	
	D3DXMATRIX matReal = NodeToWorldTransform();
	m_pDevice->SetTransform(D3DTS_WORLD, &matReal);
	if (m_pMesh){
		m_pMesh->Draw();
		m_hadDraw = true;
	} 
}

void KWNode::ResetDrawTag()
{
	//draw all its children
	for (int i=0; i<m_pChildrenNodes->Size(); ++i)
	{
		KWNode* childNode = (KWNode*)m_pChildrenNodes->ObjectAtIndex(i);
		childNode->ResetDrawTag();
	}

	m_hadDraw = false;
	if (m_isOctTreeNode)
		m_shouldDraw = false;
	else m_shouldDraw = true;
}

void KWNode::SetTranslation( const D3DXVECTOR3& trans )
{
	m_position = trans;
	CalculateNodeToParentTransform();
}

void KWNode::SetRotationX( float rad )
{
	m_rotation.x = rad;
	CalculateNodeToParentTransform();
}

void KWNode::SetRotationY( float rad )
{
	m_rotation.y = rad;
	CalculateNodeToParentTransform();
}

void KWNode::SetRotationZ( float rad )
{
	m_rotation.z = rad;
	CalculateNodeToParentTransform();
}

void KWNode::SetScaling( float fac )
{
	m_scaling.x = m_scaling.y = m_scaling.z = fac;
	CalculateNodeToParentTransform();
}

void KWNode::Translate( const D3DXVECTOR3& trans )
{
	m_position += trans;
	CalculateNodeToParentTransform();
}

void KWNode::RotateX( float rad )
{
	m_rotation.x += rad;
	CalculateNodeToParentTransform();
}

void KWNode::RotateY( float rad )
{
	m_rotation.y += rad;
	CalculateNodeToParentTransform();
}

void KWNode::RotateZ( float rad )
{
	m_rotation.z += rad;
	CalculateNodeToParentTransform();
}

void KWNode::Scale( float fac )
{
	m_scaling.x += fac;
	m_scaling.y += fac;
	m_scaling.z += fac;
	CalculateNodeToParentTransform();
}

void KWNode::CalculateNodeToParentTransform()
{
	D3DXMatrixScaling(&m_matrix, 1.0f, 1.0f, 1.0f);
	D3DXMATRIX mat;
	D3DXMatrixScaling(&mat, m_scaling.x, m_scaling.y, m_scaling.z);
	D3DXMatrixMultiply(&m_matrix, &m_matrix, &mat);
	D3DXMatrixRotationX(&mat, m_rotation.x);
	D3DXMatrixMultiply(&m_matrix, &m_matrix, &mat);
	D3DXMatrixRotationY(&mat, m_rotation.y);
	D3DXMatrixMultiply(&m_matrix, &m_matrix, &mat);
	D3DXMatrixRotationZ(&mat, m_rotation.z);
	D3DXMatrixMultiply(&m_matrix, &m_matrix, &mat);
	D3DXMatrixTranslation(&mat, m_position.x, m_position.y, m_position.z);
	D3DXMatrixMultiply(&m_matrix, &m_matrix, &mat);
}

void KWNode::AddChild( KWNode* node )
{
	m_pChildrenNodes->AddObject(node);
	node->setFather(this);
}

void KWNode::RemoveChild( KWNode* node )
{
	m_pChildrenNodes->RemoveObject(node);
	node->setFather(NULL);
}

void KWNode::RunAction( KWAction* pAction )
{
	m_pActionManager->AddAction(pAction);
}

void KWNode::RemoveAllActions()
{
	m_pActionManager->RemoveAllActions();
}

bool KWNode::IsPicking( const KWRay& pickingRay )
{
	if (!m_pMesh) return false;

	KWBoundingBox aabb = GetWorldBoundingBox();
	return aabb.IntersectWithRay(pickingRay);

// 	KWBoundingShpere boundingShpere = GetWorldBoundingShpere();
// 	if(boundingShpere.IntersectWithRay(pickingRay))
// 		return true;
// 	else return false;
}
KWNode* KWNode::GetPickingNode(const KWRay& pickingRay)
{
	for (int i=0; i<m_pChildrenNodes->Size(); ++i)
	{
		KWNode* node = (KWNode*)m_pChildrenNodes->ObjectAtIndex(i);
		KWNode* pickingNode = node->GetPickingNode(pickingRay);
		if (pickingNode) return pickingNode;
	}

	if(IsPicking(pickingRay)) 
		return this;
	else return NULL;
}

KWBoundingBox KWNode::GetBoundingBox()
{
	return m_pMesh->GetBoundingBox();
}

KWBoundingShpere KWNode::GetBoundingShpere()
{
	return m_pMesh->GetBoundingShpere();
}

KWBoundingBox KWNode::GetWorldBoundingBox()
{
	KWBoundingBox box = GetBoundingBox();/*m_pMesh->GetBoundingBox();*/
	D3DXMATRIX mat = NodeToWorldTransform();
	D3DXVec3TransformCoord(&box.m_min, &box.m_min, &mat);
	D3DXVec3TransformCoord(&box.m_max, &box.m_max, &mat);
	return box;
}

KWBoundingShpere KWNode::GetWorldBoundingShpere()
{
	KWBoundingShpere sphere = GetBoundingShpere();/*m_pMesh->GetBoundingShpere();*/
	D3DXMATRIX mat = NodeToWorldTransform();
	D3DXVec3TransformCoord(&sphere.m_center, &sphere.m_center, &mat);
	float realScale = m_scaling.x;
	KWNode* node = m_pFatherNode;
	while (node)
	{
		realScale *= node->getScaling().x;
		node = node->getFather();
	}
	sphere.m_radius *= realScale;
	return sphere;
}

D3DXVECTOR3 KWNode::ConvertToWorldSpace( const D3DXVECTOR3& pos )
{
	D3DXVECTOR3 worldPos;
	D3DXMATRIX matWorld = NodeToWorldTransform();
	D3DXVec3TransformCoord(&worldPos, &pos, &matWorld);
	return worldPos;
}

D3DXVECTOR3 KWNode::ConvertToNodeSpace( const D3DXVECTOR3& pos )
{
	D3DXVECTOR3 worldPos;
	D3DXMATRIX matWorld = WorldToNodeTransform();
	D3DXVec3TransformCoord(&worldPos, &pos, &matWorld);
	return worldPos;
}

D3DXMATRIX KWNode::NodeToParentTransform()
{
	return m_matrix;
}

D3DXMATRIX KWNode::ParentToNodeTransform()
{
	D3DXMATRIX mat;
	D3DXMatrixInverse(&mat, NULL, &NodeToParentTransform());	
	return mat;
}

D3DXMATRIX KWNode::NodeToWorldTransform()
{
	D3DXMATRIX mat = m_matrix;
	KWNode* node = m_pFatherNode;
	while (node)
	{
		D3DXMatrixMultiply(&mat, &mat, &(node->NodeToParentTransform()));
		node = node->getFather();
	}
	return mat;
}

D3DXMATRIX KWNode::WorldToNodeTransform()
{
	D3DXMATRIX mat;
	D3DXMatrixInverse(&mat, NULL, &NodeToWorldTransform());	
	return mat;
}

