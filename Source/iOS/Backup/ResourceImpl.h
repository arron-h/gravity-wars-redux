#ifndef TEXTUREIMPL_H
#define TEXTUREIMPL_H

class ResourceManagerImpl : public ResourceManager
	{
	private:
		Uint32 LoadPNG(FILE* pFile, Uint32& uiW, Uint32& uiH);
		Uint32 LoadTGA(FILE* pFile, Uint32& uiW, Uint32& uiH);
		Uint32 LoadPVR(const char* c_pszFile, Uint32& uiW, Uint32& uiH);

	protected:
		virtual Uint32 LoadTextureImpl(const char* c_pszFilename, Uint32& uiW, Uint32& uiH);
		
	public:
    virtual void GetResourcePath(char* pszBuffer, Uint32 uiBufferLen);
	};

#endif
