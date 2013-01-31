#include "stdafx.h"
#include "ViewParticles.h"

#define EXPLOSION
#define LIMIT_DIRECTION

/*!***********************************************************************
 @Function		ViewParticles
 @Access		public 
 @Returns		
 @Description	
*************************************************************************/
ViewParticles::ViewParticles()
	{
	GraphicsSystem* pGfx = GetApp()->GetGraphicsSystem();
	// We'll base our screen coords on Retina iPhone.
	Float32 fW, fH;
	fW = pGfx->GetVirtualWidth()  * 0.5f;
	fH = pGfx->GetVirtualHeight() * 0.5f;;		// Handle iPad.
	m_cam.SetProjection(PVRTMat4::Ortho(-fW, fH, fW, -fH, -1.0f, 1.0f, PVRTMat4::OGL, pGfx->IsRotated()));
	SetCamera(&m_cam);

#ifdef EXPLOSION
	m_Emitter.Initialise(RESMAN->GetTexture("particle-cloud"), enumPARTICLETYPE_Points, 1024);
	m_Emitter2.Initialise(RESMAN->GetTexture("particle"), enumPARTICLETYPE_Quads, 32, 32);
	m_Emitter.Start();
	m_Emitter2.Start();
#endif
#ifdef MUSHROOM
	m_Emitter.Initialise(RESMAN->GetTexture("particle"), enumPARTICLETYPE_Points, 1024);
	//m_Emitter.SetBlendMode(Emitter::enumBLENDMODE_Standard);
	m_Emitter.Start();
#endif
	}

/*!***********************************************************************
 @Function		Render
 @Access		virtual public 
 @Returns		void
 @Description	
*************************************************************************/
void ViewParticles::Render()
	{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

#ifdef EXPLOSION
	m_Emitter.Render();
	m_Emitter2.Render();
#endif
#ifdef MUSHROOM
	m_Emitter.Render();
#endif
	}

#pragma region EXPLOSION_REGION
void Particle_InitExplosionTrails(ParticleDesc* pDesc)
	{
	// Position behind the projectile
	pDesc->m_vPos = PVRTVec3(0.0f, 0.0f, 0.0f);

	// Random direction
#ifdef LIMIT_DIRECTION
	Float32 fRnd	= PVRT_PI_OVER_TWO + (RandomfCentre() * (PVRT_PI_OVER_TWO*0.75f));
#else
	Float32 fRnd    = Randomf() * (PVRT_PI*2);
#endif
	Float32 fRndVel = 400 + (Float32)Randomi(250);
	pDesc->m_vVel = PVRTVec3(cos(fRnd), sin(fRnd), 0.0f) * fRndVel;
	pDesc->m_fRotCur = fRnd;

	// Set properties
	Float32 fRndLife = 0.65f + Randomf()*0.4f;
	pDesc->m_fLife = fRndLife;
	pDesc->m_fDrag = 0.999f;

	pDesc->m_eMode = (enumPARTICLEMODE)(enumPARTICLEMODE_Colour | enumPARTICLEMODE_Scale | enumPARTICLEMODE_Rotate);

	pDesc->m_nNumCols = 4;
	pDesc->m_aRGBA[0].Set(1.0f, 1.0f, 1.0f, 1.0f);
	pDesc->m_aRGBA[1].Set(1.0f, 1.0f, 0.0f, 1.0f);
	pDesc->m_aRGBA[2].Set(1.0f, 0.0f, 0.0f, 1.0f);
	pDesc->m_aRGBA[3].Set(1.0f, 0.0f, 0.0f, 0.0f);

	pDesc->m_nNumScale = 2;
	pDesc->m_aScales[0] = PVRTVec3(10.0f, 0.5f, 1.0f);
	pDesc->m_aScales[1] = PVRTVec3(1.0f, 0.5f, 1.0f);
	}

