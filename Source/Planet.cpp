#include "stdafx.h"
#include "Planet.h"
#include "Resources/Mdl_Planet.h"

const Uint32 c_uiMVP		= HashMap_Hash("mxMVP");
const Uint32 c_uiVP			= HashMap_Hash("mxVP");
const Uint32 c_uiModel		= HashMap_Hash("mxModel");
const Uint32 c_uiLightDir	= HashMap_Hash("vLightDir");
const Uint32 c_uiTime		= HashMap_Hash("fTime");
const Uint32 c_uiAmp		= HashMap_Hash("fAmp");

const Float32 c_fAmpDec		= 1.0f;
const Float32 c_fAmpStart	= 3.5f;

void Planet_Particle_InitExplosionTrails(ParticleDesc* pDesc, const PVRTVec3& vPos, Float32 fAng);
void Planet_Particle_InitExplosion(ParticleDesc* pDesc, const PVRTVec3& vPos, Float32 fAng);
void Planet_Particle_InitExplosionSmoke(ParticleDesc* pDesc, const PVRTVec3& vPos, Float32 fAng);

Planet::Planet() :	m_texSurface(0), m_texGlow(0), m_uiCol(0), m_fRad(0), m_fMass(0), m_fRot(0), m_fSpeed(0), m_fHitAmplitude(0)
	{
	m_vPos.x = m_vPos.y = m_vPos.z = 0;

	m_Emitter.Initialise(RESMAN->GetTexture("particle-cloud"), enumPARTICLETYPE_Points, 32);
	m_Emitter2.Initialise(RESMAN->GetTexture("particle"), enumPARTICLETYPE_Quads, 32);
	}

Planet::~Planet()
	{
	}

/*!***********************************************************************
 @Function		Initialise
 @Access		public 
 @Param			const PVRTVec3 & vPos
 @Param			Uint32 uiCol
 @Param			Float32 fRad
 @Param			Float32 fMass
 @Returns		void
 @Description	
*************************************************************************/
void Planet::Initialise(const PVRTVec3& vPos, const PVRTVec4& vLightDir, TextureRef tex, Uint32 uiCol, Float32 fRad, Float32 fMass, Float32 fAxis, Float32 fSpeed)
	{
	m_vPos			= vPos;
	m_fRad			= fRad;
	m_fMass			= fMass;
	m_uiCol			= uiCol;
	m_fSpeed		= fSpeed;
	m_texSurface	= tex;
	m_mxAxis = PVRTMat4::RotationZ(fAxis);
	m_vLightDir     = vLightDir;

	// Work out the rotation for the light source
	Float32 fRot = atan2(vLightDir.y, vLightDir.x);
	m_mxLightRot = PVRTMat4::RotationZ(-fRot);

	// Build translation matrix
	m_mxTrans = PVRTMat4::Translation(m_vPos.x, m_vPos.y, m_vPos.z);

	m_Model = RESMAN->GetModel("planet");
	m_texGlow = RESMAN->GetTexture("glow");
	m_SurfShader = RESMAN->GetShader("Planet");
	m_shaderMVP_V_T_C = RESMAN->GetShader("MVP_V_T_C");
	}

/*!***********************************************************************
 @Function		Hit
 @Access		public 
 @Returns		void
 @Description	
*************************************************************************/
void Planet::Hit(Uint32 uiAngle)
	{
	Float32 fAngRad    = DegToRad((Float32)uiAngle);
	PVRTVec3 vStartVel = PVRTVec3(1,0,0) * PVRTMat3::Rotation2D(fAngRad);
	PVRTVec3 vStartPos = m_vPos + (vStartVel * m_fRad);

	m_fHitAmplitude = c_fAmpStart;

	m_Emitter.Start();
	m_Emitter2.Start();

	const Uint32 uiNumTrails = 32;
	for(Uint32 uiCount = 0; uiCount < uiNumTrails; uiCount++)
		{
		Particle p;
		p.Reset();
		p.m_bIsValid = true;
		Planet_Particle_InitExplosionTrails(&p.m_desc, vStartPos, fAngRad);
		m_Emitter2.AddParticle(p);
		}

	const Uint32 uiNumParticles = 16;
	for(Uint32 uiCount = 0; uiCount < uiNumParticles; uiCount++)
		{
		Particle p;
		p.Reset();
		p.m_bIsValid = true;
		Planet_Particle_InitExplosion(&p.m_desc, vStartPos, fAngRad);
		m_Emitter.AddParticle(p);
		}

	for(Uint32 uiCount = 0; uiCount < uiNumParticles; uiCount++)
		{
		Particle p;
		p.Reset();
		p.m_bIsValid = true;
		Planet_Particle_InitExplosionSmoke(&p.m_desc, vStartPos, fAngRad);
		m_Emitter.AddParticle(p);
		}
	}

