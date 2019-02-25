#include "stdafx.h"
#include "ResourceImpl.h"
#include "PVRTools.h"
#include "PVRTTexture.h"
#include "UtilsImpl.h"

#include "png.h"

#include <GLES2/gl2.h>

/*!***************************************************************************
 @Function		~FileStreamImpl
 @Description   Destructor
 *****************************************************************************/
FileStreamImpl::FileStreamImpl() : m_nativeFile(NULL)
{
}

/*!***************************************************************************
 @Function		~FileStreamImpl
 @Description   Destructor
 *****************************************************************************/
FileStreamImpl::~FileStreamImpl()
{
	Close();
}

/*!***************************************************************************
 @Function		Read
 @Input         data
 @Input         size
 @Input         count
 @Return		size_t
 @Description   fread
 *****************************************************************************/
size_t FileStreamImpl::Read(void* data, size_t size, size_t count)
{
	size_t dataRead = fread(data, size, count, m_nativeFile);

	return dataRead;
}

/*!***************************************************************************
 @Function		Write
 @Input         data
 @Input         size
 @Input         count
 @Return		size_t
 @Description   fread
 *****************************************************************************/
size_t FileStreamImpl::Write(const void* data, size_t size, size_t count)
{
	return 0;
}

/*!***************************************************************************
 @Function		Close
 @Description   fclose
 *****************************************************************************/
void FileStreamImpl::Close()
{
	if (m_nativeFile)
	{
		fclose(m_nativeFile);
	}

	m_nativeFile = NULL;
}

/*!***************************************************************************
 @Function		NativeFD
 @Return        void*
 @Description   Returns the native file descriptor
 *****************************************************************************/
void* FileStreamImpl::NativeFD()
{
	return (void*)m_nativeFile;
}

/*!***************************************************************************
 @Function		GetFileSize
 @Return        size_t
 @Description
 *****************************************************************************/
size_t FileStreamImpl::GetFileSize()
{
	fseek(m_nativeFile, 0, SEEK_END);
	size_t size = ftell(m_nativeFile);
	rewind(m_nativeFile);

	return size;
}

/*!***************************************************************************
 @Function		ResourceManagerImpl
 @Description	Constructor
 *****************************************************************************/
ResourceManagerImpl::ResourceManagerImpl()
{
}

/*!***********************************************************************
 @Function		OpenFile
 @Param			c_pszFilename
 @Param         eMode
 @Returns		FileStream*
 @Description	Requires the filestream object be deleted.
*************************************************************************/
FileStream* ResourceManagerImpl::OpenFile(const char* c_pszFilename, FileStream::enumMODE eMode)
{

	FILE* file = fopen(c_pszFilename, eMode == FileStream::eRead ? "r" : "w");

	FileStreamImpl* pStream = NULL;
	if (file)
	{
		pStream = new FileStreamImpl;
		pStream->m_nativeFile = file;
	}

	return pStream;
}

/*!***********************************************************************
 @Function		LoadFileToMemory
 @Param			c_pszFilename
 @Param         ppData
 @Param         fileSize
 @Returns		bool
 @Description	Requires that the data allocated in ppData be deleted.
*************************************************************************/
bool ResourceManagerImpl::LoadFileToMemory(const char* c_pszFilename, char** ppData, Uint32& fileSize)
{
	FileStream* pStream = OpenFile(c_pszFilename);
	if(!pStream)
		return false;
	
	fileSize = pStream->GetFileSize();
	char* pData = new char[fileSize];
	pStream->Read(pData, fileSize, 1);
	
	*ppData = pData;
	
	delete pStream;
	return true;
}

