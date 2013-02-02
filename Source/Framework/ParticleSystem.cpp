#include "stdafx.h"

/*!***********************************************************************
 @Function		Reset
 @Access		public 
 @Returns		void
 @Description	
*************************************************************************/
void ParticleDesc::Reset()
	{
	m_eMode = enumPARTICLEMODE_Default;

	//m_vRandomForce.Zero();
	m_vPos.x = m_vPos.y = m_vPos.z = 0.0f;
	m_vVel.x = m_vVel.y = m_vVel.z = 0.0f;
	m_vAcc.x = m_vAcc.y = m_vAcc.z = 0.0f;
	m_vScale.x = m_vScale.y = m_vScale.z = 1.0f;
	m_RGBA = RGBA(1,1,1,1);

	m_fAge     =
		m_fWeight  =
		m_fRotCur  = 
		m_fRotVel  = 
		m_fRotAcc  = 0.0f;

	m_fRotDrag = 
		m_fDrag    = 1.0f;

	m_fLife   = 5.0f;

	m_nNumCols  = 0;
	m_nNumScale = 0;
	}

/*!***********************************************************************
 @Function		CalcKeyframeTime
 @Access		private 
 @Param			Uint32 nTotal
 @Param			Uint32 & nKeyA
 @Param			Uint32 & nKeyB
 @Param			Float32 & fInterpolator
 @Returns		void
 @Description	
*************************************************************************/
void ParticleDesc::CalcKeyframeTime(Uint32 nTotal, Uint32 &nKeyA, Uint32 &nKeyB, Float32 &fInterpolator)
	{
	ASSERT(nTotal > 1);
	ASSERT(nTotal <= MAX_KEYFRAMES);

	Uint32 nMax = nTotal-1;

	Float32 fT = m_fAge / m_fLife;
	nKeyA = (Uint32)(nMax * fT);
	nKeyB = nKeyA + 1;

	ASSERT(nKeyA < nTotal);
	ASSERT(nKeyB < nTotal);

	Float32 fS = m_fLife / nMax;
	Float32 fTimeKeyA = nKeyA * fS;
	Float32 fTimeKeyB = nKeyB * fS;
	Float32 fTimeWin  = (fTimeKeyB - fTimeKeyA);

	ASSERT(fTimeWin > 0.01f);

	fInterpolator = (m_fAge - fTimeKeyA) / fTimeWin;
	}

/*!***********************************************************************
 @Function		Update
 @Access		public 
 @Param			double fDT
 @Returns		void
 @Description	
*************************************************************************/
void ParticleDesc::Update(double fDT)
	{
	if(m_eMode == enumPARTICLEMODE_None) 
		return;

	// --- Update position
	// Velocity
	m_vVel = m_vVel + m_vAcc * (Float32)fDT;												// v0 = v1 + a*dt
	m_vVel *= m_fDrag;

	// Calculate new position
	PVRTVec3 vDist = (m_vVel * (Float32)fDT) + (0.5f * m_vAcc * powf((Float32)fDT, 2));		// d = v*t + 1/2at^2
	m_vPos = m_vPos + vDist;																// s1 = s0 + d

	// --- Update orientation
	if(m_eMode & enumPARTICLEMODE_Rotate)
		{
		m_fRotCur = fmodf(m_fRotCur + m_fRotVel, PVRT_PI*2);		// Wrap to PI2
		m_fRotVel += m_fRotAcc;
		m_fRotVel *= m_fRotDrag;
		}

	// --- Update Random Force
/*	if(m_eMode & enumPS_MODES_RandomForce)
		{
		Vector3 v(AH_Rand32fCentre(), AH_Rand32fCentre(), AH_Rand32fCentre());
		GEVmul(v, 0.01f);
		GEVmul(v, m_vRandomForce);
		GEVadd(m_vVel, v);
		}*/

	// --- Update colour blending
	if(m_eMode & enumPARTICLEMODE_Colour && m_fLife >= 0.0f)
		{
		Uint32 keyA, keyB;
		Float32 fInterpolator;
		CalcKeyframeTime(m_nNumCols, keyA, keyB, fInterpolator);
		m_RGBA.m_fR = Interpolate_Linear(m_aRGBA[keyA].m_fR, m_aRGBA[keyB].m_fR, fInterpolator);
		m_RGBA.m_fG = Interpolate_Linear(m_aRGBA[keyA].m_fG, m_aRGBA[keyB].m_fG, fInterpolator);
		m_RGBA.m_fB = Interpolate_Linear(m_aRGBA[keyA].m_fB, m_aRGBA[keyB].m_fB, fInterpolator);
		m_RGBA.m_fA = Interpolate_Linear(m_aRGBA[keyA].m_fA, m_aRGBA[keyB].m_fA, fInterpolator);
		}

	// --- Update scale blending
	if(m_eMode & enumPARTICLEMODE_Scale && m_fLife >= 0.0f)
		{
		Uint32 keyA, keyB;
		Float32 fInterpolator;
		CalcKeyframeTime(m_nNumScale, keyA, keyB, fInterpolator);
		m_vScale.x = Interpolate_Linear(m_aScales[keyA].x, m_aScales[keyB].x, fInterpolator);
		m_vScale.y = Interpolate_Linear(m_aScales[keyA].y, m_aScales[keyB].y, fInterpolator);
		m_vScale.z = Interpolate_Linear(m_aScales[keyA].z, m_aScales[keyB].z, fInterpolator);
		}
	}


