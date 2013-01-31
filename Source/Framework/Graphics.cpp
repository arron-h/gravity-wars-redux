#include "stdafx.h"
#include "Graphics.h"

struct VertexData
	{
	Float32 fV[3];
	Float32 fUV[2];
	Uint8 u8RGBA[4];
	};

GraphicsSystem::GraphicsSystem() :  m_uiDeviceWidth(0), m_uiDeviceHeight(0), m_fVirtualWidth(0), m_fVirtualHeight(0),
									m_bRotated(false), m_eDevRes(enumDEVRES_Unknown)
	{
	}

GraphicsSystem::~GraphicsSystem()
	{
	}

/*!***********************************************************************
 @Function		GetDeviceWidth
 @Access		public 
 @Returns		Uint32
 @Description	
*************************************************************************/
Uint32 GraphicsSystem::GetDeviceWidth()
	{
	return m_uiDeviceWidth;
	}

/*!***********************************************************************
 @Function		GetDeviceHeight
 @Access		public 
 @Returns		Uint32
 @Description	
*************************************************************************/
Uint32 GraphicsSystem::GetDeviceHeight()
	{
	return m_uiDeviceHeight;
	}

/*!***********************************************************************
 @Function		GetDeviceResolution
 @Access		public 
 @Returns		enumDEVRES
 @Description	
*************************************************************************/
enumDEVRES GraphicsSystem::GetDeviceResolution()
	{
	return m_eDevRes;
	}

/*!***********************************************************************
 @Function		GetVirtualWidth
 @Access		public 
 @Returns		Float32
 @Description	
*************************************************************************/
Float32 GraphicsSystem::GetVirtualWidth()
	{
	return m_fVirtualWidth;
	}

/*!***********************************************************************
 @Function		GetVirtualHeight
 @Access		public 
 @Returns		Float32
 @Description	
*************************************************************************/
Float32 GraphicsSystem::GetVirtualHeight()
	{
	return m_fVirtualHeight;
	}

/*!***********************************************************************
 @Function		GetAspectRatio
 @Access		public 
 @Returns		Float32
 @Description	
*************************************************************************/
Float32 GraphicsSystem::GetAspectRatio()
	{
	return (Float32)m_uiDeviceWidth / (Float32)m_uiDeviceHeight;
	}

/*!***********************************************************************
 @Function		IsRotated
 @Access		public 
 @Returns		bool
 @Description	
*************************************************************************/
bool GraphicsSystem::IsRotated()
	{
	return m_bRotated;
	}

/*!***********************************************************************
 @Function		RenderQuadNew
 @Access		public 
 @Param			const TextureRef tex
 @Param			PVRTMat4 mxTrans
 @Param			const RGBA & rgbaCol
 @Returns		void
 @Description	
*************************************************************************/
void GraphicsSystem::RenderQuad(const TextureRef tex, PVRTMat4 mxTrans, const RGBA& rgbaCol)
	{
	Rectanglef fQuad;
	fQuad.m_fX = -(tex->GetWidth() * 0.5f);
	fQuad.m_fW =  (tex->GetWidth() * 0.5f);
	fQuad.m_fY = -(tex->GetHeight() * 0.5f);
	fQuad.m_fH =  (tex->GetHeight() * 0.5f);
	glBindTexture(GL_TEXTURE_2D, tex->GetHandle());
	RenderQuad(fQuad, mxTrans, FLAG_VRT | FLAG_TEX0 | FLAG_RGB, rgbaCol);
	}

