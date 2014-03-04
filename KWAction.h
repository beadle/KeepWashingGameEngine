#pragma once

#include "KWObject.h"

class KWNode;

class KWAction : public KWObject
{
public:
	SYNTHESIZE_READONLY(KWNode*, m_pTarget, Target);
	SYNTHESIZE_READONLY(bool, m_bFinished, Finished);

public:
	KWAction(void);
	virtual ~KWAction(void);

	virtual void StartWithTarget(KWNode* target);
	virtual void Update(float dt) {};
};

class KWActionMoveTo : public KWAction
{
public:
	SYNTHESIZE_READONLY(D3DXVECTOR3, m_targetPoint, TargetPoint);
	SYNTHESIZE_READONLY(float, m_velocity, Velocity);

public:
	static KWActionMoveTo* Create(const D3DXVECTOR3& targetPoint, float velocity);
	bool Init(const D3DXVECTOR3& targetPoint, float velocity);

	virtual void StartWithTarget(KWNode* target);
	virtual void Update(float dt);
};


class KWTerrain;

class KWActionMoveOnTerrain : public KWAction
{
public:
	SYNTHESIZE_READONLY(D3DXVECTOR2, m_targetPoint, TargetPoint);
	SYNTHESIZE_READONLY(float, m_velocity, Velocity);
	SYNTHESIZE_READONLY(KWTerrain*, m_terrain, Terrain);

public:
	static KWActionMoveOnTerrain* Create(KWTerrain* terrain, const D3DXVECTOR2& targetPoint, float velocity);
	bool Init(KWTerrain* terrain, const D3DXVECTOR2& targetPoint, float velocity);

	virtual void Update(float dt);
};