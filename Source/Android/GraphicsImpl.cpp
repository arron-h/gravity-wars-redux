#include "stdafx.h"

#include "Graphics.h"
#include "GraphicsImpl.h"

Touch GraphicsSystemImpl::m_Touches[2];

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
	m_bRotated = false; // TODO

	m_eDevRes = enumDEVRES_Retina; // TODO

	return true;
}

void GraphicsSystemImpl::SwapBuffers()
{
}
	
void Camera::Render()
{
}
