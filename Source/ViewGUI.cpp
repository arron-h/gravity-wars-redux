#include "stdafx.h"
#include "ViewGUI.h"

extern const Uint32 c_uiMVP;
Float32 fGalRad			= 500.0f;		// We'll alter this depending on device resolution.
Float32 fGalTexRad		= 600.0f;

const Float32 c_StarSizes[3] =		// 3 supported resolutions
	{
	1.0f,	// enumDEVRES_HVGA
	2.0f,	// enumDEVRES_Retina
	3.0f,	// enumDEVRES_XVGA
	};

const Uint32  c_uiNumTris  = 5120;
const Float32 c_fOffYScale = 0.005f;
const Float32 c_fOffXScale = 0.005f;
const Float32 c_fOffStageSpeed = 0.5f;

/*!***********************************************************************
 @Function		ViewGUI
 @Access		public 
 @Returns		
 @Description	
*************************************************************************/
ViewGUI::ViewGUI() : m_fGalTexRot(0.0f), m_eGUIState(enumGUISTATE_On)
	{	
	m_mxCam		= PVRTMat4::Identity();
	m_mxCamGUI  = PVRTMat4::Identity();
	m_mxCamScale = PVRTMat4::Identity();

	GraphicsSystem* pGfx = GetApp()->GetGraphicsSystem();
	// We'll base our screen coords on Retina iPhone.
	Float32 fW = GetScreenDimensions(true).x; 
	Float32 fH = GetScreenDimensions(true).y;
	
	Float32 fDeviceScale = (pGfx->GetDeviceResolution() == enumDEVRES_HVGA ? 0.5f : 1.0f);
	fGalRad *= fDeviceScale;
	fGalTexRad *= fDeviceScale;
	
	// To stop the stars going sub-pixel, we'll use the proper display width and height for the starfield coord system.
	Float32 fDispW = pGfx->GetDeviceWidth();
	Float32 fDispH = pGfx->GetDeviceHeight();
	if(pGfx->IsRotated())
		{
		fDispW = pGfx->GetDeviceHeight();
		fDispH = pGfx->GetDeviceWidth();
		}
	else
		{	
		fDispW = pGfx->GetDeviceWidth();
		fDispH = pGfx->GetDeviceHeight();
		}
	
	m_cam.SetProjection(PVRTMat4::Ortho(-fDispW*0.5f, fDispH*0.5f, fDispW*0.5f, -fDispH*0.5f, -1.0f, 1.0f, PVRTMat4::OGL, pGfx->IsRotated()));
	SetCamera(&m_cam);		// Use the galaxy cam as default
	
	// Slightly different camera for font rendering
	m_GUICam.SetProjection(PVRTMat4::Ortho(0.0f, fH, fW, 0.0f, -1.0f, 1.0f, PVRTMat4::OGL, pGfx->IsRotated()));

	
	// Set up star background
	m_fDists = new Float32[c_uiNumTris];
	m_vPositions = new PVRTVec3[c_uiNumTris];
	m_Tris.Initialise(c_uiNumTris);

	Float32 fX, fY;

	// Initialise all of the tris to a 'ring' weighted to the centre
	for(Uint32 i = 0; i < c_uiNumTris; ++i)
		{
		Float32 fRndRad  = Randomf() * PVRT_PI*2;
		Float32 fRndDist = pow(Randomf(), 4) * fGalRad;

		fX = cos(fRndRad) * fRndDist;
		fY = sin(fRndRad) * fRndDist;
		
		m_vPositions[i] = PVRTVec3(fX, fY, 0.0f);

		m_Tris[i].vert[0].v = PVRTVec3(FloatRound(fX), FloatRound(fY), 0.0f);
		m_Tris[i].vert[1].v = PVRTVec3(fX-2.0f, fY-2.0f, 0.0f);
		m_Tris[i].vert[2].v = PVRTVec3(fX+2.0f, fY-2.0f, 0.0f);
		memset(m_Tris[i].vert[0].rgba, 0x69, 3); m_Tris[i].vert[0].rgba[3] = 255;
		memset(m_Tris[i].vert[1].rgba, 0x69, 3); m_Tris[i].vert[1].rgba[3] = 255;
		memset(m_Tris[i].vert[2].rgba, 0x69, 3); m_Tris[i].vert[2].rgba[3] = 255;

		m_fDists[i] = fRndDist;
		}

	m_FontMenu  = RESMAN->GetFont("courbd_36");
	m_FontTitle = RESMAN->GetFont("courbd_64");
	m_FontSmall = RESMAN->GetFont("courbd_18");

	m_GalaxyBGTex = RESMAN->GetTexture("menu-galaxy-bg");
	m_VCShader    = RESMAN->GetShader("MVP_V_C");
	m_GUIShader   = RESMAN->GetShader("MVP_V_T_C");
	}

