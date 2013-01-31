#ifndef BITMAP_H
#define BITMAP_H

enum enumIMAGETYPE
	{
	enumIMAGETYPE_TGA,
	enumIMAGETYPE_PNG,
	enumIMAGETYPE_Invalid,
	};

enum enumFORMAT
	{
	enumFORMAT_RGBA8888,
	enumFORMAT_RGB888,
	enumFORMAT_RGB565,
	enumFORMAT_A8,
	enumFORMAT_Invalid,
	};

class CollisionBitmap;
class Bitmap
	{
	public:
		enum enumBLEND
			{
			BLEND_Normal,
			BLEND_Darken,
			BLEND_Multiply,
			BLEND_ColourBurn,
			BLEND_LinearBurn,
			BLEND_Lighten,
			BLEND_Screen,
			BLEND_ColourDodge,
			BLEND_LinearDodge,
			BLEND_Overlay,
			BLEND_SoftLight,
			BLEND_HardLight,
			BLEND_VividLight,
			BLEND_LinearLight,
			BLEND_PinLight,
			BLEND_Difference,
			BLEND_Exclusion,
			};

	private:
		Uint32		m_uiWidth;
		Uint32		m_uiHeight;
		Uint32		m_uiBytesPP;
		Uint8*		m_pData;
		enumFORMAT	m_eFormat;

	private:
		bool SaveAsPNG(const char* c_pszFilename);			// Defined in BitmapImpl.cpp
		bool SaveAsTGA(const char* c_pszFilename);
		bool CreateFromPNG(const char* c_pszFilename);		// Defined in BitmapImpl.cpp
		bool CreateFromTGA(const char* c_pszFilename);

		Bitmap(const Bitmap& /*rhs*/) {}
		Bitmap& operator=(const Bitmap& /*rhs*/) { return *this; }

	public:
		Bitmap() : m_uiWidth(0), m_uiHeight(0), m_uiBytesPP(0), m_pData(NULL), m_eFormat(enumFORMAT_Invalid) {}
		~Bitmap() { Delete(); }

		bool IsValid();

		void AddLayer(const Bitmap& bmLayer, Uint32 uiX, Uint32 uiY, enumBLEND eBlendMode);
		void Scale(Float32 fScaleX, Float32 fScaleY);
		void Colourise(const RGBA& rgba);
		void ToFormat(enumFORMAT eFormat);

		void Create(Uint32 uiWidth, Uint32 uiHeight, enumFORMAT eFormat);
		bool CreateFromImage(const char* c_pszFilename, enumIMAGETYPE eType);
		void Delete();
		void Copy(Bitmap& bitmapDst);

		void SetPixel(Uint32 uiX, Uint32 uiY, const RGBA& rgba);
		void GetPixel(Uint32 uiX, Uint32 uiY, RGBA& rgba) const;

		void FillRectangle(const Rectanglei& rect, const RGBA& rgba);
		bool Save(const char* c_pszFilename, enumIMAGETYPE eFormat);
		Uint32 ToOpenGL(bool bMipMap);
		CollisionBitmap* CreateCollisionBitmap();

		Uint32 GetWidth()  { return m_uiWidth; }
		Uint32 GetHeight() { return m_uiHeight; }
	};

#endif // ! GEBITMAP_H