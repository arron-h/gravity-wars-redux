#include "stdafx.h"
#include "App.h"

#include "GraphicsImpl.h"
#include "ResourceImpl.h"
#include "AudioImpl.h"

//#define RESOLUTION_RETINA 1
#define RESOLUTION_HVGA   1
//#define RESOLUTION_IPAD   1

#if RESOLUTION_RETINA
#define SCREEN_WIDTH 960
#define SCREEN_HEIGHT 640
#elif RESOLUTION_IPAD
#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768
#elif RESOLUTION_HVGA
#define SCREEN_WIDTH 480
#define SCREEN_HEIGHT 320
#endif

#include "GLEx.h"

class Win32_App : public App
	{
	private:
		__int64			m_n64TimeStart;
		static bool		m_bReadyToLoadRes;
		static HGLRC	m_rcSecContext;

	public:
		virtual bool Initialise(Uint32 uiWidth, Uint32 uiHeight, const char* c_pszWindowTitle)
			{
			DebugLog("----- Initialising Application -----");
			m_bReadyToLoadRes = false;

			m_pGraphicsSys = new GraphicsSystemImpl();
			m_pResMan	   = new ResourceManagerImpl();
			m_pAudioEng	   = new AudioEngineImpl();

			Float32 fVWidth = 1024.0f;
			Float32 fVHeight = 768.0f;
			OnInitialise(fVWidth, fVHeight);		// Callback to initialise.

			DebugLog("Creating window");
			bool bResult = m_pGraphicsSys->Initialise(uiWidth, uiHeight, fVWidth, fVHeight, false, c_pszWindowTitle);
			if(!bResult)
				{
				delete m_pGraphicsSys;
				m_pGraphicsSys = NULL;
				return false;
				}

			DebugLog("Launching background loading thread.");
			// Create a thread to load background resources
			THREADID id;
			Thread::Create(&id, ThreadFunc, this);

			// Wait until the secondary context has been created
			while(m_rcSecContext == 0) Thread::Sleep(1);

			// Share this context and make it current
			((GraphicsSystemImpl*)m_pGraphicsSys)->ShareOGLContext(m_rcSecContext);
			((GraphicsSystemImpl*)m_pGraphicsSys)->MakeCurrent();

			DebugLog("Loading GL extenstions");
			// Load GL extensions
			extern GLExtensions ext;
			ext.Load();
			
			// We can now load resources.
			m_bReadyToLoadRes = true;
			m_eState = enumSTATE_ResourceLoading;

			// Notify that OpenGL has initialised.
			OnViewInitialised();		// OpenGL initialised.
			DebugLog("View Initialised.");
	
			// Get the current tick count
			QueryPerformanceCounter((LARGE_INTEGER*)&m_n64TimeStart);

			return bResult;
			}

		virtual void Destroy()
			{
			OnDestroy();
			m_pGraphicsSys->Destroy();
			}

		virtual double GetTicks()
			{
			__int64 n64Now, n64Freq;
			QueryPerformanceCounter((LARGE_INTEGER*)&n64Now);
			QueryPerformanceFrequency((LARGE_INTEGER*)&n64Freq);
			double fTime = (double)(n64Now - m_n64TimeStart) * 1.0 / n64Freq;
			return fTime;
			}

		static void* ThreadFunc(void* pArg)
			{
			DebugLog("THREAD2: Loading thread spawned. Creating OGL Context.");
			GraphicsSystemImpl* pGFX = (GraphicsSystemImpl*)(GetApp()->GetGraphicsSystem());
			// Create an OpenGL context
			m_rcSecContext = pGFX->CreateOGLContext();

			while(!m_bReadyToLoadRes) Thread::Sleep(1);
			pGFX->SetOGLContext(m_rcSecContext);

			DebugLog("THREAD2: Loading resources.");
			((App*)pArg)->ResourceLoader();
			return 0;
			}
	};

bool Win32_App::m_bReadyToLoadRes = false;
HGLRC Win32_App::m_rcSecContext = 0;

static Win32_App g_app;

App* GetApp()
	{
	return &g_app;
	}

int main(int argc, char *argv[])
	{
	// --- Create the app!
	if(!g_app.Initialise(SCREEN_WIDTH, SCREEN_HEIGHT, "Gravity Wars Redux"))
		return 0;

	GraphicsSystemImpl* pGFXImp = (GraphicsSystemImpl*)g_app.GetGraphicsSystem();

	DebugLog("Starting Render loop");
	MSG	 msg;
	bool bFinished = false;
	while(!bFinished)
		{
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))	// Messages! Process.
			{
			if(msg.message == WM_QUIT)
				{
				bFinished = true;
				}
			else
				{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
				}
			}
		else		// No messages, run the render loop
			{
			if(pGFXImp->IsActive())
				{
				g_app.OnUpdate();
				g_app.OnRender();
				}
			}
		}

	g_app.Destroy();
	return (msg.wParam);
	}