#ifndef SPACESHIP_H
#define SPACESHIP_H

class Projectile;
class Spaceship : public ParticleDelegate
	{
	public:
		enum enumSTATE
			{
			enumSTATE_OK,
			enumSTATE_Hit,
			enumSTATE_Destroyed,
			};

	private:
		TextureRef				m_tex;
		ShaderRef				m_shader;
		PVRTVec3				m_vPos;
		Rectanglef				m_quad;
		Rectanglef				m_bb;
		RGBA					m_rgbaCol;

		CollisionBitmap*		m_pColBitmap;

		// Explosion emitters
		Emitter					m_Emitter;
		Emitter					m_Emitter2;
		Interpolator<Float32>	m_ExplosionDisc;
		TextureRef				m_texExpDisc;

		enumSTATE				m_eState;

		Projectile*				m_pProjectile;

	public:
		Spaceship();
		~Spaceship();

		void Launch(const PVRTVec3& vVelocity);
		void Hit();

		void Initialise(const PVRTVec3& vPosition, const RGBA& c_rgba);
		void Render();
		void Update(double fDT);

		virtual void OnEmitterUpdate(Emitter* pEmitter, const Particle* pParticleParent, Uint32 uiTicksSinceSpawn);

		Projectile* GetProjectile();
		enumSTATE GetState() { return m_eState; }

		const PVRTVec3& GetPosition();
		const Rectanglef& GetBoundingBox();
		const CollisionBitmap* GetCollisionBitmap();
	};

#endif
