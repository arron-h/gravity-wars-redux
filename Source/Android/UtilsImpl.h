#ifndef UTILSIMPL_H
#define UTILSIMPL_H

bool IsGLExtensionSupported(const char * const extension);

#ifdef BUILD_DEBUG
void DebugLog(const char* c_pszFormat, ...);
#endif

#endif // UTILSIMPL_H