#include "stdafx.h"
#ifdef WIN32
	#include <Windows.h>
	#include <gl/GL.h>
#else

#endif

#include "AHFont.h"
#include <stdio.h>
#include <wchar.h>

int CharacterCompareFunc(const void* pA, const void* pB)
	{
	return (*(UTXChar*)pA - *(UTXChar*)pB);
	}

int AHFont::KerningCompareFunc(const void* pA, const void* pB)
	{
	KerningPair* pPairA = (KerningPair*)pA;
	KerningPair* pPairB = (KerningPair*)pB;
	return (pPairA->uiPair - pPairB->uiPair);
	}

size_t AHFont_StrLen(const TXChar* c_pszString)
	{
#ifdef UNICODE
	return wcslen(c_pszString);
#else
	return strlen(c_pszString);
#endif
	}

TXChar* AHFont_StrCpy(TXChar* pszDest, const TXChar* c_pszSrc) 
	{
#ifdef UNICODE
	return wcscpy(pszDest, c_pszSrc);
#else
	return strcpy(pszDest, c_pszSrc);
#endif
	}

#define VERSION (4)
static const char IDENT[] = {'A','H','F'};

/*!***********************************************************************
 @Function		AHFont
 @Access		public 
 @Returns		
 @Description	
*************************************************************************/
AHFont::AHFont() :  m_uiTexHandle(0), m_pRects(NULL), m_pCharMatrics(NULL), m_pYOffsets(NULL),
					m_uiSpaceWidth(0), m_uiNumCharacters(0), m_pszCharacterList(NULL), m_uiEncoding(0),
					m_uiNumKerningPairs(0), m_pKerningPairs(NULL)
	{
	}

/*!***********************************************************************
 @Function		~AHFont
 @Access		public 
 @Returns		
 @Description	
*************************************************************************/
AHFont::~AHFont()
	{
	if(m_pRects)
		delete [] m_pRects;
	if(m_pCharMatrics)
		delete [] m_pCharMatrics;
	if(m_pYOffsets)
		delete [] m_pYOffsets;
	if(m_pszCharacterList)
		delete [] m_pszCharacterList;
	if(m_pKerningPairs)
		delete [] m_pKerningPairs;
	if(m_pUVs)
		delete [] m_pUVs;
	}

/*!***********************************************************************
 @Function		Load
 @Access		public 
 @Param			Uint32 uiTexHandle
 @Param			const char * c_pszFilename
 @Param			Uint32 uiTexW
 @Param			Uint32 uiTexH
 @Returns		void
 @Description	
*************************************************************************/
void AHFont::Load(Uint32 uiTexHandle, const char* c_pszFilename, Uint32 uiTexW, Uint32 uiTexH)
	{
	m_uiTexHandle = uiTexHandle;
	m_fTexW = (Float32)uiTexW;
	m_fTexH = (Float32)uiTexH;

	struct FONTBUILDER_HEADER			// 16 bytes
		{
		char	acIdent[3];
		Uint8	uVersion;
		Uint8	uEncodingType;
		short	wNumCharacters;
		Uint8	uSpaceWidth;			// The width of the 'Space' character
		short	wNumKerningPairs;
		short	wAscent;				// The height of the character, in pixels, from the base line
		short	wLineSpace;				// The base line to base line dimension, in pixels.
		short	wBorderWidth;
		} header;

	FILE* pFile = fopen(c_pszFilename, "rb");
	if(!pFile)
		{
		printf("Couldn't open: %s\n", c_pszFilename);
		return;
		}

	// --- Read the header
	fread(&header, sizeof(FONTBUILDER_HEADER), 1, pFile);

	// Check the ident
	if(header.acIdent[0] != IDENT[0] || header.acIdent[1] != IDENT[1] || header.acIdent[2] != IDENT[2])
		{
		printf("Invalid font file: %s\n", c_pszFilename);
		fclose(pFile);
		return;
		}

	// Check version
	if(header.uVersion > VERSION)
		{
		printf("Font versions do not match!\n");
		fclose(pFile);
		return;
		}

	// Copy options
	m_uiNextLineH		= header.wLineSpace;
	m_uiEncoding		= header.uEncodingType;
	m_uiSpaceWidth		= header.uSpaceWidth;
	m_uiNumCharacters	= header.wNumCharacters & 0xFFFF;
	m_uiNumKerningPairs = header.wNumKerningPairs & 0xFFFF;

	// --- Create some space and read the rest of the file
	m_pRects  = new Rectanglei[m_uiNumCharacters];
	m_pCharMatrics = new CharMetrics[m_uiNumCharacters];
	m_pYOffsets   = new Sint32[m_uiNumCharacters];
	m_pszCharacterList = new unsigned char[m_uiNumCharacters * m_uiEncoding];		// Need to make sure we create enough space for UTF-16
	
	// Read the data
	fread(m_pszCharacterList, m_uiEncoding, m_uiNumCharacters, pFile);		// Read character list
	fread(m_pRects, sizeof(Rectanglei), m_uiNumCharacters, pFile);			// Read Rectangles
	fread(m_pCharMatrics, sizeof(CharMetrics), m_uiNumCharacters, pFile);				// Read character widths
	fread(m_pYOffsets, sizeof(Sint32), m_uiNumCharacters, pFile);			// Read Y offsets

	if(m_uiNumKerningPairs)
		{
		m_pKerningPairs = new KerningPair[m_uiNumKerningPairs];
		fread(m_pKerningPairs, sizeof(KerningPair), m_uiNumKerningPairs, pFile);
		}

	// --- Build UVs
	m_pUVs = new CharacterUV[m_uiNumCharacters];
	for(Uint32 uiChar = 0; uiChar < m_uiNumCharacters; uiChar++)
		{
		m_pUVs[uiChar].fUL = m_pRects[uiChar].m_nX / m_fTexW;
		m_pUVs[uiChar].fUR = m_pUVs[uiChar].fUL + m_pRects[uiChar].m_nW / m_fTexW;
		m_pUVs[uiChar].fVB = m_pRects[uiChar].m_nY / m_fTexH;
		m_pUVs[uiChar].fVT = m_pUVs[uiChar].fVB + m_pRects[uiChar].m_nH / m_fTexH;
		}

	fclose(pFile);
	}

