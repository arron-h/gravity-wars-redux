#ifndef AHFONT_H
#define AHFONT_H

enum enumENCODINGTYPE
	{
	enumENCODINGTYPE_ASCII	= 1,
	enumENCODINGTYPE_UTF16	= 2,
	};

enum enumTEXTJUSTIFY
	{
	enumTEXTJUSTIFY_Left,
	enumTEXTJUSTIFY_Centre,
	enumTEXTJUSTIFY_Right,
	};

static const Uint32 INVALID_CHAR = 0xFFFFFFFF;

class AHFont
	{
	private:
		struct CharacterUV
			{
			Float32 fUL;
			Float32 fVT;
			Float32 fUR;
			Float32 fVB;
			} *m_pUVs;

		struct KerningPair
			{
			Uint32	uiPair;
			char	uOffset;
			} *m_pKerningPairs;

		struct CharMetrics
			{
			Sint16 nXOff;				// Prefix offset
			Uint16 nAdv;				// Character width
			} *m_pCharMatrics;

	private:
		unsigned int		m_uiTexHandle;
		Float32				m_fTexW;
		Float32				m_fTexH;

		Rectanglei*			m_pRects;
		Sint32*				m_pYOffsets;
		Uint32				m_uiNextLineH;

		Uint32				m_uiSpaceWidth;	

		Uint32				m_uiNumCharacters;
		unsigned char*		m_pszCharacterList;

		Uint32				m_uiNumKerningPairs;

		Uint8				m_uiEncoding;

		friend class Text;

	private:
		Uint32 FindCharacter(TXChar character) const;
		void ApplyKerning(const TXChar cA, const TXChar cB, Float32& fOffset) const;
		static int KerningCompareFunc(const void* pA, const void* pB);

	public:
		AHFont();
		~AHFont();
		
		void Load(Uint32 uiTexHandle, const char* c_pszFilename, Uint32 uiTexW, Uint32 uiTexH);
		void RenderString(const TXChar* c_pszString, Sint32 nX, Sint32 nY, enumTEXTJUSTIFY eJustify = enumTEXTJUSTIFY_Centre, Float32 fScale = 1.0f, Float32 fKern = 0.0f, const RGBA* pRGBA = NULL);
		Float32 MeasureLength(const TXChar* c_pszString, Float32 fKern) const;
		Float32 GetHeight() const;
	};

#endif