/*!***********************************************************************
 @Function		~ViewGUI
 @Access		public 
 @Returns		
 @Description	
*************************************************************************/
ViewGUI::~ViewGUI()
	{
	delete [] m_fDists;
	delete [] m_vPositions;
	}

/*!***********************************************************************
 @Function		Update
 @Access		public 
 @Param			double fDT
 @Returns		void
 @Description	
*************************************************************************/
void ViewGUI::Update(double dt)
	{
	// Update star field
	PVRTVec3 vNewPos;
	Float32 fAngle;
	PVRTMat4 mxRot;
	const Float32 c_fSpeed = 7.0f;
	const Float32 fStarSize = c_StarSizes[GFX->GetDeviceResolution()];
	const PVRTVec3 vOffsets[] = {PVRTVec3(-fStarSize, -fStarSize, 0.0f), PVRTVec3(fStarSize, -fStarSize, 0.0f)};
	for(Uint32 i = 0; i < m_Tris.Count(); ++i)
		{
		fAngle = 1.0f / m_fDists[i] * (Float32)dt * c_fSpeed;
		PVRTMatrixRotationZ(mxRot, fAngle);

		PVRTVec3& vCurrPos = m_vPositions[i];
		PVRTMatrixVectorMultiply(vNewPos, vCurrPos, mxRot);
		vCurrPos = vNewPos;
		
		vNewPos.x = FloatRound(vNewPos.x);
		vNewPos.y = FloatRound(vNewPos.y);

		m_Tris[i].vert[0].v = vNewPos;
		m_Tris[i].vert[1].v = vNewPos + vOffsets[0];
		m_Tris[i].vert[2].v = vNewPos + vOffsets[1];
		}

	m_fGalTexRot += 1.0f / fGalRad * (Float32)dt * c_fSpeed;

	// Animate the off-animation.
	if(m_eGUIState & enumGUISTATE_Off_Mask)
		{
		m_OffInterpAlpha.Update(dt);
		m_OffInterp.Update(dt);
		m_OffInterpGUI.Update(dt);
		Float32 fXScale = (m_eGUIState == enumGUISTATE_Off ? 1.0f : m_OffInterp.Value());
		Float32 fYScale = (m_eGUIState == enumGUISTATE_Off ? m_OffInterp.Value() : c_fOffYScale);
		m_mxCamScale = PVRTMat4::Scale(fXScale, fYScale, 1.0f);
		m_cam.Set(m_mxCamScale * m_mxCam);
		Float32 fXPos = (m_eGUIState == enumGUISTATE_Off ? 0.0f : m_OffInterpGUI.Value());
		Float32 fYPos = (m_eGUIState == enumGUISTATE_Off ? m_OffInterpGUI.Value() : GetScreenDimensions(false).y*0.5f);
		m_GUICam.Set(PVRTMat4::Translation(fXPos, fYPos, 0.0f) * m_mxCamScale * m_mxCamGUI);

		if(!m_OffInterp.IsActive() && m_eGUIState == enumGUISTATE_Off)
			{		// Next stage
			m_OffInterp.Open(1.0f, c_fOffXScale, enumINTERPOLATOR_CubicEaseIn, c_fOffStageSpeed);
			m_OffInterpGUI.Open(0.0f, GetScreenDimensions(false).x*0.5f, enumINTERPOLATOR_CubicEaseIn, c_fOffStageSpeed);
			m_eGUIState = enumGUISTATE_Off2;
			}
		else if(!m_OffInterp.IsActive() && m_eGUIState == enumGUISTATE_Off2)
			{
			m_eGUIState = enumGUISTATE_Off_Done;
			}
		}
	}

/*!***********************************************************************
 @Function		Render
 @Access		public 
 @Returns		void
 @Description	
*************************************************************************/
void ViewGUI::Render()
	{
	PVRTMat4 mxMVP;
	glDisable(GL_DEPTH_TEST);

	// Render the galaxy background
	glBindTexture(GL_TEXTURE_2D, m_GalaxyBGTex->GetHandle());
	Shader::Use(m_GUIShader);
	mxMVP = m_cam.GetProjection() * m_cam.GetView();
	glUniformMatrix4fv(m_GUIShader->GetUniform(c_uiMVP), 1, GL_FALSE, mxMVP.f);
	GFX->RenderQuad(Rectanglef(-fGalTexRad, -fGalTexRad, fGalTexRad, fGalTexRad), 
		PVRTMat4::RotationZ(m_fGalTexRot), FLAG_VRT | FLAG_TEX0 | FLAG_RGB);

	glDisable(GL_BLEND);
	// Render the stars
	Shader::Use(m_VCShader);
	glUniformMatrix4fv(m_VCShader->GetUniform(c_uiMVP), 1, GL_FALSE, mxMVP.f);
	GFX->RenderPrimitiveList(m_Tris.Pointer(), m_Tris.Count(), FLAG_VRT | FLAG_RGB);
	glEnable(GL_BLEND);
        
	// Render Menu text
	if(m_eGUIState != enumGUISTATE_Off2)
		{
		Shader::Use(m_GUIShader);
		mxMVP = m_GUICam.GetProjection() * m_GUICam.GetView();
		glUniformMatrix4fv(m_GUIShader->GetUniform(c_uiMVP), 1, GL_FALSE, mxMVP.f);

		RenderText();
		}
	
	if(m_eGUIState & enumGUISTATE_Off_Mask)
		{
		// Quick screen aligned quad for white mask
		Shader::Use(m_VCShader);
		glEnable(GL_BLEND);
		PVRTMat4 mxMVP = m_cam.GetProjection() * m_mxCamScale;
		glUniformMatrix4fv(m_VCShader->GetUniform(c_uiMVP), 1, GL_FALSE, mxMVP.f);
		Float32 fHW = m_GalaxyBGTex->GetWidth()  * 0.5f;
		Float32 fHH = m_GalaxyBGTex->GetHeight() * 0.5f;
		GFX->RenderQuad(Rectanglef(-fHW, -fHH, fHW, fHH), 
			PVRTMat4::Identity(), FLAG_VRT | FLAG_RGB, RGBA(0,1,0,m_OffInterpAlpha.Value()));
		Shader::Use(0);
		glDisable(GL_BLEND);
		}

	Shader::Use(0);
	glEnable(GL_DEPTH_TEST);
	}

