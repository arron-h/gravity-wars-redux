#include "stdafx.h"
#include "RGBA.h"
#include "Rectangle.h"
#include "Bitmap.h"
#include "CollisionBitmap.h"

#ifndef max
	#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
	#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif


const Float32 c_f1Div255 = 1.0f / 255.0f;

#pragma pack(push)
#pragma pack(1)			// Stop aligning to 4bytes ffs!
typedef struct
	{
	Uint8  identsize;          // size of ID field that follows 18 byte header (0 usually)
	Uint8  colourmaptype;      // type of colour map 0=none, 1=has palette
	Uint8  imagetype;          // type of image 0=none,1=indexed,2=rgb,3=grey,+8=rle packed

	Sint16 colourmapstart;     // first colour map entry in palette
	Sint16 colourmaplength;    // number of colours in palette
	Uint8  colourmapbits;      // number of bits per palette entry 15,16,24,32

	Sint16 xstart;             // image x origin
	Sint16 ystart;             // image y origin
	Sint16 width;              // image width in pixels
	Sint16 height;             // image height in pixels
	Uint8  bits;               // image bits per pixel 8,16,24,32
	Uint8  descriptor;         // image descriptor bits (vh flip bits)

	// pixel data follows header
	} TGA_HEADER;
#pragma pack(pop)



// ------ Blending function prototypes
typedef Float32 (*BlendFuncPtr)(Float32, Float32);
Float32 Blend_Normal(Float32 fSrc, Float32 fDst);
Float32 Blend_Darken(Float32 fSrc, Float32 fDst);
Float32 Blend_Multiply(Float32 fSrc, Float32 fDst);
Float32 Blend_ColourBurn(Float32 fSrc, Float32 fDst);
Float32 Blend_LinearBurn(Float32 fSrc, Float32 fDst);
Float32 Blend_Lighten(Float32 fSrc, Float32 fDst);
Float32 Blend_Screen(Float32 fSrc, Float32 fDst);
Float32 Blend_ColourDodge(Float32 fSrc, Float32 fDst);
Float32 Blend_LinearDodge(Float32 fSrc, Float32 fDst);
Float32 Blend_Overlay(Float32 fSrc, Float32 fDst);
Float32 Blend_SoftLight(Float32 fSrc, Float32 fDst);
Float32 Blend_HardLight(Float32 fSrc, Float32 fDst);
Float32 Blend_VividLight(Float32 fSrc, Float32 fDst);
Float32 Blend_LinearLight(Float32 fSrc, Float32 fDst);
Float32 Blend_PinLight(Float32 fSrc, Float32 fDst);
Float32 Blend_Difference(Float32 fSrc, Float32 fDst);
Float32 Blend_Exclusion(Float32 fSrc, Float32 fDst);

BlendFuncPtr BlendFunctions[] =
	{
	Blend_Normal,			// BLEND_Normal
	Blend_Darken,			// BLEND_Darken
	Blend_Multiply,			// BLEND_Multiply
	Blend_ColourBurn,		// BLEND_ColourBurn
	Blend_LinearBurn,		// BLEND_LinearBurn
	Blend_Lighten,			// BLEND_Lighten
	Blend_Screen,			// BLEND_Screen
	Blend_ColourDodge,		// BLEND_ColourDodge
	Blend_LinearDodge,		// BLEND_LinearDodge
	Blend_Overlay,			// BLEND_Overlay
	Blend_SoftLight,		// BLEND_SoftLight
	Blend_HardLight,		// BLEND_HardLight
	Blend_VividLight,		// BLEND_VividLight
	Blend_LinearLight,		// BLEND_LinearLight
	Blend_PinLight,			// BLEND_PinLight
	Blend_Difference,		// BLEND_Difference
	Blend_Exclusion,		// BLEND_Exclusion
	};

/*!***********************************************************************
 @Function		IsValid
 @Access		public 
 @Returns		bool
 @Description	
*************************************************************************/
bool Bitmap::IsValid()
	{
	return (m_pData ? true : false);
	}

