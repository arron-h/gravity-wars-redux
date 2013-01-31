#include "stdafx.h"
#include "Bitmap.h"

#define PNG_DEBUG 3
#include "../libs/libpng/include/png.h"

#define PNG_ABORT_HELPER(x) { printf(x); return false; }

/*!***********************************************************************
 @Function		CreateFromPNG
 @Access		private 
 @Param			const char * c_pszFilename
 @Returns		bool
 @Description	Creates a bitmap from a given PNG file.
*************************************************************************/
bool Bitmap::CreateFromPNG(const char* c_pszFilename)
	{
	FILE* pFile = fopen(c_pszFilename, "rb");
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

	m_uiWidth  = info_ptr->width;
	m_uiHeight = info_ptr->height;
	int nColourType = info_ptr->color_type;
	int nBitDepth   = info_ptr->bit_depth;
	int nBPP		= info_ptr->pixel_depth / nBitDepth;

	m_uiBytesPP = (Uint32)nBPP;
	switch(nColourType)
		{
		case PNG_COLOR_TYPE_RGB_ALPHA:	m_eFormat = enumFORMAT_RGBA8888;	break;
		case PNG_COLOR_TYPE_RGB:		m_eFormat = enumFORMAT_RGB888;		break;
		case PNG_COLOR_TYPE_GRAY_ALPHA:	m_eFormat = enumFORMAT_A8;			break;
		case PNG_COLOR_TYPE_GRAY:		m_eFormat = enumFORMAT_A8;			break;
		default:
			m_eFormat = enumFORMAT_Invalid;
			ASSERT(!"Unhandled type!");
			break;
		}
		
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
	char szFile[128];
	strcpy(szFile, c_pszFilename);
	strcat(szFile, ".png");
	FILE* pFile = fopen(szFile, "wb");
	if(!pFile)
		{
		printf("Error opening file: %s\n", c_pszFilename);
		return false;
		}

	png_byte color_type = 0;
	switch(m_eFormat)
		{
		case enumFORMAT_RGBA8888:	color_type = PNG_COLOR_TYPE_RGB_ALPHA;	break;
		case enumFORMAT_RGB888:		color_type = PNG_COLOR_TYPE_RGB;		break;
		case enumFORMAT_RGB565:		ASSERT(!"Not supported!");				break;
		case enumFORMAT_A8:			color_type = PNG_COLOR_TYPE_GRAY_ALPHA;	break;			// Experiment with this
		default:
			ASSERT(!"Unhandled image type");
		}

	png_byte	bit_depth = 8;								// 8 BitsPP

	// --- Init
	png_structp png_ptr  = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if(!png_ptr)
		PNG_ABORT_HELPER("png_create_write_struct failed!")

	png_infop info_ptr = png_create_info_struct(png_ptr);
	if(!info_ptr)
		PNG_ABORT_HELPER("png_create_info_struct failed!")
	
	if(setjmp(png_jmpbuf(png_ptr)))
		PNG_ABORT_HELPER("png_init_io error!")

	png_init_io(png_ptr, pFile);

	// --- Write header
	if(setjmp(png_jmpbuf(png_ptr)))
		PNG_ABORT_HELPER("Error writing header.")

	png_set_IHDR(png_ptr, info_ptr, m_uiWidth, m_uiHeight,
				bit_depth, color_type, PNG_INTERLACE_NONE,
				PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
	png_write_info(png_ptr, info_ptr);

	// --- Write data
	// --- Set up row pointers that point to the data in each row
	Uint8** ppRowPointers = new Uint8*[m_uiHeight];
	for(Uint32 uiY = 0; uiY < m_uiHeight; uiY++)
		ppRowPointers[uiY] = (Uint8*)&m_pData[uiY * m_uiWidth  * m_uiBytesPP];

	if(setjmp(png_jmpbuf(png_ptr)))
		PNG_ABORT_HELPER("Error writing data.")
	png_write_image(png_ptr, ppRowPointers);

	// --- Finish writing
	if(setjmp(png_jmpbuf(png_ptr)))
		PNG_ABORT_HELPER("Error ending writing.")
	png_write_end(png_ptr, NULL);

	delete [] ppRowPointers;

    fclose(pFile);

	return true;
	}