#ifndef AUDIOIMPL_H
#define AUDIOIMPL_H

class JAASoundLib;
class AudioEngineImpl : public AudioEngine
{
private:
	JAASoundLib*      m_pSL;
	
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
