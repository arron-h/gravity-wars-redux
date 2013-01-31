#include "stdafx.h"
#include "Projectile.h"
#include "Planet.h"
#include "Spaceship.h"

#define PARTICLE_FPS 1/60.0f

const Uint32 c_uiMVP = HashMap_Hash("mxMVP");

// Some constants
const Float32 fParticleTrailSepar = 5.0f;
const Float32 fParticleGuideTrailDist = 500.0f;
const Float32 fParticleGuideTrailFadeT = 2.0f;


// ---------------------------------------------- Particle Effects
void Particle_InitTrail(ParticleDesc* pDesc, const PVRTVec3& vStartPos, const RGBA& c_Colour)
	{
	// Position behind the projectile
	pDesc->m_vPos = vStartPos;

	// Set properties
	pDesc->m_fLife = 1.0f;
	pDesc->m_fDrag = 0.99f;

	pDesc->m_eMode = (enumPARTICLEMODE)(enumPARTICLEMODE_Colour | enumPARTICLEMODE_Scale);

	pDesc->m_nNumCols = 2;
	pDesc->m_aRGBA[0] = c_Colour;
	pDesc->m_aRGBA[1].Set(0.0f, 0.0f, 0.0f, 0.0f);

	pDesc->m_nNumScale = 2;
	pDesc->m_aScales[0] = PVRTVec3(0.85f, 0.85f, 1.0f);
	pDesc->m_aScales[1] = PVRTVec3(0.0f, 0.0f, 0.0f);
	}

void Particle_InitGuideTrail(ParticleDesc* pDesc, const PVRTVec3& vPos, Float32 fRot, const RGBA& c_Colour)
	{
	// Position behind the projectile
	pDesc->m_vPos = vPos;

	// Set properties
	pDesc->m_fRotCur = fRot;
	pDesc->m_fLife = -1.0f;
	pDesc->m_fDrag = 0.99f;

	pDesc->m_eMode = (enumPARTICLEMODE)(enumPARTICLEMODE_Colour);

	pDesc->m_nNumCols = 2;
	pDesc->m_aRGBA[0] = c_Colour;
	pDesc->m_aRGBA[1].Set(c_Colour.m_fR, c_Colour.m_fG, c_Colour.m_fB, 0.0f);
	}

void Particle_InitDissipate(ParticleDesc* pDesc, const PVRTVec3& vStartPos, const PVRTVec3& vOrigVel, Uint32 uiIdx, Uint32 uiCount)
	{
	// Position behind the projectile
	pDesc->m_vPos = vStartPos;

	Float32 fDelta = (Float32)uiIdx / (Float32)uiCount;
	Float32 fRad = atan2(vOrigVel.y, vOrigVel.x);
	Float32 fRndVel = 0.5f + (Randomf() * 0.5f);
	Float32 fAngleX = cos(((fDelta-0.5f*2) * DegToRad((Float32)uiCount)) + fRad);
	Float32 fAngleY = sin(((fDelta-0.5f*2) * DegToRad((Float32)uiCount)) + fRad);
	PVRTVec3 vAngle(fAngleX, fAngleY, 0.0f);
	pDesc->m_vVel = vAngle.normalize() * vOrigVel.length() * fRndVel;

	// Set properties
	pDesc->m_fLife = 1.5f;
	pDesc->m_fDrag = 0.998f;

	pDesc->m_eMode = (enumPARTICLEMODE)(enumPARTICLEMODE_Colour | enumPARTICLEMODE_Scale);

	pDesc->m_nNumCols = 3;
	pDesc->m_aRGBA[0].Set(1.0f, 1.0f, 0.0f, 1.0f);
	pDesc->m_aRGBA[1].Set(1.0f, 0.5f, 0.0f, 0.85f);
	pDesc->m_aRGBA[2].Set(1.0f, 0.0f, 0.0f, 0.25f);

	pDesc->m_nNumScale = 2;
	pDesc->m_aScales[0] = PVRTVec3(0.5f, 0.5f, 1.0f);
	pDesc->m_aScales[1] = PVRTVec3(0.0f, 0.0f, 0.0f);
	}

