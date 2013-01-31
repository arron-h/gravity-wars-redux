#include "stdafx.h"

#import <UIKit/UIKit.h>
#import <AVFoundation/AVFoundation.h>

#include "SoundEngine/SoundEngine.h"

#include "AudioImpl.h"


#define AUDIOTYPE_MUSIC 1
#define AUDIOTYPE_SFX   2


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
	Uint32 uiIndex = 0;
	TXChar* pszExt = strrchr(c_pszFilename, '.') + 1;
	if(pszExt[0] == 'm' || pszExt[0] == 'M' && pszExt[1] == 'p' || pszExt[1] == 'P' && pszExt[2] == '3')
		{
		// Load Music
		NSURL* MusicURL = [NSURL fileURLWithPath:[NSString stringWithFormat:@"%@/%s", [[NSBundle mainBundle] resourcePath], c_pszFilename]];
		NSError* Error;
		AVAudioPlayer* Music = [[AVAudioPlayer alloc] initWithContentsOfURL:MusicURL error:&Error];

		uiIndex = (AUDIOTYPE_MUSIC << 16) | ([m_MusicStreams count] & 0xFFFF);
		[m_MusicStreams addObject:Music];
		[Music release];	// Array owns it
		}
	else if(pszExt[0] == 'w' || pszExt[0] == 'W' && pszExt[1] == 'a' || pszExt[1] == 'A' && pszExt[2] == 'v' || pszExt[2] == 'V')
		{
		TXChar szPath[1024];
		RESMAN->GetResourcePath(szPath, 1024, enumRESTYPE_SFX);
		strcat(szPath, c_pszFilename);
		TXChar* pszPathExt = strrchr(szPath, '.') + 1;
		
		// iOS uses .caf
		pszPathExt[0] = 'c'; pszPathExt[1] = 'a'; pszPathExt[2] = 'f';
		
		UInt32 uiID;
		OSStatus result = SoundEngine_LoadEffect(szPath, &uiID);
		if(result != noErr)
			DebugLog("Error loading: %s", c_pszFilename);
		
		uiIndex = (AUDIOTYPE_SFX << 16) | (uiID & 0xFFFF);
		}

	return uiIndex;
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
	Uint32 uiAudioType = (Sound >> 16);
	if(uiAudioType == AUDIOTYPE_MUSIC)
		{
		Uint32 uiIdx = Sound & 0xFFFF;
		AVAudioPlayer* Music = [m_MusicStreams objectAtIndex:uiIdx];
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
	Uint32 uiAudioType = (Sound >> 16);
	if(uiAudioType == AUDIOTYPE_MUSIC)
		{
		Uint32 uiIdx = Sound & 0xFFFF;
		AVAudioPlayer* Music = [m_MusicStreams objectAtIndex:uiIdx];
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
	Uint32 uiAudioType = (Sound >> 16);
	if(uiAudioType == AUDIOTYPE_MUSIC)
		{
		Uint32 uiIdx = Sound & 0xFFFF;
		AVAudioPlayer* Music = [m_MusicStreams objectAtIndex:uiIdx];
		[Music play];
		if(bLoop)
			Music.numberOfLoops = -1;
		m_ActiveMusic = Music;
		
		DebugLog("Playing music with ID: %d", uiIdx);
		}
	else 
		{
		Uint32 uiID = Sound & 0xFFFF;
		SoundEngine_StartEffect(uiID);
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
	
	Uint32 uiAudioType = (Sound >> 16);
	if(uiAudioType == AUDIOTYPE_MUSIC)
		{
		Uint32 uiIdx = Sound & 0xFFFF;
		AVAudioPlayer* Music = [m_MusicStreams objectAtIndex:uiIdx];
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
	Uint32 uiAudioType = (Sound >> 16);
	if(uiAudioType == AUDIOTYPE_MUSIC)
		{
		Uint32 uiIdx = Sound & 0xFFFF;
		AVAudioPlayer* Music = [m_MusicStreams objectAtIndex:uiIdx];
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
	