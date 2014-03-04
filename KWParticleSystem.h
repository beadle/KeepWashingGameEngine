#pragma once

#include "KWObject.h"
#include "KWMacros.h"
#include "KWNode.h"
#include "D3DUtility.h"

#include <d3d9.h>
#include <d3dx9.h>
#include <list>

using std::list;

struct KWParticle
{
	D3DXVECTOR3 m_position;
	D3DXVECTOR3 m_velocity;
	D3DXVECTOR3 m_acceleration;
	float m_lifeTime;
	float m_age;
	D3DCOLOR m_color;
	D3DCOLOR m_colorFade;
	bool m_isAlive;
};

struct ParticleVertex
{
	D3DXVECTOR3 m_position;
	D3DCOLOR m_color;
	static const DWORD FVF;
};

class KWParticleSystem : public KWNode
{
protected:
	SYNTHESIZE(float, m_size, ParticleSize);
	SYNTHESIZE(float, m_emitRate, EmitRate);

	SYNTHESIZE(int, m_maxParticles, MaxParticles);
	SYNTHESIZE(KWBoundingBox, m_boundingBox, BoundingBox);


	LPDIRECT3DTEXTURE9 m_tex;
	LPDIRECT3DVERTEXBUFFER9 m_vb;

	list<KWParticle> m_particles;

	DWORD m_vbSize;
	DWORD m_vbOffset;
	DWORD m_vbBatchSize;

public:
	KWParticleSystem(void);
	virtual ~KWParticleSystem(void);

	static KWParticleSystem* Create(LPDIRECT3DDEVICE9 pDevice, const char* texFileName);
	virtual bool Init(LPDIRECT3DDEVICE9 pDevice, const char* texFileName);
	virtual void Reset();
	virtual void ReleaseSystem();

	virtual void ResetParticle(KWParticle* particle);
	virtual void AddParticle();

	virtual void Update(float dt);

	virtual void PreDraw();
	virtual void Draw();
	virtual void PostDraw();

	bool isEmpty();
	bool isDead();

protected:
	void RemoveDeadParticles();

};

