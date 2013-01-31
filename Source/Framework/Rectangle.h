#ifndef RECTANGLE_H
#define RECTANGLE_H

class Rectanglef
	{
	public:
		Float32		m_fX;
		Float32		m_fY;
		Float32		m_fW;
		Float32		m_fH;

		Rectanglef()
			{
			}

		Rectanglef(Float32 fX, Float32 fY, Float32 fW, Float32 fH) : m_fX(fX), m_fY(fY), m_fW(fW), m_fH(fH)
			{
			}

		inline bool Contains(const PVRTVec2& vPoint) const
			{
			if(vPoint.x > m_fX && vPoint.x < m_fW && vPoint.y > m_fY && vPoint.y < m_fH)
				return true;

			return false;
			}

		inline PVRTVec2 ClosestPointTo(const PVRTVec2 &p) const 
			{
			PVRTVec2 r;

			// X
			if (p.x < m_fX) 
				r.x = m_fX;
			else if (p.x > m_fW) 
				r.x = m_fW;
			else 
				r.x = p.x;
			
			// Y
			if (p.y < m_fY)
				r.y = m_fY;
			else if (p.y > m_fH)
				r.y = m_fH;
			else
				r.y = p.y;

			return r;
			}
	};

class Rectanglei
	{
	public:
		Sint32		m_nX;
		Sint32		m_nY;
		Sint32		m_nW;
		Sint32		m_nH;

		Rectanglei()
			{
			}

		Rectanglei(Sint32 nX, Sint32 nY, Sint32 nW, Sint32 nH) : m_nX(nX), m_nY(nY), m_nW(nW), m_nH(nH)
			{
			}

		inline void Reset()
			{
			m_nX = m_nY = m_nW = m_nH = 0;
			}

		inline Sint32 Area() const
			{
			return m_nW * m_nH;
			}

		// Tests whether the given character rectangle can fit inside this rectangle
		// 1 = Doesn't fit. -1 = Fits with gaps. 0 = Fits perfectly.
		inline Sint32 DoesFit(const Rectanglei& rectToTest) const
			{	
			if(	rectToTest.m_nW == m_nW && rectToTest.m_nH == m_nH)
				return 0;
			else if(rectToTest.m_nW <= m_nW && rectToTest.m_nH <= m_nH)
				return -1;
			else
				return 1;
			}
	};

#endif