void Particle_InitDissipateInitial(ParticleDesc* pDesc, const PVRTVec3& vStartPos, const PVRTVec3& vOrigVel)
	{
	// Position behind the projectile
	pDesc->m_vPos = vStartPos;
	pDesc->m_vVel = vOrigVel;

	// Set properties
	pDesc->m_fLife = 1.5f;
	pDesc->m_fDrag = 0.99f;

	pDesc->m_eMode = (enumPARTICLEMODE)(enumPARTICLEMODE_Colour | enumPARTICLEMODE_Scale);

	pDesc->m_nNumCols = 2;
	pDesc->m_aRGBA[0].Set(1.0f, 1.0f, 1.0f, 1.0f);
	pDesc->m_aRGBA[1].Set(1.0f, 1.0f, 0.0f, 0.25f);

	pDesc->m_nNumScale = 4;
	pDesc->m_aScales[0] = PVRTVec3(0.75f, 0.75f, 1.0f);
	pDesc->m_aScales[1] = PVRTVec3(0.75f, 0.75f, 1.0f);
	pDesc->m_aScales[2] = PVRTVec3(0.75f, 0.75f, 1.0f);
	pDesc->m_aScales[3] = PVRTVec3(0.0f, 0.0f, 0.0f);
	}


Projectile::Projectile() : m_bActive(false), m_uiActiveTrail(0)
	{
	m_tex = RESMAN->GetTexture("projectile");
	m_TrailEmitter.Initialise(RESMAN->GetTexture("particle"), enumPARTICLETYPE_Points, 1024);
	m_GuideTrailEmitter[0].Initialise(RESMAN->GetTexture("projectile-trail"), enumPARTICLETYPE_Quads, 1024);
	m_GuideTrailEmitter[1].Initialise(RESMAN->GetTexture("projectile-trail"), enumPARTICLETYPE_Quads, 1024);

	m_vTrailOffset = PVRTVec3(-12.0f, 0.0f, 0.0f);		// Offset
	m_ProjShader = RESMAN->GetShader("MVP_V_T_C");
	}

/*!***********************************************************************
 @Function		Launch
 @Access		public 
 @Param			const PVRTVec3 & vInitialPos
 @Returns		void
 @Description	
*************************************************************************/
void Projectile::Launch(const PVRTVec3& vInitialPos, const PVRTVec3& vVelocity, const RGBA& c_rgbaTrailColour)
	{
	m_bActive = true;
	m_vPosition = vInitialPos;
	m_vVelocity = vVelocity;
	m_fTime    = 0.0f;
	m_fOOBTime = 0.0f;
	m_TrailColour = c_rgbaTrailColour;

	m_TrailEmitter.Start();
	m_GuideTrailEmitter[m_uiActiveTrail].Start();
	m_GuideTrailEmitter[!m_uiActiveTrail].SetAll(Emitter::enumVALUE_Life, fParticleGuideTrailFadeT);
	}

/*!***********************************************************************
 @Function		Hit
 @Access		public 
 @Returns		void
 @Description	
*************************************************************************/
void Projectile::Hit()
	{
	// Hit something.

	m_bActive = false;
	m_uiActiveTrail = !m_uiActiveTrail;		// Switch trail
	}

/*!***********************************************************************
 @Function		Dissipate
 @Access		public 
 @Returns		void
 @Description	
*************************************************************************/
void Projectile::Dissipate()
	{
	// Timed-out
	// Create a bunch of particles
	Uint32 uiMaxParticles = 64;
	for(Uint32 i = 0; i < uiMaxParticles; i++)
		{
		Particle p;
		p.Reset();
		Particle_InitDissipate(&p.m_desc, m_vPosition, m_vVelocity, i, uiMaxParticles);
		m_TrailEmitter.AddParticle(p);
		}

#ifdef _DEBUG
	m_TrailEmitter.DebugPrint();
#endif
	
	m_bActive = false;
	m_uiActiveTrail = !m_uiActiveTrail;		// Switch trail
	}


