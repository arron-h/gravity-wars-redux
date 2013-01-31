#ifndef INTERPOLATOR_H
#define INTERPOLATOR_H

enum enumINTERPOLATOR
	{
	enumINTERPOLATOR_Linear,
	enumINTERPOLATOR_QuadraticEaseOut,
	enumINTERPOLATOR_QuadraticEaseIn,
	enumINTERPOLATOR_QuadraticEaseInOut,
	enumINTERPOLATOR_CubicEaseOut,
	enumINTERPOLATOR_CubicEaseIn,
	enumINTERPOLATOR_CubicEaseInOut,
	enumINTERPOLATOR_QuarticEaseOut,
	enumINTERPOLATOR_QuarticEaseIn,
	enumINTERPOLATOR_QuarticEaseInOut,
	enumINTERPOLATOR_QuinticEaseOut,
	enumINTERPOLATOR_QuinticEaseIn,
	enumINTERPOLATOR_QuinticEaseInOut,
	enumINTERPOLATOR_SinusoidalEaseOut,
	enumINTERPOLATOR_SinusoidalEaseIn,
	enumINTERPOLATOR_SinusoidalEaseInOut,
	enumINTERPOLATOR_ExponentialEaseOut,
	enumINTERPOLATOR_ExponentialEaseIn,
	enumINTERPOLATOR_ExponentialEaseInOut,
	enumINTERPOLATOR_CircularEaseOut,
	enumINTERPOLATOR_CircularEaseIn,
	enumINTERPOLATOR_CircularEaseInOut,
	enumINTERPOLATOR_MAX,
	};

// ------ Interpolator Prototypes
typedef Float32 (*InterpolateFuncPtr)(Float32, Float32, Float32);
Float32 Interpolate_Linear(Float32 start, Float32 end, Float32 t);
Float32 Interpolate_QuadraticEaseOut(Float32 start, Float32 end, Float32 t);
Float32 Interpolate_QuadraticEaseIn(Float32 start, Float32 end, Float32 t);
Float32 Interpolate_QuadraticEaseInOut(Float32 start, Float32 end, Float32 t);
Float32 Interpolate_CubicEaseOut(Float32 start, Float32 end, Float32 t);
Float32 Interpolate_CubicEaseIn(Float32 start, Float32 end, Float32 t);
Float32 Interpolate_CubicEaseInOut(Float32 start, Float32 end, Float32 t);
Float32 Interpolate_QuarticEaseOut(Float32 start, Float32 end, Float32 t);
Float32 Interpolate_QuarticEaseIn(Float32 start, Float32 end, Float32 t);
Float32 Interpolate_QuarticEaseInOut(Float32 start, Float32 end, Float32 t);
Float32 Interpolate_QuinticEaseOut(Float32 start, Float32 end, Float32 t);
Float32 Interpolate_QuinticEaseIn(Float32 start, Float32 end, Float32 t);
Float32 Interpolate_QuinticEaseInOut(Float32 start, Float32 end, Float32 t);
Float32 Interpolate_SinusoidalEaseOut(Float32 start, Float32 end, Float32 t);
Float32 Interpolate_SinusoidalEaseIn(Float32 start, Float32 end, Float32 t);
Float32 Interpolate_SinusoidalEaseInOut(Float32 start, Float32 end, Float32 t);
Float32 Interpolate_ExponentialEaseOut(Float32 start, Float32 end, Float32 t);
Float32 Interpolate_ExponentialEaseIn(Float32 start, Float32 end, Float32 t);
Float32 Interpolate_ExponentialEaseInOut(Float32 start, Float32 end, Float32 t);
Float32 Interpolate_CircularEaseOut(Float32 start, Float32 end, Float32 t);
Float32 Interpolate_CircularEaseIn(Float32 start, Float32 end, Float32 t);
Float32 Interpolate_CircularEaseInOut(Float32 start, Float32 end, Float32 t);

Float32 Interpolate_Func(enumINTERPOLATOR eType, Float32 fStart, Float32 fEnd, Float32 fPerc);

template<class TType>
class Interpolator
	{
	private:
		bool				m_bActive;
		Float32				m_fPerc;
		Float32				m_fPercDelta;
		TType				m_Start;
		TType				m_End;
		TType				m_Current;
		enumINTERPOLATOR	m_eType;

	public:
		Interpolator() : m_bActive(false), m_eType(enumINTERPOLATOR_Linear) 
			{ 		
			}

		inline bool IsActive() const { return m_bActive; }

		void Open(const TType& c_Start, const TType& c_End, enumINTERPOLATOR eType, Float32 fTimeSecs = 0)
			{
			m_eType		= eType;
			m_Start		= c_Start;
			m_End		= c_End;
			m_Current	= c_Start;
			m_bActive	= false;
			

			if(fTimeSecs > 0.0f)
				Start(fTimeSecs);
			}

		void Start(Float32 fTimeSecs)
			{
			m_bActive = true;
			m_fPercDelta = 1.0f / fTimeSecs;
			m_fPerc	= 0.0f;
			}

		const TType& Value() { return m_Current; }

		void Update(double fDT)
			{
			m_fPerc += (Float32)fDT * m_fPercDelta;
			if(m_fPerc < 0.0f) 
				m_fPerc = 0.0f;
			else if(m_fPerc > 1.0f) 
				{
				m_bActive = false;
				m_fPerc = 1.0f;
				}

			// Get the time on the curve
			Float32 fTime = Interpolate_Func(m_eType, 0.0, 1.0f, m_fPerc);

			// Linear interpolate this value
			m_Current = Interpolate(m_Start, m_End, fTime);
			}
	};


#endif
