#include "stdafx.h"
#include <Windows.h>

#include "Graphics.h"
#include "GraphicsImpl.h"

bool GraphicsSystemImpl::m_bKeys[256];
bool GraphicsSystemImpl::m_bWindowActive = false;
bool GraphicsSystemImpl::m_bMouseDown[2];
Touch GraphicsSystemImpl::m_Touches[2];

GraphicsSystemImpl::GraphicsSystemImpl() : m_hDC(NULL), m_hRC(NULL), m_hWnd(NULL)
	{
	memset(m_bKeys, 0, sizeof(m_bKeys));
	}

GraphicsSystemImpl::~GraphicsSystemImpl()
	{
	}

void GraphicsSystemImpl::Resize(unsigned int width, unsigned int height)
	{
	}

void GraphicsSystemImpl::Destroy()
	{
	if (m_hRC)
		{
		wglMakeCurrent(NULL,NULL);
		wglDeleteContext(m_hRC);
		m_hRC = NULL;
		}

	if (m_hDC && !ReleaseDC(m_hWnd, m_hDC))
		m_hDC = NULL;

	if (m_hWnd && !DestroyWindow(m_hWnd))
		m_hWnd = NULL;

	if (!UnregisterClass(m_szWindowTitle, m_hInstance))
		m_hInstance = NULL;

	m_bWindowActive = false;
	}

bool GraphicsSystemImpl::Initialise(Uint32 uiWidth, Uint32 uiHeight, Float32 fVWidth, Float32 fVHeight, bool bRotated, const char* c_pszWindowTitle)
	{
	// --- Init some variables
	GLuint		PixelFormat;
	WNDCLASS	wc;
	DWORD		dwExStyle;
	DWORD		dwStyle;
	RECT		WindowRect;

	m_uiDeviceWidth = uiWidth;
	m_uiDeviceHeight = uiHeight;
	m_fVirtualWidth = fVWidth;
	m_fVirtualHeight = fVHeight;
	m_bRotated = false;			// PC isn't rotated

	if(uiWidth == 320 && uiHeight == 480 || uiWidth == 480 && uiHeight == 320)
		m_eDevRes = enumDEVRES_HVGA;
	else if(uiWidth == 640 && uiHeight == 960 || uiWidth == 960 && uiHeight == 640)
		m_eDevRes = enumDEVRES_Retina;
	else if(uiWidth == 768 && uiHeight == 1024 || uiWidth == 1024 && uiHeight == 768)
		m_eDevRes = enumDEVRES_XGA;

	strcpy(m_szWindowTitle, c_pszWindowTitle);

	// --- Set window sizes
	WindowRect.left		= (long)0;
	WindowRect.right	= (long)uiWidth;
	WindowRect.top		= (long)0;
	WindowRect.bottom	=( long)uiHeight;

	m_hInstance			= GetModuleHandle(NULL);				// Grab An Instance For Our Window
	wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	// Redraw On Size, And Own DC For Window.
	wc.lpfnWndProc		= (WNDPROC) WndProc;					// WndProc Handles Messages
	wc.cbClsExtra		= 0;									// No Extra Window Data
	wc.cbWndExtra		= 0;									// No Extra Window Data
	wc.hInstance		= m_hInstance;							// Set The Instance
	wc.hIcon			= LoadIcon(NULL, IDI_WINLOGO);			// Load The Default Icon
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);			// Load The Arrow Pointer
	wc.hbrBackground	= NULL;									// No Background Required For GL
	wc.lpszMenuName		= NULL;									// We Don't Want A Menu
	wc.lpszClassName	= c_pszWindowTitle;							// Set The Class Name

	if (!RegisterClass(&wc))
		return false;

	dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;			// Window Extended Style
	dwStyle   = WS_OVERLAPPEDWINDOW;						// Windows Style

	AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);

	// Create The Window
	if (!(m_hWnd=CreateWindowEx(	dwExStyle,							// Extended Style For The Window
		c_pszWindowTitle,												// Class Name
		c_pszWindowTitle,												// Window Title
		dwStyle |														// Defined Window Style
		WS_CLIPSIBLINGS |												// Required Window Style
		WS_CLIPCHILDREN,												// Required Window Style
		0, 0,															// Window Position
		WindowRect.right-WindowRect.left,								// Calculate Window Width
		WindowRect.bottom-WindowRect.top,								// Calculate Window Height
		NULL,															// No Parent Window
		NULL,															// No Menu
		m_hInstance,													// Instance
		NULL)))															// Dont Pass Anything To WM_CREATE
		{
		Destroy();											// Reset The Display
		return false;											// Return FALSE
		}

	static PIXELFORMATDESCRIPTOR pfd =
		{
		sizeof(PIXELFORMATDESCRIPTOR),				// Size Of This Pixel Format Descriptor
		1,											// Version Number
		PFD_DRAW_TO_WINDOW |						// Format Must Support Window
		PFD_SUPPORT_OPENGL |						// Format Must Support OpenGL
		PFD_DOUBLEBUFFER,							// Must Support Double Buffering
		PFD_TYPE_RGBA,								// Request An RGBA Format
		32,										// Select Our Color Depth
		0, 0, 0, 0, 0, 0,							// Color Bits Ignored
		0,											// No Alpha Buffer
		0,											// Shift Bit Ignored
		0,											// No Accumulation Buffer
		0, 0, 0, 0,									// Accumulation Bits Ignored
		16,											// 16Bit Z-Buffer (Depth Buffer)
		0,											// No Stencil Buffer
		0,											// No Auxiliary Buffer
		PFD_MAIN_PLANE,								// Main Drawing Layer
		0,											// Reserved
		0, 0, 0										// Layer Masks Ignored
		};

	// --- Get a device context
	if (!(m_hDC = GetDC(m_hWnd)))
		{
		Destroy();
		return false;
		}

	// --- Find a matching format
	if (!(PixelFormat = ChoosePixelFormat(m_hDC, &pfd)))
		{
		Destroy();
		return false;
		}

	// --- Set window pixel format
	if(!SetPixelFormat(m_hDC, PixelFormat, &pfd))
		{
		Destroy();
		return false;
		}

	// --- Get a rendering context
	if ( !(m_hRC = wglCreateContext(m_hDC) ) )
		{
		Destroy();
		return false;
		}

	// --- Make this context current
	//SetOGLContext(m_hRC);

	ShowWindow(m_hWnd, SW_SHOW);
	SetForegroundWindow(m_hWnd);						// Slightly higher priority
	SetFocus(m_hWnd);									// Sets keyboard focus to the window

	return true;
	}

