#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>

#include <sys/time.h>

#include "stdafx.h"
#include "App.h"

#include "GraphicsImpl.h"
#include "ResourceImpl.h"
#include "AudioImpl.h"

bool TestEGLError(const char* pszLocation)
{
	/*
		eglGetError returns the last error that has happened using egl,
		not the status of the last called function. The user has to
		check after every single egl call or at least once every frame.
	*/
	EGLint iErr = eglGetError();
	if (iErr != EGL_SUCCESS)
	{
		DebugLog("%s failed (%d).", pszLocation, iErr);
		return false;
	}

	return true;
}

class Linux_App : public App
	{
	private:
		timeval			m_timeStart;
		static bool		m_bReadyToLoadRes;

		EGLDisplay      m_eglDisplay;
		EGLSurface      m_eglSurface;
		EGLContext      m_eglContext;
		SDL_Window*     m_glesWindow;

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

			DebugLog("Loading resources.");

			// Android TEMP
			this->ResourceLoader();

			// We can now load resources.
			m_bReadyToLoadRes = true;
			m_eState = enumSTATE_ResourceLoading;

			// Notify that OpenGL has initialised.
			OnViewInitialised();		// OpenGL initialised.
			DebugLog("View Initialised.");

			// Get the current tick count
			gettimeofday(&m_timeStart, NULL);

			return bResult;
			}

		virtual void Destroy()
			{
			OnDestroy();
			m_pGraphicsSys->Destroy();
			m_pAudioEng->StopAll();
			}

		virtual double GetTicks()
			{
			timeval now;
			gettimeofday(&now, NULL);

			double elapsedTime;
			elapsedTime = (now.tv_sec - m_timeStart.tv_sec);      // sec to ms
			elapsedTime += (now.tv_usec - m_timeStart.tv_usec) / 1000000.0;   // us to ms
			return elapsedTime;
			}

		bool InitialiseEGL(EGLint w, EGLint h)
		{
			// EGL variables
			EGLConfig			eglConfig	= 0;
			EGLint ai32ContextAttribs[]     = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };

			m_eglDisplay = eglGetDisplay((EGLNativeDisplayType) EGL_DEFAULT_DISPLAY);

			EGLint iMajorVersion, iMinorVersion;

			if (!eglInitialize(m_eglDisplay, &iMajorVersion, &iMinorVersion))
			{
				DebugLog("Error: eglInitialize() failed.");
				return false;
			}

			eglBindAPI(EGL_OPENGL_ES_API);
			if (!TestEGLError("eglBindAPI"))
			{
				return false;
			}

			EGLint pi32ConfigAttribs[11];
			pi32ConfigAttribs[0] = EGL_RED_SIZE;
			pi32ConfigAttribs[1] = 5;
			pi32ConfigAttribs[2] = EGL_GREEN_SIZE;
			pi32ConfigAttribs[3] = 6;
			pi32ConfigAttribs[4] = EGL_BLUE_SIZE;
			pi32ConfigAttribs[5] = 5;
			pi32ConfigAttribs[6] = EGL_SURFACE_TYPE;
			pi32ConfigAttribs[7] = EGL_WINDOW_BIT;
			pi32ConfigAttribs[8] = EGL_RENDERABLE_TYPE;
			pi32ConfigAttribs[9] = EGL_OPENGL_ES2_BIT;
			pi32ConfigAttribs[10] = EGL_NONE;

			EGLint num_config;
			if(!eglChooseConfig(m_eglDisplay, pi32ConfigAttribs, &eglConfig, 1, &num_config) || num_config != 1)
			{
				return false;
			}

			EGLint visualID;
			eglGetConfigAttrib(m_eglDisplay, eglConfig, EGL_NATIVE_VISUAL_ID, &visualID);

			m_glesWindow = SDL_CreateWindow("Gravity Wars Redux", 0, 0, w, h, SDL_WINDOW_OPENGL);

			// Change the format of our window to match our config
			//ANativeWindow_setBuffersGeometry(m_pApp->window, 0, 0, visualID);
			SDL_SysWMinfo sysInfo;
			SDL_VERSION(&sysInfo.version);
			SDL_GetWindowWMInfo(m_glesWindow, &sysInfo);

			m_eglSurface = eglCreateWindowSurface(m_eglDisplay, eglConfig, (EGLNativeWindowType)sysInfo.info.x11.window, NULL);

			if (!TestEGLError("eglCreateWindowSurface"))
			{
				return false;
			}

			m_eglContext = eglCreateContext(m_eglDisplay, eglConfig, NULL, ai32ContextAttribs);
			if (!TestEGLError("eglCreateContext"))
			{
				return false;
			}

			eglMakeCurrent(m_eglDisplay, m_eglSurface, m_eglSurface, m_eglContext);
			if (!TestEGLError("eglMakeCurrent"))
			{
				return false;
			}

			return true;
		}

		bool DestroyEGL()
		{
			eglMakeCurrent(m_eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT) ;
			eglTerminate(m_eglDisplay);
			SDL_DestroyWindow(m_glesWindow);
			return true;
		}

		void SwapBuffers()
		{
			eglSwapBuffers(m_eglDisplay, m_eglSurface);
		}
	};

bool Linux_App::m_bReadyToLoadRes = false;

static Linux_App* g_app;

App* GetApp()
{
	return g_app;
}

int main(int argc, char** argv)
{
	g_app = new Linux_App();

	EGLint w, h;
	w = 1024;
	h = 768;
	g_app->InitialiseEGL(w, h);

	// --- Create the app!
	g_app->Initialise(w, h, "Gravity Wars Redux");

   	SDL_Event event;

	bool runGame = 1;
   	while(runGame)
   	{
 	       // Read all pending events.
		while(SDL_PollEvent(&event))
		{
			switch(event.type)
			{
				case SDL_QUIT:
					DebugLog("Shutting down the application...");
					runGame = 0;
					break;
				case SDL_KEYUP:
					switch(event.key.keysym.sym)
					{
						case SDLK_UP:
							g_app->OnKeyPress(App::enumKEY_Up);
							break;
						case SDLK_DOWN:
							g_app->OnKeyPress(App::enumKEY_Down);
							break;
						case SDLK_LEFT:
							g_app->OnKeyPress(App::enumKEY_Left);
							break;
						case SDLK_RIGHT:
							g_app->OnKeyPress(App::enumKEY_Right);
							break;
						case SDLK_RETURN:
							g_app->OnKeyPress(App::enumKEY_Return);
							break;
					}
					break;
			}
		}

		g_app->OnUpdate();
		g_app->OnRender();

		g_app->SwapBuffers();
    	}

	g_app->Destroy();
	g_app->DestroyEGL();
}
