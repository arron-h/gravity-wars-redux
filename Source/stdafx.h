// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#ifndef STDAFX_H
#define STDAFX_H

#include <stdio.h>
#ifdef WIN32
	#include <tchar.h>
#endif
#include <assert.h>
#include <math.h>
#include <stdlib.h>

typedef unsigned int	Uint32;
typedef signed int		Sint32;
typedef unsigned char	Uint8;
typedef signed char		Sint8;
typedef signed short	Sint16;
typedef unsigned short	Uint16;
typedef float			Float32;
typedef float			Float32_Clamp;

#define ASSERT assert
#define CASSERT(exp, name) typedef int dummy##name [(exp ) ? 1 : -1]

#ifdef UNICODE
	typedef wchar_t		TXChar;
	typedef wchar_t		UTXChar;
#else
	typedef char			TXChar;
	typedef unsigned char	UTXChar;
#endif

#ifdef WIN32
	#include <Windows.h>
	#include <gl/GL.h> 
	#include "glext.h"
	#include "GLEx.h"
#endif

#ifdef PLATFORM_iOS
	// Include GL here
	#import <OpenGLES/ES2/gl.h>
	#import <OpenGLES/ES2/glext.h>
	#define glOrtho		glOrthof
#endif

#ifdef __QNX__
	#include <GLES2/gl2.h>
	#include <GLES2/gl2ext.h>
#endif

#ifdef PLATFORM_ANDROID
	#include <GLES2/gl2.h>
	#include <GLES2/gl2ext.h>
#endif

#ifdef __linux__
	#include <GLES2/gl2.h>
	#include <GLES2/gl2ext.h>
#endif

#include "PVRTools.h"

// Some useful macros
#define RGBAUint(r, g, b, a)	((GLuint) (((a) << 24) | ((b) << 16) | ((g) << 8) | (r)))
#define RGBAfUint(r, g, b, a)	((GLuint) ((((GLuint)a*255) << 24) | (((GLuint)b*255)) << 16) | (((GLuint)g*255)) << 8) | ((GLuint)r*255)))
#define GETA(a)					((GLubyte) (a >> 24))
#define GETB(b)					((GLubyte) (b >> 16))
#define GETG(g)					((GLubyte) (g >> 8))
#define GETR(r)					((GLubyte) (r))

#define ah_max(a,b)            (((a) > (b)) ? (a) : (b))
#define ah_min(a,b)            (((a) < (b)) ? (a) : (b))

#define ELEMS_IN_ARRAY(x)		(sizeof(x) / sizeof(x[0]))

inline Sint32	Randomi()				{ return (rand() & RAND_MAX); }
inline Sint32	Randomi(Sint32 nMax)	{ return (Randomi() % nMax);  }
inline Float32	Randomf()				{ return ((Float32)Randomi() / (Float32)RAND_MAX); }
inline Float32	RandomfCentre()			{ return (Randomf() * 2) - 1; }

inline Float32  CapNorm(Float32 fVal)   { Float32 l = ah_min(fVal, 1.0f); return ah_max(l, 0.0f); }

#define EPSILON 0.000001f
inline Float32	DegToRad(Float32 fDeg) { return fDeg * (PVRT_PI / 180.0f); }
inline Float32	RadToDeg(Float32 fDeg) { return fDeg * (180.0f / PVRT_PI); }
inline bool		FloatIsEqual(Float32 fA, Float32 fB) { return (fabs(fA - fB) <= EPSILON ? true : false); }
inline Float32	FloatRound(Float32 fVal) { 	fVal += fVal > 0.0f ? 0.5f : -0.5f; return (Float32)((int)(fVal)); }
inline Float32	Interpolate(Float32 fS, Float32 fE, Float32 fPercent)
	{
	return ((fE - fS) * fPercent) + fS;
	}
inline Float32  EaseTo(Float32 fS, Float32 fE, Float32 fPercent)
	{
	if(fPercent > 1.0f)
		return fE;

	Float32 fTS = fPercent * fPercent;
	Float32 fTC = fTS * fPercent;
	Float32 fNewT =  0.006849315068492068f * fTC * fTS + -1.0273972602739718f * fTS * fTS + 4.041095890410959f * fTC + -6.027397260273972f * fTS + 4.006849315068493f * fPercent;

	return Interpolate(fS, fE, fNewT);
	}

inline Sint32	MakePow2(Sint32 nVal) { Sint32 nReturn = 1; while(nReturn < nVal) nReturn <<= 1; return nReturn; }
inline Uint32	MakePow2(Uint32 nVal) { Uint32 nReturn = 1; while(nReturn < nVal) nReturn <<= 1; return nReturn; }

#ifdef BUILD_DEBUG
void			DebugLog(const char* c_pszFormat, ...);
#else
inline void		DebugLog(const char* c_pszFormat, ...) {}
#endif

// Some game defined
#define PROJECTILE_TIMEOUT 12.0f
#define PROJECTILE_OOB_TIMEOUT 3.0f

struct Touch
	{
	Float32 fX;
	Float32 fY;
	};

struct Vector2i
	{
	Sint32 x;
	Sint32 y;

	Vector2i() {}
	Vector2i(Sint32 nX, Sint32 nY) : x(nX), y(nY) {}
	};


enum enumLANGUAGE
	{
	enumLANGUAGE_English,
	enumLANGUAGE_French,

	enumLANGUAGE_MAX,
	enumLANGUAGE_FIRST  = enumLANGUAGE_English,
	enumLANGUAGE_LAST   = enumLANGUAGE_French,
	};

// Framework includes

#define GFX GetApp()->GetGraphicsSystem()

#include "Thread.h"
#include "RGBA.h"
#include "Interpolator.h"
#include "Rectangle.h"
#include "Primitives.h"
#include "CollisionBitmap.h"
#include "Bitmap.h"
#include "HashMap.h"
#include "AHFont.h"
#include "Resource.h"
#include "Camera.h"
#include "ParticleSystem.h"
#include "View.h"
#include "Graphics.h"
#include "ViewManager.h"
#include "Audio.h"
#include "ObjectPool.h"

#include "App.h"
#include "Strings.h"

// Game includes
#include "Player.h"
#include "GameData.h"


extern App* GetApp();

#endif // STDAFX_H
