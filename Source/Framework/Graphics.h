#ifndef GRAPHICS_H
#define GRAPHICS_H

const Uint32 FLAG_VRT	= (1 << 1);
const Uint32 FLAG_TEX0	= (1 << 2);
const Uint32 FLAG_TEX1	= (1 << 3);
const Uint32 FLAG_RGB	= (1 << 4);
const Uint32 FLAG_NRM	= (1 << 5);
const Uint32 FLAG_TAN	= (1 << 6);
const Uint32 FLAG_BIN	= (1 << 7);

enum enumATTRIBUTE
	{
	enumATTRIBUTE_POSITION,		// 0
	enumATTRIBUTE_TEXCOORD0,	// 1
	enumATTRIBUTE_RGBA,			// 2
	enumATTRIBUTE_NORMAL,		// 3
	enumATTRIBUTE_TANGENT,		// 4
	};

enum enumDEVRES
	{
	enumDEVRES_HVGA,			// iPhone 3GS/iPod 3
	enumDEVRES_Retina,			// 
	enumDEVRES_XGA,				// iPad

	enumDEVRES_Unknown,
	};

class GraphicsSystem
	{
	protected:
		Uint32		m_uiDeviceWidth;
		Uint32		m_uiDeviceHeight;
		Float32		m_fVirtualWidth;
		Float32		m_fVirtualHeight;
		enumDEVRES	m_eDevRes;
		bool		m_bRotated;

	public:
		GraphicsSystem();
		virtual ~GraphicsSystem();

		virtual bool Initialise(Uint32 uiWidth, Uint32 uiHeight, Float32 fVWidth, Float32 fVHeight, bool bRotated, const char* c_pszWindowTitle = NULL) = 0;
		virtual void Destroy() = 0;
		virtual void SwapBuffers() = 0;
		
		Uint32 GetDeviceWidth();
		Uint32 GetDeviceHeight();
		enumDEVRES GetDeviceResolution();
		Float32 GetVirtualWidth();
		Float32 GetVirtualHeight();
		Float32 GetAspectRatio();
		bool IsRotated();

		void RenderQuad(const TextureRef tex, PVRTMat4 mxTrans, const RGBA& rgbaCol = 0xFFFFFFFF);
		void RenderQuad(const Rectanglef& quad, PVRTMat4 mxTrans, Uint32 eOpts, const RGBA& rgbaCol = 0xFFFFFFFF, PVRTVec2* pUVs = NULL);

		void RenderMesh(SPODMesh* pMesh, Uint32 eOpts, Uint32 uiVBO = 0, Uint32 uiIdxVBO = 0);

		void RenderPrimitiveList(const PrimTriangle* pPrims, Uint32 uiCount, Uint32 uiFlags);
		void RenderPrimitiveList(const PrimQuad* pPrims, Uint32 uiCount, Uint32 uiFlags);
	};


#endif