/*!***********************************************************************
 @Function		Create
 @Access		public 
 @Param			Uint32 uiWidth
 @Param			Uint32 uiHeight
 @Param			Uint32 uiBPP
 @Returns		void
 @Description	Creates an empty canvas.
*************************************************************************/
void Bitmap::Create(Uint32 uiWidth, Uint32 uiHeight, enumFORMAT eFormat)
	{
	m_uiWidth		= uiWidth;
	m_uiHeight		= uiHeight;
	m_eFormat		= eFormat;
	switch(eFormat)
		{
		case enumFORMAT_RGBA8888:	m_uiBytesPP = 4;	break;
		case enumFORMAT_RGB888:		m_uiBytesPP = 3;	break;
		case enumFORMAT_RGB565:		m_uiBytesPP = 2;	break;
		case enumFORMAT_A8:			m_uiBytesPP = 1;	break;
		default:					ASSERT(!"Unhandled format");	break;
		}

	m_pData			= new Uint8[uiWidth * uiHeight * m_uiBytesPP];
	memset(m_pData, 0, uiWidth * uiHeight * m_uiBytesPP);
	}

/*!***********************************************************************
 @Function		Copy
 @Access		public 
 @Param			Bitmap & bitmapDst
 @Returns		void
 @Description	
*************************************************************************/
void Bitmap::Copy(Bitmap& bitmapDst)
	{
	ASSERT(!bitmapDst.IsValid());
	bitmapDst.m_eFormat   = m_eFormat;
	bitmapDst.m_uiWidth   = m_uiWidth;
	bitmapDst.m_uiHeight  = m_uiHeight;
	bitmapDst.m_uiBytesPP = m_uiBytesPP;

	Uint32 uiDataSize = m_uiWidth * m_uiHeight * m_uiBytesPP;
	bitmapDst.m_pData = new Uint8[uiDataSize];
	memcpy(bitmapDst.m_pData, m_pData, uiDataSize);
	}

/*!***********************************************************************
 @Function		CreateFromImage
 @Access		public 
 @Param			const char * c_pszFilename
 @Param			enumIMAGETYPE eType
 @Returns		bool
 @Description	Creates a bitmap from a given image file.
*************************************************************************/
bool Bitmap::CreateFromImage(const char* c_pszFilename, enumIMAGETYPE eType)
	{
	ASSERT(c_pszFilename);

	bool bReturn = false;
	switch(eType)
		{
		case enumIMAGETYPE_TGA:
			bReturn = CreateFromTGA(c_pszFilename);
			break;
		case enumIMAGETYPE_PNG:
			bReturn = CreateFromPNG(c_pszFilename);
			break;
		case enumIMAGETYPE_Invalid:
		default:
			bReturn = false;
		}

	return bReturn;
	}


/*!***********************************************************************
 @Function		Delete
 @Access		public 
 @Returns		void
 @Description	Free's the memory associated with this bitmap.
*************************************************************************/
void Bitmap::Delete()
	{
	m_uiWidth		= 0;
	m_uiHeight		= 0;
	m_uiBytesPP		= 0;
	m_eFormat		= enumFORMAT_Invalid;

	if(m_pData)
		delete [] m_pData;
	m_pData = NULL;
	}


