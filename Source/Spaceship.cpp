#include "stdafx.h"
#include "Spaceship.h"
#include "Projectile.h"

const Uint32 c_uiMVP = HashMap_Hash("mxMVP");

void Spaceship_Particle_InitExplosionTrails(ParticleDesc* pDesc, const PVRTVec3& vPos);
void Spaceship_Particle_InitExplosion(ParticleDesc* pDesc, const PVRTVec3& vPos);
void Spaceship_Particle_InitExplosionSmoke(ParticleDesc* pDesc, const PVRTVec3& vPos);
void Spaceship_Particle_InitExplosionSmokeTrail(ParticleDesc* pDesc, const PVRTVec3& vPos, const PVRTVec3& vVel);

Spaceship::Spaceship() : m_tex(0), m_pProjectile(NULL), m_eState(enumSTATE_OK), m_pColBitmap(NULL)
	{
	m_shader = RESMAN->GetShader("MVP_V_T_C");

	// Create a projectile
	m_pProjectile = new Projectile;

	// Initialise emitters ready for explosion
	m_Emitter.Initialise(RESMAN->GetTexture("particle-cloud"), enumPARTICLETYPE_Points, 32);
	m_Emitter2.Initialise(RESMAN->GetTexture("particle"), enumPARTICLETYPE_Quads, 32, 32);

	Bitmap bm;
	bm.CreateFromImage("spaceship-collision.tga", enumIMAGETYPE_TGA);
	m_pColBitmap = bm.CreateCollisionBitmap();
	}

Spaceship::~Spaceship()
	{
	delete m_pProjectile;
	delete m_pColBitmap;
	}

/*!***********************************************************************
 @Function		Initialise
 @Access		public 
 @Param			const PVRTVec3 & vPosition
 @Returns		void
 @Description	
*************************************************************************/
void Spaceship::Initialise(const PVRTVec3& vPosition, const RGBA& c_rgba)
	{
	m_rgbaCol = c_rgba;
	m_tex			= RESMAN->GetTexture("spaceship");
	m_texExpDisc	= RESMAN->GetTexture("explosiondisc");
	m_vPos = vPosition;

	m_quad.m_fX = m_vPos.x - m_tex->GetWidth()  * 0.5f;
	m_quad.m_fY = m_vPos.y - m_tex->GetHeight() * 0.5f;
	m_quad.m_fW = m_vPos.x + m_tex->GetWidth()  * 0.5f;
	m_quad.m_fH = m_vPos.y + m_tex->GetHeight()  * 0.5f;

	m_bb.m_fX = m_vPos.x - 31.0f;
	m_bb.m_fY = m_vPos.y - 9.0f;
	m_bb.m_fW = m_vPos.x + 31.0f;
	m_bb.m_fH = m_vPos.y + 12.0f;
	}

/*!***********************************************************************
 @Function		Render
 @Access		public 
 @Returns		void
 @Description	
*************************************************************************/
void Spaceship::Render()
	{
	m_pProjectile->DrawGeom();
		
	Shader::Use(m_shader);	
	Camera* pCam = VIEWMAN->GetActiveView()->GetCamera();
	PVRTMat4 mxMVP = pCam->GetProjection() * pCam->GetView();
	glUniformMatrix4fv(m_shader->GetUniform(c_uiMVP), 1, GL_FALSE, mxMVP.f);

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);

	if(m_eState == enumSTATE_OK)
		{
		glBindTexture(GL_TEXTURE_2D, m_tex->GetHandle());
		GFX->RenderQuad(m_quad, PVRTMat4::Identity(), FLAG_VRT | FLAG_TEX0 | FLAG_RGB, m_rgbaCol);
		}

	if(m_ExplosionDisc.IsActive())
		{
		PVRTMat4 mxScale = PVRTMat4::Translation(m_vPos) * PVRTMat4::Scale(m_ExplosionDisc.Value(), m_ExplosionDisc.Value(), 1.0f);
		GFX->RenderQuad(m_texExpDisc, mxScale, RGBA(1.0f, 1.0f, 1.0f, 1.0f-(m_ExplosionDisc.Value()*0.1f)));
		}

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	m_Emitter.Render();
	m_Emitter2.Render();

	Shader::Use(0);
	}

/*!***********************************************************************
 @Function		Update
 @Access		public 
 @Param			double fDT
 @Returns		void
 @Description	
*************************************************************************/
void Spaceship::Update(double fDT)
	{
	m_Emitter.Update(fDT);
	m_Emitter2.Update(fDT);

	if(m_ExplosionDisc.IsActive())
		m_ExplosionDisc.Update(fDT);

	if(m_Emitter.IsActive() && m_Emitter2.IsActive() && m_eState == enumSTATE_Hit &&
	   m_Emitter.ActiveParticles() == 0 && m_Emitter2.ActiveParticles() == 0)
		m_eState = enumSTATE_Destroyed;
	}

