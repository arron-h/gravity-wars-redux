#include "stdafx.h"
#include "ResourceImpl.h"
#include "PVRTools.h"
#include "PVRTTexture.h"

#include "../../libs/libpng/include/png.h"


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
	return fread(data, size, count, m_pFileHandle);
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
	return fwrite(data, size, count, m_pFileHandle);
}

/*!***************************************************************************
 @Function		Close
 @Description   fclose
 *****************************************************************************/
void FileStreamImpl::Close()
{
	fclose(m_pFileHandle);
}

/*!***************************************************************************
 @Function		NativeFD
 @Return        void*
 @Description   Returns the native file descriptor
 *****************************************************************************/
void* FileStreamImpl::NativeFD()
{
	return m_pFileHandle;
}

/*!***************************************************************************
 @Function		GetFileSize
 @Return        size_t
 @Description
 *****************************************************************************/
size_t FileStreamImpl::GetFileSize()
{
	size_t s;
	fseek(m_pFileHandle, 0, SEEK_END);
	s = ftell(m_pFileHandle);
	rewind(m_pFileHandle);

	return s;
}


/*!***************************************************************************
 @Function		ResourceManagerImpl
 @Description   Constructor
 *****************************************************************************/
ResourceManagerImpl::ResourceManagerImpl()
{
	NSString* readPath = [[NSBundle mainBundle] resourcePath];
	[readPath getCString:m_szResPath maxLength:1024 encoding:NSUTF8StringEncoding];
	strcat(m_szResPath, "/");
}

/*!***************************************************************************
 @Function		OpenFile
 @Input         c_pszFilename
 @Input         eMode
 @Return		FileStream*
 @Description
 *****************************************************************************/
FileStream* ResourceManagerImpl::OpenFile(const char* c_pszFilename, FileStream::enumMODE eMode)
{
	char szBuffer[1024];
	sprintf(szBuffer, "%s/%s", m_szResPath, c_pszFilename);
	
	FILE* pFile = NULL;
	switch (eMode)
	{
		case FileStream::eRead:
			pFile = fopen(szBuffer, "rb");
			break;
		case FileStream::eWrite:
			pFile = fopen(szBuffer, "wb");
			break;
	}
	
	if(!pFile)
	{
		return NULL;
	}
	
	FileStreamImpl* fs = new FileStreamImpl();
	fs->m_pFileHandle = pFile;
	
	return fs;
}

/*!***************************************************************************
 @Function		LoadFileToMemory
 @Input         c_pszFilename
 @Input         ppData
 @Input         fileSize
 @Return		bool
 @Description
 *****************************************************************************/
bool ResourceManagerImpl::LoadFileToMemory(const char* c_pszFilename, char** ppData, Uint32& fileSize)
{
	char szBuffer[1024];
	sprintf(szBuffer, "%s/%s", m_szResPath, c_pszFilename);
	
	FILE* pFile = fopen(szBuffer, "rb");
	if(!pFile)
	{
		return false;
	}
	
	fseek (pFile , 0 , SEEK_END);
	long lSize = ftell(pFile);
	rewind (pFile);
	
	char* pData = new char[lSize];
	size_t res = fread(pData, 1, lSize, pFile);
	if(res != lSize)
	{
		delete [] pData;
		return false;
	}
	
	fileSize = lSize;
	ppData = &pData;
	
	fclose(pFile);
	return true;
}

/*!***************************************************************************
 @Function		LoadTextureImpl
 @Input         c_pszFilename
 @Input         uiW
 @Input         uiH
 @Return		Uint32
 @Description
 *****************************************************************************/
Uint32 ResourceManagerImpl::LoadTextureImpl(const char* c_pszFilename, Uint32& uiW, Uint32& uiH)
	{
	FILE* pFile = NULL;
	char szFile[256];

	ASSERT(strchr(c_pszFilename, '.') == NULL);		// Shouldn't load with an extension (we'll load the correct file ourselves).

	// Try PVR first
	sprintf(szFile, "%s/%s.pvr", m_szResPath, c_pszFilename);
	pFile = fopen(szFile, "rb");
	if(pFile)
		{
		fclose(pFile);
		return LoadPVR(szFile, uiW, uiH);
		}

	// Try PNG second
	sprintf(szFile, "%s/%s.png", m_szResPath, c_pszFilename);
	pFile = fopen(szFile, "rb");
	if(pFile)
		{
		return LoadPNG(pFile, uiW, uiH);
		}

	// Bail.
	return 0;
	}

/*!***********************************************************************
 @Function		LoadPNG
 @Access		private 
 @Param			const char * c_pszFilename
 @Returns		bool
 @Description	
*************************************************************************/
Uint32 ResourceManagerImpl::LoadPNG(FILE* pFile, Uint32& uiW, Uint32& uiH)
{
	unsigned char header[8];		// AH: Why 8?
	fread(header, 1, 8, pFile);
	if(png_sig_cmp(header, 0, 8))
		{
		fclose(pFile);
		return 0;
		}

	// Create PNG structs
	png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	png_infop info_ptr	= png_create_info_struct(png_ptr);
	setjmp(png_jmpbuf(png_ptr));

	png_init_io(png_ptr, pFile);
	png_set_sig_bytes(png_ptr, 8);

	png_read_info(png_ptr, info_ptr);

	uiW = png_get_image_width(png_ptr, info_ptr);
	uiH = png_get_image_height(png_ptr, info_ptr);
	int nBPP		= png_get_rowbytes(png_ptr, info_ptr) / uiW;

	png_read_update_info(png_ptr, info_ptr);

	// Read the file
	setjmp(png_jmpbuf(png_ptr));

	Uint8* pDecompressedData = (Uint8*)malloc(sizeof(Uint8) * uiW * uiH * nBPP);
	Uint8** ppRowPtrs = (Uint8**)malloc(sizeof(Uint8*) * uiH);
	for(Uint32 uiY = 0; uiY < uiH; ++uiY)
		ppRowPtrs[uiY] = &pDecompressedData[uiY * uiW * nBPP];

	png_read_image(png_ptr, ppRowPtrs);
	fclose(pFile);	

	free(ppRowPtrs);

	GLuint hTexture;
	glGenTextures(1, &hTexture);
	glBindTexture(GL_TEXTURE_2D, hTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, (nBPP==4 ? GL_RGBA : GL_RGB), uiW, uiH, 0, (nBPP==4 ? GL_RGBA : GL_RGB), GL_UNSIGNED_BYTE, pDecompressedData);

	glBindTexture(GL_TEXTURE_2D, 0);
	free(pDecompressedData);

	return hTexture;
}

/*!***********************************************************************
 @Function		LoadPVR
 @Access		private 
 @Param			const char * c_pszFilename
 @Returns		bool
 @Description	
*************************************************************************/	
Uint32 ResourceManagerImpl::LoadPVR(const char* c_pszFile, Uint32& uiW, Uint32& uiH)
{
	DebugLog("Found PVR alternative! Loading: %s", c_pszFile);
	GLuint hTexture;
	
	CPVRTResourceFile TexFile(c_pszFile);
	if (!TexFile.IsOpen()) return 0;
	
	const void* pointer = TexFile.DataPtr();
	PVR_Texture_Header* psPVRHeader = (PVR_Texture_Header*)pointer;
	if(psPVRHeader->dwHeaderSize != sizeof(PVR_Texture_Header))
		{
		// Old header. Bail.
		DebugLog("Error loading PVR: PVR Texture generated with old-style header. Failing.");
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
