#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <android/asset_manager.h>

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
	m_bInitialised = false;
	
	// create engine
    if(slCreateEngine(&m_engineObject, 0, NULL, 0, NULL, NULL) != SL_RESULT_SUCCESS)
	{
		DebugLog("slCreateEngine failed");
		return;
	}
	
	// realize the engine
	if((*m_engineObject)->Realize(m_engineObject, SL_BOOLEAN_FALSE) != SL_RESULT_SUCCESS)
	{
		DebugLog("Realize failed");
		return;
	}
	
    // get the engine interface, which is needed in order to create other objects
    if((*m_engineObject)->GetInterface(m_engineObject, SL_IID_ENGINE, &m_engineEngine) != SL_RESULT_SUCCESS)
	{
		DebugLog("GetInterface failed");
		return;
	}
	
	// create output mix
	if((*m_engineEngine)->CreateOutputMix(m_engineEngine, &m_output, 0, NULL, NULL) != SL_RESULT_SUCCESS)
	{
		DebugLog("CreateOutputMix failed");
		return;
	}
	
	// realize output mix
	if((*m_output)->Realize(m_output, SL_BOOLEAN_FALSE) != SL_RESULT_SUCCESS)
	{
		DebugLog("OutputMix Realize failed");
		return;
	}
	
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
	// destroy player objects
	for(int i = 0; i < m_Descs.size(); ++i)
	{
		SAudioImplDesc& d = m_Descs[i];
		
		// destroy file descriptor audio player object, and invalidate all associated interfaces
		if (d.fdPlayerObject != NULL)
		{
			(*d.fdPlayerObject)->Destroy(d.fdPlayerObject);
			d.fdPlayerObject   = NULL;
			d.fdPlayerPlay     = NULL;
			d.fdPlayerSeek     = NULL;
			d.fdPlayerVolume   = NULL;
		}
	}

	// destroy output mix object, and invalidate all associated interfaces
    if (m_output != NULL)
	{
        (*m_output)->Destroy(m_output);
        m_output = NULL;
    }

	// destroy engine object, and invalidate all associated interfaces
	if (m_engineObject != NULL)
	{
        (*m_engineObject)->Destroy(m_engineObject);
        m_engineObject = NULL;
        m_engineEngine = NULL;
    }
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
		return NULL;
	}
		
	off_t start, length;
    int fd = AAsset_openFileDescriptor(asset, &start, &length);
    delete fs;
	
	SLObjectItf   fdPlayerObject   = NULL;
	SLPlayItf     fdPlayerPlay     = NULL;
	SLSeekItf     fdPlayerSeek     = NULL;
	SLVolumeItf   fdPlayerVolume   = NULL;
	
	// configure audio source
    SLDataLocator_AndroidFD loc_fd = { SL_DATALOCATOR_ANDROIDFD, fd, start, length };
	SLDataFormat_MIME format_mime  = { SL_DATAFORMAT_MIME, NULL, SL_CONTAINERTYPE_UNSPECIFIED };
    SLDataSource audioSrc          = { &loc_fd, &format_mime };
	
	// configure audio sink
    SLDataLocator_OutputMix loc_outmix = { SL_DATALOCATOR_OUTPUTMIX, m_output };
    SLDataSink audioSnk                = { &loc_outmix, NULL };
	
	// create audio player
    const SLInterfaceID ids[3] = {SL_IID_SEEK, SL_IID_MUTESOLO, SL_IID_VOLUME};
    const SLboolean req[3]     = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE};
    if((*m_engineEngine)->CreateAudioPlayer(m_engineEngine, &fdPlayerObject, &audioSrc, &audioSnk, 3, ids, req) != SL_RESULT_SUCCESS)
	{
		return NULL;
	}

	// realize the player
    if((*fdPlayerObject)->Realize(fdPlayerObject, SL_BOOLEAN_FALSE) != SL_RESULT_SUCCESS)
	{
		return NULL;
	}
	
    // get the play interface
    if((*fdPlayerObject)->GetInterface(fdPlayerObject, SL_IID_PLAY, &fdPlayerPlay) != SL_RESULT_SUCCESS)
	{
		return NULL;
	}
	
    // get the seek interface
    if((*fdPlayerObject)->GetInterface(fdPlayerObject, SL_IID_SEEK, &fdPlayerSeek) != SL_RESULT_SUCCESS)
	{
		return NULL;
	}
	
    // get the volume interface
    if((*fdPlayerObject)->GetInterface(fdPlayerObject, SL_IID_VOLUME, &fdPlayerVolume) != SL_RESULT_SUCCESS)
	{
		return NULL;
	}
	
	SAudioImplDesc d;
	d.fdPlayerObject   = fdPlayerObject;
	d.fdPlayerPlay     = fdPlayerPlay;
	d.fdPlayerSeek     = fdPlayerSeek;
	d.fdPlayerVolume   = fdPlayerVolume;
	d.playCount        = 0;
	
	int idx = m_Descs.size();
	m_Descs.push_back(d);
	
	AudioStream* pAS = new AudioStream;
	pAS->m_uiHandle = idx;
	pAS->m_eType    = AudioStream::eType_Invalid;

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
	Uint32 hndl             = Sound->m_uiHandle;
	const SAudioImplDesc& d = m_Descs[hndl];
	
	SLmillisecond msec = 0;
	if (NULL != d.fdPlayerPlay)
	{
		(*d.fdPlayerPlay)->GetDuration(d.fdPlayerPlay, &msec);
	}
	
	DebugLog("Clip ID %d play length is %d", hndl, msec / 1000.0f);
	
	return msec / 1000.0f;
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
	Uint32 hndl       = Sound->m_uiHandle;
	SAudioImplDesc& d = m_Descs[hndl];
	
	DebugLog("Attempting to play track: %d", hndl);
	
	// Set looping
	if(d.playCount == 0 && bLoop)
	{
		if(NULL != d.fdPlayerSeek)
		{
			(*d.fdPlayerSeek)->SetLoop(d.fdPlayerSeek, SL_BOOLEAN_TRUE, 0, SL_TIME_UNKNOWN);
		}
	}

	// make sure the asset audio player was created
    if (NULL != d.fdPlayerPlay)
	{
        // set the player's state
		(*d.fdPlayerPlay)->SetPlayState(d.fdPlayerPlay, SL_PLAYSTATE_STOPPED);
        (*d.fdPlayerPlay)->SetPlayState(d.fdPlayerPlay, SL_PLAYSTATE_PLAYING);
		
		d.playCount++;
    }
}

