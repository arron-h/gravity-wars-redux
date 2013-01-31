#ifndef AUDIOIMPL_H
#define AUDIOIMPL_H

#include <vector>
#include <PVRTString.h>
#include <mm/renderer.h>
#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alut.h>

// The number of max number of sound sources.
#define SOUNDMANAGER_MAX_NBR_OF_SOURCES 32

class AudioEngineImpl : public AudioEngine
	{
	private:
		mmr_context_t*				m_ctxt;
		mmr_connection_t*			m_connection;
		int							m_aoid;
		int							m_ActiveMusic;
		std::vector<CPVRTString>	m_MusicTrackFilenames;

		ALCcontext*					m_alContext;
		ALCdevice*					m_alDevice;
		ALuint						m_SoundSources[SOUNDMANAGER_MAX_NBR_OF_SOURCES];

	public:
		AudioEngineImpl();
		~AudioEngineImpl();

		virtual AudioRef Load(const TXChar* c_pszFilename);
		virtual Float32 GetPlayLength(AudioRef Sound);
		virtual void SetPlayPosition(AudioRef Sound, Float32 fTimeS);

		virtual void Play(AudioRef Sound, bool bLoop);
		virtual void Stop(AudioRef Sound);
		virtual void SetVolume(AudioRef Sound, Float32_Clamp fVol);
		virtual void StopAll();
	};

#endif
