#ifndef COLLISION_BITMAP
#define COLLISION_BITMAP

class CollisionBitmap
	{
	friend class Bitmap;
	public:
		enum enumSOURCE
			{
			enumSOURCE_Centre,
			enumSOURCE_BottomLeft,
			enumSOURCE_TopLeft,
			};

	private:
		Uint8*		m_pData;
		Uint32		m_uiWidth;
		Uint32		m_uiHeight;

		Rectanglei	m_BoundingBox;

		void DumpToTGA(const char* c_pszFilename);
		void CalculateBoundingBox();

		CollisionBitmap();			// Instantiated from Bitmap class
	public:
		~CollisionBitmap();

		bool Check(const PVRTVec2& vPoint, enumSOURCE eSource) const;
		bool Check(Uint32 uiX, Uint32 uiY) const;

		const Rectanglei& GetBoundingBox() const;
	};

#endif