/*!***********************************************************************
 @Function		SetPixel
 @Access		public 
 @Param			Uint32 uiX
 @Param			Uint32 uiY
 @Param			const RGBA & rgba
 @Returns		void
 @Description	Sets the given pixel with RGBA value.
*************************************************************************/
void Bitmap::SetPixel(Uint32 uiX, Uint32 uiY, const RGBA& rgba)
	{
	ASSERT(m_pData && uiX < m_uiWidth && uiY < m_uiHeight);

	Uint8* pDataLoc = m_pData + (uiY * (m_uiWidth * m_uiBytesPP)) + (uiX * m_uiBytesPP);
	if(m_eFormat == enumFORMAT_RGBA8888)
		{
		*(pDataLoc + 0) = (Uint8)(rgba.m_fR * 255.0f) & 0xFF;
		*(pDataLoc + 1) = (Uint8)(rgba.m_fG * 255.0f) & 0xFF;
		*(pDataLoc + 2) = (Uint8)(rgba.m_fB * 255.0f) & 0xFF;
		*(pDataLoc + 3) = (Uint8)(rgba.m_fA * 255.0f) & 0xFF;
		}
	else if(m_eFormat == enumFORMAT_RGB888)
		{
		*(pDataLoc + 0) = (Uint8)(rgba.m_fR * 255.0f) & 0xFF;
		*(pDataLoc + 1)= (Uint8)(rgba.m_fG * 255.0f) & 0xFF;
		*(pDataLoc + 2)= (Uint8)(rgba.m_fB * 255.0f) & 0xFF;
		}
	else if(m_eFormat == enumFORMAT_RGB565)
		{
		Uint8 u8R = (Uint8)(rgba.m_fR * 31.0f) & 0x1F;		// 5 Bits
		Uint8 u8G = (Uint8)(rgba.m_fG * 63.0f) & 0x3F;		// 6 Bits
		Uint8 u8B = (Uint8)(rgba.m_fB * 31.0f) & 0x1F;		// 5 Bits
		*((Uint16*)pDataLoc) = (u8R << 11) | (u8G << 5) | u8B;			// Might need to do some endian-shit here.
		}
	else if(m_eFormat == enumFORMAT_A8)
		{
		*pDataLoc = (Uint8)(rgba.m_fA * 255.0f) & 0xFF;
		}
	}

/*!***********************************************************************
 @Function		GetPixel
 @Access		public 
 @Param			Uint32 uiX
 @Param			Uint32 uiY
 @Returns		Uint32
 @Description	Returns the given pixel.
*************************************************************************/
void Bitmap::GetPixel(Uint32 uiX, Uint32 uiY, RGBA& rgba) const
	{
	ASSERT(m_pData && uiX < m_uiWidth && uiY < m_uiHeight);
	Uint8* pDataLoc = m_pData + (uiY * (m_uiWidth * m_uiBytesPP)) + (uiX * m_uiBytesPP);
	
	if(m_eFormat == enumFORMAT_RGBA8888)
		{
		rgba.Set(*(pDataLoc+0)*c_f1Div255, 
				 *(pDataLoc+1)*c_f1Div255, 
				 *(pDataLoc+2)*c_f1Div255, 
				 *(pDataLoc+3)*c_f1Div255 
				 );
		}
	else if(m_eFormat == enumFORMAT_RGB888)
		{
		rgba.Set(*(pDataLoc+0)*c_f1Div255, 
				 *(pDataLoc+1)*c_f1Div255, 
				 *(pDataLoc+2)*c_f1Div255,
				 1.0f 
				 );
		}
	else if(m_eFormat == enumFORMAT_RGB565)
		{
		Uint16 u16Data = *((Uint16*)pDataLoc);
		Float32 fR = ((u16Data >> 11) & 0x1F) / 31.0f;
		Float32 fG = ((u16Data >> 5)  & 0x3F) / 63.0f;
		Float32 fB = ((u16Data)       & 0x1F) / 31.0f;
		rgba.Set(fR, fG, fB, 1.0f);
		}
	else if(m_eFormat == enumFORMAT_A8)
		{
		rgba.Set(1, 1, 1, *pDataLoc * c_f1Div255);
		}
	}


/*!***********************************************************************
 @Function		FillRectangle
 @Access		public 
 @Param			const Rectanglei & rect
 @Param			const RGBA & rgba
 @Returns		void
 @Description	Fills the bitmap with given RGBA and Rectangle.
*************************************************************************/
void Bitmap::FillRectangle(const Rectanglei& rect, const RGBA& rgba)
	{
	ASSERT(rect.m_nX >= 0 && rect.m_nY >= 0);
	for(Uint32 uiY = (Uint32)rect.m_nY; uiY < (Uint32)rect.m_nH; ++uiY)
		{
		for(Uint32 uiX = (Uint32)rect.m_nX; uiX < (Uint32)rect.m_nW; ++uiX)
			{
			SetPixel(uiX, uiY, rgba);
			}
		}
	}

