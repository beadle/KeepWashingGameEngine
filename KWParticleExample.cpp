#include "KWParticleExample.h"


KWParticleSnow* KWParticleSnow::Create( LPDIRECT3DDEVICE9 pDevice, const KWBoundingBox& boundingBox, int numParticles )
{
	KWParticleSnow* snow = new KWParticleSnow;
	if (snow && snow->Init(pDevice, boundingBox, numParticles))
	{
		snow->AutoRelease();
	}
	else
	{
		KW_SAFE_DELETE(snow);
	}
	return snow;
}

bool KWParticleSnow::Init( LPDIRECT3DDEVICE9 pDevice, const KWBoundingBox& boundingBox, int numParticles )
{
	bool ret = KWParticleSystem::Init(pDevice, "snowflake.dds");
	if (!ret) return false;

	m_boundingBox   = boundingBox;
	m_size          = 5.0f;
	m_vbSize        = 2048;
	m_vbOffset      = 0; 
	m_vbBatchSize   = 512; 

	for(int i = 0; i < numParticles; i++)
		AddParticle();

	return true;
}

void KWParticleSnow::ResetParticle( KWParticle* particle )
{
	particle->m_isAlive  = true;

	// get random x, z coordinate for the position of the snow flake.
	d3d::GetRandomVector(
		&particle->m_position,
		&m_boundingBox.m_min,
		&m_boundingBox.m_max);

	// no randomness for height (y-coordinate).  Snow flake
	// always starts at the top of bounding box.
	particle->m_position.y = m_boundingBox.m_max.y; 

	// snow flakes fall downwards and slightly to the left
	particle->m_velocity.x = d3d::GetRandomFloat(0.0f, 1.0f) * -3.0f;
	particle->m_velocity.y = d3d::GetRandomFloat(0.0f, 1.0f) * -10.0f;
	particle->m_velocity.z = 0.0f;

	// white snow flake
	particle->m_color = d3d::WHITE;
}

void KWParticleSnow::Update( float dt )
{
	KWParticleSystem::Update(dt);

	std::list<KWParticle>::iterator i;
	for(i = m_particles.begin(); i != m_particles.end(); i++)
	{
		i->m_position += i->m_velocity * dt;

		// is the point outside bounds?
		if( m_boundingBox.ContainPoint( i->m_position ) == false ) 
		{
			// nope so kill it, but we want to recycle dead 
			// particles, so respawn it instead.
			ResetParticle( &(*i) );
		}
	}
}

//particle fire
KWParticleFire* KWParticleFire::Create( LPDIRECT3DDEVICE9 pDevice, const D3DXVECTOR3& pos, int numParticles )
{
	KWParticleFire* fire = new KWParticleFire;
	if (fire && fire->Init(pDevice, pos, numParticles))
	{
		fire->AutoRelease();
	}
	else
	{
		KW_SAFE_DELETE(fire);
	}
	return fire;
}

bool KWParticleFire::Init( LPDIRECT3DDEVICE9 pDevice, const D3DXVECTOR3& pos, int numParticles )
{
	if (!KWParticleSystem::Init(pDevice, "flare.bmp"))
		return false;

	m_position        = pos;
	m_size          = 2.5f;
	m_vbSize        = 2048;
	m_vbOffset      = 0;   
	m_vbBatchSize   = 512; 

	for(int i = 0; i < numParticles; i++)
		AddParticle();

	return true;
}

void KWParticleFire::ResetParticle( KWParticle* particle )
{
	particle->m_isAlive  = true;
	particle->m_position = m_position;

	D3DXVECTOR3 min = D3DXVECTOR3(-1.0f, -1.0f, -1.0f);
	D3DXVECTOR3 max = D3DXVECTOR3( 1.0f,  1.0f,  1.0f);

	d3d::GetRandomVector(
		&particle->m_velocity,
		&min,
		&max);

	// normalize to make spherical
	D3DXVec3Normalize(
		&particle->m_velocity,
		&particle->m_velocity);

	particle->m_velocity *= 100.0f;

	particle->m_color = D3DXCOLOR(
		d3d::GetRandomFloat(0.0f, 1.0f),
		d3d::GetRandomFloat(0.0f, 1.0f),
		d3d::GetRandomFloat(0.0f, 1.0f),
		1.0f);

	particle->m_age      = 0.0f;
	particle->m_lifeTime = 2.0f; // lives for 2 seconds
}

void KWParticleFire::Update( float dt )
{
	std::list<KWParticle>::iterator i;

	for(i = m_particles.begin(); i != m_particles.end(); i++)
	{
		// only update living particles
		if( i->m_isAlive )
		{
			i->m_position += i->m_velocity * dt;

			i->m_age += dt;

			if(i->m_age > i->m_lifeTime) // kill 
				ResetParticle(&(*i));
				//i->m_isAlive = false;
		}
	}
}

void KWParticleFire::PreDraw()
{
	KWParticleSystem::PreDraw();

	m_pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
	m_pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	// read, but don't write particles to z-buffer
	m_pDevice->SetRenderState(D3DRS_ZWRITEENABLE, false);
}

void KWParticleFire::PostDraw()
{
	KWParticleSystem::PostDraw();

	m_pDevice->SetRenderState(D3DRS_ZWRITEENABLE, true);
}
