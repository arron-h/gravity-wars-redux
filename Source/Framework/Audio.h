#ifndef AUDIO_H
#define AUDIO_H

static const Uint32 AUDIOENGINE_INVALID = 0xFBFBFBFB;

class AudioEngine
	{
	protected:
		bool		m_bInitialised;

	public:
		AudioEngine() 
			{ 
			m_bInitialised = false; 
			}

		virtual ~AudioEngine() {}

		virtual AudioRef Load(const TXChar* c_pszFilename) = 0;
		
		virtual Float32 GetPlayLength(AudioRef Sound) = 0;
		virtual void SetPlayPosition(AudioRef Sound, Float32 fTimeS) = 0;

		virtual void Play(AudioRef Sound, bool bLoop) = 0;
		virtual void Stop(AudioRef Sound) = 0;
		virtual void SetVolume(AudioRef Sound, Float32_Clamp fVol) = 0;
		
		virtual void StopAll() = 0;
	};

#endif