void Particle_InitExplosion(ParticleDesc* pDesc)
	{
	// Position behind the projectile
	pDesc->m_vPos = PVRTVec3(0.0f, 0.0f, 0.0f);

	// Random direction
#ifdef LIMIT_DIRECTION
	Float32 fRnd	= PVRT_PI_OVER_TWO + (RandomfCentre() * (PVRT_PI_OVER_TWO*0.75f));
#else
	Float32 fRnd    = Randomf() * (PVRT_PI*2);
#endif
	Float32 fRndVel = 150 + (Float32)Randomi(150);
	pDesc->m_vVel = PVRTVec3(cos(fRnd), sin(fRnd), 0.0f) * fRndVel;

	// Set properties
	pDesc->m_fLife = 0.75f;
	pDesc->m_fDrag = 0.999f;

	pDesc->m_eMode = (enumPARTICLEMODE)(enumPARTICLEMODE_Colour | enumPARTICLEMODE_Scale);

	pDesc->m_nNumCols = 4;
	pDesc->m_aRGBA[0].Set(1.0f, 1.0f, 1.0f, 0.5f);
	pDesc->m_aRGBA[1].Set(1.0f, 1.0f, 0.0f, 0.5f);
	pDesc->m_aRGBA[2].Set(0.5f, 0.0f, 0.0f, 0.2f);
	pDesc->m_aRGBA[3].Set(0.25f, 0.0f, 0.0f, 0.0f);

	pDesc->m_nNumScale = 2;
	pDesc->m_aScales[0] = PVRTVec3(0.0f, 0.0f, 1.0f);
	pDesc->m_aScales[1] = PVRTVec3(4.0f, 4.0f, 1.0f);
	}

void Particle_InitExplosionSmoke(ParticleDesc* pDesc)
	{
	// Position behind the projectile
	pDesc->m_vPos = PVRTVec3(0.0f, 0.0f, 0.0f);

	// Random direction
#ifdef LIMIT_DIRECTION
	Float32 fRnd	= PVRT_PI_OVER_TWO + (RandomfCentre() * (PVRT_PI_OVER_TWO*0.75f));
#else
	Float32 fRnd    = Randomf() * (PVRT_PI*2);
#endif
	Float32 fRndVel = 100 + (Float32)Randomi(100);
	pDesc->m_vVel = PVRTVec3(cos(fRnd), sin(fRnd), 0.0f) * fRndVel;

	// Set properties
	pDesc->m_fLife = 1.0f;
	pDesc->m_fDrag = 0.9999f;

	pDesc->m_eMode = (enumPARTICLEMODE)(enumPARTICLEMODE_Colour | enumPARTICLEMODE_Scale);

	pDesc->m_nNumCols = 2;
	pDesc->m_aRGBA[0].Set(1.0f, 1.0f, 1.0f, 0.5f);
	pDesc->m_aRGBA[1].Set(0.0f, 0.0f, 0.0f, 0.0f);

	pDesc->m_nNumScale = 2;
	pDesc->m_aScales[0] = PVRTVec3(0.0f, 0.0f, 1.0f);
	pDesc->m_aScales[1] = PVRTVec3(7.0f, 7.0f, 1.0f);
	}

void Particle_InitExplosionSmokeTrail(ParticleDesc* pDesc, const PVRTVec3& vPos, const PVRTVec3& vVel)
	{
	// Position behind the projectile
	pDesc->m_vPos = vPos;
	pDesc->m_vVel = vVel;

	// Set properties
	pDesc->m_fLife = 1.0f;
	pDesc->m_fDrag = 0.995f;

	pDesc->m_eMode = (enumPARTICLEMODE)(enumPARTICLEMODE_Colour | enumPARTICLEMODE_Scale);

	pDesc->m_nNumCols = 2;
	pDesc->m_aRGBA[0].Set(1.0f, 1.0f, 1.0f, 0.1f);
	pDesc->m_aRGBA[1].Set(0.0f, 0.0f, 0.0f, 0.0f);

	pDesc->m_nNumScale = 2;
	pDesc->m_aScales[0] = PVRTVec3(0.25f, 0.25f, 1.0f);
	pDesc->m_aScales[1] = PVRTVec3(2.0f, 2.0f, 1.0f);
	}
#pragma endregion EXPLOSION_REGION

