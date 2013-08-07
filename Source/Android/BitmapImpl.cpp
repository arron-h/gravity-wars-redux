#include "stdafx.h"
#include "Bitmap.h"

#define PNG_DEBUG 3
#include "../libs/libpng/include/png.h"

/*!***********************************************************************
 @Function		CreateFromPNG
 @Access		private 
 @Param			const char * c_pszFilename
 @Returns		bool
 @Description	Creates a bitmap from a given PNG file.
*************************************************************************/
bool Bitmap::CreateFromPNG(const char* c_pszFilename)
{
	char szPath[1024];
	RESMAN->GetResourcePath(szPath, 1024, enumRESTYPE_Texture);
	strcat(szPath, c_pszFilename);

	FILE* pFile = fopen(szPath, "rb");
	if(!pFile)
	{
		printf("Couldn't load: %s\n", c_pszFilename);
		return false;
	}

	unsigned char header[8];		// AH: Why 8?
	fread(header, 1, 8, pFile);
	if(png_sig_cmp(header, 0, 8))
	{
		printf("%s is not a valid PNG file!", c_pszFilename);
		fclose(pFile);
		return false;
	}

	// Create PNG structs
	png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	png_infop info_ptr	= png_create_info_struct(png_ptr);
	setjmp(png_jmpbuf(png_ptr));

	png_init_io(png_ptr, pFile);
	png_set_sig_bytes(png_ptr, 8);

	png_read_info(png_ptr, info_ptr);
	
	// variables to pass to get info
	int bit_depth, color_type;
	png_uint_32 twidth, theight;

	// get info about png
  	png_get_IHDR(png_ptr, info_ptr, &twidth, &theight, &bit_depth, &color_type, NULL, NULL, NULL);
  	
  	m_uiWidth = twidth;
  	m_uiHeight = theight;

	int numBytes = 0;
	switch(color_type)
	{
		case PNG_COLOR_TYPE_RGB_ALPHA:	m_eFormat = enumFORMAT_RGBA8888; numBytes = 4; break;
		case PNG_COLOR_TYPE_RGB:		m_eFormat = enumFORMAT_RGB888;   numBytes = 3; break;
		case PNG_COLOR_TYPE_GRAY_ALPHA:	m_eFormat = enumFORMAT_A8;       numBytes = 1; break;
		case PNG_COLOR_TYPE_GRAY:		m_eFormat = enumFORMAT_A8;       numBytes = 1; break;
		default:
			m_eFormat = enumFORMAT_Invalid;
			ASSERT(!"Unhandled type!");
			break;
	}
	
	int nBPP    = numBytes * bit_depth;
	m_uiBytesPP = nBPP;
		
	int nNumPasses = png_set_interlace_handling(png_ptr);
	png_read_update_info(png_ptr, info_ptr);

	// Read the file
	setjmp(png_jmpbuf(png_ptr));

	m_pData = new Uint8[m_uiWidth * m_uiHeight * nBPP];
	Uint8** ppRowPtrs =  new Uint8*[m_uiHeight];
	for(Uint32 uiY = 0; uiY < m_uiHeight; ++uiY)
		ppRowPtrs[uiY] = &m_pData[uiY * m_uiWidth * nBPP];

	png_read_image(png_ptr, ppRowPtrs);
	fclose(pFile);	

	delete [] ppRowPtrs;

	return true;
}

/*!***********************************************************************
 @Function		SaveAsPNG
 @Access		private 
 @Param			const char * c_pszFilename
 @Returns		bool
 @Description	Saves the bitmap as PNG file.
*************************************************************************/
bool Bitmap::SaveAsPNG(const char* c_pszFilename)
{
	return true;  // Not needed.
}