/*!***********************************************************************
 @Function		Scale
 @Access		public 
 @Param			Float32 fScaleX
 @Param			Float32 fScaleY
 @Returns		void
 @Description	
*************************************************************************/
void Bitmap::Scale(Float32 fScaleX, Float32 fScaleY)
	{
	Uint32 uiNewWidth  = (Uint32)(m_uiWidth  * fScaleX);
	Uint32 uiNewHeight = (Uint32)(m_uiHeight * fScaleY);
	Uint8* pNewData = new Uint8[uiNewWidth * uiNewHeight * m_uiBytesPP];		// AH: Can't think of a way of resizing without a new. Even going smaller.

	Float32 fScaleRecipX = 1.0f / fScaleX;
	Float32 fScaleRecipY = 1.0f / fScaleY;
	
	RGBA OrigPix;
	for(Uint32 uiNewY = 0; uiNewY < uiNewHeight; ++uiNewY)
		{
		for(Uint32 uiNewX = 0; uiNewX < uiNewWidth; ++uiNewX)
			{
			// This is equivalent to GL_NEAREST at the moment. Should support GL_LINEAR.
			Uint8* pDataLoc = pNewData + (uiNewY * (uiNewWidth * m_uiBytesPP)) + (uiNewX * m_uiBytesPP);
			
			// Find the pixel in the original Bitmap using manhatten distance.
			GetPixel((Uint32)(uiNewX * fScaleRecipX), (Uint32)(uiNewY * fScaleRecipY), OrigPix);

			if(m_eFormat == enumFORMAT_RGBA8888)
				{
				*(pDataLoc + 0) = (Uint8)(OrigPix.m_fR * 255.0f) & 0xFF;
				*(pDataLoc + 1) = (Uint8)(OrigPix.m_fG * 255.0f) & 0xFF;
				*(pDataLoc + 2) = (Uint8)(OrigPix.m_fB * 255.0f) & 0xFF;
				*(pDataLoc + 3) = (Uint8)(OrigPix.m_fA * 255.0f) & 0xFF;
				}
			else if(m_eFormat == enumFORMAT_RGB888)
				{
				*(pDataLoc + 0) = (Uint8)(OrigPix.m_fR * 255.0f) & 0xFF;
				*(pDataLoc + 1)= (Uint8)(OrigPix.m_fG * 255.0f) & 0xFF;
				*(pDataLoc + 2)= (Uint8)(OrigPix.m_fB * 255.0f) & 0xFF;
				}
			else if(m_eFormat == enumFORMAT_RGB565)
				{
				Uint8 u8R = (Uint8)(OrigPix.m_fR * 31.0f) & 0x1F;		// 5 Bits
				Uint8 u8G = (Uint8)(OrigPix.m_fG * 63.0f) & 0x3F;		// 6 Bits
				Uint8 u8B = (Uint8)(OrigPix.m_fB * 31.0f) & 0x1F;		// 5 Bits
				*((Uint16*)pDataLoc) = (u8R << 11) | (u8G << 5) | u8B;			// Might need to do some endian-shit here.
				}
			else if(m_eFormat == enumFORMAT_A8)
				{
				*pDataLoc = (Uint8)(OrigPix.m_fA * 255.0f) & 0xFF;
				}
			}
		}

	// Replace array.
	delete [] m_pData;
	m_pData = pNewData;

	m_uiWidth  = uiNewWidth;
	m_uiHeight = uiNewHeight;
	}

/*!***********************************************************************
 @Function		Colourise
 @Access		public 
 @Param			const RGBA & rgba
 @Returns		void
 @Description	
*************************************************************************/
void Bitmap::Colourise(const RGBA& rgba)
	{
	RGBA pix;
	BlendFuncPtr blendfunc = BlendFunctions[BLEND_Overlay];		// Use OVERLAY style to colourise.
	for(Uint32 uiY = 0; uiY < m_uiHeight; ++uiY)
		{
		for(Uint32 uiX = 0; uiX < m_uiWidth; ++uiX)
			{
			GetPixel(uiX, uiY, pix);
			pix.m_fR = blendfunc(rgba.m_fR, pix.m_fR);
			pix.m_fG = blendfunc(rgba.m_fG, pix.m_fG);
			pix.m_fB = blendfunc(rgba.m_fB, pix.m_fB);
			SetPixel(uiX, uiY, pix);
			}
		}	
	}

