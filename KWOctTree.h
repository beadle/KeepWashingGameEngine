#pragma once


#include <vector>

using std::vector;

#include "KWGeometry.h"
#include "KWObject.h"
#include "KWArray.h"

typedef enum
{
	OctTree_000,
	OctTree_001,
	OctTree_101,
	OctTree_100,
	OctTree_010,
	OctTree_011,
	OctTree_111,
	OctTree_110,

	OctTree_NodeCount,
}OctTreeNodePosition;

class KWOctTreeNode : public KWObject
{
public:
	KWOctTreeNode*	pFather;
	KWOctTreeNode*	pChildren[OctTree_NodeCount];
	KWArray*		pNodes;
	KWBoundingBox	aabb;
	int		LayerNum;
	int		MaxLayerNum;
	bool	IsDivided;

public:
	KWOctTreeNode(void);
	~KWOctTreeNode(void);

	void			Divide();
	void			ReleaseNode();
	void			UpdateDrawTag(const KWQuadPyramid& pyramid);
	KWBoundingBox	GetChildBoundingBox(OctTreeNodePosition child_pos);
	

};

class KWOctTree : public KWObject
{
public:
	int MaxLayerNum;
	KWOctTreeNode* Root;

public:
	KWOctTree(void);
	~KWOctTree(void);

	static KWOctTree* Create();

	void ConstructTree(KWArray* nodes, float sideLength);
	void ReleaseTree();
	void TagVisibleNodes(const KWQuadPyramid& pyramid);
	
};