/*!***********************************************************************
 @Function		LoadTextureImpl
 @Param			c_pszFilename
 @Param         uiW
 @Param         uiH
 @Returns		Uint32
 @Description	
*************************************************************************/
Uint32 ResourceManagerImpl::LoadTextureImpl(const char* c_pszFilename, Uint32& uiW, Uint32& uiH)
{
	char szFile[256];

	ASSERT(strchr(c_pszFilename, '.') == NULL);		// Shouldn't load with an extension (we'll load the correct file ourselves).

	Uint32 handle = 0;
	FileStream* pStream = NULL;

	// Try PVR first
	sprintf(szFile, "%s.pvr", c_pszFilename);
	pStream = OpenFile(szFile);
	if(pStream)
	{
		handle = LoadPVR(pStream, uiW, uiH);
	}

	// Try PNG second
	if(handle == 0)
	{
		sprintf(szFile, "%s.png", c_pszFilename);
		pStream = OpenFile(szFile);
		if(pStream)
		{
			handle = LoadPNG(pStream, uiW, uiH);
		}
	}
	
	delete pStream;

	// Return handle.
	return handle;
}

/*!***********************************************************************
 @Function		LoadPVR
 @Access		private 
 @Param			const char * c_pszFilename
 @Returns		bool
 @Description	
*************************************************************************/
Uint32 ResourceManagerImpl::LoadPVR(FileStream* pFile, Uint32& uiW, Uint32& uiH)
{
	DebugLog("Found PVR alternative!");
	GLuint hTexture;
	
	int fileSize = pFile->GetFileSize();
	char* pointer = new char[fileSize];
	pFile->Read(pointer, fileSize, 1);

	PVR_Texture_Header* psPVRHeader = (PVR_Texture_Header*)pointer;
	if(psPVRHeader->dwHeaderSize != sizeof(PVR_Texture_Header))
	{
		// Old header. Bail.
		DebugLog("Error loading PVR: PVR Texture generated with old-style header. Failing.");
		return 0;
	}
	
    if(!IsGLExtensionSupported("GL_IMG_texture_compression_pvrtc"))
	{
		DebugLog("Error loading PVR: PVRTC not supported!");
		return 0;
	}

	Uint32 u32NumSurfs = psPVRHeader->dwNumSurfs;
	Uint32 ePixelType = psPVRHeader->dwpfFlags & PVRTEX_PIXELTYPE;
	GLenum eTextureFormat = 0;

	switch(ePixelType)
	{
		case OGL_PVRTC2:
			eTextureFormat = psPVRHeader->dwAlphaBitMask==0 ? GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG : GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG ;	// PVRTC2
			break;
		case OGL_PVRTC4:
			eTextureFormat = psPVRHeader->dwAlphaBitMask==0 ? GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG : GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG ;	// PVRTC4
			break;
		default:
			// Unsupported texture format. Bail.
			DebugLog("Error loading PVR: Unsupported format.");
			return 0;
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT,1);				// Never have row-aligned in psPVRHeaders
	glGenTextures(1, &hTexture);
	glBindTexture(GL_TEXTURE_2D, hTexture);

	for(unsigned int i=0; i<u32NumSurfs; i++)
	{
		char *theTexturePtr = ((char*)psPVRHeader + psPVRHeader->dwHeaderSize) + psPVRHeader->dwTextureDataSize * i;
		char *theTextureToLoad = 0;
		int		nMIPMapLevel;
		int		nTextureLevelsNeeded = (psPVRHeader->dwpfFlags & PVRTEX_MIPMAP)? psPVRHeader->dwMipMapCount : 0;
		unsigned int		nSizeX= psPVRHeader->dwWidth, nSizeY = psPVRHeader->dwHeight;
		unsigned int		CompressedImageSize = 0;

		for(nMIPMapLevel = 0; nMIPMapLevel <= nTextureLevelsNeeded; nSizeX=PVRT_MAX(nSizeX/2, (unsigned int)1), nSizeY=PVRT_MAX(nSizeY/2, (unsigned int)1), nMIPMapLevel++)
		{
			theTextureToLoad = theTexturePtr;

			// Load Texture
			if ((psPVRHeader->dwpfFlags & PVRTEX_PIXELTYPE)==OGL_PVRTC2)
				CompressedImageSize = ( PVRT_MAX(nSizeX, PVRTC2_MIN_TEXWIDTH) * PVRT_MAX(nSizeY, PVRTC2_MIN_TEXHEIGHT) * psPVRHeader->dwBitCount) / 8;
			else
				CompressedImageSize = ( PVRT_MAX(nSizeX, PVRTC4_MIN_TEXWIDTH) * PVRT_MAX(nSizeY, PVRTC4_MIN_TEXHEIGHT) * psPVRHeader->dwBitCount) / 8;

			glCompressedTexImage2D(GL_TEXTURE_2D, nMIPMapLevel, eTextureFormat, nSizeX, nSizeY, 0, CompressedImageSize, theTextureToLoad);

			if(glGetError())
			{
				DebugLog("Error loading PVR:glTexImage2D() failed.");
				return 0;
			}

			// offset the texture pointer by one mip-map level
			theTexturePtr += CompressedImageSize;
		}
	}

	if(!psPVRHeader->dwMipMapCount)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}

	uiW = psPVRHeader->dwWidth;
	uiH = psPVRHeader->dwHeight;

	return hTexture;
}

