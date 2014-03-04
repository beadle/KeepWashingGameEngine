#include "KWOctTree.h"
#include "KWNode.h"

KWOctTree::KWOctTree(void)
{
	MaxLayerNum = 2;
	Root = NULL;
}


KWOctTree::~KWOctTree(void)
{
	ReleaseTree();
}

KWOctTree* KWOctTree::Create()
{
	KWOctTree* tree = new KWOctTree;
	tree->AutoRelease();
	return tree;
}

void KWOctTree::ConstructTree( KWArray* nodes, float sideLength )
{
	ReleaseTree();
	Root = new KWOctTreeNode;
	Root->pFather = NULL;
	Root->aabb.m_min = D3DXVECTOR3(-sideLength, -sideLength, -sideLength);
	Root->aabb.m_max = D3DXVECTOR3(sideLength, sideLength, sideLength);
	Root->LayerNum = 0;
	Root->MaxLayerNum = MaxLayerNum;
	for (int i=0; i<nodes->Size(); ++i)
	{
		KWNode* node = (KWNode*)nodes->ObjectAtIndex(i);
		if (node->getIsOctTreeNode())
		{
			Root->pNodes->AddObject(node);
		}
	}
	Root->Divide();
}

void KWOctTree::ReleaseTree()
{
	KW_SAFE_RELEASE_NULL(Root);
}

void KWOctTree::TagVisibleNodes( const KWQuadPyramid& pyramid )
{
	Root->UpdateDrawTag(pyramid);
}

KWOctTreeNode::KWOctTreeNode( void )
{
	pFather = NULL;
	ZeroMemory(pChildren, sizeof(pChildren));
	pNodes = KWArray::Create();
	pNodes->Retain();
	IsDivided = false;
	LayerNum = 0;
	MaxLayerNum = 3;
}

KWOctTreeNode::~KWOctTreeNode( void )
{
	ReleaseNode();
	KW_SAFE_RELEASE(pNodes);
}

void KWOctTreeNode::Divide()
{
	if (pNodes->Size() == 0 || LayerNum >= MaxLayerNum)
	{
		IsDivided = false;
		return;
	}

	IsDivided = true;

	for (int i=0; i<OctTree_NodeCount; ++i)
	{
		OctTreeNodePosition childPosition = (OctTreeNodePosition)i;
		KWOctTreeNode* node = new KWOctTreeNode;
// 		if (pChildren[childPosition] == NULL)
// 			node = new KWOctTreeNode;
// 		else node = pChildren[childPosition];

		KW_SAFE_RELEASE(pChildren[i]);
		pChildren[i] = node;
		node->pFather = this;
		node->aabb = GetChildBoundingBox(childPosition);
		node->LayerNum = LayerNum + 1;
		node->MaxLayerNum = MaxLayerNum;
		
		for (int j=0; j<pNodes->Size(); ++j)
		{
			KWNode* n = (KWNode*)pNodes->ObjectAtIndex(j);
			KWBoundingBox box = n->GetWorldBoundingBox();
			if (node->aabb.IntersectWithBox(box))
				node->pNodes->AddObject(n);
		}

		node->Divide();
	}
}

KWBoundingBox KWOctTreeNode::GetChildBoundingBox( OctTreeNodePosition child_pos )
{
	KWBoundingBox box = aabb;
	D3DXVECTOR3 aabbcenter = aabb.GetCenter();
	switch(child_pos)
	{
	case OctTree_000:
		box.m_max = aabbcenter;
		break;
	case OctTree_001:
		box.m_min.z = aabbcenter.z;
		box.m_max = aabbcenter;
		box.m_max.z = aabb.m_max.z;
		break;
	case OctTree_101:
		box.m_min = aabbcenter;
		box.m_min.y = aabb.m_min.y;
		box.m_max.y = aabbcenter.y;
		break;
	case OctTree_100:
		box.m_min.x = aabbcenter.x;
		box.m_max = aabbcenter;
		box.m_max.x = aabb.m_max.x;
		break;
	case OctTree_010:
		box.m_min.y = aabbcenter.y;
		box.m_max = aabbcenter;
		box.m_max.y = aabb.m_max.y;
		break;
	case OctTree_011:
		box.m_min = aabbcenter;
		box.m_min.x = aabb.m_min.x;
		box.m_max.x = aabbcenter.x;
		break;
	case OctTree_111:
		box.m_min = aabbcenter;
		break;
	case OctTree_110:
		box.m_min = aabbcenter;
		box.m_min.z = aabb.m_min.z;
		box.m_max.z = aabbcenter.z;
		break;
	}
	return box;
}

void KWOctTreeNode::UpdateDrawTag( const KWQuadPyramid& pyramid )
{
	if (!pyramid.IntersectWithBox(aabb)) return;

	//draw current nodes
	for (int i=0; i<pNodes->Size(); ++i)
	{
		KWNode* node = (KWNode*)pNodes->ObjectAtIndex(i);
		if (pyramid.IntersectWithBox(node->GetWorldBoundingBox()))
		{
			node->setShouldDraw(true);
		}
	}
	
	if (!IsDivided) return;

	//update childs draw tag
	for (int i=0; i<OctTree_NodeCount; ++i)
	{
		pChildren[i]->UpdateDrawTag(pyramid);
	}
}

void KWOctTreeNode::ReleaseNode()
{
	//KW_SAFE_RELEASE(pNodes);
	pNodes->Clear();
	for (int i=0; i<OctTree_NodeCount; ++i)
	{
		if (pChildren[i])
		{
			pChildren[i]->ReleaseNode();
			pChildren[i]->Release();
			pChildren[i] = NULL;
		}
	}
}
