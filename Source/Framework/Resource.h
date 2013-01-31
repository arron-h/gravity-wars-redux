#ifndef TEXTURE_H
#define TEXTURE_H

#include <map>
#include <string>

class Texture
	{
	friend class ResourceManager;
	private:
		Uint32		m_uiWidth;
		Uint32		m_uiHeight;
		Uint32		m_uiHandle;
#ifdef _DEBUG
		char		m_szName[256];
#endif

	public:
		Texture();

		Uint32 GetWidth() const;
		Uint32 GetHeight() const;
		Uint32 GetHandle() const;
	};

class Model
	{
	friend class ResourceManager;
	private:
		Uint32				m_puiVBO;
		Uint32				m_puiIndexVBO;

		CPVRTModelPOD		m_Model;

		void BuildVBO();

	public:
		Model();
		~Model();

		void RenderSegment(Uint32 uiIndex, Uint32 uiOpts);
		Uint32 GetNumSegments() const;
	};

class Shader
	{
	friend class ResourceManager;
	private:
		Uint32					m_uiVertHandle;		// Vertex Shader handle
		Uint32					m_uiFragHandle;		// Fragment Shader handle

		Uint32					m_uiHandle;			// Program handle
#ifdef _DEBUG
		char					m_szName[256];
#endif
		HashMap<Uint32>			m_Uniforms;
		static const Shader*	m_pActive;
	public:
		Shader();

		Uint32 GetHandle() const { return m_uiHandle; }
		Uint32 GetUniform(Uint32 uiHash) const { return m_Uniforms.Get(uiHash); }
		bool BindUniform(const char* c_pszName);

		static void Use(const Shader* shader);
	};

class AudioStream
	{
	public:
		enum EType
		{
			eType_Music,
			eType_Sfx,
		};

		EType		m_eType;
		Uint32		m_uiHandle;
	};

typedef const Texture		*TextureRef;
typedef AudioStream			*AudioRef;
typedef Shader				*ShaderRef;
typedef Model				*ModelRef;
typedef AHFont				*FontRef;

enum enumRESTYPE
	{
	enumRESTYPE_Shader,
	enumRESTYPE_Texture,
	enumRESTYPE_Font,
	enumRESTYPE_Model,
	enumRESTYPE_SFX,
	};

class ResourceManager
	{
	protected:
		typedef std::map<std::string, Texture>	TextureList;
		typedef std::map<std::string, Model*>   ModelList;
		typedef std::map<std::string, AHFont*>  FontList;
		typedef std::map<std::string, Shader*>  ShaderList;
		typedef std::map<std::string, AudioRef> AudioList;
		TextureList		m_TexList;
		ModelList		m_ModelList;
		FontList		m_FontList;
		ShaderList		m_ShaderList;
		AudioList		m_AudioList;

	protected:
		// Implementation method
		virtual Uint32 LoadTextureImpl(const char* c_pszFilename, Uint32& uiW, Uint32& uiH) = 0;
		void FinaliseModel(Model* pModel, const char* c_pszName);

	public:
		virtual ~ResourceManager();
		
		// Implementation dependent
		virtual void GetResourcePath(char* pszBuffer, Uint32 uiBufferLen, enumRESTYPE eType) = 0;

		TextureRef GetTexture(const char* c_pszName);
		ModelRef GetModel(const char* c_pszName);
		FontRef GetFont(const char* c_pszName);
		ShaderRef GetShader(const char* c_pszName);
		AudioRef GetAudioStream(const char* c_pszName);

		bool LoadModelFromFile(const char* c_pszModel);
		bool LoadModelFromMemory(const char* c_pszModel, const char* c_pszData, Uint32 uiDataSize);
		bool LoadTexture(const char* c_pszName);
		bool LoadFont(const char* c_pszName);
		bool LoadAudioStream(const char* c_pszName);
		
		bool LoadShaderSource(const char* c_pszFilename, Uint32 uiType, Uint32& uiHandleOUT);
		ShaderRef CreateShaderProgram(const char* c_pszName, Uint32 uiVert, Uint32 uiFrag, const char** c_ppszAttribs, Uint32 uiNumAtribs);
	};

#endif