/*!***********************************************************************
 @Function		OnEmitterUpdate
 @Access		public 
 @Param			const Particle * pParticle
 @Returns		void
 @Description	
*************************************************************************/
void Spaceship::OnEmitterUpdate(Emitter* pEmitter, const Particle* pParticleParent, Uint32 uiTicksSinceSpawn)
	{
	if(uiTicksSinceSpawn > 30)
		{
		Particle p;
		p.Reset();
		p.m_bIsValid = true;
		Spaceship_Particle_InitExplosionSmokeTrail(&p.m_desc, pParticleParent->m_desc.m_vPos, pParticleParent->m_desc.m_vVel);
		pEmitter->AddParticle(p);
		}
	}

/*!***********************************************************************
 @Function		Hit
 @Access		public 
 @Returns		void
 @Description	
*************************************************************************/
void Spaceship::Hit()
	{
	m_eState = enumSTATE_Hit;

	m_Emitter.Start();
	m_Emitter2.Start();

	m_ExplosionDisc.Open(0.0f, 10.0f, enumINTERPOLATOR_Linear, 0.25f);

	const Uint32 uiNumTrails = 32;
	for(Uint32 uiCount = 0; uiCount < uiNumTrails; uiCount++)
		{
		Particle p;
		p.Reset();
		p.m_bIsValid = true;
		Spaceship_Particle_InitExplosionTrails(&p.m_desc, m_vPos);
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
		Spaceship_Particle_InitExplosion(&p.m_desc, m_vPos);
		m_Emitter.AddParticle(p);
		}

	for(Uint32 uiCount = 0; uiCount < uiNumParticles; uiCount++)
		{
		Particle p;
		p.Reset();
		p.m_bIsValid = true;
		Spaceship_Particle_InitExplosionSmoke(&p.m_desc, m_vPos);
		m_Emitter.AddParticle(p);
		}
	}

/*!***********************************************************************
 @Function		Launch
 @Access		public 
 @Param			const PVRTVec3 & vVelocity
 @Returns		void
 @Description	
*************************************************************************/
void Spaceship::Launch(const PVRTVec3& vVelocity)
	{
	// Work out where to place the projectile outside of the ships BB.
	PVRTVec3 vLaunchDir = GetPosition() + vVelocity;	// Code for when we remove the touch.
	PVRTVec2 vStartPos = GetBoundingBox().ClosestPointTo(PVRTVec2(vLaunchDir.x, vLaunchDir.y));
	m_pProjectile->Launch(PVRTVec3(vStartPos.x, vStartPos.y, 0.0f), vVelocity, m_rgbaCol);
	}

/*!***********************************************************************
 @Function		GetPosition
 @Access		public 
 @Returns		const PVRTVec3&
 @Description	
*************************************************************************/
const PVRTVec3& Spaceship::GetPosition()
	{
	return m_vPos;
	}

/*!***********************************************************************
 @Function		GetBoundingBox
 @Access		public 
 @Returns		const Rectanglef&
 @Description	
*************************************************************************/
const Rectanglef& Spaceship::GetBoundingBox()
	{
	return m_bb;
	}

/*!***********************************************************************
 @Function		GetCollisionBitmap
 @Access		public 
 @Returns		const CollisionBitmap*
 @Description	
*************************************************************************/
const CollisionBitmap* Spaceship::GetCollisionBitmap()
	{
	return m_pColBitmap;
	}

/*!***********************************************************************
 @Function		GetProjectile
 @Access		public 
 @Returns		Projectile*
 @Description	
*************************************************************************/
Projectile* Spaceship::GetProjectile()
	{ 
	return m_pProjectile; 
	}

#pragma region PARTICLE_EFFECTS
void Spaceship_Particle_InitExplosionTrails(ParticleDesc* pDesc, const PVRTVec3& vPos)
	{
	// Position behind the projectile
	pDesc->m_vPos = vPos;

	// Random direction
	Float32 fRnd    = Randomf() * (PVRT_PI*2);
	Float32 fRndVel = 100 + (Float32)Randomi(62);
	pDesc->m_vVel = PVRTVec3(cos(fRnd), sin(fRnd), 0.0f) * fRndVel;
	pDesc->m_fRotCur = fRnd;

	// Set properties
	Float32 fRndLife = 1.3f + Randomf()*0.8f;
	pDesc->m_fLife = fRndLife;
	pDesc->m_fDrag = 0.999f;

	pDesc->m_eMode = (enumPARTICLEMODE)(enumPARTICLEMODE_Colour | enumPARTICLEMODE_Scale | enumPARTICLEMODE_Rotate);

	pDesc->m_nNumCols = 4;
	pDesc->m_aRGBA[0].Set(1.0f, 1.0f, 1.0f, 1.0f);
	pDesc->m_aRGBA[1].Set(1.0f, 1.0f, 0.0f, 1.0f);
	pDesc->m_aRGBA[2].Set(1.0f, 0.0f, 0.0f, 1.0f);
	pDesc->m_aRGBA[3].Set(1.0f, 0.0f, 0.0f, 0.0f);

	pDesc->m_nNumScale = 2;
	pDesc->m_aScales[0] = PVRTVec3(2.5f, 0.125f, 1.0f);
	pDesc->m_aScales[1] = PVRTVec3(0.25f, 0.125f, 1.0f);
	}