/*!***********************************************************************
 @Function		Update
 @Access		public 
 @Param			double fDT
 @Param			Planet * pPlanets
 @Param			Uint32 uiNumPlanets
 @Returns		Uint32
 @Description	
*************************************************************************/
Uint32 Projectile::Update(double fDT, Planet* pPlanets, Uint32 uiNumPlanets, Spaceship* pShips, Uint32 uiNumShips)
	{
	if(m_bActive)
		{
		// Have we timed out?
		m_fTime += (Float32)fDT;
		if(m_fTime > PROJECTILE_TIMEOUT)
			return PROJRESULT_EXPIRED;

		// Have we gone off the game screen for > 3 seconds?
		Float32 fGameX = GFX->GetVirtualWidth()  * 0.5f;
		Float32 fGameY = GFX->GetVirtualHeight() * 0.5f;
		if(m_vPosition.x > fGameX || m_vPosition.x < -fGameX || m_vPosition.y > fGameY || m_vPosition.y < -fGameY)
			m_fOOBTime += (Float32)fDT;
		else
			m_fOOBTime = 0.0f;
		if(m_fOOBTime > PROJECTILE_OOB_TIMEOUT)
			return PROJRESULT_EXPIRED;

		// Calculate acceleration
		PVRTVec3 vAccel = PVRTVec3(0,0,0);
		for(Uint32 i = 0; i < uiNumPlanets; ++i)
			{
			Float32 fPlanetDistance = PVRTVec3(pPlanets[i].GetPosition() - m_vPosition).length();
			Float32 fPlanetForce = pPlanets[i].GetMass() / powf(fPlanetDistance, 2);
			vAccel += (pPlanets[i].GetPosition() - m_vPosition) * fPlanetForce;
			}
		
		// Calculate velocity
		m_vVelocity = m_vVelocity + vAccel * (Float32)fDT;									// v0 = v1 + a*dt

		// Calculate new position
		PVRTVec3 vDist = (m_vVelocity * (Float32)fDT) + (0.5f * vAccel * powf((Float32)fDT, 2));		// d = v*t + 1/2at^2
		m_vPosition = m_vPosition + vDist;											// s1 = s0 + d

		// Create Model matrix
		Float32 fRad = atan2(m_vVelocity.y, m_vVelocity.x);
		m_mxModel = PVRTMat4::RotationZ(fRad);
		m_mxModel.f[12] = m_vPosition.x;
		m_mxModel.f[13] = m_vPosition.y;

		// --- Update particles
		// First, update the rocket burn trail
		static Float32 fParticleTime = 0.0f;
		fParticleTime += (Float32)fDT;
		if(fParticleTime > PARTICLE_FPS)
			{
			const Particle* ParticleTail = m_TrailEmitter.Tail();
			PVRTVec3 vLastPos;
			PVRTVec3 vNewPos  = m_vPosition + (m_vTrailOffset * m_mxModel);
			PVRTVec3 vDelta(0,0,0);
			Sint32 nNum = 0;
			if(ParticleTail)
				{
				vLastPos =  ParticleTail->m_desc.m_vPos;
				vDelta = vNewPos - vLastPos;
				nNum = (Sint32)(ceil(vDelta.length() / fParticleTrailSepar));
				}
			else
				{
				vLastPos = vNewPos;
				nNum = 1;		// Make sure 1 particle gets spawned
				}
			if(nNum > 0)
				{
				PVRTVec3 vStep = vDelta / (Float32)nNum;
				PVRTVec3 vPos;
				for(Sint32 i = 1; i <= nNum; ++i)
					{
					vPos = vLastPos + (vStep * (Float32)i);
					Particle p;
					p.Reset();
					Particle_InitTrail(&p.m_desc, vPos, m_TrailColour);
					p.m_bIsValid = true;
					m_TrailEmitter.AddParticle(p);
					}
				}
			fParticleTime = 0.0f;
			}

		// Now update the guide trails
			{
			const Particle* GuideTail = m_GuideTrailEmitter[m_uiActiveTrail].Tail();
			PVRTVec3 vDist;
			bool bForce = false;
			if(GuideTail)
				vDist = GuideTail->m_desc.m_vPos - m_vPosition;
			else
				bForce = true;

			if(vDist.lenSqr() > fParticleGuideTrailDist || bForce)
				{
				Particle p;
				p.Reset();
				Particle_InitGuideTrail(&p.m_desc, m_vPosition, fRad, m_TrailColour);
				p.m_bIsValid = true;
				m_GuideTrailEmitter[m_uiActiveTrail].AddParticle(p);
				}
			}
		}

	// Make sure the emitter gets updated.
	m_TrailEmitter.Update(fDT);
	m_GuideTrailEmitter[0].Update(fDT);
	m_GuideTrailEmitter[1].Update(fDT);

	if(!m_bActive)
		return PROJRESULT_INVALID;

	// -------------------------- Check collisions
	Uint32 uiColisionResult;

	// Check Planets
	Uint32 i;
	for(i = 0; i < uiNumPlanets; ++i)
		{
		Float32 fDistSqrd = PVRTVec3(m_vPosition - pPlanets[i].GetPosition()).lenSqr();
		if(fDistSqrd < powf(pPlanets[i].GetRadius(), 2.0f))
			{
			// --- Hit!
			// --- Use loword for result
			uiColisionResult = PROJRESULT_HITPLANET;

			// --- Use first 12 MSBits for angle of collision
			// Work out the angle
			PVRTVec3 vDir  = (m_vPosition - pPlanets[i].GetPosition()).normalize();
			Float32 fAngle = acos(vDir.dot(PVRTVec3(1,0,0)));
			if(vDir.y < 0.0f)
				fAngle = (PVRT_PI*2) - fAngle;
			Uint32 uiAngle = (Uint32)RadToDeg(fAngle); // Multiply to degrees for precision sake.
			uiColisionResult |= (uiAngle << 20);

			// --- Use next 4 bits for hit data (planet ID)
			uiColisionResult |= (i << 16);
			
			return uiColisionResult;
			}
		}

	// Check ships
	for(i = 0; i < uiNumShips; ++i)
		{
		if(pShips[i].GetBoundingBox().Contains(PVRTVec2(m_vPosition.x, m_vPosition.y)))		// Quick test
			{
			// Ok, now test the collision bitmap for proper collisions
			PVRTVec2 vLocalPoint = PVRTVec2(m_vPosition.x, m_vPosition.y) - PVRTVec2(pShips[i].GetPosition().x, pShips[i].GetPosition().y);
			if(pShips[i].GetCollisionBitmap()->Check(vLocalPoint, CollisionBitmap::enumSOURCE_Centre))
				{
				// Hit!
				// Use loword for result
				uiColisionResult = PROJRESULT_HITSPACESHIP;
				// Use hiword for hit data
				uiColisionResult |= (i << 16);
				return uiColisionResult;
				}
			}
		}


	return PROJRESULT_MISS;
	}

/*!***********************************************************************
 @Function		DrawGeom
 @Access		public 
 @Returns		void
 @Description	
*************************************************************************/
void Projectile::DrawGeom()
	{
	Shader::Use(m_ProjShader);
	Camera* pCam = VIEWMAN->GetActiveView()->GetCamera();
	PVRTMat4 mxMVP = pCam->GetProjection() * pCam->GetView();
	glUniformMatrix4fv(m_ProjShader->GetUniform(c_uiMVP), 1, GL_FALSE, mxMVP.f);

	// Render particles
	m_TrailEmitter.Render();
	m_GuideTrailEmitter[0].Render();
	m_GuideTrailEmitter[1].Render();

	if(!m_bActive)
		{
		Shader::Use(0);
		return;
		}
		

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBindTexture(GL_TEXTURE_2D, m_tex->GetHandle());
	GetApp()->GetGraphicsSystem()->RenderQuad(Rectanglef(-12, -6, 12, 6), m_mxModel, FLAG_VRT | FLAG_TEX0 | FLAG_RGB);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	Shader::Use(0);
	}
