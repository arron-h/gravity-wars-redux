#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H

enum enumPARTICLEMODE
	{
	enumPARTICLEMODE_Default		= 0x0,
	enumPARTICLEMODE_None			= 0x1,
	enumPARTICLEMODE_Scale			= 0x2,
	enumPARTICLEMODE_Colour			= 0x4,
	enumPARTICLEMODE_Rotate			= 0x8,
	};

enum enumPARTICLETYPE
	{
	enumPARTICLETYPE_Invalid,
	enumPARTICLETYPE_Points,
	enumPARTICLETYPE_Quads,
	};

#define MAX_KEYFRAMES 8

class Particle;
class Emitter;
class ParticleDelegate
	{
	public:
		virtual void OnEmitterUpdate(Emitter* pEmitter, const Particle* pParticleParent, Uint32 uiTicksSinceSpawn) = 0;
	};

class ParticleDesc
	{
	public:
		enumPARTICLEMODE	m_eMode;
		
		PVRTVec3			m_vPos;
		PVRTVec3    		m_vVel;
		PVRTVec3    		m_vAcc;
		Float32				m_fDrag;

		Float32   			m_fRotCur;
		Float32   			m_fRotVel;
		Float32   			m_fRotAcc;
		Float32   			m_fRotDrag;

		PVRTVec3			m_vScale;
		Float32				m_fAge;
		Float32				m_fWeight;
		//Vector3			m_vRandomForce;

		RGBA				m_RGBA;

		Float32				m_fLife;		// Life < 0 means stay on infinitely.

		Uint32				m_nNumCols;
		RGBA				m_aRGBA[MAX_KEYFRAMES];

		Uint32				m_nNumScale;
		PVRTVec3			m_aScales[MAX_KEYFRAMES];

	private:
		void CalcKeyframeTime(Uint32 nTotal, Uint32 &nKeyA, Uint32 &nKeyB, Float32 &fInterpolator);

	public:
		void Reset();
		void Update(double fDT);
	};

class Particle
	{
	friend class Emitter;

	public:
		bool				m_bIsValid;
		ParticleDesc		m_desc;

	private:
		enumPARTICLETYPE	m_eType;
		void*				m_pPrimitive;
		Emitter*			m_pParent;

		void BuildPoints();
		void BuildQuads();

	public:
		Particle() : m_bIsValid(false) {}
		void Reset();
		void Update(double fDT);
		void BuildGeometry();
	};

class Emitter
	{
	friend class Particle;
	public:
		enum enumVALUE
			{
			enumVALUE_Life,
			};

		enum enumBLENDMODE
			{
			enumBLENDMODE_Standard,
			enumBLENDMODE_Additive,
			};

	private:
		TextureRef					m_tex;
		bool						m_bIsValid;
		bool						m_bIsActive;
		enumBLENDMODE				m_eBlendMode;

		// For child emitter
		Uint32						m_uiTicksSinceLastSpawn;
		const Particle*				m_pParent;
		ParticleDelegate*			m_pDelegate;
		
		Uint32						m_uiMaxParticles;
		Uint32						m_uiActiveParticles;
		Particle*					m_pParticles;
		Particle*					m_pTail;

		// Child Emitters
		Emitter*					m_pEmitters;
		Uint32						m_uiMaxEmitterChildren;

		// Primitives to render as particles
		enumPARTICLETYPE			m_eParticleType;
		PrimQuad*					m_pQuads;
		PrimPoint*					m_pPoints;

	private:
		Emitter* FindChildFromParent(const Particle* pParent);

	public:
		Emitter() : m_pParticles(NULL), m_pQuads(NULL), m_pPoints(NULL), m_pTail(NULL), 
					m_pEmitters(NULL), m_pParent(NULL), m_pDelegate(NULL), m_eBlendMode(enumBLENDMODE_Additive)  { Reset(); }
		virtual ~Emitter() { Reset(); }

		// Implementation dependent (declaration in ParticleSystemImpl.cpp)
		void Render();

		void Reset();
		void Initialise(TextureRef tex, enumPARTICLETYPE eType, Uint32 uiMaxParticles = 256, Uint32 uiNumChildren = 0);
		void AddParticle(Particle& src);
		Emitter* AddEmitter(const Particle* pParent, ParticleDelegate* pDelegate);
		void Update(double fDT);

		const Particle* Tail();

		void SetAll(enumVALUE eValue, Float32 fVal);
		void SetBlendMode(enumBLENDMODE eMode) { m_eBlendMode = eMode; }
	
		inline void Stop()		{ m_bIsActive = false; }
		inline void Start()		{ m_bIsActive = true; m_uiActiveParticles = 0; m_pTail = NULL; }
		inline bool IsActive()	{ return m_bIsActive; }
		inline bool IsValid()	{ return m_bIsValid; }
		inline Uint32 ActiveParticles() { return m_uiActiveParticles; }

#ifdef _DEBUG
		void DebugPrint()
			{
			DebugLog("Active Particles: %d", m_uiActiveParticles);
			}
#endif
	};

#endif
