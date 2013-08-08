#include "stdafx.h"

#import <UIKit/UIKit.h>
#import <AVFoundation/AVFoundation.h>

#include "SoundEngine/SoundEngine.h"

#include "AudioImpl.h"


/*!***********************************************************************
 @Function		AudioEngineImpl
 @Access		public 
 @Returns		
 @Description	
*************************************************************************/
AudioEngineImpl::AudioEngineImpl()
	{
	// Initialise
	m_MusicStreams = [[NSMutableArray alloc] init];
	SoundEngine_Initialize(44100);
	}

/*!***********************************************************************
 @Function		~AudioEngineImpl
 @Access		public 
 @Returns		
 @Description	
*************************************************************************/
AudioEngineImpl::~AudioEngineImpl()
	{
	[m_MusicStreams release];
	SoundEngine_Teardown();
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
	// Bit of a hack but it will do for now. Assume all music is MP3.
	TXChar* pszExt = strrchr(c_pszFilename, '.') + 1;
	
	AudioStream* pStream = new AudioStream;
	pStream->m_uiHandle = 0;
	pStream->m_eType    = AudioStream::eType_Invalid;
	
	if((pszExt[0] == 'm' || pszExt[0] == 'M') && (pszExt[1] == 'p' || pszExt[1] == 'P') && pszExt[2] == '3')
	{
		// Load Music
		NSURL* MusicURL = [NSURL fileURLWithPath:[NSString stringWithFormat:@"%@/%s", [[NSBundle mainBundle] resourcePath], c_pszFilename]];
		NSError* Error;
		AVAudioPlayer* Music = [[AVAudioPlayer alloc] initWithContentsOfURL:MusicURL error:&Error];

		[m_MusicStreams addObject:Music];
		[Music release];	// Array owns it
		
		pStream->m_eType = AudioStream::eType_Music;
		pStream->m_uiHandle = [m_MusicStreams count] - 1;
	}
	else if((pszExt[0] == 'w' || pszExt[0] == 'W') && (pszExt[1] == 'a' || pszExt[1] == 'A') && (pszExt[2] == 'v' || pszExt[2] == 'V'))
	{
		TXChar szPath[1024];
		
		NSString* readPath = [[NSBundle mainBundle] resourcePath];
		[readPath getCString:szPath maxLength:1024 encoding:NSUTF8StringEncoding];
		strcat(szPath, "/");
		
		strcat(szPath, c_pszFilename);
		TXChar* pszPathExt = strrchr(szPath, '.') + 1;
		
		// iOS uses .caf
		pszPathExt[0] = 'c'; pszPathExt[1] = 'a'; pszPathExt[2] = 'f';
		
		UInt32 uiID;
		OSStatus result = SoundEngine_LoadEffect(szPath, &uiID);
		if(result != noErr)
			DebugLog("Error loading: %s", c_pszFilename);
		
		pStream->m_uiHandle = uiID;
		pStream->m_eType    = AudioStream::eType_Sfx;
	}

	return pStream;
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
	if(Sound->m_eType == AudioStream::eType_Music)
	{
		AVAudioPlayer* Music = [m_MusicStreams objectAtIndex:Sound->m_uiHandle];
		return Music.duration;
	}
		
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
	if(Sound->m_eType == AudioStream::eType_Music)
	{
		AVAudioPlayer* Music = [m_MusicStreams objectAtIndex:Sound->m_uiHandle];
		Music.currentTime = fTimeS;
	}
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
	if(Sound->m_eType == AudioStream::eType_Music)
	{
		AVAudioPlayer* Music = [m_MusicStreams objectAtIndex:Sound->m_uiHandle];
		[Music play];
		if(bLoop)
			Music.numberOfLoops = -1;
		m_ActiveMusic = Music;
		
		DebugLog("Playing music with ID: %d", Sound->m_uiHandle);
	}
	else 
	{
		SoundEngine_StartEffect(Sound->m_uiHandle);
	}
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
	// Sanity check
	if(fVol < 0.0f)		 fVol = 0.0f;
	else if(fVol > 1.0f) fVol = 1.0f;
	
	if(Sound->m_eType == AudioStream::eType_Music)
	{
		AVAudioPlayer* Music = [m_MusicStreams objectAtIndex:Sound->m_uiHandle];
		[Music setVolume:fVol];
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
	if(Sound->m_eType == AudioStream::eType_Music)
	{
		AVAudioPlayer* Music = [m_MusicStreams objectAtIndex:Sound->m_uiHandle];
		[Music stop];
		m_ActiveMusic = nil;
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
	if(m_ActiveMusic)
	{
		[m_ActiveMusic stop];
		DebugLog("Stopped active music");
	}
	m_ActiveMusic = nil;
}
	