static void FileStream_LoadFn(png_structp png_ptr, png_bytep data, png_size_t length) 
{
	png_voidp p = png_get_io_ptr(png_ptr);
	if(!p)
		return;
		
	FileStream* pStream = (FileStream*)p;
	pStream->Read(data, length, 1);
}

/*!***********************************************************************
 @Function		LoadPNG
 @Access		private 
 @Param			const char * c_pszFilename
 @Returns		bool
 @Description	
*************************************************************************/
Uint32 ResourceManagerImpl::LoadPNG(FileStream* pFile, Uint32& uiW, Uint32& uiH)
{
	unsigned char header[8];
	pFile->Read(header, 1, 8);
	if(png_sig_cmp(header, 0, 8))
	{
		return 0;
	}

	// Create PNG structs
	png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	png_infop info_ptr	= png_create_info_struct(png_ptr);
	setjmp(png_jmpbuf(png_ptr));

	png_set_read_fn(png_ptr, pFile, FileStream_LoadFn);
	png_set_sig_bytes(png_ptr, 8);

	png_read_info(png_ptr, info_ptr);
	
	// variables to pass to get info
	int bit_depth, color_type;
	png_uint_32 twidth, theight;

	// get info about png
  	png_get_IHDR(png_ptr, info_ptr, &twidth, &theight, &bit_depth, &color_type, NULL, NULL, NULL);
  	
  	uiW = twidth;
  	uiH = theight;
  	
  	int numBytes = 0;
  	switch(color_type)
	{
		case PNG_COLOR_TYPE_RGB_ALPHA:	numBytes = 4; break;
		case PNG_COLOR_TYPE_RGB:		numBytes = 3; break;
		case PNG_COLOR_TYPE_GRAY_ALPHA:	numBytes = 1; break;
		case PNG_COLOR_TYPE_GRAY:		numBytes = 1; break;
	}
	
	int nBPP = numBytes * bit_depth;
	int nNumPasses = png_set_interlace_handling(png_ptr);
	png_read_update_info(png_ptr, info_ptr);

	// Read the file
	setjmp(png_jmpbuf(png_ptr));

	Uint8* pDecompressedData = (Uint8*)malloc(sizeof(Uint8) * uiW * uiH * numBytes);
	Uint8** ppRowPtrs = (Uint8**)malloc(sizeof(Uint8*) * uiH);
	for(Uint32 uiY = 0; uiY < uiH; ++uiY)
		ppRowPtrs[uiY] = &pDecompressedData[uiY * uiW * numBytes];

	png_read_image(png_ptr, ppRowPtrs);

	free(ppRowPtrs);

	GLuint hTexture;
	glGenTextures(1, &hTexture);
	glBindTexture(GL_TEXTURE_2D, hTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, (numBytes==4 ? GL_RGBA : GL_RGB), uiW, uiH, 0, (numBytes==4 ? GL_RGBA : GL_RGB), GL_UNSIGNED_BYTE, pDecompressedData);

	glBindTexture(GL_TEXTURE_2D, 0);
	free(pDecompressedData);

	return hTexture;
}
