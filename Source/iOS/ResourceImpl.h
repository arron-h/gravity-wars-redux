#ifndef TEXTUREIMPL_H
#define TEXTUREIMPL_H

class FileStreamImpl : public FileStream
{
public:
	FILE*       m_pFileHandle;
	
	virtual ~FileStreamImpl();
	virtual size_t Read(void* data, size_t size, size_t count);
	virtual size_t Write(const void* data, size_t size, size_t count);
	virtual void Close();
	virtual void* NativeFD();
	
	virtual size_t GetFileSize();
};

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
		
		virtual FileStream* OpenFile(const char* c_pszFilename, FileStream::enumMODE = FileStream::eRead);
		virtual bool LoadFileToMemory(const char* c_pszFilename, char** ppData, Uint32& fileSize);
	};

#endif
