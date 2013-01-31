#include "stdafx.h"
#include "AudioImpl.h"

using namespace irrklang;

/*!***********************************************************************
 @Function		AudioEngineImpl
 @Access		public 
 @Returns		
 @Description	
*************************************************************************/
AudioEngineImpl::AudioEngineImpl()
	{
	m_Engine = createIrrKlangDevice();
	m_bInitialised = true;
	}

/*!***********************************************************************
 @Function		~AudioEngineImpl
 @Access		public 
 @Returns		
 @Description	
*************************************************************************/
AudioEngineImpl::~AudioEngineImpl()
	{
	// Drop any held effects
	for(Uint32 i = 0; i < m_Streams.size(); ++i)
		{
		if(m_Streams[i].m_pSoundID)
			m_Streams[i].m_pSoundID->drop();
		}

	m_Engine->drop();
	}

/*!***********************************************************************
 @Function		Load
 @Access		public 
 @Param			const TXChar * c_pszFilename
 @Returns		AudioRef
 @Description	
*************************************************************************/
AudioRef AudioEngineImpl::Load(const TXChar* c_pszFilename)
	{
	ISoundSource* pSource = m_Engine->addSoundSourceFromFile(c_pszFilename);
	if(!pSource)
		return AUDIOENGINE_INVALID;

	AudioStreamImpl streamdata;
	streamdata.m_pSourceHandle = pSource;

	Uint32 uiHandle = m_Streams.size();
	m_Streams.push_back(streamdata);

	return uiHandle;
	}

/*!***********************************************************************
 @Function		GetPlayLength
 @Access		public 
 @Param			AudioRef Sound
 @Returns		Float32
 @Description	
*************************************************************************/
Float32 AudioEngineImpl::GetPlayLength(AudioRef Sound)
	{
	ISoundSource* pSource = m_Streams[Sound].m_pSourceHandle;
	if(!pSource)
		return 0.0f;

	return (pSource->getPlayLength() / 1000.0f);
	}

/*!***********************************************************************
 @Function		SetPlayPosition
 @Access		public 
 @Param			AudioRef Sound
 @Param			Float32 fTimeS
 @Returns		void
 @Description	
*************************************************************************/
void AudioEngineImpl::SetPlayPosition(AudioRef Sound, Float32 fTimeS)
	{
	ISound* pSoundID = m_Streams[Sound].m_pSoundID;
	if(!pSoundID)
		return;		// Can't set play position!

	pSoundID->setPlayPosition((ik_u32)(fTimeS * 1000));
	}

/*!***********************************************************************
 @Function		Play
 @Access		public 
 @Param			AudioRef Sound
 @Returns		void
 @Description	
*************************************************************************/
void AudioEngineImpl::Play(AudioRef Sound, bool bLoop)
	{
	ISoundSource* pSource = m_Streams[Sound].m_pSourceHandle;
	if(!pSource)
		return;

	m_Streams[Sound].m_pSoundID = m_Engine->play2D(pSource, false, false, true);
	m_Streams[Sound].m_pSoundID->setIsLooped(bLoop);
	}

/*!***********************************************************************
 @Function		SetVolume
 @Access		public 
 @Param			AudioRef Sound
 @Returns		void
 @Description	
*************************************************************************/
void AudioEngineImpl::SetVolume(AudioRef Sound, Float32_Clamp fVol)
	{
	ISound* pSoundID = m_Streams[Sound].m_pSoundID;
	if(!pSoundID)
		return;		// Can't set volume!

	pSoundID->setVolume(fVol);
	}

/*!***********************************************************************
 @Function		Stop
 @Access		public 
 @Param			AudioRef Sound
 @Returns		void
 @Description	
*************************************************************************/
void AudioEngineImpl::Stop(AudioRef Sound)
	{
	ISound* pSoundID = m_Streams[Sound].m_pSoundID;
	if(!pSoundID)
		return;		// Can't set volume!

	pSoundID->stop();
	m_Streams[Sound].m_pSoundID = NULL;
	}

/*!***********************************************************************
 @Function		StopAll
 @Access		public 
 @Returns		void
 @Description	
*************************************************************************/
void AudioEngineImpl::StopAll()
	{
	m_Engine->stopAllSounds();
	}