void Spaceship_Particle_InitExplosion(ParticleDesc* pDesc, const PVRTVec3& vPos)
	{
	// Position behind the projectile
	pDesc->m_vPos = vPos;

	// Random direction
	Float32 fRnd    = Randomf() * (PVRT_PI*2);
	Float32 fRndVel = 37.5f + (Float32)Randomi(37);
	pDesc->m_vVel = PVRTVec3(cos(fRnd), sin(fRnd), 0.0f) * fRndVel;

	// Set properties
	pDesc->m_fLife = 1.0f;
	pDesc->m_fDrag = 0.9999f;

	pDesc->m_eMode = (enumPARTICLEMODE)(enumPARTICLEMODE_Colour | enumPARTICLEMODE_Scale);

	pDesc->m_nNumCols = 4;
	pDesc->m_aRGBA[0].Set(1.0f, 1.0f, 1.0f, 0.5f);
	pDesc->m_aRGBA[1].Set(1.0f, 1.0f, 0.0f, 0.5f);
	pDesc->m_aRGBA[2].Set(0.5f, 0.0f, 0.0f, 0.2f);
	pDesc->m_aRGBA[3].Set(0.25f, 0.0f, 0.0f, 0.0f);

	pDesc->m_nNumScale = 2;
	pDesc->m_aScales[0] = PVRTVec3(0.0f, 0.0f, 1.0f);
	pDesc->m_aScales[1] = PVRTVec3(2.0f, 2.0f, 1.0f);
	}

void Spaceship_Particle_InitExplosionSmoke(ParticleDesc* pDesc, const PVRTVec3& vPos)
	{
	// Position behind the projectile
	pDesc->m_vPos = vPos;

	// Random direction
	Float32 fRnd    = Randomf() * (PVRT_PI*2);
	Float32 fRndVel = 25 + (Float32)Randomi(25);
	pDesc->m_vVel = PVRTVec3(cos(fRnd), sin(fRnd), 0.0f) * fRndVel;

	// Set properties
	pDesc->m_fLife = 2.0f;
	pDesc->m_fDrag = 0.9999f;

	pDesc->m_eMode = (enumPARTICLEMODE)(enumPARTICLEMODE_Colour | enumPARTICLEMODE_Scale);

	pDesc->m_nNumCols = 2;
	pDesc->m_aRGBA[0].Set(1.0f, 1.0f, 1.0f, 0.5f);
	pDesc->m_aRGBA[1].Set(0.0f, 0.0f, 0.0f, 0.0f);

	pDesc->m_nNumScale = 2;
	pDesc->m_aScales[0] = PVRTVec3(0.0f, 0.0f, 1.0f);
	pDesc->m_aScales[1] = PVRTVec3(2.5f, 2.5f, 1.0f);
	}

void Spaceship_Particle_InitExplosionSmokeTrail(ParticleDesc* pDesc, const PVRTVec3& vPos, const PVRTVec3& vVel)
	{
	// Position behind the projectile
	pDesc->m_vPos = vPos;
	pDesc->m_vVel = vVel;

	// Set properties
	pDesc->m_fLife = 2.0f;
	pDesc->m_fDrag = 0.995f;

	pDesc->m_eMode = (enumPARTICLEMODE)(enumPARTICLEMODE_Colour | enumPARTICLEMODE_Scale);

	pDesc->m_nNumCols = 2;
	pDesc->m_aRGBA[0].Set(1.0f, 1.0f, 1.0f, 0.4f);
	pDesc->m_aRGBA[1].Set(0.0f, 0.0f, 0.0f, 0.0f);

	pDesc->m_nNumScale = 2;
	pDesc->m_aScales[0] = PVRTVec3(0.1f, 0.1f, 1.0f);
	pDesc->m_aScales[1] = PVRTVec3(0.5f, 0.5f, 1.0f);
	}
#pragma endregion PARTICLE_EFFECTS