LRESULT CALLBACK GraphicsSystemImpl::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
	GraphicsSystem* pGfx = GetApp()->GetGraphicsSystem();

	switch (uMsg)
		{
		case WM_ACTIVATE:
			{
			if (!HIWORD(wParam))
				m_bWindowActive = true;
			else
				m_bWindowActive = false;

			return 0;
			}
		case WM_SYSCOMMAND:
			{
			switch (wParam)
				{
				case SC_SCREENSAVE:
				case SC_MONITORPOWER:
					return 0;
				}
			break;
			}
		case WM_CLOSE:
			{
			PostQuitMessage(0);
			return 0;
			}
		case WM_KEYDOWN:
			{
			m_bKeys[wParam] = true;
			return 0;
			}
		case WM_LBUTTONDOWN:
			{
			Float32 fX = LOWORD(lParam);
			Float32 fY = HIWORD(lParam);
			Sint32 nTouch = 1;
			memset(m_Touches, 0, sizeof(m_Touches));

			m_Touches[0].fX = fX;
			m_Touches[0].fY = fY;

			GetApp()->OnTouchDown(m_Touches, nTouch);

			break;
			}
		case WM_LBUTTONUP:
			{
			Float32 fX = LOWORD(lParam);
			Float32 fY = HIWORD(lParam);
			Sint32 nTouch = 1;
			memset(m_Touches, 0, sizeof(m_Touches));

			m_Touches[0].fX = fX;
			m_Touches[0].fY = fY;

			GetApp()->OnTouchUp(m_Touches, nTouch);

			break;
			}
		case WM_MOUSEMOVE:
			{
			Float32 fX = LOWORD(lParam);
			Float32 fY = HIWORD(lParam);
			Sint32 nTouch = 0;
			memset(m_Touches, 0, sizeof(m_Touches));
			if(wParam & MK_LBUTTON)
				{
				m_Touches[nTouch].fX = fX;
				m_Touches[nTouch].fY = fY;
				nTouch++;
				}

			if(nTouch > 0)
				GetApp()->OnTouchMoved(m_Touches, nTouch);

			break;
			}
		case WM_KEYUP:
			{
			m_bKeys[wParam] = false;
			if(wParam == VK_ESCAPE)
				PostQuitMessage(0);
			else
				GetApp()->OnKeyUp(wParam);
			
			return 0;
			}
			break;
		case WM_SIZE:
			{
			Resize(LOWORD(lParam),HIWORD(lParam));
			return 0;
			}
		}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

void GraphicsSystemImpl::SwapBuffers()
	{
	::SwapBuffers(m_hDC);
	}

bool GraphicsSystemImpl::IsActive()
	{
	return m_bWindowActive;
	}

HGLRC GraphicsSystemImpl::CreateOGLContext()
	{
	return wglCreateContext(m_hDC);
	}

bool GraphicsSystemImpl::ShareOGLContext(HGLRC secContext)
	{
	return wglShareLists(secContext, m_hRC) ? true : false;
	}

void GraphicsSystemImpl::SetOGLContext(HGLRC hContext)
	{
	wglMakeCurrent(m_hDC, hContext);
	}

void GraphicsSystemImpl::MakeCurrent()
	{
	SetOGLContext(m_hRC);
	}


// ----------------------------------------------------------------------------------------
// ----------------------------------------------------------------------------------------
// ----------------------------------------------------------------- CAMERA IMPLEMTATION
void Camera::Render()
	{
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(m_mxProjection.f);

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(m_mxView.f);
	}