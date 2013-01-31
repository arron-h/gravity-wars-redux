#ifndef PRIMITIVES_H
#define PRIMITIVES_H

struct PrimPoint
	{
	PVRTVec3	v;
	Float32		s;
	Uint8		rgba[4];
	};

struct PrimVertex
	{
	PVRTVec3	v;
	PVRTVec3	n;
	PVRTVec2	uv;
	Uint8		rgba[4];

	void Reset()
		{
		v.x = v.y = v.z = n.x = n.y = n.z = 0.0f;
		uv.x = uv.y = 0.0f;
		rgba[0] = rgba[1] = rgba[2] = rgba[3] = 0;
		}
	};

struct PrimQuad
	{
	static Uint32 uiVerts;
	PrimVertex	vert[4];
	};

struct PrimTriangle
	{
	static Uint32 uiVerts;
	PrimVertex vert[3];
	};

template <class TType>
class PrimitiveList
	{
	private:
		TType*		m_pList;
		Uint32		m_uiCount;

	public:
		PrimitiveList() : m_pList(NULL), m_uiCount(0)
			{
			}

		~PrimitiveList()
			{
			if(m_pList)
				delete [] m_pList;
			}

		void Initialise(Uint32 uiCount)
			{
			m_uiCount = uiCount;
			m_pList = new TType[uiCount];

//			for(Uint32 i = 0; i < uiCount; i++)
//				for(Uint32 j = 0; j < TType::uiVerts; j++)
//				m_pList[i].vert[j].Reset();				
			}

		Uint32 Count() const
			{ 
			return m_uiCount; 
			}

		const TType* Pointer() const
			{
			return m_pList;
			}

		TType& operator[](Uint32 i) 
			{ 
			ASSERT(i < m_uiCount && i >= 0);
			return m_pList[i];
			}
	};

#endif
