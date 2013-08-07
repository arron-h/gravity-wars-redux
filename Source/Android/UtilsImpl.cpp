#include "stdafx.h"
#include <stdarg.h>
#include <android/log.h>

#ifdef BUILD_DEBUG
void DebugLog(const char* c_pszFormat, ...)
	{
	Sint32 nSize;
	char szBuffer[1024];

	va_list vaList;
	va_start(vaList, c_pszFormat);
	nSize = vsnprintf(szBuffer, sizeof(szBuffer) - 1, c_pszFormat, vaList);
	va_end(vaList);

	if(nSize < 0)
		szBuffer[sizeof(szBuffer) - 1] = 0;
	else
		szBuffer[nSize] = 0;

	__android_log_print(ANDROID_LOG_ERROR, "GWRedux", "%s\n", szBuffer);
	}
#endif
