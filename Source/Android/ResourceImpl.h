#ifndef RESOURCEIMPL_H
#define RESOURCEIMPL_H

struct AAssetManager;
struct AAsset;

class FileStreamImpl : public FileStream
{
public:
	AAsset*	         m_Asset;
	
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
	AAssetManager*       m_AssetManager;
	
private:
	Uint32 LoadPNG(FileStream* pFile, Uint32& uiW, Uint32& uiH);
	Uint32 LoadPVR(FileStream* pFile, Uint32& uiW, Uint32& uiH);

protected:
	virtual Uint32 LoadTextureImpl(const char* c_pszFilename, Uint32& uiW, Uint32& uiH);

public:
	ResourceManagerImpl(AAssetManager* mgr);
		
	virtual FileStream* OpenFile(const char* c_pszFilename, FileStream::enumMODE = FileStream::eRead);
	virtual bool LoadFileToMemory(const char* c_pszFilename, char** ppData, Uint32& fileSize);
};

#endif // RESOURCEIMPL_H