/*!***********************************************************************
 @Function		Reset
 @Access		public 
 @Returns		void
 @Description	
*************************************************************************/
void Particle::Reset()
	{
	m_pPrimitive = NULL;
	m_desc.Reset();
	m_bIsValid = false;
	m_eType    = enumPARTICLETYPE_Invalid;
	}

/*!***********************************************************************
 @Function		Update
 @Access		public 
 @Param			double fDT
 @Returns		void
 @Description	
*************************************************************************/
void Particle::Update(double fDT)
	{
	if(!m_bIsValid)
		return;

	// --- Age particle
	if(m_desc.m_fLife >= 0.0f)
		{
		m_desc.m_fAge += (Float32)fDT;
		if(m_desc.m_fAge >= m_desc.m_fLife)
			{
			m_bIsValid = false;
			return;
			}
		}


	// --- Update particle
	m_desc.Update(fDT);
	}

/*!***********************************************************************
 @Function		BuildPoints
 @Access		private 
 @Returns		void
 @Description	
*************************************************************************/
void Particle::BuildPoints()
	{
	// Points use Position, Colour and single float Size for each particle
	PrimPoint* pPrim = (PrimPoint*)m_pPrimitive;
	
	pPrim->v		= m_desc.m_vPos;
	pPrim->rgba[0]	= m_desc.m_RGBA.RedChannel();
	pPrim->rgba[1]	= m_desc.m_RGBA.GreenChannel();
	pPrim->rgba[2]	= m_desc.m_RGBA.BlueChannel();
	pPrim->rgba[3]	= m_desc.m_RGBA.AlphaChannel();
	pPrim->s		= m_desc.m_vScale.x * m_pParent->m_tex->GetWidth();		// Use X scale for size. We could use the vector length but that would involve a square root.
	}

/*!***********************************************************************
 @Function		BuildQuads
 @Access		private 
 @Returns		void
 @Description	
*************************************************************************/
void Particle::BuildQuads()
	{
	// Quads require 4 vertices with Position, Colour and UVs.
	const PVRTMat3 mxRot = PVRTMat3::RotationZ(m_desc.m_fRotCur);
	const Float32 fHW = (m_pParent->m_tex->GetWidth()  * 0.5f) * m_desc.m_vScale.x;
	const Float32 fHH = (m_pParent->m_tex->GetHeight() * 0.5f) * m_desc.m_vScale.y;
	const RGBA& rgba = m_desc.m_RGBA;
	PrimQuad* pPrim = (PrimQuad*)m_pPrimitive;

	static const PVRTVec2 UVs[4] =
		{
			PVRTVec2(0.0f, 0.0f),	// TL
			PVRTVec2(0.0f, 1.0f),	// BL
			PVRTVec2(1.0f, 0.0f),	// TR
			PVRTVec2(1.0f, 1.0f),	// BR
		};

	static const Float32 fSigns[4*2] =
		{
		-1.0f, +1.0f,				// TL
		-1.0f, -1.0f,				// BL
		+1.0f, +1.0f,				// TR
		+1.0f, -1.0f,				// BR
		};

	PrimVertex* pV = NULL;
	for(Uint32 i = 0; i < 4; ++i)
		{
		pV = &pPrim->vert[i];
		memset(pV, 0, sizeof(PrimVertex));
		pV->v.x = fHW*fSigns[i*2];
		pV->v.y = fHH*fSigns[i*2+1];
		pV->uv  = UVs[i];
		pV->rgba[0] = rgba.RedChannel();	pV->rgba[1] = rgba.GreenChannel();	
		pV->rgba[2] = rgba.BlueChannel();	pV->rgba[3] = rgba.AlphaChannel();
		pV->v *= mxRot;
		pV->v += m_desc.m_vPos;
		}
	}