/*!***********************************************************************
 @Function		ToFormat
 @Access		public 
 @Param			enumFORMAT eFormat
 @Returns		void
 @Description	Converts from native format to new given format.
*************************************************************************/
void Bitmap::ToFormat(enumFORMAT eFormat)
	{
	if(m_eFormat == enumFORMAT_A8)
		{
		if(eFormat == enumFORMAT_RGBA8888)
			{
			// Expand to 32bit
			Uint8* pNewData = new Uint8[m_uiWidth * m_uiHeight * 4];
			for(Uint32 uiPix = 0, uiNewIdx = 0; uiPix < m_uiWidth * m_uiHeight; uiPix++, uiNewIdx+=4)
				{
				pNewData[uiNewIdx+0] = m_pData[uiPix];
				pNewData[uiNewIdx+1] = m_pData[uiPix];
				pNewData[uiNewIdx+2] = m_pData[uiPix];
				pNewData[uiNewIdx+3] = m_pData[uiPix];
				}
			m_uiBytesPP = 4;
			m_eFormat = enumFORMAT_RGBA8888;
			delete [] m_pData;
			m_pData = pNewData;
			}
		else
			{
			ASSERT(!"Unsupported operation");
			}
		}
	else
		{
		ASSERT(!"Unsupported operation");
		}
	}

/*!***********************************************************************
 @Function		AddLayer
 @Access		public 
 @Param			const Bitmap & bmLayer
 @Param			Uint32 uiX
 @Param			Uint32 uiY
 @Param			enumBLEND eBlendMode
 @Returns		void
 @Description	Adds a layer to the bitmap.
*************************************************************************/
void Bitmap::AddLayer(const Bitmap& bmLayer, Uint32 uiX, Uint32 uiY, enumBLEND eBlendMode)
	{
	RGBA LayerPixel, BMPixel, MergedPixel, BlendedPixel;
	for(Uint32 uiBMY = uiY, uiLY = 0; uiBMY < m_uiHeight && uiLY < bmLayer.m_uiHeight; ++uiBMY, ++uiLY)
		{
		for(Uint32 uiBMX = uiX, uiLX = 0; uiBMX < m_uiWidth && uiLX < bmLayer.m_uiWidth; ++uiBMX, ++uiLX)
			{

	/*		bmLayer.GetPixel(uiLX, uiLY, TmpRGBA);
			uiLayerPixel = TmpRGBA;
			GetPixel(uiBMX, uiBMY, TmpRGBA);
			uiBMPixel = TmpRGBA;
			SrcR = BlendFunctions[eBlendMode](GETR(uiBMPixel), GETR(uiLayerPixel));
			SrcG = BlendFunctions[eBlendMode](GETG(uiBMPixel), GETG(uiLayerPixel));
			SrcB = BlendFunctions[eBlendMode](GETB(uiBMPixel), GETB(uiLayerPixel));
			SrcA = GETA(uiLayerPixel) / 255.0f;

			DstR = (Uint8)(SrcR * SrcA) + (GETR(uiBMPixel) * (1 - SrcA));
			DstG = (Uint8)(SrcG * SrcA) + (GETG(uiBMPixel) * (1 - SrcA));
			DstB = (Uint8)(SrcB * SrcA) + (GETB(uiBMPixel) * (1 - SrcA));
			DstA = GETA(uiBMPixel);
			SetPixel(uiBMX, uiBMY, RGBAUint(DstR, DstG, DstB, DstA));			*/

			bmLayer.GetPixel(uiLX, uiLY, LayerPixel);
			GetPixel(uiBMX, uiBMY, BMPixel);

			BlendedPixel.m_fR = BlendFunctions[eBlendMode](LayerPixel.m_fR, BMPixel.m_fR);
			BlendedPixel.m_fG = BlendFunctions[eBlendMode](LayerPixel.m_fG, BMPixel.m_fG);
			BlendedPixel.m_fB = BlendFunctions[eBlendMode](LayerPixel.m_fB, BMPixel.m_fB);
			
			// Blend using SRC_ALPHA, ONE_MINUS_SRC_ALPHA
			MergedPixel.m_fR = BlendedPixel.m_fR * LayerPixel.m_fA + BMPixel.m_fR * (1.0f - LayerPixel.m_fA);
			MergedPixel.m_fG = BlendedPixel.m_fG * LayerPixel.m_fA + BMPixel.m_fG * (1.0f - LayerPixel.m_fA);
			MergedPixel.m_fB = BlendedPixel.m_fB * LayerPixel.m_fA + BMPixel.m_fB * (1.0f - LayerPixel.m_fA);
			MergedPixel.m_fA = LayerPixel.m_fA + BMPixel.m_fA;
			SetPixel(uiBMX, uiBMY, MergedPixel);
			}
		}
	}


