#include "KWParticleSystem.h"


const DWORD ParticleVertex::FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE;


KWParticleSystem::KWParticleSystem(void)
{
	m_vb = NULL;
	m_tex = NULL;

	m_vbSize = 2000;
}


KWParticleSystem::~KWParticleSystem(void)
{
	ReleaseSystem();
}

KWParticleSystem* KWParticleSystem::Create( LPDIRECT3DDEVICE9 pDevice, const char* texFileName )
{
	KWParticleSystem* sys = new KWParticleSystem;
	if (sys && sys->Init(pDevice, texFileName))
	{
		sys->AutoRelease();
	}
	else
	{
		KW_SAFE_DELETE(sys);
	}
	return sys;
}

bool KWParticleSystem::Init( LPDIRECT3DDEVICE9 pDevice, const char* texFileName )
{
	m_pDevice = pDevice;
	HRESULT hr = D3DXCreateTextureFromFileA(m_pDevice, texFileName, &m_tex);

	if (FAILED(hr)) return false;

	hr = m_pDevice->CreateVertexBuffer(
		m_vbSize * sizeof(KWParticle),
		D3DUSAGE_DYNAMIC | D3DUSAGE_POINTS | D3DUSAGE_WRITEONLY,
		ParticleVertex::FVF,
		D3DPOOL_DEFAULT,
		&m_vb,
		0);

	if (FAILED(hr)) return false;

	return true;
}

void KWParticleSystem::ReleaseSystem()
{
	COM_SAFE_RELEASE_NULL(m_tex);
	COM_SAFE_RELEASE_NULL(m_vb);
}

void KWParticleSystem::Reset()
{
	list<KWParticle>::iterator it;
	for (it = m_particles.begin(); it != m_particles.end(); ++it)
	{
		ResetParticle(&(*it));
	}
}

void KWParticleSystem::ResetParticle( KWParticle* particle )
{
	

}

void KWParticleSystem::AddParticle()
{
	KWParticle particle;
	ResetParticle(&particle);
	m_particles.push_back(particle);
}

void KWParticleSystem::Update( float dt )
{
	KWNode::Update(dt);
}

void KWParticleSystem::PreDraw()
{
	m_pDevice->SetRenderState(D3DRS_LIGHTING, false);
	m_pDevice->SetRenderState(D3DRS_POINTSPRITEENABLE, true);
	m_pDevice->SetRenderState(D3DRS_POINTSCALEENABLE, true); 
	m_pDevice->SetRenderState(D3DRS_POINTSIZE, d3d::FtoDw(m_size));
	m_pDevice->SetRenderState(D3DRS_POINTSIZE_MIN, d3d::FtoDw(0.0f));

	// control the size of the particle relative to distance
	m_pDevice->SetRenderState(D3DRS_POINTSCALE_A, d3d::FtoDw(0.0f));
	m_pDevice->SetRenderState(D3DRS_POINTSCALE_B, d3d::FtoDw(0.0f));
	m_pDevice->SetRenderState(D3DRS_POINTSCALE_C, d3d::FtoDw(1.0f));

	// use alpha from texture
	m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);

	m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	m_pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
}

void KWParticleSystem::Draw()
{
	KWNode::Draw();
	//
	// Remarks:  The render method works by filling a section of the vertex buffer with data,
	//           then we render that section.  While that section is rendering we lock a new
	//           section and begin to fill that section.  Once that sections filled we render it.
	//           This process continues until all the particles have been drawn.  The benifit
	//           of this method is that we keep the video card and the CPU busy.  

	if( !m_particles.empty() )
	{
		//
		// set render states
		//

		PreDraw();

		m_pDevice->SetTexture(0, m_tex);
		m_pDevice->SetFVF(ParticleVertex::FVF);
		m_pDevice->SetStreamSource(0, m_vb, 0, sizeof(ParticleVertex));

		//
		// render batches one by one
		//

		// start at beginning if we're at the end of the vb
		if(m_vbOffset >= m_vbSize)
			m_vbOffset = 0;

		ParticleVertex* v = 0;

		m_vb->Lock(
			m_vbOffset    * sizeof( ParticleVertex ),
			m_vbBatchSize * sizeof( ParticleVertex ),
			(void**)&v,
			m_vbOffset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD);

		DWORD numParticlesInBatch = 0;

		//
		// Until all particles have been rendered.
		//
		std::list<KWParticle>::iterator i;
		for(i = m_particles.begin(); i != m_particles.end(); i++)
		{
			if( i->m_isAlive )
			{
				//
				// Copy a batch of the living particles to the
				// next vertex buffer segment
				//
				v->m_position = i->m_position;
				v->m_color    = (D3DCOLOR)i->m_color;
				v++; // next element;

				numParticlesInBatch++; //increase batch counter

				// if this batch full?
				if(numParticlesInBatch == m_vbBatchSize) 
				{
					//
					// Draw the last batch of particles that was
					// copied to the vertex buffer. 
					//
					m_vb->Unlock();

					m_pDevice->DrawPrimitive(
						D3DPT_POINTLIST,
						m_vbOffset,
						m_vbBatchSize);

					//
					// While that batch is drawing, start filling the
					// next batch with particles.
					//

					// move the offset to the start of the next batch
					m_vbOffset += m_vbBatchSize; 

					// don't offset into memory thats outside the vb's range.
					// If we're at the end, start at the beginning.
					if(m_vbOffset >= m_vbSize) 
						m_vbOffset = 0;       

					m_vb->Lock(
						m_vbOffset    * sizeof( ParticleVertex ),
						m_vbBatchSize * sizeof( ParticleVertex ),
						(void**)&v,
						m_vbOffset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD);

					numParticlesInBatch = 0; // reset for new batch
				}	
			}
		}

		m_vb->Unlock();

		// its possible that the LAST batch being filled never 
		// got rendered because the condition 
		// (numParticlesInBatch == _vbBatchSize) would not have
		// been satisfied.  We draw the last partially filled batch now.

		if( numParticlesInBatch )
		{
			m_pDevice->DrawPrimitive(
				D3DPT_POINTLIST,
				m_vbOffset,
				numParticlesInBatch);
		}

		// next block
		m_vbOffset += m_vbBatchSize; 

		//
		// reset render states
		//

		PostDraw();
	}
}

void KWParticleSystem::PostDraw()
{
	m_pDevice->SetRenderState(D3DRS_LIGHTING,          true);
	m_pDevice->SetRenderState(D3DRS_POINTSPRITEENABLE, false);
	m_pDevice->SetRenderState(D3DRS_POINTSCALEENABLE,  false);
	m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,  false);
}

bool KWParticleSystem::isEmpty()
{
	return m_particles.empty();
}

bool KWParticleSystem::isDead()
{
	std::list<KWParticle>::iterator i;
	for(i = m_particles.begin(); i != m_particles.end(); i++)
	{
		// is there at least one living particle?  If yes,
		// the system is not dead.
		if( i->m_isAlive )
			return false;
	}
	// no living particles found, the system must be dead.
	return true;
}

void KWParticleSystem::RemoveDeadParticles()
{
	std::list<KWParticle>::iterator i;

	i = m_particles.begin();

	while( i != m_particles.end() )
	{
		if( i->m_isAlive == false )
		{
			// erase returns the next iterator, so no need to
			// incrememnt to the next one ourselves.
			i = m_particles.erase(i); 
		}
		else
		{
			i++; // next in list
		}
	}
}