/*!***********************************************************************
 @Function		BuildGeom
 @Access		public 
 @Returns		void
 @Description	
*************************************************************************/
void Particle::BuildGeometry()
	{
	if(!m_bIsValid) 
		return;

	if(m_eType == enumPARTICLETYPE_Points)
		BuildPoints();
	else if(m_eType == enumPARTICLETYPE_Quads)
		BuildQuads();
	else
		ASSERT(!"Unknown type");
	}



/*!***********************************************************************
 @Function		Reset
 @Access		public 
 @Returns		void
 @Description	
*************************************************************************/
void Emitter::Reset()
	{
	m_bIsValid          = false;
	m_bIsActive			= false;
	m_uiTicksSinceLastSpawn = 0;
	m_tex				= 0;
	m_uiMaxParticles    = 0;
	m_uiActiveParticles	= 0;
	m_uiMaxEmitterChildren = 0;
	m_pTail				= NULL;
	if(m_pParticles)
		{
		delete [] m_pParticles;
		m_pParticles = NULL;
		}
	if(m_pPoints)
		{
		delete [] m_pPoints;
		m_pPoints = NULL;
		}
	if(m_pQuads)
		{
		delete [] m_pQuads;
		m_pQuads = NULL;
		}

	if(m_pEmitters)
		{
		delete [] m_pEmitters;
		m_pEmitters = NULL;
		}
	}

/*!***********************************************************************
 @Function		Initialise
 @Access		public 
 @Param			TextureRef tex
 @Param			Uint32 uiMaxParticles
 @Returns		void
 @Description	
*************************************************************************/
void Emitter::Initialise(TextureRef tex, enumPARTICLETYPE eType, Uint32 uiMaxParticles, Uint32 uiNumChildren)
	{
	Reset();

	m_bIsValid		= true;
	m_tex			= tex;
	m_eParticleType = eType;

	// Allocate space for the particle data
	m_uiMaxParticles = uiMaxParticles;
	m_pParticles     = new Particle[uiMaxParticles];

	// Allocate space for child emitters
	m_uiMaxEmitterChildren  = uiNumChildren;
	if(uiNumChildren)
		m_pEmitters				= new Emitter[uiNumChildren];

	// Allocate space for the primitive depending on particle type
	switch(eType)
		{
		case enumPARTICLETYPE_Points:
			m_pPoints = new PrimPoint[uiMaxParticles];
			break;
		case enumPARTICLETYPE_Quads:
			m_pQuads  = new PrimQuad[uiMaxParticles];
			break;
		case enumPARTICLETYPE_Invalid:
		default:
			ASSERT(!"Unhandled particle type");
		}	
	}

/*!***********************************************************************
 @Function		AddParticle
 @Access		public 
 @Param			Particle & src
 @Returns		void
 @Description	
*************************************************************************/
void Emitter::AddParticle(Particle& src)
	{
	ASSERT(m_bIsValid);
	ASSERT(m_uiActiveParticles < m_uiMaxParticles);

	if(!m_bIsActive)
		return;

	if(m_eParticleType == enumPARTICLETYPE_Points)		// Check to make sure rotations aren't trying to be used
		ASSERT((src.m_desc.m_eMode & enumPARTICLEMODE_Rotate) == false);

	Uint32 i = m_uiActiveParticles;
	src.m_eType   = m_eParticleType;
	src.m_pParent = this;
	switch(m_eParticleType)
		{
		case enumPARTICLETYPE_Points:	src.m_pPrimitive = (void*)&m_pPoints[i];	break;
		case enumPARTICLETYPE_Quads:	src.m_pPrimitive = (void*)&m_pQuads[i];		break;
		default:
			ASSERT(!"Invalid");
		}

	if(src.m_desc.m_nNumScale > 0)
		src.m_desc.m_vScale = src.m_desc.m_aScales[0];		// Make sure scale is set to the first scale key.
	if(src.m_desc.m_nNumCols > 0)
		src.m_desc.m_RGBA = src.m_desc.m_aRGBA[0];			// Make sure RGBA is set to the first RGBA key.

	m_pParticles[i] = src;
	m_pTail = &m_pParticles[i];
	m_uiActiveParticles++;

	m_uiTicksSinceLastSpawn = 0;
	}

