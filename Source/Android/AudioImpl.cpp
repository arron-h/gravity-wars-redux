#include <android/asset_manager.h>
#include "jaasl.h"

#include "stdafx.h"
#include "AudioImpl.h"

/*!***********************************************************************
 @Function		AudioEngineImpl
 @Access		public 
 @Returns		
 @Description	
*************************************************************************/
AudioEngineImpl::AudioEngineImpl()
{
	m_pSL = new JAASoundLib;
	if(!m_pSL->initialise())
	{
		DebugLog("Failed to initialise sound engine");
	}
}

/*!***********************************************************************
 @Function		~AudioEngineImpl
 @Access		public 
 @Returns		
 @Description	
*************************************************************************/
AudioEngineImpl::~AudioEngineImpl()
{
	m_pSL->destroy();
	
	delete m_pSL;
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
	FileStream* fs = RESMAN->OpenFile(c_pszFilename);
	if(!fs)
	{
		DebugLog("Failed to load file: %s", c_pszFilename);
		return NULL;
	}
		
	AAsset* asset = (AAsset*)fs->NativeFD();
	if(!asset)
	{
		DebugLog("Failed to retrieve native file descriptor: %s", c_pszFilename);
		delete fs;
		return NULL;
	}
	
	unsigned int hndl;
	if(!m_pSL->loadFromAsset(asset, hndl))
	{
		delete fs;
		return NULL;
	}
	
	AudioStream* pAS = new AudioStream;
	pAS->m_uiHandle = hndl;
	pAS->m_eType    = AudioStream::eType_Sfx;
	
	delete fs;

	return pAS;
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
	unsigned long l = m_pSL->playLength(Sound->m_uiHandle);
	
	DebugLog("Play length for handle %d is %d", Sound->m_uiHandle, l);
	
	return l / 1000.0f;
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
	m_pSL->setPlayPosition(Sound->m_uiHandle, (unsigned long)(fTimeS * 1000));
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
	m_pSL->setLooped(Sound->m_uiHandle, bLoop);
	m_pSL->play(Sound->m_uiHandle);
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
	m_pSL->setVolume(Sound->m_uiHandle, fVol);
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
	m_pSL->stop(Sound->m_uiHandle);
}

/*!***********************************************************************
 @Function		StopAll
 @Access		public 
 @Returns		void
 @Description	
*************************************************************************/
void AudioEngineImpl::StopAll()
{
	m_pSL->stopAll();
}
