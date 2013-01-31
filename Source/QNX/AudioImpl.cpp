#include <bps/audiomixer.h>
#include <mm/renderer.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "stdafx.h"
#include "AudioImpl.h"

#if ! defined(DEFAULT_AUDIO_OUT)
    #define DEFAULT_AUDIO_OUT "audio:default"
#endif

#define AUDIOTYPE_MUSIC 1
#define AUDIOTYPE_SFX   2

// Error message function for OpenAL.
static void reportOpenALError(ALenum error)
{
    if (error != AL_NO_ERROR)
    	DebugLog("OpenAL reported the following error: %s", alutGetErrorString(error));
}

bool loadWav(FILE* file, ALuint buffer)
{
	unsigned char stream[12];

	// Verify the wave fmt magic value meaning format.
	if (fread(stream, 1, 8, file) != 8 || memcmp(stream, "fmt ", 4) != 0 ) 	{
		DebugLog("Failed to verify the magic value for the wave file format.");
		return false;
	}

	unsigned int section_size;
	section_size  = stream[7]<<24;
	section_size |= stream[6]<<16;
	section_size |= stream[5]<<8;
	section_size |= stream[4];

	// Check for a valid pcm format.
	if (fread(stream, 1, 2, file) != 2 || stream[1] != 0 || stream[0] != 1) {
		DebugLog("Unsupported audio file format (must be a valid PCM format).");
		return false;
	}

	// Get the channel count (16-bit little-endian).
	int channels;
	if (fread(stream, 1, 2, file) != 2) {
		DebugLog("Failed to read the wave file's channel count.");
		return false;
	}
	channels  = stream[1]<<8;
	channels |= stream[0];

	// Get the sample frequency (32-bit little-endian).
	ALuint frequency;
	if (fread(stream, 1, 4, file) != 4) {
		DebugLog("Failed to read the wave file's sample frequency.");
		return false;
	}

	frequency  = stream[3]<<24;
	frequency |= stream[2]<<16;
	frequency |= stream[1]<<8;
	frequency |= stream[0];

	// The next 6 bytes hold the block size and bytes-per-second.
	// We don't need that info, so just read and ignore it.
	// We could use this later if we need to know the duration.
	if (fread(stream, 1, 6, file) != 6) {
		DebugLog("Failed to read past the wave file's block size and bytes-per-second.");
		return false;
	}

	// Get the bit depth (16-bit little-endian).
	int bits;
	if (fread(stream, 1, 2, file) != 2) {
		DebugLog("Failed to read the wave file's bit depth.");
		return false;
	}
	bits  = stream[1]<<8;
	bits |= stream[0];

	// Now convert the given channel count and bit depth into an OpenAL format.
	ALuint format = 0;
	if (bits == 8) {
		if (channels == 1)
			format = AL_FORMAT_MONO8;
		else if (channels == 2)
			format = AL_FORMAT_STEREO8;
	}
	else if (bits == 16) {
		if (channels == 1)
			format = AL_FORMAT_MONO16;
		else if (channels == 2)
			format = AL_FORMAT_STEREO16;
	}
	else {
		DebugLog("Incompatible wave file format: ( %d, %d)", channels, bits);
		return false;
	}

	// Check against the size of the format header as there may be more data that we need to read.
	if (section_size > 16) {
		unsigned int length = section_size - 16;

		// Extension size is 2 bytes.
		if (fread(stream, 1, length, file) != length) {
			DebugLog("Failed to read extension size from wave file.");
			return false;
		}
	}

	// Read in the rest of the file a chunk (section) at a time.
	while (true) {
		// Check if we are at the end of the file without reading the data.
		if (feof(file)) {
			DebugLog("Failed to load wave file; file appears to have no data.");
			return false;
		}

		// Read in the type of the next section of the file.
		if (fread(stream, 1, 4, file) != 4) {
			DebugLog("Failed to read next section type from wave file.");
			return false;
		}

		// Data chunk.
		if (memcmp(stream, "data", 4) == 0) {
			// Read how much data is remaining and buffer it up.
			unsigned int dataSize;
			if (fread(&dataSize, sizeof(int), 1, file) != 1) {
				DebugLog("Failed to read size of data section from wave file.");
				return false;
			}

			char* data = new char[dataSize];
			if (fread(data, sizeof(char), dataSize, file) != dataSize) 	{
				DebugLog("Failed to load wave file; file is missing data.");
				delete data;
				return false;
			}

			alBufferData(buffer, format, data, dataSize, frequency);
			ALenum error = alGetError();
			if (error != AL_NO_ERROR)
			{
				DebugLog("Buffer data failed to upload.");
				reportOpenALError(error);
			}
			delete data;

			// We've read the data, so return now.
			return true;
		}
		// Other chunk - could be any of the following:
		// - Fact ("fact")
		// - Wave List ("wavl")
		// - Silent ("slnt")
		// - Cue ("cue ")
		// - Playlist ("plst")
		// - Associated Data List ("list")
		// - Label ("labl")
		// - Note ("note")
		// - Labeled Text ("ltxt")
		// - Sampler ("smpl")
		// - Instrument ("inst")
		else {
			// Store the name of the chunk so we can report errors informatively.
			char chunk[5] = { 0 };
			memcpy(chunk, stream, 4);

			// Read the chunk size.
			if (fread(stream, 1, 4, file) != 4) {
				DebugLog("Failed to read size of chunk from wave file.");
				return false;
			}

			section_size  = stream[3]<<24;
			section_size |= stream[2]<<16;
			section_size |= stream[1]<<8;
			section_size |= stream[0];

			// Seek past the chunk.
			if (fseek(file, section_size, SEEK_CUR) != 0) {
				DebugLog("Failed to seek past chunk in wave file.");
				return false;
			}
		}
	}

	return true;
}

