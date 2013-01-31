#ifndef PLANET_H
#define PLANET_H

class Planet
	{
	private:
		PVRTVec3		m_vPos;
		ModelRef		m_Model;
		TextureRef		m_texSurface;
		TextureRef		m_texGlow;
		Uint32			m_uiCol;

		Float32			m_fRad;
		Float32			m_fMass;
		PVRTMat4		m_mxAxis;
		PVRTMat4		m_mxModel;
		PVRTMat4		m_mxLightRot;
		PVRTMat4		m_mxTrans;
		Float32			m_fRot;
		Float32			m_fSpeed;

		PVRTVec3		m_vLightDir;
		ShaderRef		m_SurfShader;
		ShaderRef		m_shaderMVP_V_T_C;

		Float32			m_fHitAmplitude;

		Emitter			m_Emitter;
		Emitter			m_Emitter2;

	public:
		Planet(); 
		~Planet();
		
		void Initialise(const PVRTVec3& vPos, const PVRTVec4& vLightDir, TextureRef tex, Uint32 uiCol, Float32 fRad, Float32 fMass, Float32 fAxis, Float32 fSpeed);
		void Update(double fDT);
		void DrawGeom();

		const PVRTVec3& GetPosition() { return m_vPos; }
		Float32 GetRadius() { return m_fRad; }
		Float32 GetMass() { return m_fMass; }

		void Hit(Uint32 uiAngle);
	};

#endif