/*!***********************************************************************
 @Function		FindCharacter
 @Access		private 
 @Param			TXChar character
 @Returns		Uint32
 @Description	
*************************************************************************/
Uint32 AHFont::FindCharacter(TXChar character) const
	{
	unsigned char* pItem = (unsigned char*)bsearch(&character, m_pszCharacterList, m_uiNumCharacters, m_uiEncoding, CharacterCompareFunc);
	if(!pItem)
		return INVALID_CHAR;

	Uint32 uiIdx = (pItem - m_pszCharacterList) / m_uiEncoding;
	return uiIdx;
	}

/*!***********************************************************************
 @Function		MeasureLength
 @Access		public 
 @Param			const TXChar * c_pszString
 @Param			Float32 fKern
 @Returns		Float32
 @Description	
*************************************************************************/
Float32 AHFont::MeasureLength(const TXChar* c_pszString, Float32 fKern) const
	{
	Float32 fLength = 0;
	TXChar txNextChar = 0;
	Uint32 uiIdx;
	for(Uint32 uiIndex = 0; uiIndex < AHFont_StrLen(c_pszString); uiIndex++)
		{
		uiIdx = FindCharacter(c_pszString[uiIndex]);
		if(uiIdx == INVALID_CHAR)		// No character found. Add a space.
			{
			fLength += m_uiSpaceWidth;
			continue;
			}

		txNextChar = c_pszString[uiIndex + 1];
		Float32 fKernOffset = 0;
		ApplyKerning(c_pszString[uiIndex], txNextChar, fKernOffset);
		fKernOffset += fKern;

		fLength += m_pCharMatrics[uiIdx].nAdv + fKernOffset;		// Add on this characters width
		}

	return fLength;
	}

/*!***********************************************************************
 @Function		ApplyKerning
 @Access		private 
 @Param			const TXChar cA
 @Param			const TXChar cB
 @Param			Float32 & fOffset
 @Returns		void
 @Description	
*************************************************************************/
void AHFont::ApplyKerning(const TXChar cA, const TXChar cB, Float32& fOffset) const
	{
	// --- Check the kerning table
	Uint32 uiPairToSearch = ((UTXChar)cA << 16) | (UTXChar)cB;
	KerningPair* pItem = (KerningPair*)bsearch(&uiPairToSearch, m_pKerningPairs, m_uiNumKerningPairs, sizeof(KerningPair), KerningCompareFunc);
	if(pItem)
		fOffset += (Float32)pItem->uOffset;
	}

/*!***********************************************************************
 @Function		GetHeight
 @Access		public 
 @Returns		Float32
 @Description	
*************************************************************************/
Float32 AHFont::GetHeight() const
	{
	return (Float32)m_uiNextLineH;
	}