/*!***********************************************************************
 @Function		AudioEngineImpl
 @Access		public 
 @Returns		
 @Description	
*************************************************************************/
AudioEngineImpl::AudioEngineImpl()
	: m_ctxt(NULL), m_connection(NULL), m_aoid(0), m_ActiveMusic(0),
	  m_alContext(NULL), m_alDevice(NULL)
	{
	// --- Initialise QNX Audio library
	const char *mmrname = NULL;
	const char *ctxtname = "gwredux";
	const char *audioout = DEFAULT_AUDIO_OUT;

	mode_t mode = S_IRUSR | S_IXUSR;
	strm_dict_t *aoparams = NULL; // output parameters

	m_connection = mmr_connect(mmrname);
	if(m_connection == NULL)
		return;

	m_ctxt = mmr_context_create(m_connection, ctxtname, 0, mode);
	if(m_ctxt == NULL)
		return;

	// Get output ID
	m_aoid = mmr_output_attach(m_ctxt, audioout, "audio");
	if(m_aoid < 0)
		return;

	strm_dict_t * dict = strm_dict_new();
	dict = strm_dict_set(dict, "volume", "100");
	mmr_output_parameters(m_ctxt, m_aoid, dict);
	strm_dict_destroy(dict);

	// --- Initialise OpenAL
	if (!alutInit(NULL, NULL))
		{
		DebugLog("alutInit failed.");
		}

	m_alDevice = alcOpenDevice(NULL);
	if (m_alDevice == NULL)
		{
		DebugLog("alcOpenDevice failed.");
		}

	//Create a context
	m_alContext = alcCreateContext(m_alDevice, NULL);

	// Generate a number of sources used to attach buffers and play.
	alGenSources(SOUNDMANAGER_MAX_NBR_OF_SOURCES, m_SoundSources);
	ALenum error = alGetError();
	if (error != AL_NO_ERROR)
	{
		DebugLog("An error occurred generating sources: %d", error);
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
	if(m_ctxt)
	{
		mmr_stop(m_ctxt);
		mmr_input_detach(m_ctxt);
		mmr_context_destroy(m_ctxt);
	}

	if(m_connection)
		mmr_disconnect( m_connection );
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
	TXChar szPath[1024];
	RESMAN->GetResourcePath(szPath, 1024, enumRESTYPE_SFX);
	strcat(szPath, c_pszFilename);

	AudioStream* pStream = new AudioStream;

	// Bit of a hack but it will do for now. Assume all music is MP3.
	const char* pszExt = strrchr(c_pszFilename, '.') + 1;
	if((pszExt[0] == 'm' || pszExt[0] == 'M') && (pszExt[1] == 'p' || pszExt[1] == 'P') && (pszExt[2] == '3'))
		{
		// Load Music (just append filename so we can load later)
		char cwd[PATH_MAX];
		char inputurl[PATH_MAX];
		getcwd(cwd, PATH_MAX);
		snprintf(inputurl, PATH_MAX, "file://%s/%s", cwd, szPath);

		m_MusicTrackFilenames.push_back(inputurl);
		int id = m_MusicTrackFilenames.size();

		pStream->m_eType = AudioStream::eType_Music;
		pStream->m_uiHandle = id;
		}
	else if((pszExt[0] == 'w' || pszExt[0] == 'W') && (pszExt[1] == 'a' || pszExt[1] == 'A') && (pszExt[2] == 'v' || pszExt[2] == 'V'))
		{
		// Load SFX
		ALuint uiBufferID;
		// Generate buffers to hold audio data.
		alGenBuffers(1, &uiBufferID);

		ALenum error = alGetError();
		if (error != AL_NO_ERROR)
		{
			reportOpenALError(error);
			DebugLog("Failed to generate buffer ID");
			delete pStream;
			return NULL;
		}

		// Load sound file.
		FILE* file = fopen(szPath, "rb");
		if (!file)
		{
			DebugLog("Failed to load audio file: %s", szPath);
			delete pStream;
			return NULL;
		}

		// Read the file header
		char header[12];
		if (fread(header, 1, 12, file) != 12)
		{
			DebugLog("Invalid header for audio file %s", szPath);
			alDeleteBuffers(1, &uiBufferID);
			goto cleanup;
		}

		// Check the file format & load the buffer with audio data.
		if (memcmp(header, "RIFF", 4) == 0)
		{
			if (!loadWav(file, uiBufferID))
			{
				DebugLog("Invalid wav file: %s", szPath);
				alDeleteBuffers(1, &uiBufferID);
				goto cleanup;
			}
		}
		else
		{
			DebugLog("Unsupported audio file: %s", szPath);
			goto cleanup;
		}

		DebugLog("SFX '%s' with ID: %u loaded successfully.", szPath, uiBufferID);

		cleanup:
			if (file)
			{
				fclose(file);
			}

		pStream->m_eType = AudioStream::eType_Sfx;
		pStream->m_uiHandle = uiBufferID;
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
	/*
	ISoundSource* pSource = m_Streams[Sound].m_pSourceHandle;
	if(!pSource)
		return 0.0f;

	return (pSource->getPlayLength() / 1000.0f);*/
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
	/*
	ISound* pSoundID = m_Streams[Sound].m_pSoundID;
	if(!pSoundID)
		return;		// Can't set play position!

	pSoundID->setPlayPosition((ik_u32)(fTimeS * 1000));*/
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
	const char *inputtype = "track";

	if(!Sound)
		return;

	unsigned int uiID = Sound->m_uiHandle;
	if(Sound->m_eType == AudioStream::eType_Music && uiID >= 1)
		{

		CPVRTString& val = m_MusicTrackFilenames[uiID-1];

		if(m_ActiveMusic)
		{
			mmr_stop(m_ctxt);
			mmr_input_detach(m_ctxt);
			m_ActiveMusic = 0;
		}

		if(mmr_input_attach(m_ctxt, val.c_str(), inputtype) < 0)
			return;
		if(mmr_play(m_ctxt) < 0)
			return;

//		SetVolume(Sound, 1.0f);

		m_ActiveMusic = uiID;
		DebugLog("Playing music with ID: %d", uiID);
		}
	else
		{
		static uint sourceIndex = 0;

		// Get the corresponding buffer id set up in the init function.
		ALuint bufferID = uiID;

		if (bufferID != 0)
			{
			// Increment which source we are using, so that we play in a "free" source.
			sourceIndex = (sourceIndex + 1) % SOUNDMANAGER_MAX_NBR_OF_SOURCES;

			// Get the source in which the sound will be played.
			ALuint source = m_SoundSources[sourceIndex];

			if (alIsSource (source) == AL_TRUE)
				{
				// Attach the buffer to an available source.
				alSourcei(source, AL_BUFFER, bufferID);

				ALenum error = alGetError();
				if (error != AL_NO_ERROR)
					{
					reportOpenALError(error);
					}

				// Play the source.
				alSourcePlay(source);

				error = alGetError();
				if (error != AL_NO_ERROR)
					{
					reportOpenALError(error);
					}
				}
			}
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
	if(!Sound)
		return;

	// Sanity check
	if(fVol < 0.0f)		 fVol = 0.0f;
	else if(fVol > 1.0f) fVol = 1.0f;

	if(Sound->m_eType == AudioStream::eType_Music)
		{
		char szBuf[100];
		int ivol = (int)(fVol*100.f);
		sprintf(szBuf, "%d", ivol);

		strm_dict_t * dict = strm_dict_new();
		dict = strm_dict_set(dict, "volume", szBuf);

		mmr_output_parameters(m_ctxt, m_aoid, dict);

		strm_dict_destroy(dict);
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
	if(!Sound)
		return;

	if(Sound->m_eType == AudioStream::eType_Music && m_ActiveMusic)
		{
		mmr_stop(m_ctxt);
		mmr_input_detach(m_ctxt);
		m_ActiveMusic = 0;
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
		mmr_stop(m_ctxt);
		mmr_input_detach(m_ctxt);
		m_ActiveMusic = 0;
		}
	}
