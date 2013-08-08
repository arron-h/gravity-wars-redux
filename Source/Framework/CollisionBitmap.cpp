#include "stdafx.h"
#include "CollisionBitmap.h"

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

/*!***********************************************************************
 @Function		CollisionBitmap
 @Access		private 
 @Returns		
 @Description	
*************************************************************************/
CollisionBitmap::CollisionBitmap() : m_pData(NULL), m_uiWidth(0), m_uiHeight(0)
	{

	}

/*!***********************************************************************
 @Function		~CollisionBitmap
 @Access		public 
 @Returns		
 @Description	
*************************************************************************/
CollisionBitmap::~CollisionBitmap()
	{
	if(m_pData)
		delete [] m_pData;
		
	m_pData = NULL;
	}

/*!***********************************************************************
 @Function		Check
 @Access		public 
 @Param			PVRTVec2 vPoint
 @Returns		bool
 @Description	
*************************************************************************/
bool CollisionBitmap::Check(const PVRTVec2& vPoint, enumSOURCE eSource) const
	{
	bool bReturn = false;
	Uint32 uiX, uiY;
	switch(eSource)
		{
		case enumSOURCE_Centre:
			// Translate coords so they are in Bottom Left coordinate space
			uiX = (Uint32)(vPoint.x + (m_uiWidth / 2));
			uiY = (Uint32)(vPoint.y + (m_uiHeight / 2));
			bReturn = Check(uiX, uiY);
			break;
		case enumSOURCE_TopLeft:
			// Translate coords so they are in Bottom Left coordinate space
			uiX = (Uint32)vPoint.x;
			uiY = m_uiHeight - (Uint32)vPoint.y;
			bReturn = Check(uiX, uiY);
			break;
		case enumSOURCE_BottomLeft:
			uiX = (Uint32)vPoint.x;
			uiY = (Uint32)vPoint.y;
			bReturn = Check(uiX, uiY);
			break;
		default:
			ASSERT(!"Unhandled case!");
		}
	
	return bReturn;
	}

/*!***********************************************************************
 @Function		Check
 @Access		public 
 @Param			Uint32 uiX
 @Param			Uint32 uiY
 @Returns		bool
 @Description	
*************************************************************************/
bool CollisionBitmap::Check(Uint32 uiX, Uint32 uiY) const
	{
	ASSERT(uiX < m_uiWidth && uiY < m_uiHeight);

	bool bResult = false;
	
	Uint32 uiIndex = (uiY*m_uiWidth)+uiX;
	Uint32 uiByte  = uiIndex / 8;
	Uint32 uiBit   = uiIndex % 8;

	if(m_pData[uiByte] & (1 << uiBit))
		bResult = true;

	return bResult;
	}

/*!***********************************************************************
 @Function		GetBoundingBox
 @Access		public 
 @Returns		const Rectanglei&
 @Description	
*************************************************************************/
const Rectanglei& CollisionBitmap::GetBoundingBox() const
	{
	return m_BoundingBox;
	}

/*!***********************************************************************
 @Function		DumpToTGA
 @Access		private 
 @Param			const char * c_pszFilename
 @Returns		void
 @Description	Debug dump to a TGA
*************************************************************************/
void CollisionBitmap::DumpToTGA(const char* c_pszFilename)
{
	char szFile[1024];
	strcpy(szFile, c_pszFilename);
	strcat(szFile, ".tga");

	TGA_HEADER header;
	memset(&header, 0, sizeof(TGA_HEADER));
	header.imagetype = 3;
	header.width  = m_uiWidth;
	header.height = m_uiHeight;
	header.bits   = 8;

	Uint32 uiBytesToWrite = m_uiWidth * m_uiHeight;

	FileStream* pFile = RESMAN->OpenFile(szFile, FileStream::eWrite);
	if(!pFile)
		return;

	Uint8* pExpandedData = new Uint8[uiBytesToWrite];
	memset(pExpandedData, 0, uiBytesToWrite);
	
	// Unpack into new buffer
	for(Uint32 uiY = 0; uiY < m_uiHeight; uiY++)
	{
		for(Uint32 uiX = 0; uiX < m_uiWidth; uiX++)
		{
			Uint32 uiIndex = (uiY*m_uiWidth) + uiX;
			pExpandedData[uiIndex] = (Check(uiX, uiY) ? 255 : 0);
		}
	}

	pFile->Write(&header,			sizeof(TGA_HEADER),	1);
	pFile->Write(pExpandedData,	sizeof(Uint8),		uiBytesToWrite);

	delete pFile;
	delete [] pExpandedData;
}

/*!***********************************************************************
 @Function		CalculateBoundingBox
 @Access		private 
 @Returns		void
 @Description	Calculates a nice BB for this collision bitmap for trivial rejection
*************************************************************************/
void CollisionBitmap::CalculateBoundingBox()
	{
	m_BoundingBox.m_nX = m_BoundingBox.m_nY = 0x7FFFFFFF;		// A big number
	m_BoundingBox.m_nW = m_BoundingBox.m_nH = 0xF0000000;		// A small number

	for(Sint32 Y = 0; Y < (Sint32)m_uiHeight; Y++)
		{
		for(Sint32 X = 0; X < (Sint32)m_uiWidth; X++)
			{
			if(Check(X, Y))			// It's a Hit. Process.
				{
				if(X < m_BoundingBox.m_nX)
					m_BoundingBox.m_nX = X;
				if(Y < m_BoundingBox.m_nY)
					m_BoundingBox.m_nY = Y;
				if(X > m_BoundingBox.m_nW)
					m_BoundingBox.m_nW = X;
				if(Y > m_BoundingBox.m_nH)
					m_BoundingBox.m_nH = Y;
				}
			}
		}

	// Flip the coordinates so that they are Top Left orientated
	m_BoundingBox.m_nH = m_uiHeight - m_BoundingBox.m_nH;
	m_BoundingBox.m_nY = m_uiHeight - m_BoundingBox.m_nY;
	}
