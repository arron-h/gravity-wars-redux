#include "stdafx.h"
#include "TextureImpl.h"
#include "../../libs/libpng/include/png.h"

Uint32 ResourceManagerImpl::Load(const char* c_pszFilename, Uint32& uiW, Uint32& uiH)
	{
	FILE* pFile = NULL;
	char szFile[256];
	char szPath[512];
	GetResourcePath(szPath, 512);

	ASSERT(strchr(c_pszFilename, '.') == NULL);		// Shouldn't load with an extension (we'll load the correct file ourselves).
	
	// Try PVR first
	sprintf(szFile, "%s/%s.pvr", szPath, c_pszFilename);
	pFile = fopen(szFile, "rb");
	if(pFile)
    {
    fclose(pFile);
		return LoadPVR(szFile, uiW, uiH);
		}

	// Try PNG second
	sprintf(szFile, "%s/%s.png", szPath, c_pszFilename);
	pFile = fopen(szFile, "rb");
	if(pFile)
    {
    fclose(pFile);
		return LoadPNG(szFile, uiW, uiH);
		}

	// Bail.
	return 0;
	}

/*!***********************************************************************
 @Function		LoadPNG
 @Access		private 
 @Param			const char * c_pszFilename
 @Returns		bool
 @Description	
*************************************************************************/
Uint32 ResourceManagerImpl::LoadPNG(const char* c_pszFile, Uint32& uiW, Uint32& uiH)
	{
	CGImageRef textureImage;
	CGContextRef textureContext;
	GLuint hTexture = 0;
	
	NSString *path = [NSString stringWithUTF8String:c_pszFile];
	
	UIImage *uiImage = [UIImage imageWithContentsOfFile:path];
	if(uiImage) 
		{
		textureImage = uiImage.CGImage;
		
		uiW = CGImageGetWidth(textureImage);
		uiH = CGImageGetHeight(textureImage);
		
		GLubyte *textureData;
		if(textureImage) 
			{
			textureData = (GLubyte *) malloc(uiW * uiH * 4);
			textureContext = CGBitmapContextCreate(textureData, uiW, uiH, 8, uiW * 4, CGImageGetColorSpace(textureImage), kCGImageAlphaPremultipliedLast);
			CGContextDrawImage(textureContext, CGRectMake(0.0, 0.0, (float)uiW, (float)uiH), textureImage);
			}

		glGenTextures(1, &hTexture);
		glBindTexture(GL_TEXTURE_2D, hTexture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, uiW, uiH, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureData);

		glBindTexture(GL_TEXTURE_2D, 0);
		free(textureData);
		CGContextRelease(textureContext);
		}

	return hTexture;
	}

/*!***********************************************************************
 @Function		LoadPVR
 @Access		private 
 @Param			const char * c_pszFilename
 @Returns		bool
 @Description	
*************************************************************************/	
Uint32 ResourceManagerImpl::LoadPVR(const char* c_pszFile, Uint32& uiW, Uint32& uiH)
	{
	// Support
	}

/*!***********************************************************************
 @Function		LoadPVR
 @Access		private 
 @Param			const char * c_pszFilename
 @Returns		bool
 @Description	
*************************************************************************/	
void ResourceManagerImpl::GetResourcePath(char* pszBuffer, Uint32 uiBufferLen)
	{
	NSString* readPath = [[NSBundle mainBundle] resourcePath];
	[readPath getCString:pszBuffer maxLength:uiBufferLen encoding:NSUTF8StringEncoding];
	}
