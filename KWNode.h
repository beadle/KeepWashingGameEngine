#pragma once

#include <d3d9.h>
#include <d3dx9.h>

#include "KWObject.h"
#include "KWMacros.h"
#include "KWArray.h"
#include "KWGeometry.h"
#include "KWMesh.h"
#include "KWActionManager.h"

class KWNode : public KWObject
{
protected:
	D3DXMATRIX m_matrix;

	SYNTHESIZE_READONLY(D3DXVECTOR3, m_position, Position);
	SYNTHESIZE_READONLY(D3DXVECTOR3, m_rotation, Rotation);
	SYNTHESIZE_READONLY(D3DXVECTOR3, m_scaling, Scaling);

	LPDIRECT3DDEVICE9 m_pDevice;

	KWArray* m_pChildrenNodes;

	SYNTHESIZE(bool, m_shouldDraw, ShouldDraw);
	SYNTHESIZE(bool, m_hadDraw, HadDraw);
	SYNTHESIZE(bool, m_isOctTreeNode, IsOctTreeNode);

protected:
	SYNTHESIZE_RETAIN(KWMesh*, m_pMesh, Mesh);
	SYNTHESIZE(KWNode*, m_pFatherNode, Father);
	SYNTHESIZE_READONLY(KWActionManager*, m_pActionManager, ActionManager);

public:
	KWNode(void);
	virtual ~KWNode(void);

	static KWNode* Create();

	//matrix
	void CalculateNodeToParentTransform();

	D3DXMATRIX NodeToParentTransform();
	D3DXMATRIX ParentToNodeTransform();
	D3DXMATRIX NodeToWorldTransform();
	D3DXMATRIX WorldToNodeTransform();
	

	void SetTranslation(const D3DXVECTOR3& trans);
	void SetRotationX(float rad);
	void SetRotationY(float rad);
	void SetRotationZ(float rad);
	void SetScaling(float fac);

	void Translate(const D3DXVECTOR3& trans);
	void RotateX(float rad);
	void RotateY(float rad);
	void RotateZ(float rad);
	void Scale(float fac);

	//geters and seters
	virtual KWBoundingBox		GetBoundingBox();
	virtual KWBoundingShpere	GetBoundingShpere();
	KWBoundingBox				GetWorldBoundingBox();
	KWBoundingShpere			GetWorldBoundingShpere();

	//convertions
	D3DXVECTOR3 ConvertToWorldSpace(const D3DXVECTOR3& pos);
	D3DXVECTOR3 ConvertToNodeSpace(const D3DXVECTOR3& pos);

	//child
	void AddChild(KWNode* node);
	void RemoveChild(KWNode* node);

	//action
	void RunAction(KWAction* pAction);
	void RemoveAllActions();

	//interface
	virtual void Update(float dt);
	virtual void Draw();
	virtual void ResetDrawTag();

	virtual KWNode* GetPickingNode(const KWRay& pickingRay);
	virtual bool IsPicking(const KWRay& pickingRay);
};

