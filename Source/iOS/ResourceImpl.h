#ifndef TEXTUREIMPL_H
#define TEXTUREIMPL_H

class ResourceManagerImpl : public ResourceManager
	{
	private:
		Uint32 LoadPNG(FILE* pFile, Uint32& uiW, Uint32& uiH);
		Uint32 LoadPVR(const char* c_pszFile, Uint32& uiW, Uint32& uiH);
		
		TXChar		m_szResPath[1024];

	protected:
		virtual Uint32 LoadTextureImpl(const char* c_pszFilename, Uint32& uiW, Uint32& uiH);

	public:
		ResourceManagerImpl();
		
		virtual void GetResourcePath(char* pszBuffer, Uint32 uiBufferLen, enumRESTYPE eType);
	};

#endif
