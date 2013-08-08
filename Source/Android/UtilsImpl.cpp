#include "stdafx.h"
#include <stdarg.h>
#include <android/log.h>

#include "UtilsImpl.h"

bool IsGLExtensionSupported(const char * const extension)
{
	// The recommended technique for querying OpenGL extensions;
	// from http://opengl.org/resources/features/OGLextensions/
	const GLubyte *extensions = NULL;
	const GLubyte *start;
	GLubyte *where, *terminator;
	
	/* Extension names should not have spaces. */
	where = (GLubyte *) strchr(extension, ' ');
	if (where || *extension == '\0')
		return 0;
	
	extensions = glGetString(GL_EXTENSIONS);
	
	/* It takes a bit of care to be fool-proof about parsing the
	 OpenGL extensions string. Don't be fooled by sub-strings, etc. */
	start = extensions;
	for (;;) {
		where = (GLubyte *) strstr((const char *) start, extension);
		if (!where)
			break;
		terminator = where + strlen(extension);
		if (where == start || *(where - 1) == ' ')
			if (*terminator == ' ' || *terminator == '\0')
				return true;
		start = terminator;
	}
	
	return false;
}

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
