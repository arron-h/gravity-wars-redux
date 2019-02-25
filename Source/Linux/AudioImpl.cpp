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
}

/*!***********************************************************************
 @Function		~AudioEngineImpl
 @Access		public 
 @Returns		
 @Description	
*************************************************************************/
AudioEngineImpl::~AudioEngineImpl()
{
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

	AudioStream* stream = new AudioStream();
	stream->m_eType = AudioStream::eType_Invalid;
	stream->m_uiHandle = 0;
		
	return stream;
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
	return 0.0f;
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
}

/*!***********************************************************************
 @Function		StopAll
 @Access		public 
 @Returns		void
 @Description	
*************************************************************************/
void AudioEngineImpl::StopAll()
{
}
