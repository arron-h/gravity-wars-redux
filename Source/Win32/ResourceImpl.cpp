#include "stdafx.h"
#include "ResourceImpl.h"
#include "../../libs/libpng/include/png.h"

Uint32 ResourceManagerImpl::LoadTextureImpl(const char* c_pszFilename, Uint32& uiW, Uint32& uiH)
	{
	FILE* pFile = NULL;
	char szFile[256];

	ASSERT(strchr(c_pszFilename, '.') == NULL);		// Shouldn't load with an extension (we'll load the correct file ourselves).

	// Try PNG first
	sprintf(szFile, "%s.png", c_pszFilename);
	pFile = fopen(szFile, "rb");
	if(pFile)
		return LoadPNG(pFile, uiW, uiH);

	// Try TGA second
	sprintf(szFile, "%s.tga", c_pszFilename);
	pFile = fopen(szFile, "rb");
	if(pFile)
		return LoadTGA(pFile, uiW, uiH);

	// Bail.
	return 0;
	}

/*!***********************************************************************
 @Function		LoadTGA
 @Access		private 
 @Param			FILE * pFile
 @Param			Uint32 & uiW
 @Param			Uint32 & uiH
 @Returns		Uint32
 @Description	
*************************************************************************/
Uint32 ResourceManagerImpl::LoadTGA(FILE* pFile, Uint32& uiW, Uint32& uiH)
	{
	// Support
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

	uiW = info_ptr->width;
	uiH = info_ptr->height;
	int nColourType = info_ptr->color_type;
	int nBitDepth   = info_ptr->bit_depth;
	int nBPP		= info_ptr->pixel_depth / nBitDepth;

	int nNumPasses = png_set_interlace_handling(png_ptr);
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
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, uiW, uiH, 0, (nBPP==4 ? GL_RGBA : GL_RGB), GL_UNSIGNED_BYTE, pDecompressedData);

	glBindTexture(GL_TEXTURE_2D, 0);
	free(pDecompressedData);

	return hTexture;
	}

/*!***********************************************************************
 @Function		GetResourcePath
 @Access		virtual public 
 @Param			char * pszBuffer
 @Param			Uint32 uiBufferLen
 @Returns		void
 @Description	
*************************************************************************/
void ResourceManagerImpl::GetResourcePath(char* pszBuffer, Uint32 uiBufferLen, enumRESTYPE eType)
	{
	if(eType == enumRESTYPE_Shader)
		strcpy(pszBuffer, "../Shaders/GL");
	else
		pszBuffer[0] = 0;
	}





















