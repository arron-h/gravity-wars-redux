#ifndef AUDIOIMPL_H
#define AUDIOIMPL_H

#include <vector>
#include <PVRTString.h>

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

// The number of max number of sound sources.
#define SOUNDMANAGER_MAX_NBR_OF_SOURCES 32

struct SAudioImplDesc
{
	SLObjectItf   fdPlayerObject;
	SLPlayItf     fdPlayerPlay;
	SLSeekItf     fdPlayerSeek;
	SLVolumeItf   fdPlayerVolume;
	int           playCount;
};

class AudioEngineImpl : public AudioEngine
{
private:
	SLObjectItf                   m_engineObject;
	SLEngineItf                   m_engineEngine;
	
	SLObjectItf                   m_output;
	
	std::vector<SAudioImplDesc>   m_Descs;
	
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
