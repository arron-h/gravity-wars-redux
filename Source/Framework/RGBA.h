#ifndef RGBA_H
#define RGBA_H

class RGBA
	{
	public:
		Float32		m_fR;
		Float32		m_fG;
		Float32		m_fB;
		Float32		m_fA;

	public:
		RGBA() {}
		RGBA(const Float32 fR, const Float32 fG, const Float32 fB, const Float32 fA) : m_fR(fR), m_fG(fG), m_fB(fB), m_fA(fA) {}
		RGBA(Uint32 uiColour) { Set(uiColour); }

		void Set(Float32 fR, Float32 fG, Float32 fB, Float32 fA = 1.0f) { m_fR = fR; m_fG = fG; m_fB = fB; m_fA = fA; }
		void Set(Uint32 uiColour)
			{
			m_fR = (Float32)((uiColour >> 0)  & 0xFF) / 255.0f;
			m_fG = (Float32)((uiColour >> 8)  & 0xFF) / 255.0f;
			m_fB = (Float32)((uiColour >> 16) & 0xFF) / 255.0f;
			m_fA = (Float32)((uiColour >> 24) & 0xFF) / 255.0f;
			}

		inline Uint8 RedChannel()   const { return (Uint8)(m_fR * 255); }
		inline Uint8 BlueChannel()  const { return (Uint8)(m_fB * 255); }
		inline Uint8 GreenChannel() const { return (Uint8)(m_fG * 255); }
		inline Uint8 AlphaChannel() const { return (Uint8)(m_fA * 255); }

		RGBA& operator+=(const RGBA &rhs) 
			{
			m_fR += rhs.m_fR;
			m_fG += rhs.m_fG;
			m_fB += rhs.m_fB;
			m_fA += rhs.m_fA;			// Should we do this?

			if(m_fR > 1.0f) m_fR = 1.0f;
			if(m_fG > 1.0f) m_fG = 1.0f;
			if(m_fB > 1.0f) m_fB = 1.0f;
			if(m_fA > 1.0f) m_fA = 1.0f;

			return *this;
			}

		const RGBA operator+(const RGBA& rhs) const
			{
			RGBA result = *this;
			result += rhs;
			return result;
			}

		RGBA& operator-=(const RGBA &rhs) 
			{
			m_fR -= rhs.m_fR;
			m_fG -= rhs.m_fG;
			m_fB -= rhs.m_fB;
			m_fA -= rhs.m_fA;			// Should we do this?

			if(m_fR < 0.0f) m_fR = 0.0f;
			if(m_fG < 0.0f) m_fG = 0.0f;
			if(m_fB < 0.0f) m_fB = 0.0f;
			if(m_fA < 0.0f) m_fA = 0.0f;

			return *this;
			}

		const RGBA operator-(const RGBA& rhs) const
			{
			RGBA result = *this;
			result -= rhs;
			return result;
			}

		operator const Uint32() const
			{ 
			Uint32 uiR = (Uint32)(m_fR * 255.0f) & 0xFF;
			Uint32 uiG = (Uint32)(m_fG * 255.0f) & 0xFF;
			Uint32 uiB = (Uint32)(m_fB * 255.0f) & 0xFF;
			Uint32 uiA = (Uint32)(m_fA * 255.0f) & 0xFF;
			return (uiA << 24) | (uiB << 16) | (uiG << 8) | uiR; 
			}		
	};

static const RGBA rgbaWHITE(1,1,1,1);

#endif // ! RGBA_H
