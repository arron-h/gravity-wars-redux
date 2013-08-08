#include "stdafx.h"
#include "Bitmap.h"

#define PNG_DEBUG 3
#include "../libs/libpng/include/png.h"

static void FileStream_LoadFn(png_structp png_ptr, png_bytep data, png_size_t length) 
{
	png_voidp p = png_get_io_ptr(png_ptr);
	if(!p)
		return;
		
	FileStream* pStream = (FileStream*)p;
	pStream->Read(data, length, 1);
}

/*!***********************************************************************
 @Function		CreateFromPNG
 @Access		private 
 @Param			const char * c_pszFilename
 @Returns		bool
 @Description	Creates a bitmap from a given PNG file.
*************************************************************************/
bool Bitmap::CreateFromPNG(const char* c_pszFilename)
{
	FileStream* pFile = RESMAN->OpenFile(c_pszFilename);
	if(!pFile)
	{
		DebugLog("Couldn't load: %s", c_pszFilename);
		return false;
	}

	unsigned char header[8];		// AH: Why 8?
	pFile->Read(header, 1, 8);
	if(png_sig_cmp(header, 0, 8))
	{
		DebugLog("%s is not a valid PNG file!", c_pszFilename);
		delete pFile;
		return false;
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
	
	m_uiBytesPP = numBytes;
		
	int nNumPasses = png_set_interlace_handling(png_ptr);
	png_read_update_info(png_ptr, info_ptr);

	// Read the file
	setjmp(png_jmpbuf(png_ptr));

	m_pData = new Uint8[m_uiWidth * m_uiHeight * numBytes];
	Uint8** ppRowPtrs =  new Uint8*[m_uiHeight];
	for(Uint32 uiY = 0; uiY < m_uiHeight; ++uiY)
		ppRowPtrs[uiY] = &m_pData[uiY * m_uiWidth * numBytes];

	png_read_image(png_ptr, ppRowPtrs);
	
	delete pFile;
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
