#include "stdafx.h"
#include <vector>
#include <string>

const Shader* Shader::m_pActive = 0;

Texture::Texture() : m_uiWidth(0), m_uiHeight(0), m_uiHandle(0)
	{
#ifdef _DEBUG
	m_szName[0] = 0;
#endif
	}

Model::Model() : m_puiVBO(0), m_puiIndexVBO(0)
	{
	}

Model::~Model()
	{
	m_Model.Destroy();
	}

Shader::Shader() : m_uiHandle(0), m_uiFragHandle(0), m_uiVertHandle(0)
	{
#ifdef _DEBUG
	m_szName[0] = 0;
#endif
	}

ResourceManager::~ResourceManager()
	{
	TextureList::iterator texit;
	TextureRef tex;
	for(texit = m_TexList.begin(); texit != m_TexList.end(); ++texit)
		{
		tex = &texit->second;
		glDeleteTextures(1, &tex->m_uiHandle);
		}

	ModelList::iterator mdlit;
	for(mdlit = m_ModelList.begin(); mdlit != m_ModelList.end(); ++mdlit)
		{
		delete mdlit->second;
		}

	FontList::iterator fntit;
	for(fntit = m_FontList.begin(); fntit != m_FontList.end(); ++fntit)
		{
		delete fntit->second;
		}

	ShaderList::iterator shdrit;
	ShaderRef shader;
	for(shdrit = m_ShaderList.begin(); shdrit != m_ShaderList.end(); ++shdrit)
		{
		shader = shdrit->second;
		glDeleteProgram(shader->m_uiHandle);
		glDeleteShader(shader->m_uiVertHandle);
		glDeleteShader(shader->m_uiFragHandle);
		delete shader;
		}

	AudioList::iterator audioit;
	for(audioit = m_AudioList.begin(); audioit != m_AudioList.end(); ++audioit)
		{
		delete fntit->second;
		}

	m_TexList.clear();
	m_ModelList.clear();
	m_ShaderList.clear();
	m_FontList.clear();
	m_AudioList.clear();
	}

/*!***********************************************************************
 @Function		GetWidth
 @Access		public 
 @Returns		Uint32
 @Description	
*************************************************************************/
Uint32 Texture::GetWidth() const
	{
	return m_uiWidth;
	}

/*!***********************************************************************
 @Function		GetHeight
 @Access		public 
 @Returns		Uint32
 @Description	
*************************************************************************/
Uint32 Texture::GetHeight() const
	{
	return m_uiHeight;
	}

/*!***********************************************************************
 @Function		GetHandle
 @Access		public 
 @Returns		Uint32
 @Description	
*************************************************************************/
Uint32 Texture::GetHandle() const
	{
	return m_uiHandle;
	}