#pragma region MUSHROOM_REGION
void Particle_InitMushroom(ParticleDesc* pDesc)
	{
	Float32 fRndDir = Randomf() * PVRT_PI*2;
	Float32 fRndHeight = Randomf() * 10.0f;
	pDesc->m_vPos = PVRTVec3(0,0,0);
	pDesc->m_vVel = PVRTVec3(cos(fRndDir) * 20.0f, sin(fRndDir)*fRndHeight, sin(fRndDir) * 20.0f) ;

	// Set properties
	pDesc->m_fLife = 5.0f;
	pDesc->m_fDrag = 1.0f;
	
	pDesc->m_RGBA = RGBA(1,1,1,0.1f);
	pDesc->m_vScale = PVRTVec3(2,2,2);

	pDesc->m_eMode = (enumPARTICLEMODE)(enumPARTICLEMODE_Scale);

	/*pDesc->m_nNumCols = 4;
	pDesc->m_aRGBA[0].Set(1.0f, 1.0f, 1.0f, 0.5f);
	pDesc->m_aRGBA[1].Set(1.0f, 1.0f, 0.0f, 0.5f);
	pDesc->m_aRGBA[2].Set(0.5f, 0.0f, 0.0f, 0.2f);
	pDesc->m_aRGBA[3].Set(0.25f, 0.0f, 0.0f, 0.0f);*/

	pDesc->m_nNumScale = 2;
	pDesc->m_aScales[0] = PVRTVec3(0.0f, 0.0f, 1.0f);
	pDesc->m_aScales[1] = PVRTVec3(1.0f, 1.0f, 1.0f);
	}
#pragma endregion MUSHROOM_REGION

/*!***********************************************************************
 @Function		Update
 @Access		virtual public 
 @Param			double dt
 @Returns		void
 @Description	
*************************************************************************/
void ViewParticles::Update(double dt)
	{
#ifdef EXPLOSION
	m_Emitter.Update(dt);
	m_Emitter2.Update(dt);
#endif
#ifdef MUSHROOM
	m_Emitter.Update(dt);
#endif
	}

/*!***********************************************************************
 @Function		OnEmitterUpdate
 @Access		public 
 @Param			const Particle * pParticle
 @Returns		void
 @Description	
*************************************************************************/
void ViewParticles::OnEmitterUpdate(Emitter* pEmitter, const Particle* pParticleParent, Uint32 uiTicksSinceSpawn)
	{
#ifdef EXPLOSION
	if(uiTicksSinceSpawn > 30)
		{
		Particle p;
		p.Reset();
		p.m_bIsValid = true;
		Particle_InitExplosionSmokeTrail(&p.m_desc, pParticleParent->m_desc.m_vPos, pParticleParent->m_desc.m_vVel);
		pEmitter->AddParticle(p);
		}
#endif
	}

/*!***********************************************************************
 @Function		OnTouchUp
 @Access		virtual public 
 @Param			Touch * pTouches
 @Param			Uint32 uiNum
 @Returns		void
 @Description	
*************************************************************************/
void ViewParticles::OnTouchUp(Touch* pTouches, Uint32 uiNum)
	{
#ifdef MUSHROOM
	const Uint32 uiNumParticles = 512;
	for(Uint32 uiCount = 0; uiCount < uiNumParticles; uiCount++)
		{
		Particle p;
		p.Reset();
		p.m_bIsValid = true;
		Particle_InitMushroom(&p.m_desc);
		m_Emitter.AddParticle(p);
		}
	
#endif
#ifdef EXPLOSION
	const Uint32 uiNumTrails = 32;
	for(Uint32 uiCount = 0; uiCount < uiNumTrails; uiCount++)
		{
		Particle p;
		p.Reset();
		p.m_bIsValid = true;
		Particle_InitExplosionTrails(&p.m_desc);
		m_Emitter2.AddParticle(p);

		Emitter* pEmitter = m_Emitter2.AddEmitter(m_Emitter2.Tail(), this);
		pEmitter->Initialise(RESMAN->GetTexture("particle-cloud2"), enumPARTICLETYPE_Points);
		pEmitter->Start();
		}
	
	const Uint32 uiNumParticles = 16;
	for(Uint32 uiCount = 0; uiCount < uiNumParticles; uiCount++)
		{
		Particle p;
		p.Reset();
		p.m_bIsValid = true;
		Particle_InitExplosion(&p.m_desc);
		m_Emitter.AddParticle(p);
		}

	for(Uint32 uiCount = 0; uiCount < uiNumParticles; uiCount++)
		{
		Particle p;
		p.Reset();
		p.m_bIsValid = true;
		Particle_InitExplosionSmoke(&p.m_desc);
		m_Emitter.AddParticle(p);
		}
#endif
	}