/*!***********************************************************************
 @Function		Update
 @Access		public 
 @Param			double fDT
 @Returns		void
 @Description	
*************************************************************************/
void Planet::Update(double fDT)
	{
	m_Emitter.Update(fDT);
	m_Emitter2.Update(fDT);

	m_fRot += m_fSpeed * (float)fDT;
	
	Float32 fFPSScale = 60.0f / (1.0f / (Float32)fDT);
	m_fHitAmplitude *= powf(0.98f, fFPSScale);

	// Rotate the model
	PVRTMat4 mxScale = PVRTMat4::Scale(m_fRad, m_fRad, m_fRad);
	PVRTMat4 mxRot   = PVRTMat4::RotationY(m_fRot);
	m_mxModel = m_mxTrans * mxScale * m_mxAxis * mxRot;
	}

/*!***********************************************************************
 @Function		DrawGeom
 @Access		public 
 @Returns		void
 @Description	
*************************************************************************/
void Planet::DrawGeom()
	{
	// --- Render the Planet model.
 	glBindTexture(GL_TEXTURE_2D, m_texSurface->GetHandle());	// Bind surface texture
	Shader::Use(m_SurfShader);
	Camera* pCam = VIEWMAN->GetActiveView()->GetCamera();

	PVRTMat4 mxMV = pCam->GetProjection() * pCam->GetView();
	PVRTMat4 mxRot = m_mxAxis * PVRTMat4::RotationY(m_fRot);
	PVRTVec3 vLight = m_vLightDir * mxRot;
	glUniformMatrix4fv(m_SurfShader->GetUniform(c_uiVP), 1, GL_FALSE, mxMV.f);
	glUniformMatrix4fv(m_SurfShader->GetUniform(c_uiModel), 1, GL_FALSE, m_mxModel.f);
	glUniform3fv(m_SurfShader->GetUniform(c_uiLightDir), 1, &vLight.x);
	glUniform1f(m_SurfShader->GetUniform(c_uiTime), (Float32)GetApp()->GetTicks());
	glUniform1f(m_SurfShader->GetUniform(c_uiAmp), m_fHitAmplitude);
	//glUniform1f(m_shader->GetUniform(c_uiTime), m_fRot);
	m_Model->RenderSegment(0, FLAG_VRT | FLAG_TEX0 | FLAG_NRM);
	Shader::Use(0);
 
	// --- Now draw the glow overlay
	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBindTexture(GL_TEXTURE_2D, m_texGlow->GetHandle());
	Shader::Use(m_shaderMVP_V_T_C);

	PVRTMat4 mxGlowMVP = pCam->GetProjection() * pCam->GetView() * m_mxTrans * m_mxLightRot;
	glUniformMatrix4fv(m_shaderMVP_V_T_C->GetUniform(c_uiMVP), 1, GL_FALSE, mxGlowMVP.f);
	Float32 fSRad = m_fRad * 1.28f; // Approximate texture scale (texture is 128x128).
	GetApp()->GetGraphicsSystem()->RenderQuad(Rectanglef(-fSRad, -fSRad, fSRad, fSRad), PVRTMat4::Identity(), FLAG_VRT | FLAG_TEX0 | FLAG_RGB, m_uiCol);
	
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);

	// Render particles using m_shaderMVP_V_T_C.
	PVRTMat4 mxExpMVP = pCam->GetProjection() * pCam->GetView();
	glUniformMatrix4fv(m_shaderMVP_V_T_C->GetUniform(c_uiMVP), 1, GL_FALSE, mxExpMVP.f);
	m_Emitter.Render();
	m_Emitter2.Render();

	Shader::Use(0);
	}

#pragma region EXPLOSION_REGION
void Planet_Particle_InitExplosionTrails(ParticleDesc* pDesc, const PVRTVec3& vPos, Float32 fAng)
	{
	// Position behind the projectile
	pDesc->m_vPos = vPos;

	// Random direction
	Float32 fRnd	= fAng + (RandomfCentre() * (PVRT_PI_OVER_TWO*0.75f));
	Float32 fRndVel = 75 + (Float32)Randomi(46);
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
	pDesc->m_aScales[0] = PVRTVec3(1.875f, 0.093f, 1.0f);
	pDesc->m_aScales[1] = PVRTVec3(0.1875f, 0.093f, 1.0f);
	}

void Planet_Particle_InitExplosion(ParticleDesc* pDesc, const PVRTVec3& vPos, Float32 fAng)
	{
	// Position behind the projectile
	pDesc->m_vPos = vPos;

	// Random direction
	Float32 fRnd	= fAng + (RandomfCentre() * (PVRT_PI_OVER_TWO*0.75f));
	Float32 fRndVel = 28.125f + (Float32)Randomi(28);
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
	pDesc->m_aScales[1] = PVRTVec3(1.5f, 1.5f, 1.0f);
	}

void Planet_Particle_InitExplosionSmoke(ParticleDesc* pDesc, const PVRTVec3& vPos, Float32 fAng)
	{
	// Position behind the projectile
	pDesc->m_vPos = vPos;

	// Random direction
	Float32 fRnd	= fAng + (RandomfCentre() * (PVRT_PI_OVER_TWO*0.75f));
	Float32 fRndVel = 18.75f + (Float32)Randomi(18);
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
	pDesc->m_aScales[1] = PVRTVec3(1.875f, 1.875f, 1.0f);
	}
#pragma endregion EXPLOSION_REGION