/*!***********************************************************************
 @Function		BuildVBO
 @Access		private 
 @Returns		void
 @Description	
*************************************************************************/
void Model::BuildVBO()
	{
	// Build VBO
	glGenBuffers(1, &m_puiVBO);
	glGenBuffers(1, &m_puiIndexVBO);
	SPODMesh& Mesh = m_Model.pMesh[0];
	unsigned int uiSize = Mesh.nNumVertex * Mesh.sVertex.nStride;
	// Vertex data
	glBindBuffer(GL_ARRAY_BUFFER, m_puiVBO);
	glBufferData(GL_ARRAY_BUFFER, uiSize, Mesh.pInterleaved, GL_STATIC_DRAW);

	// Face data
	uiSize = PVRTModelPODCountIndices(Mesh) * sizeof(GLshort);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_puiIndexVBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, uiSize, Mesh.sFaces.pData, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

/*!***********************************************************************
 @Function		BindUniform
 @Access		public 
 @Param			const char * c_pszName
 @Returns		void
 @Description	
*************************************************************************/
bool Shader::BindUniform(const char* c_pszName)
	{
	Sint32 nResult = glGetUniformLocation(m_uiHandle, c_pszName);
	if(nResult == -1)
		return false;

	Uint32 uiHash = HashMap_Hash(c_pszName);
	HashPair<Uint32> hashPair;
	hashPair.uiHash = uiHash;
	hashPair.Val    = (Uint32)nResult;
	m_Uniforms.Add(hashPair);
	return true;
	}

/*!***********************************************************************
 @Function		Use
 @Access		public static 
 @Param			ShaderRef shader
 @Returns		void
 @Description	
*************************************************************************/
void Shader::Use(const Shader* shader)
	{
	if(m_pActive == shader)
		return;

	Uint32 uiHandle = 0;
	if(shader)
		{
		uiHandle = shader->GetHandle();
		m_pActive = shader;
		}
	else
		{
		m_pActive = 0;
		}
		
	glUseProgram(uiHandle);
	}

/*!***********************************************************************
 @Function		RenderSegment
 @Access		public 
 @Param			Uint32 uiIndex
 @Returns		void
 @Description	
*************************************************************************/
void Model::RenderSegment(Uint32 uiIndex, Uint32 uiOpts)
	{
	if(m_puiVBO == 0)
		BuildVBO();

	// --- Draw the model
	SPODMesh* pMesh = &m_Model.pMesh[uiIndex];
	GFX->RenderMesh(pMesh, uiOpts, m_puiVBO, m_puiIndexVBO);
	}

/*!***********************************************************************
 @Function		GetNumSegments
 @Access		public 
 @Returns		Uint32
 @Description	
*************************************************************************/
Uint32 Model::GetNumSegments() const
	{
	return m_Model.nNumMesh;
	}

/*!***********************************************************************
 @Function		GetTexture
 @Access		public 
 @Param			const char * c_pszName
 @Returns		TextureRef
 @Description	
*************************************************************************/
TextureRef ResourceManager::GetTexture(const char* c_pszName)
	{
	// Check if the texture is in the list
	TextureList::const_iterator it = m_TexList.find(c_pszName);
	
	TextureRef texReturn;

	if(it == m_TexList.end())
		{
		ASSERT(!"ERROR: Texture not found!");
		return NULL;
		}
	else
		{
		texReturn = (TextureRef)&it->second;
		}

	return texReturn;
	}

/*!***********************************************************************
 @Function		GetModel
 @Access		public 
 @Param			const char * c_pszName
 @Returns		ModelRef
 @Description	
*************************************************************************/
ModelRef ResourceManager::GetModel(const char* c_pszName)
	{
	// Check if the model is in the list
	ModelList::const_iterator it = m_ModelList.find(c_pszName);

	ModelRef mdlReturn;

	if(it == m_ModelList.end())
		{
		ASSERT(!"ERROR: Model not found!");
		return NULL;
		}
	else
		{
		mdlReturn = (ModelRef)it->second;
		}

	return mdlReturn;
	}

/*!***********************************************************************
 @Function		GetFont
 @Access		public 
 @Param			const char * c_pszName
 @Returns		FontRef
 @Description	
*************************************************************************/
FontRef ResourceManager::GetFont(const char* c_pszName)
	{
	// Check if the model is in the list
	FontList::const_iterator it = m_FontList.find(c_pszName);

	FontRef fntReturn;

	if(it == m_FontList.end())
		{
		ASSERT(!"ERROR: Font not found!");
		return NULL;
		}
	else
		{
		fntReturn = (FontRef)it->second;
		}

	return fntReturn;
	}

/*!***********************************************************************
 @Function		GetShader
 @Access		public 
 @Param			const char * c_pszName
 @Returns		ShaderRef
 @Description	
*************************************************************************/
ShaderRef ResourceManager::GetShader(const char* c_pszName)
	{
	// Check if the model is in the list
	ShaderList::const_iterator it = m_ShaderList.find(c_pszName);

	ShaderRef shaderReturn;

	if(it == m_ShaderList.end())
		{
		ASSERT(!"ERROR: Shader not found!");
		return NULL;
		}
	else
		{
		shaderReturn = (ShaderRef)it->second;
		}

	return shaderReturn;
	}

/*!***********************************************************************
 @Function		GetAudioStream
 @Access		public 
 @Param			const char * c_pszName
 @Returns		AudioRef
 @Description	
*************************************************************************/
AudioRef ResourceManager::GetAudioStream(const char* c_pszName)
	{
	// Check if the model is in the list
	AudioList::const_iterator it = m_AudioList.find(c_pszName);

	if(it == m_AudioList.end())
		{
		ASSERT(!"ERROR: Audio Stream not found!");
		return NULL;
		}
	else
		{
		return (AudioRef)it->second;
		}
	}

/*!***********************************************************************
 @Function		FinaliseModel
 @Access		protected 
 @Param			Model * pModel
 @Returns		void
 @Description	Builds VBO and adds to model list
*************************************************************************/
void ResourceManager::FinaliseModel(Model* pModel, const char* c_pszName)
	{
	// Add to the list
	m_ModelList.insert(ModelList::value_type(c_pszName, pModel));
	}

/*!***********************************************************************
 @Function		LoadModelFromFile
 @Access		public 
 @Param			const char * c_pszModel
 @Returns		bool
 @Description	
*************************************************************************/
bool ResourceManager::LoadModelFromFile(const char* c_pszModel)
	{
	Model* pMdl = new Model;
	if(pMdl->m_Model.ReadFromFile(c_pszModel) != PVR_SUCCESS)
		{
		delete pMdl;
		return false;
		}

	FinaliseModel(pMdl, c_pszModel);
	return true;
	}

/*!***********************************************************************
 @Function		LoadModelFromMemory
 @Access		public 
 @Param			const char * c_pszModel
 @Param			const char * c_pszData
 @Param			Uint32 uiDataSize
 @Returns		bool
 @Description	
*************************************************************************/
bool ResourceManager::LoadModelFromMemory(const char* c_pszModel, const char* c_pszData, Uint32 uiDataSize)
	{
	Model* pMdl = new Model;
	if(pMdl->m_Model.ReadFromMemory(c_pszData, uiDataSize) != PVR_SUCCESS)
		{
		delete pMdl;
		return false;
		}

	FinaliseModel(pMdl, c_pszModel);
	return true;
	}

/*!***********************************************************************
 @Function		LoadTexture
 @Access		public 
 @Param			const char * c_pszName
 @Returns		bool
 @Description	
*************************************************************************/
bool ResourceManager::LoadTexture(const char* c_pszName)
	{
	Uint32 uiW, uiH, uiHandle;
	uiHandle = LoadTextureImpl(c_pszName, uiW, uiH);
	if(!uiHandle)
		{
		DebugLog("ERROR: Failed to load: %s", c_pszName);
		return false;
		}

	Texture tex;
	tex.m_uiWidth  = uiW;
	tex.m_uiHeight = uiH;
	tex.m_uiHandle = uiHandle;
#ifdef _DEBUG
	strcpy(tex.m_szName, c_pszName);
#endif
	// Add to the list
	m_TexList.insert(TextureList::value_type(c_pszName, tex));
	return true;
	}

/*!***********************************************************************
 @Function		LoadFont
 @Access		public 
 @Param			const char * c_pszName
 @Returns		bool
 @Description	
*************************************************************************/
bool ResourceManager::LoadFont(const char* c_pszName)
	{
	char szPath[1024];
	GetResourcePath(szPath, 1024, enumRESTYPE_Font);
	strcat(szPath, c_pszName);
	
	// Try load texture
	bool bResult = LoadTexture(c_pszName);
	if(!bResult)
		return false;

	char szFilename[1024];
	sprintf(szFilename, "%s.fnt", szPath);

	TextureRef tex = GetTexture(c_pszName);

	AHFont* pFnt = new AHFont;
	pFnt->Load(tex->GetHandle(), szFilename, tex->GetWidth(), tex->GetHeight());

	// Add to the list
	m_FontList.insert(FontList::value_type(c_pszName, pFnt));
	return true;
	}

/*!***********************************************************************
 @Function		LoadAudioStream
 @Access		public 
 @Param			const char * c_pszName
 @Returns		bool
 @Description	
*************************************************************************/
bool ResourceManager::LoadAudioStream(const char* c_pszName)
	{
	// Try load texture
	AudioRef newref = GetApp()->GetAudioEngine()->Load(c_pszName);
	if(newref == NULL)
		return false;

	// Add to the list
	m_AudioList.insert(AudioList::value_type(c_pszName, newref));
	return true;
	}

/*!***********************************************************************
 @Function		LoadShader
 @Access		public 
 @Param			const char * c_pszName
 @Returns		bool
 @Description	
*************************************************************************/
bool ResourceManager::LoadShaderSource(const char* c_pszFilename, Uint32 uiType, Uint32& uiHandleOUT)
	{
	char szFilename[256];
	char szResPath[256];
	GetResourcePath(szResPath, 256, enumRESTYPE_Shader);

	sprintf(szFilename, "%s/%s", szResPath, c_pszFilename);

	FILE* pFile = fopen(szFilename, "rb");
	if(!pFile)
		return false;

	Uint32 uiDataSize;
	fseek(pFile, SEEK_SET, SEEK_END);
	uiDataSize = ftell(pFile);
	rewind(pFile);

	if(uiDataSize == 0)
		{
		fclose(pFile);
		return false;
		}

	// Load the data file to memory.
	char* pszShaderSource = (char*)malloc(uiDataSize + 1);		// + 1 for NULL terminate.
	fread(pszShaderSource, uiDataSize, 1, pFile);
	fclose(pFile);

	pszShaderSource[uiDataSize] = 0;

	// Create and compile the shader object
	uiHandleOUT = glCreateShader((GLenum)uiType);
	glShaderSource(uiHandleOUT, 1, (const char**)&pszShaderSource, NULL);

	DebugLog("------------ COMPILING: %s", c_pszFilename);
	glCompileShader(uiHandleOUT);

	// Test if compilation succeeded
	GLint ShaderCompiled;
	glGetShaderiv(uiHandleOUT, GL_COMPILE_STATUS, &ShaderCompiled);
	if (!ShaderCompiled)
		{
		int i32InfoLogLength, i32CharsWritten;
		glGetShaderiv(uiHandleOUT, GL_INFO_LOG_LENGTH, &i32InfoLogLength);
		char* pszInfoLog = new char[i32InfoLogLength];
		glGetShaderInfoLog(uiHandleOUT, i32InfoLogLength, &i32CharsWritten, pszInfoLog);
		
		DebugLog("Failed to compile shader: %s", pszInfoLog);
		
		delete [] pszInfoLog;
		glDeleteShader(uiHandleOUT);
		return false;
		}

	free(pszShaderSource);
	return true;
	}


/*!***********************************************************************
 @Function		CreateShaderProgram
 @Access		public 
 @Param			const char * c_pszName
 @Param			Uint32 uiVert
 @Param			Uint32 uiFrag
 @Param			const char * * c_ppszAttribs
 @Param			Uint32 uiNumAtribs
 @Returns		bool
 @Description	
*************************************************************************/
ShaderRef ResourceManager::CreateShaderProgram(const char* c_pszName, Uint32 uiVert, Uint32 uiFrag, const char** c_ppszAttribs, Uint32 uiNumAtribs)
	{
	Uint32 uiProg = glCreateProgram();

	glAttachShader(uiProg, uiFrag);
	glAttachShader(uiProg, uiVert);

	for (Uint32 i = 0; i < uiNumAtribs; ++i)
		{
		glBindAttribLocation(uiProg, i, c_ppszAttribs[i]);
		}

	// Link the program object
	glLinkProgram(uiProg);

	GLint Linked;
	glGetProgramiv(uiProg, GL_LINK_STATUS, &Linked);
	if (!Linked)
		{
		int i32InfoLogLength, i32CharsWritten;
		glGetProgramiv(uiProg, GL_INFO_LOG_LENGTH, &i32InfoLogLength);
		char* pszInfoLog = new char[i32InfoLogLength];
		glGetProgramInfoLog(uiProg, i32InfoLogLength, &i32CharsWritten, pszInfoLog);

		DebugLog("Failed to link: %s - %s", c_pszName, pszInfoLog);

		delete [] pszInfoLog;
		return NULL;
		}

	Shader* newShader = new Shader;
	newShader->m_uiHandle     = uiProg;
	newShader->m_uiFragHandle = uiFrag;
	newShader->m_uiVertHandle = uiVert;

	// Add to the list
	std::pair<ShaderList::iterator, bool> shdrPair = m_ShaderList.insert(ShaderList::value_type(c_pszName, newShader));
	ShaderRef shaderRef = shdrPair.first->second;

	// Use it
	Shader::Use(shaderRef);

	return shaderRef;
	}