/*!***********************************************************************
 @Function		RenderString
 @Access		public 
 @Param			const TXChar * c_pszString
 @Param			Sint32 nX
 @Param			Sint32 nY
 @Param			enumTEXTJUSTIFY eJustify
 @Param			Float32 fScale
 @Param			Float32 fKern
 @Param			const RGBA * pRGBA
 @Returns		void
 @Description	
*************************************************************************/
void AHFont::RenderString(const TXChar* c_pszString, Sint32 nX, Sint32 nY, enumTEXTJUSTIFY eJustify, Float32 fScale, Float32 fKern, const RGBA* pRGBA)
	{
	if(!m_pszCharacterList)
		return;

#ifdef _DEBUG
	GLint nActiveProg;
	glGetIntegerv(GL_CURRENT_PROGRAM, &nActiveProg);
	ASSERT(nActiveProg != 0);
#endif

	glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnableVertexAttribArray(enumATTRIBUTE_POSITION);
	glEnableVertexAttribArray(enumATTRIBUTE_TEXCOORD0);
	glEnableVertexAttribArray(enumATTRIBUTE_RGBA);


	Float32 fLen = MeasureLength(c_pszString, fKern) * fScale;
	if(eJustify == enumTEXTJUSTIFY_Centre)
		nX = nX - ((Sint32)fLen / 2);
	else if(eJustify == enumTEXTJUSTIFY_Right)
		nX = nX - (Sint32)fLen;

	Float32 fXOffset  = 0;
	Float32 fYOffset  = 0;
	TXChar txNextChar = 0;
	Uint32 uiNumCharsInString = AHFont_StrLen(c_pszString);
	for(Uint32 uiIndex = 0; uiIndex < uiNumCharsInString; uiIndex++)
		{
		if(c_pszString[uiIndex] == '\n')
			{
			fYOffset -= (float)m_uiNextLineH;
			fXOffset  = 0;
			continue;
			}
		Uint32 uiIdx = FindCharacter(c_pszString[uiIndex]);
		if(uiIdx == INVALID_CHAR)		// No character found. Add a space.
			{
			fXOffset += m_uiSpaceWidth * fScale;
			continue;
			}
	
		txNextChar = c_pszString[uiIndex + 1];
		Float32 fKernOffset = 0;
		ApplyKerning(c_pszString[uiIndex], txNextChar, fKernOffset);
		fKernOffset += fKern;
		
		/*
		1___________3
		|			|
		|			|
		|			|
		|___________|
		2			4
		*/
		Float32 fAOff = m_pCharMatrics[uiIdx].nXOff * fScale;		// The A offset. Could include overhang or underhang

		PrimQuad quad;
		// --- Verts
		memset(&quad.vert[0], 0, sizeof(PrimVertex));
		memset(&quad.vert[1], 0, sizeof(PrimVertex));
		memset(&quad.vert[2], 0, sizeof(PrimVertex));
		memset(&quad.vert[3], 0, sizeof(PrimVertex));
		quad.vert[0].v.x = fXOffset + nX + fAOff + 0.0f;							quad.vert[0].v.y = fYOffset + nY + (-m_pYOffsets[uiIdx] * fScale) - 0.0f;
		quad.vert[1].v.x = fXOffset + nX + fAOff + 0.0f;							quad.vert[1].v.y = fYOffset + nY + (-m_pYOffsets[uiIdx] * fScale) - (m_pRects[uiIdx].m_nH  * fScale);
		quad.vert[2].v.x = fXOffset + nX + fAOff + (m_pRects[uiIdx].m_nW * fScale);	quad.vert[2].v.y = fYOffset + nY + (-m_pYOffsets[uiIdx] * fScale) - 0.0f;
		quad.vert[3].v.x = fXOffset + nX + fAOff + (m_pRects[uiIdx].m_nW * fScale);	quad.vert[3].v.y = fYOffset + nY + (-m_pYOffsets[uiIdx] * fScale) - (m_pRects[uiIdx].m_nH  * fScale);

		// --- RGBA
		if(!pRGBA)
			pRGBA = &rgbaWHITE;
		Uint8 u8RGBA[4] = { pRGBA->RedChannel(), pRGBA->GreenChannel(), pRGBA->BlueChannel(), pRGBA->AlphaChannel() };
		memcpy(quad.vert[0].rgba, u8RGBA, sizeof(Uint8) * 4);
		memcpy(quad.vert[1].rgba, u8RGBA, sizeof(Uint8) * 4);
		memcpy(quad.vert[2].rgba, u8RGBA, sizeof(Uint8) * 4);
		memcpy(quad.vert[3].rgba, u8RGBA, sizeof(Uint8) * 4);

		// --- UVs
		quad.vert[0].uv.x = m_pUVs[uiIdx].fUL;		quad.vert[0].uv.y = m_pUVs[uiIdx].fVB;
		quad.vert[1].uv.x = m_pUVs[uiIdx].fUL;		quad.vert[1].uv.y = m_pUVs[uiIdx].fVT;
		quad.vert[2].uv.x = m_pUVs[uiIdx].fUR;		quad.vert[2].uv.y = m_pUVs[uiIdx].fVB;
		quad.vert[3].uv.x = m_pUVs[uiIdx].fUR;		quad.vert[3].uv.y = m_pUVs[uiIdx].fVT;


		glBindTexture(GL_TEXTURE_2D, m_uiTexHandle);
		glVertexAttribPointer(enumATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(PrimVertex), &quad.vert[0].v);
		glVertexAttribPointer(enumATTRIBUTE_TEXCOORD0, 2, GL_FLOAT, GL_FALSE, sizeof(PrimVertex), &quad.vert[0].uv);
		glVertexAttribPointer(enumATTRIBUTE_RGBA, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(PrimVertex), &quad.vert[0].rgba);

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		// Apply kerning and scale it. px+=(chr.sx1-chr.sx0+kx)*sx;
		fXOffset += (m_pCharMatrics[uiIdx].nAdv + fKernOffset) * fScale;		// Add on this characters width
		}

	glDisableVertexAttribArray(enumATTRIBUTE_POSITION);
	glDisableVertexAttribArray(enumATTRIBUTE_TEXCOORD0);
	glDisableVertexAttribArray(enumATTRIBUTE_RGBA);
	
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	}