/*!***********************************************************************
 @Function		AddEmitter
 @Access		public 
 @Param			Particle * pParent
 @Returns		Emitter*
 @Description	
*************************************************************************/
Emitter* Emitter::AddEmitter(const Particle* pParent, ParticleDelegate* pDelegate)
	{
	ASSERT(m_pEmitters);

	Uint32 uiIndex = 0xFFFFFFFF;
	// Find a free emitter with no parent
	for(Uint32 i = 0; i < m_uiMaxEmitterChildren; i++)
		{
		if(m_pEmitters[i].m_pParent == NULL)
			{
			uiIndex = i;
			break;
			}
		}

	ASSERT(uiIndex != 0xFFFFFFFF);

	m_pEmitters[uiIndex].m_pDelegate = pDelegate;
	m_pEmitters[uiIndex].m_pParent   = pParent;

	return &m_pEmitters[uiIndex];
	}

/*!***********************************************************************
 @Function		Tail
 @Access		public 
 @Returns		const Particle&
 @Description	
*************************************************************************/
const Particle* Emitter::Tail()
	{
	return m_pTail;
	}

/*!***********************************************************************
 @Function		SetAll
 @Access		public 
 @Param			enumVALUE eValue
 @Param			Float32 fVal
 @Returns		void
 @Description	
*************************************************************************/
void Emitter::SetAll(enumVALUE eValue, Float32 fVal)
	{
	switch(eValue)
		{
		case enumVALUE_Life:
			for(Uint32 uiIndex = 0; uiIndex < m_uiActiveParticles; uiIndex++)
				m_pParticles[uiIndex].m_desc.m_fLife = fVal;	
			break;
		default:
			ASSERT(!"Uhandled value");
		}	
	}

/*!***********************************************************************
 @Function		Update
 @Access		public 
 @Param			double fDT
 @Returns		void
 @Description	
*************************************************************************/
void Emitter::Update(double fDT)
	{
	if(m_pDelegate && m_bIsActive)
		{
		m_pDelegate->OnEmitterUpdate(this, m_pParent, m_uiTicksSinceLastSpawn);
		}
		
	m_uiTicksSinceLastSpawn++;

	// --- Update all particles
	Uint32 uiIndex;
	for(uiIndex = 0; uiIndex < m_uiActiveParticles; uiIndex++)
		m_pParticles[uiIndex].Update(fDT);

	// --- Kill dead particles
	for(uiIndex = 0; uiIndex < m_uiActiveParticles; uiIndex++)
		{
		if(!m_pParticles[uiIndex].m_bIsValid)
			{
			Uint32 uiLastIndex = m_uiActiveParticles - 1;
			if(m_pEmitters)
				{
				Emitter* pChild = FindChildFromParent(&m_pParticles[uiIndex]);
				ASSERT(pChild);
				pChild->Stop();
				pChild->m_pParent = NULL;
				}

			if(uiIndex != uiLastIndex)
				{
				m_pParticles[uiIndex].m_desc = m_pParticles[uiLastIndex].m_desc;		// Replace with last particle.
				m_pParticles[uiIndex].m_bIsValid = true;
				if(m_pEmitters)
					{
					Emitter* pChild = FindChildFromParent(&m_pParticles[uiLastIndex]);
					ASSERT(pChild);
					pChild->m_pParent = &m_pParticles[uiIndex];
					}
				}

			m_uiActiveParticles--;
			}
		}

	// --- Update child emitters
	if(m_pEmitters)
		{
		for(uiIndex = 0; uiIndex < m_uiMaxEmitterChildren; uiIndex++)
			if(m_pEmitters[uiIndex].IsValid())
				m_pEmitters[uiIndex].Update(fDT);
		}
	}

/*!***********************************************************************
 @Function		FindChildFromParent
 @Access		private 
 @Param			Particle * pParent
 @Returns		Emitter*
 @Description	
*************************************************************************/
Emitter* Emitter::FindChildFromParent(const Particle* pParent)
	{
	for(Uint32 uiEmitter = 0; uiEmitter < m_uiMaxEmitterChildren; uiEmitter++)
		{
		if(m_pEmitters[uiEmitter].m_pParent == pParent)
			{
			return &m_pEmitters[uiEmitter];
			}
			
		}

	return NULL;
	}