/*!***********************************************************************
 @Function		RenderQuadNew
 @Access		public 
 @Param			const Rectanglef & quad
 @Param			PVRTMat4 mxTrans
 @Param			Uint32 eOpts
 @Param			const RGBA & rgbaCol
 @Param			PVRTVec2 * pUVs
 @Returns		void
 @Description	
*************************************************************************/
void GraphicsSystem::RenderQuad(const Rectanglef& quad, PVRTMat4 mxTrans, Uint32 eOpts, const RGBA& rgbaCol, PVRTVec2* pUVs)
	{
	PVRTVec4 vTL = (PVRTVec4(quad.m_fX, quad.m_fH, 0.0f, 1.0f) * mxTrans) + PVRTVec4(mxTrans.f[12], mxTrans.f[13], mxTrans.f[14], 0.0f);
	PVRTVec4 vTR = (PVRTVec4(quad.m_fW, quad.m_fH, 0.0f, 1.0f) * mxTrans) + PVRTVec4(mxTrans.f[12], mxTrans.f[13], mxTrans.f[14], 0.0f);
	PVRTVec4 vBL = (PVRTVec4(quad.m_fX, quad.m_fY, 0.0f, 1.0f) * mxTrans) + PVRTVec4(mxTrans.f[12], mxTrans.f[13], mxTrans.f[14], 0.0f);
	PVRTVec4 vBR = (PVRTVec4(quad.m_fW, quad.m_fY, 0.0f, 1.0f) * mxTrans) + PVRTVec4(mxTrans.f[12], mxTrans.f[13], mxTrans.f[14], 0.0f);

	PVRTVec2 vTTL = (pUVs ? pUVs[0] : PVRTVec2(0.0f, 0.0f));
	PVRTVec2 vTBR = (pUVs ? pUVs[1] : PVRTVec2(1.0f, 1.0f));

	const VertexData pInterleaved[] =
		{
			{
				vTL.x, vTL.y, 0.0f,
				vTTL.x, vTTL.y,
				rgbaCol.RedChannel(), rgbaCol.GreenChannel(), rgbaCol.BlueChannel(), rgbaCol.AlphaChannel(),
			},

			{
				vBL.x, vBL.y, 0.0f,
				vTTL.x, vTBR.y,
				rgbaCol.RedChannel(), rgbaCol.GreenChannel(), rgbaCol.BlueChannel(), rgbaCol.AlphaChannel(),
			},

			{
				vTR.x, vTR.y, 0.0f,
				vTBR.x, vTTL.y,
				rgbaCol.RedChannel(), rgbaCol.GreenChannel(), rgbaCol.BlueChannel(), rgbaCol.AlphaChannel(),
			},

			{
				vBR.x, vBR.y, 0.0f,
				vTBR.x, vTBR.y,
				rgbaCol.RedChannel(), rgbaCol.GreenChannel(), rgbaCol.BlueChannel(), rgbaCol.AlphaChannel(),
			},
		};

	if(eOpts & FLAG_VRT)  glEnableVertexAttribArray(enumATTRIBUTE_POSITION);
	if(eOpts & FLAG_TEX0) glEnableVertexAttribArray(enumATTRIBUTE_TEXCOORD0);
	if(eOpts & FLAG_RGB)  glEnableVertexAttribArray(enumATTRIBUTE_RGBA);

	if(eOpts & FLAG_VRT)  glVertexAttribPointer(enumATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), pInterleaved[0].fV);
	if(eOpts & FLAG_TEX0) glVertexAttribPointer(enumATTRIBUTE_TEXCOORD0, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), pInterleaved[0].fUV);
	if(eOpts & FLAG_RGB)  glVertexAttribPointer(enumATTRIBUTE_RGBA, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(VertexData), pInterleaved[0].u8RGBA);

	glDisable(GL_DEPTH_TEST);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glEnable(GL_DEPTH_TEST);

	// -- Disable
	if(eOpts & FLAG_VRT)  glDisableVertexAttribArray(enumATTRIBUTE_POSITION);
	if(eOpts & FLAG_TEX0) glDisableVertexAttribArray(enumATTRIBUTE_TEXCOORD0);
	if(eOpts & FLAG_RGB)  glDisableVertexAttribArray(enumATTRIBUTE_RGBA);
	}