/*!***********************************************************************
 @Function		GetScreenDimensions
 @Access		private 
 @Returns		PVRTVec2
 @Description	
*************************************************************************/
Vector2i ViewGUI::GetScreenDimensions(bool bRotate)
	{
	Sint32 ViewW, ViewH;
	if(GFX->GetDeviceResolution() == enumDEVRES_XGA)
		{
		if(bRotate && GFX->IsRotated())
			{
			ViewW = 768;
			ViewH = 1024;
			}
		else
			{
			ViewW = 1024;
			ViewH = 768;
			}
		}
	else
		{
		if(bRotate && GFX->IsRotated())
			{
			ViewW = 640;
			ViewH = 960;
			}
		else 
			{
			ViewW = 960;
			ViewH = 640;			
			}
		}
	return Vector2i(ViewW, ViewH);
	}

/*!***********************************************************************
 @Function		Close
 @Access		protected 
 @Returns		void
 @Description	
*************************************************************************/
void ViewGUI::Close()
	{
	m_OffInterp.Open(1.0f, c_fOffYScale, enumINTERPOLATOR_CubicEaseIn, c_fOffStageSpeed);
	m_OffInterpGUI.Open(0.0f, GetScreenDimensions(false).y*0.5f, enumINTERPOLATOR_CubicEaseIn, c_fOffStageSpeed);
	m_OffInterpAlpha.Open(0.0f, 1.0f, enumINTERPOLATOR_CubicEaseIn, c_fOffStageSpeed);
	m_eGUIState = enumGUISTATE_Off;
	}

/*!***********************************************************************
 @Function		Set
 @Access		public 
 @Param			const char * c_pszMsg
 @Param			bool bInsertionMarker
 @Param			bool bShowCaret
 @Param			Float32 fSpeed
 @Returns		void
 @Description	
*************************************************************************/
void ViewGUI::SMessage::Set(const char* c_pszMsg, bool bInsertionMarker, bool bShowCaret, Float32 fSpeed)
	{
	ASSERT(c_pszMsg);
	uiMsgLen = strlen(c_pszMsg) + (bInsertionMarker ? 2 : 0);
	ASSERT(uiMsgLen > 0);
	sprintf(szMessage, "%s%s", (bInsertionMarker ? "> " : ""), c_pszMsg);
	szMsgToRender[0]	= 0;
	uiDisplayCount		= 0;
	fCurrUpTime			= 0;
	fCharDelta			= fSpeed;
	bCaret				= bShowCaret;
	}


/*!***********************************************************************
 @Function		Update
 @Access		public 
 @Param			Float32 fDT
 @Returns		void
 @Description	
*************************************************************************/
void ViewGUI::SMessage::Update(Float32 fDT)
	{
	fCurrUpTime += fDT;

	if(fCurrUpTime > uiDisplayCount * fCharDelta && uiDisplayCount < uiMsgLen)
		{
		uiDisplayCount++;
		strncpy(szMsgToRender, szMessage, uiDisplayCount);
		szMsgToRender[uiDisplayCount] = 0;
		}

	if(uiDisplayCount == uiMsgLen)
		{
		// Update Caret
		if(bCaret)
			{
			if(fCurrUpTime - 0.5f > floorf(fCurrUpTime))
				{
				szMsgToRender[uiDisplayCount]   = '_';
				szMsgToRender[uiDisplayCount+1] = 0;
				}
			else
				{
				szMsgToRender[uiDisplayCount]   = 0;
				}
			}

		// Update next item
		if(pNext)
			pNext->Update(fDT);
		}

	}
