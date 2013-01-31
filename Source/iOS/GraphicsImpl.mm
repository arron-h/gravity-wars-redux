#include "stdafx.h"

#include "Graphics.h"
#include "GraphicsImpl.h"


GraphicsSystemImpl::GraphicsSystemImpl()
	{
	}

GraphicsSystemImpl::~GraphicsSystemImpl()
	{
	}

void GraphicsSystemImpl::Destroy()
	{
	}

bool GraphicsSystemImpl::Initialise(Uint32 uiWidth, Uint32 uiHeight, Float32 fVWidth, Float32 fVHeight, bool bRotated, const char* c_pszWindowTitle)
	{
	m_uiDeviceWidth = uiWidth;
	m_uiDeviceHeight = uiHeight;
	m_fVirtualWidth = fVWidth;
	m_fVirtualHeight = fVHeight;
	m_bRotated = true;			// ITODO

	if(uiWidth == 320 && uiHeight == 480 || uiWidth == 480 && uiHeight == 320)
		m_eDevRes = enumDEVRES_HVGA;
	else if(uiWidth == 640 && uiHeight == 960 || uiWidth == 960 && uiHeight == 640)
		m_eDevRes = enumDEVRES_Retina;
	else if(uiWidth == 768 && uiHeight == 1024 || uiWidth == 1024 && uiHeight == 768)
		m_eDevRes = enumDEVRES_XGA;

	return true;
	}
	
void GraphicsSystemImpl::SwapBuffers()
	{
	}
	
void Camera::Render()
{
}