/*!***********************************************************************
 @Function		RenderMesh
 @Access		public 
 @Param			SPODMesh * pMesh
 @Param			Uint32 eOpts
 @Returns		void
 @Description	
*************************************************************************/
void GraphicsSystem::RenderMesh(SPODMesh* pMesh, Uint32 eOpts, Uint32 uiVBO, Uint32 uiIdxVBO)
	{
	if(eOpts & FLAG_VRT)  glEnableVertexAttribArray(enumATTRIBUTE_POSITION);
	if(eOpts & FLAG_TEX0) glEnableVertexAttribArray(enumATTRIBUTE_TEXCOORD0);
	if(eOpts & FLAG_RGB)  glEnableVertexAttribArray(enumATTRIBUTE_RGBA);
	if(eOpts & FLAG_NRM)  glEnableVertexAttribArray(enumATTRIBUTE_NORMAL);

	// Bind VBO
	glBindBuffer(GL_ARRAY_BUFFER, uiVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, uiIdxVBO);

	// VBO offsets
	if(eOpts & FLAG_VRT)  glVertexAttribPointer(enumATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, pMesh->sVertex.nStride, pMesh->sVertex.pData);
	if(eOpts & FLAG_TEX0) glVertexAttribPointer(enumATTRIBUTE_TEXCOORD0, 2, GL_FLOAT, GL_FALSE, pMesh->psUVW[0].nStride, pMesh->psUVW[0].pData);
	if(eOpts & FLAG_RGB)  glVertexAttribPointer(enumATTRIBUTE_RGBA, 4, GL_FLOAT, GL_FALSE, pMesh->sVtxColours.nStride, pMesh->sVtxColours.pData);
	if(eOpts & FLAG_NRM)  glVertexAttribPointer(enumATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, pMesh->sNormals.nStride, pMesh->sNormals.pData);

	// Indexed Triangle list
	glDrawElements(GL_TRIANGLES, pMesh->nNumFaces*3, GL_UNSIGNED_SHORT, 0);

	if(eOpts & FLAG_VRT)  glDisableVertexAttribArray(enumATTRIBUTE_POSITION);
	if(eOpts & FLAG_TEX0) glDisableVertexAttribArray(enumATTRIBUTE_TEXCOORD0);
	if(eOpts & FLAG_RGB)  glDisableVertexAttribArray(enumATTRIBUTE_RGBA);
	if(eOpts & FLAG_NRM)  glDisableVertexAttribArray(enumATTRIBUTE_NORMAL);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

/*!***********************************************************************
 @Function		RenderPrimitiveList
 @Access		public 
 @Param			const PrimTriangle * pPrims
 @Returns		void
 @Description	
*************************************************************************/
void GraphicsSystem::RenderPrimitiveList(const PrimTriangle* pPrims, Uint32 uiCount, Uint32 uiFlags)
	{
	if(uiFlags & FLAG_VRT)  glEnableVertexAttribArray(enumATTRIBUTE_POSITION);
	if(uiFlags & FLAG_TEX0) glEnableVertexAttribArray(enumATTRIBUTE_TEXCOORD0);
	if(uiFlags & FLAG_RGB) glEnableVertexAttribArray(enumATTRIBUTE_RGBA);

	/*		PrimVertex:
	PVRTVec3	v;
	PVRTVec3	n;
	PVRTVec2	uv;
	Uint8		rgba[4];
	*/

	if(uiFlags & FLAG_VRT)  glVertexAttribPointer(enumATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(PrimVertex), &pPrims[0].vert[0].v);
	if(uiFlags & FLAG_TEX0) glVertexAttribPointer(enumATTRIBUTE_TEXCOORD0, 2, GL_FLOAT, GL_FALSE, sizeof(PrimVertex), &pPrims[0].vert[0].uv);
	if(uiFlags & FLAG_RGB)glVertexAttribPointer(enumATTRIBUTE_RGBA, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(PrimVertex), pPrims[0].vert[0].rgba);

	glDisable(GL_DEPTH_TEST);
	glDrawArrays(GL_TRIANGLES, 0, uiCount*3);
	glEnable(GL_DEPTH_TEST);

	// -- Disable
	if(uiFlags & FLAG_VRT)  glDisableVertexAttribArray(enumATTRIBUTE_POSITION);
	if(uiFlags & FLAG_TEX0) glDisableVertexAttribArray(enumATTRIBUTE_TEXCOORD0);
	if(uiFlags & FLAG_RGB)  glDisableVertexAttribArray(enumATTRIBUTE_RGBA);
	}

/*!***********************************************************************
 @Function		RenderPrimitiveList
 @Access		public 
 @Param			const PrimQuad * pPrims
 @Returns		void
 @Description	
*************************************************************************/
void GraphicsSystem::RenderPrimitiveList(const PrimQuad* pPrims, Uint32 uiCount, Uint32 uiFlags)
	{

	}