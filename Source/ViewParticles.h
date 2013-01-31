#ifndef VIEWPARTICLES_H
#define VIEWPARTICLES_H

class ViewParticles : public View, public ParticleDelegate
	{
	private:
		Camera			m_cam;
		Emitter			m_Emitter;
		Emitter			m_Emitter2;

	public:
		ViewParticles();

		virtual void Render();
		virtual void Update(double dt);
		virtual void OnEmitterUpdate(Emitter* pEmitter, const Particle* pParticleParent, Uint32 uiTicksSinceSpawn);

		virtual void OnTouchUp(Touch* pTouches, Uint32 uiNum);
	};

#endif