float gain_to_attenuation(float gain)
{
    return gain < 0.01F ? -96.0F : 20 * log10( gain );
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
	Uint32 hndl             = Sound->m_uiHandle;
	const SAudioImplDesc& d = m_Descs[hndl];
	
	SLmillibel millibel = (SLmillibel)(gain_to_attenuation(fVol) * 100);
	
	// make sure the asset audio player was created
    if (NULL != d.fdPlayerVolume)
	{
        // set the player's state
		DebugLog("Clip ID %d set volume to %d (%f)", hndl, millibel, fVol);
        (*d.fdPlayerVolume)->SetVolumeLevel(d.fdPlayerVolume, millibel);
    }
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
	Uint32 hndl             = Sound->m_uiHandle;
	const SAudioImplDesc& d = m_Descs[hndl];
	
	DebugLog("Attempting to stop track: %d", hndl);
	
	if (NULL != d.fdPlayerPlay)
	{
        // set the player's state
	    (*d.fdPlayerPlay)->SetPlayState(d.fdPlayerPlay, SL_PLAYSTATE_STOPPED);
    }
}

/*!***********************************************************************
 @Function		StopAll
 @Access		public 
 @Returns		void
 @Description	
*************************************************************************/
void AudioEngineImpl::StopAll()
{
	for(int i = 0; i < m_Descs.size(); ++i)
	{
		const SAudioImplDesc& d = m_Descs[i];
		if (NULL != d.fdPlayerPlay)
		{
			// set the player's state
			(*d.fdPlayerPlay)->SetPlayState(d.fdPlayerPlay, SL_PLAYSTATE_STOPPED);
		}
	}
}
