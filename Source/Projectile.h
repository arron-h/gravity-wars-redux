#ifndef PROJECTILE_H
#define PROJECTILE_H

class Planet;
class Spaceship;
class Projectile
	{
	public:
		enum
			{
			PROJRESULT_INVALID		= 0x0,
			PROJRESULT_MISS			= 0x1,
			PROJRESULT_HITPLANET	= 0x2,
			PROJRESULT_HITSPACESHIP = 0x4,
			PROJRESULT_EXPIRED		= 0x8,
			PROJRESULT_OOB			= 0x10,
			};
	private:
		bool		m_bActive;
		PVRTVec3	m_vPosition;
		PVRTVec3	m_vVelocity;
		PVRTMat4	m_mxModel;
		
		Float32		m_fTime;
		Float32		m_fOOBTime;

		PVRTVec3	m_vTrailOffset;
		Emitter		m_TrailEmitter;
		Emitter		m_GuideTrailEmitter[2];			// 2 so we can fade one guide trail off while another is active
		Uint32		m_uiActiveTrail;
		RGBA		m_TrailColour;
				
		TextureRef	m_tex;
		ShaderRef	m_ProjShader;

	public:
		Projectile();

		void Launch(const PVRTVec3& vInitialPos, const PVRTVec3& vVelocity, const RGBA& c_rgbaTrailColour);
		void Hit();
		void Dissipate();

		Uint32 Update(double fDT, Planet* pPlanets, Uint32 uiNumPlanets, Spaceship* pShips, Uint32 uiNumShips);
		void DrawGeom();
	};

#endif