/*!***********************************************************************
 @Function		SaveAsTGA
 @Access		private 
 @Param			const char * c_pszFilename
 @Returns		bool
 @Description	Saves the image as TGA v1.0 file.
*************************************************************************/
bool Bitmap::SaveAsTGA(const char* c_pszFilename)
	{
	char szPath[1024];
	RESMAN->GetResourcePath(szPath, 1024, enumRESTYPE_Texture);
	strcat(szPath, c_pszFilename);
	strcat(szPath, ".tga");

	TGA_HEADER header;
	memset(&header, 0, sizeof(TGA_HEADER));
	switch(m_eFormat)
		{
		case enumFORMAT_RGBA8888:
		case enumFORMAT_RGB888:	
			header.imagetype = 2;
			break;
		case enumFORMAT_RGB565:
			ASSERT(!"Unsupported");
			break;
		case enumFORMAT_A8:
			header.imagetype = 3;
			break;
		default:
			ASSERT(!"Unhandled image type");
		}
	
	header.width  = m_uiWidth;
	header.height = m_uiHeight;
	header.bits   = m_uiBytesPP * 8;

	Uint32 uiBytesToWrite = m_uiWidth * m_uiHeight * m_uiBytesPP;

	FILE* pFile = fopen(szPath, "wb");	
	if(!pFile)
		return false;

	fwrite(&header,		sizeof(TGA_HEADER),	1,				pFile);
	fwrite(m_pData,		sizeof(Uint8),		uiBytesToWrite,	pFile);
	fclose(pFile);

	return true;
	}

/*!***********************************************************************
 @Function		CreateFromTGA
 @Access		private 
 @Param			const char * c_pszFilename
 @Returns		bool
 @Description	Creats a bitmap from a given TGA file.
*************************************************************************/
bool Bitmap::CreateFromTGA(const char* c_pszFilename)
	{
	char szPath[1024];
	RESMAN->GetResourcePath(szPath, 1024, enumRESTYPE_Texture);
	strcat(szPath, c_pszFilename);

	FILE* pFile = fopen(szPath, "rb");
	if(!pFile)
		return false;

	TGA_HEADER header;
	fread(&header, sizeof(TGA_HEADER), 1, pFile);
	if(header.imagetype != 2 && header.imagetype != 3)		// Unsupported
		return false;

	if(header.bits == 32)		{ m_eFormat = enumFORMAT_RGBA8888;	m_uiBytesPP = 4; }
	else if(header.bits == 24)	{ m_eFormat = enumFORMAT_RGB888;	m_uiBytesPP = 3; }
	else if(header.bits	== 16)	{ ASSERT(false);}	// Support
	else if(header.bits == 8)	{ m_eFormat = enumFORMAT_A8;		m_uiBytesPP = 1; }
	else ASSERT(false);

	// --- Read in the image data
	Uint32 uiDataSize = (header.width * header.height * m_uiBytesPP);
	m_pData = new Uint8[uiDataSize];

	fread(m_pData, uiDataSize, 1, pFile);

	m_uiWidth  = header.width;
	m_uiHeight = header.height;

	// --- Need to swap from BGRA to RGBA
	Uint8 u8Tmp;
	if(m_uiBytesPP == 3 || m_uiBytesPP == 4)
		{
		for(Uint32 uiIdx = 0; uiIdx < m_uiWidth * m_uiHeight; uiIdx++)
			{
			u8Tmp = m_pData[uiIdx + 0];
			m_pData[uiIdx + 0] = m_pData[uiIdx + 2];		// Swap B and R
			m_pData[uiIdx + 2] = u8Tmp;
			}
		}

	fclose(pFile);
	return true;
	}


