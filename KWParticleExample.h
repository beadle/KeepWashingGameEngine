#pragma once

#include "KWParticleSystem.h"


//particle snow
class KWParticleSnow : public KWParticleSystem
{
public:
	static KWParticleSnow* Create(LPDIRECT3DDEVICE9 pDevice, const KWBoundingBox& boundingBox, int numParticles);
	virtual bool Init(LPDIRECT3DDEVICE9 pDevice, const KWBoundingBox& boundingBox, int numParticles);

	void ResetParticle(KWParticle* particle);
	void Update(float dt);
};

//particle fire
class KWParticleFire : public KWParticleSystem
{
public:
	static KWParticleFire* Create(LPDIRECT3DDEVICE9 pDevice, const D3DXVECTOR3& pos, int numParticles);
	virtual bool Init(LPDIRECT3DDEVICE9 pDevice, const D3DXVECTOR3& pos, int numParticles);

	void ResetParticle(KWParticle* particle);
	void Update(float dt);
	void PreDraw();
	void PostDraw();
};