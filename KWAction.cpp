#include "KWAction.h"
#include "KWNode.h"
#include "KWTerrain.h"

#include <math.h>


KWAction::KWAction(void)
{
	m_pTarget = NULL;
	m_bFinished = false;
}


KWAction::~KWAction(void)
{
}

void KWAction::StartWithTarget( KWNode* target )
{
	m_pTarget = target;
}

KWActionMoveTo* KWActionMoveTo::Create( const D3DXVECTOR3& targetPoint, float velocity )
{
	KWActionMoveTo* moveTo = new KWActionMoveTo;
	if (moveTo && moveTo->Init(targetPoint, velocity))
	{
		moveTo->AutoRelease();
	}
	else
	{
		KW_SAFE_DELETE(moveTo);
	}
	return moveTo;
}

bool KWActionMoveTo::Init( const D3DXVECTOR3& targetPoint, float velocity )
{
	m_targetPoint = targetPoint;
	m_velocity = velocity;

	return true;
}

void KWActionMoveTo::StartWithTarget( KWNode* target )
{
	KWAction::StartWithTarget(target);


}

void KWActionMoveTo::Update( float dt )
{
	D3DXVECTOR3 currPos = m_pTarget->getPosition();

	float dist = D3DXVec3Length(&(currPos-m_targetPoint));
	if (dist < 1.0f)
	{
		m_bFinished = true;
		return;
	}

	D3DXVECTOR3 delta = m_targetPoint - currPos;
	D3DXVec3Normalize(&delta, &delta);
	delta *= m_velocity * dt;
	m_pTarget->SetTranslation(currPos+delta);
}



KWActionMoveOnTerrain* KWActionMoveOnTerrain::Create( KWTerrain* terrain, const D3DXVECTOR2& targetPoint, float velocity )
{
	KWActionMoveOnTerrain* ac = new KWActionMoveOnTerrain;
	ac->Init(terrain, targetPoint, velocity);
	ac->AutoRelease();
	return ac;
}

bool KWActionMoveOnTerrain::Init( KWTerrain* terrain, const D3DXVECTOR2& targetPoint, float velocity )
{
	m_targetPoint = targetPoint;
	m_velocity = velocity;
	m_terrain = terrain;

	return true;
}

void KWActionMoveOnTerrain::Update( float dt )
{
	D3DXVECTOR3 pos = m_pTarget->getPosition();
	D3DXVECTOR2 xzPos(pos.x, pos.z);
	D3DXVECTOR2 delta = m_targetPoint - xzPos;

	if (D3DXVec2LengthSq(&delta) < 1.0f)
	{
		m_bFinished = true;
		return;
	}

	D3DXVec2Normalize(&delta, &delta);
	delta *= m_velocity * dt;
	xzPos += delta;
	float y = m_terrain->GetHeight(xzPos.x, xzPos.y) + m_pTarget->GetWorldBoundingBox().GetHeight() / 2;
	pos.x = xzPos.x;
	pos.z = xzPos.y;
	pos.y = y;
	m_pTarget->SetTranslation(pos);
	//m_pTarget->SetRotationY(D3DX_PI+atan2f(delta.y, delta.x));
}