/*!***********************************************************************
 @Function		Save
 @Access		public 
 @Param			const char * c_pszFilename
 @Param			enumIMAGETYPE eFormat
 @Returns		bool
 @Description	Saves the bitmap to a file.
*************************************************************************/
bool Bitmap::Save(const char* c_pszFilename, enumIMAGETYPE eFormat)
	{
	ASSERT(c_pszFilename);
	
	bool bReturn = false;
	switch(eFormat)
		{
		case enumIMAGETYPE_TGA:
			bReturn = SaveAsTGA(c_pszFilename);
			break;
		case enumIMAGETYPE_PNG:
			bReturn = SaveAsPNG(c_pszFilename);
			break;
		case enumIMAGETYPE_Invalid:
		default:
			bReturn = false;
		}

	return bReturn;
	}


/*!***********************************************************************
 @Function		ToOpenGL
 @Access		public 
 @Param			bool bMipMap
 @Returns		Uint32
 @Description	Uploads the bitmap to OpenGL.
*************************************************************************/
Uint32 Bitmap::ToOpenGL(bool bMipMap)
	{
	Uint32 uiHandle;
	glGenTextures(1, &uiHandle);
	glBindTexture(GL_TEXTURE_2D, uiHandle);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if(bMipMap)
		{
#ifdef WIN32
		glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
#endif
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		}
	else
		{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		}

	GLenum eDataFormat, eInternalFormat, eDataType;
	switch(m_eFormat)
		{
		case enumFORMAT_RGBA8888:
			eDataFormat = GL_RGBA;
			eInternalFormat = GL_RGBA;
			eDataType = GL_UNSIGNED_BYTE;
			break;
		case enumFORMAT_RGB888:
			eDataFormat = GL_RGB;
			eInternalFormat = GL_RGB;
			eDataType = GL_UNSIGNED_BYTE;
			break;
		case enumFORMAT_RGB565:
			eDataFormat = GL_RGB;
#ifdef WIN32
			eInternalFormat = GL_RGB16;
#else
			eInternalFormat = GL_RGB;
#endif
			eDataType = GL_UNSIGNED_SHORT_5_6_5;
			break;
		case enumFORMAT_A8:
			eDataFormat = GL_LUMINANCE;
			eInternalFormat = GL_LUMINANCE;
			eDataType = GL_UNSIGNED_BYTE;
			break;
		default:
			ASSERT(!"Unhandled image type");
		}
	
	glTexImage2D(GL_TEXTURE_2D, 0, eInternalFormat, m_uiWidth, m_uiHeight, 0, eDataFormat, eDataType, (Uint8*)m_pData);
	
#ifdef PLATFORM_IOS
	glGenerateMipmap(GL_TEXTURE_2D);
#endif
	return uiHandle;
	}

