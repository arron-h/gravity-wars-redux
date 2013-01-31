#ifndef AUDIOIMPL_H
#define AUDIOIMPL_H

#include <irrKlang.h>
#include <vector>

class AudioEngineImpl : public AudioEngine
	{
	private:
		struct AudioStreamImpl
			{
			irrklang::ISound*				m_pSoundID;
			irrklang::ISoundSource*			m_pSourceHandle;

			AudioStreamImpl() : m_pSoundID(NULL), m_pSourceHandle(NULL) {}
			};


		irrklang::ISoundEngine*			m_Engine;
		std::vector<AudioStreamImpl>	m_Streams;

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
