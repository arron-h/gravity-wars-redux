#include "stdafx.h"
#include "Interpolator.h"

InterpolateFuncPtr InterpolatorFunctions[enumINTERPOLATOR_MAX] =
	{
	Interpolate_Linear,
	Interpolate_QuadraticEaseOut,
	Interpolate_QuadraticEaseIn,
	Interpolate_QuadraticEaseInOut,
	Interpolate_CubicEaseOut,
	Interpolate_CubicEaseIn,
	Interpolate_CubicEaseInOut,
	Interpolate_QuarticEaseOut,
	Interpolate_QuarticEaseIn,
	Interpolate_QuarticEaseInOut,
	Interpolate_QuinticEaseOut,
	Interpolate_QuinticEaseIn,
	Interpolate_QuinticEaseInOut,
	Interpolate_SinusoidalEaseOut,
	Interpolate_SinusoidalEaseIn,
	Interpolate_SinusoidalEaseInOut,
	Interpolate_ExponentialEaseOut,
	Interpolate_ExponentialEaseIn,
	Interpolate_ExponentialEaseInOut,
	Interpolate_CircularEaseOut,
	Interpolate_CircularEaseIn,
	Interpolate_CircularEaseInOut,
	};

#define BoundsCheck(t, start, end) \
	if (t <= 0.f) return start;        \
	else if (t >= 1.f) return end;


// Thanks to http://iphonedevelopment.blogspot.com/
Float32 Interpolate_Linear(Float32 start, Float32 end, Float32 t)
	{
	return ((end - start)*t) + start;
	}
Float32 Interpolate_QuadraticEaseOut(Float32 start, Float32 end, Float32 t)
	{
	Float32 fT = 1.0f - t;
	return ((start - end)*fT*fT) + end;
	}
Float32 Interpolate_QuadraticEaseIn(Float32 start, Float32 end, Float32 t)
	{
	return ((end - start)*t*t) + start;
	}
Float32 Interpolate_QuadraticEaseInOut(Float32 start, Float32 end, Float32 t)
	{
	// TODO
	return end;
	}
Float32 Interpolate_CubicEaseOut(Float32 start, Float32 end, Float32 t)
	{
	Float32 fT = 1.0f - t;
	return ((start - end)*fT*fT*fT) + end;
	}
Float32 Interpolate_CubicEaseIn(Float32 start, Float32 end, Float32 t)
	{
	return ((end - start)*t*t*t) + start;
	}
Float32 Interpolate_CubicEaseInOut(Float32 start, Float32 end, Float32 t)
	{
	// TODO
	return end;
	}
Float32 Interpolate_QuarticEaseOut(Float32 start, Float32 end, Float32 t)
	{
	Float32 fT = 1.0f - t;
	return ((start - end)*fT*fT*fT*fT) + end;
	}
Float32 Interpolate_QuarticEaseIn(Float32 start, Float32 end, Float32 t)
	{
	return ((end - start)*t*t*t*t) + start;
	}
Float32 Interpolate_QuarticEaseInOut(Float32 start, Float32 end, Float32 t)
	{
	// TODO
	return end;
	}
Float32 Interpolate_QuinticEaseOut(Float32 start, Float32 end, Float32 t)
	{
	Float32 fT = 1.0f - t;
	return ((start - end)*fT*fT*fT*fT*fT) + end;
	}
Float32 Interpolate_QuinticEaseIn(Float32 start, Float32 end, Float32 t)
	{
	return ((end - start)*t*t*t*t*t) + start;
	}
Float32 Interpolate_QuinticEaseInOut(Float32 start, Float32 end, Float32 t)
	{
	// TODO
	return end;
	}
Float32 Interpolate_SinusoidalEaseOut(Float32 start, Float32 end, Float32 t)
	{
	return end * sinf(t * (PVRT_PI/2)) + start - 1.f;
	}
Float32 Interpolate_SinusoidalEaseIn(Float32 start, Float32 end, Float32 t)
	{
	return -end * cosf(t * (PVRT_PI/2)) + end + start - 1.f;
	}
Float32 Interpolate_SinusoidalEaseInOut(Float32 start, Float32 end, Float32 t)
	{
	return -end/2.f * (cosf(PVRT_PI*t) - 1.f) + start - 1.f;
	}
Float32 Interpolate_ExponentialEaseOut(Float32 start, Float32 end, Float32 t)
	{
	return end * (-powf(2.f, -10.f * t) + 1.f ) + start - 1.f;
	}
Float32 Interpolate_ExponentialEaseIn(Float32 start, Float32 end, Float32 t)
	{
	return end * powf(2.f, 10.f * (t - 1.f) ) + start - 1.f;
	}
Float32 Interpolate_ExponentialEaseInOut(Float32 start, Float32 end, Float32 t)
	{
	t *= 2.f;
	if (t < 1.f) 
		return end/2.f * powf(2.f, 10.f * (t - 1.f) ) + start - 1.f;
	t--;
	return end/2.f * ( -powf(2.f, -10.f * t) + 2.f ) + start - 1.f;
	}
Float32 Interpolate_CircularEaseOut(Float32 start, Float32 end, Float32 t)
	{
	t--;
	return end * sqrtf(1.f - t * t) + start - 1.f;
	}
Float32 Interpolate_CircularEaseIn(Float32 start, Float32 end, Float32 t)
	{
	return -end * (sqrtf(1.f - t * t) - 1.f) + start - 1.f;
	}
Float32 Interpolate_CircularEaseInOut(Float32 start, Float32 end, Float32 t)
	{
	t *= 2.f;
	if (t < 1.f) 
		return -end/2.f * (sqrtf(1.f - t * t) - 1.f) + start - 1.f;
	t -= 2.f;
	return end/2.f * (sqrtf(1.f - t * t) + 1.f) + start - 1.f;
	}

Float32 Interpolate_Func(enumINTERPOLATOR eType, Float32 fStart, Float32 fEnd, Float32 fPerc)
	{
	return(InterpolatorFunctions[eType](fStart, fEnd, fPerc));
	}