/*!***********************************************************************
 @Function		CreateCollisionBitmap
 @Access		public 
 @Returns		CollisionBitmap*
 @Description	
*************************************************************************/
CollisionBitmap* Bitmap::CreateCollisionBitmap()
	{
	CollisionBitmap* cbm = new CollisionBitmap;
	cbm->m_uiWidth  = m_uiWidth;
	cbm->m_uiHeight = m_uiHeight;

	Uint32 uiDataSize = (Uint32)(ceil((m_uiWidth * m_uiHeight) / 8.0f));
	cbm->m_pData      = new Uint8[uiDataSize];
	memset(cbm->m_pData, 0, uiDataSize);			// Set all to OFF

	// Any pixel that is >128 = 1; <128 = 0.
	Uint32 uiByte = 0;
	Uint32 uiBit  = 0;
	RGBA rgbaPix;
	for(Uint32 uiY = 0; uiY < m_uiHeight; ++uiY)
		{
		for(Uint32 uiX = 0; uiX < m_uiWidth; ++uiX)
			{
			GetPixel(uiX, uiY, rgbaPix);
			if(rgbaPix.AlphaChannel() > 128)
				{
				cbm->m_pData[uiByte] |= (1 << uiBit);
				}

			uiBit++;
			if(uiBit > 7)
				{
				uiBit = 0;
				uiByte++;
				}
			}
		}

	cbm->CalculateBoundingBox();
	cbm->DumpToTGA("test");

	return cbm;
	}

/*!***********************************************************************
*************************************************************************/
/*!**** From: http://www.nathanm.com/photoshop-blending-math/
*************************************************************************/
Float32 Blend_Normal(Float32 fSrc, Float32 fDst)
	{
	return fSrc;
	}

Float32 Blend_Darken(Float32 fSrc, Float32 fDst)
	{
	return min(fDst, fSrc);
	}

Float32 Blend_Multiply(Float32 fSrc, Float32 fDst)
	{
	return (fSrc * fDst);
	}

Float32 Blend_ColourBurn(Float32 fSrc, Float32 fDst)
	{
	return (1.0f - (1.0f - fDst) / fSrc);
	}

Float32 Blend_LinearBurn(Float32 fSrc, Float32 fDst)
	{
	return (fDst + fSrc - 1.0f);
	}

Float32 Blend_Lighten(Float32 fSrc, Float32 fDst)
	{
	return max(fDst, fSrc);
	}

Float32 Blend_Screen(Float32 fSrc, Float32 fDst)
	{
	return (1.0f - (1.0f - fDst) * (1.0f - fSrc));
	}

Float32 Blend_ColourDodge(Float32 fSrc, Float32 fDst)
	{
	return (fDst / (1.0f - fSrc));
	}

Float32 Blend_LinearDodge(Float32 fSrc, Float32 fDst)
	{
	return (fDst + fSrc);
	}

Float32 Blend_Overlay(Float32 fSrc, Float32 fDst)
	{
	if(fDst > 0.5f)
		return (1.0f - (1.0f - 2.0f * (fDst - 0.5f)) * (1.0f - fSrc));
	else
		return ((2.0f * fDst) * fSrc);
	}

Float32 Blend_SoftLight(Float32 fSrc, Float32 fDst)
	{
	if (fSrc > 0.5f) 
		return (1.0f - (1.0f - fDst) * (1.0f - (fSrc - 0.5f)));
	else
		return fDst * (fSrc + 0.5f);
	}

Float32 Blend_HardLight(Float32 fSrc, Float32 fDst)
	{
	return Blend_Overlay(fDst, fSrc);
	}

Float32 Blend_VividLight(Float32 fSrc, Float32 fDst)
	{
	if (fSrc > 0.5f) 
		return (1.0f - (1.0f - fDst) / (2 * (fSrc - 0.5f)));
	else
		return (fDst / (1.0f - 2 * fSrc));
	}

Float32 Blend_LinearLight(Float32 fSrc, Float32 fDst)
	{
	if (fSrc > 0.5f)
		return (fDst + 2 * (fSrc - 0.5f));
	else
		return (fDst + 2 * fSrc - 1.0f);
	}

Float32 Blend_PinLight(Float32 fSrc, Float32 fDst)
	{
	if (fSrc > 0.5f) 
		return max(fDst, 2.0f * (fSrc - 0.5f));
	else
		return min(fDst, 2 * fSrc);
	}

Float32 Blend_Difference(Float32 fSrc, Float32 fDst)
	{
	return fabs(fDst - fSrc);
	}

Float32 Blend_Exclusion(Float32 fSrc, Float32 fDst)
	{
	return (0.5f - 2 * (fDst - 0.5f) * (fSrc - 0.5f));
	}