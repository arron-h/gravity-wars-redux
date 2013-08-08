#include <EGL/egl.h>
#include <GLES2/gl2.h>

#include <android/log.h>
#include <android/window.h>
#include <android_native_app_glue.h>

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

class Android_App : public App
	{
	private:
		timeval			m_timeStart;
		static bool		m_bReadyToLoadRes;
		
		EGLDisplay      m_eglDisplay;
		EGLSurface      m_eglSurface;
		EGLContext      m_eglContext;
		
	public:
		android_app*    m_pApp;
		bool            m_bAnimating;

	public:
		virtual bool Initialise(Uint32 uiWidth, Uint32 uiHeight, const char* c_pszWindowTitle)
			{
			DebugLog("----- Initialising Application -----");
			m_bReadyToLoadRes = false;

			m_pGraphicsSys = new GraphicsSystemImpl();
			m_pResMan	   = new ResourceManagerImpl(m_pApp->activity->assetManager);
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
			
		bool InitialiseEGL(EGLint& w, EGLint& h)
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
		
			// Change the format of our window to match our config
			ANativeWindow_setBuffersGeometry(m_pApp->window, 0, 0, visualID);
		
			m_eglSurface = eglCreateWindowSurface(m_eglDisplay, eglConfig, m_pApp->window, NULL);
		
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
			
			// Query width and height of the window surface created by utility code
			eglQuerySurface(m_eglDisplay, m_eglSurface, EGL_WIDTH, &w);
			eglQuerySurface(m_eglDisplay, m_eglSurface, EGL_HEIGHT, &h);
		}
		
		bool DestroyEGL()
		{
			eglMakeCurrent(m_eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT) ;
			eglTerminate(m_eglDisplay);
			return true;
		}
		
		void SwapBuffers()
		{
			eglSwapBuffers(m_eglDisplay, m_eglSurface);
		}
		
		static int HandleInput(struct android_app* app, AInputEvent* event)
		{
			Android_App* myapp = (Android_App*) app->userData;
			
			if(myapp)
			{
				switch(AInputEvent_getType(event))
				{
					case AINPUT_EVENT_TYPE_MOTION: // Handle touch events
					{
						switch(AMotionEvent_getAction(event))
						{
							case AMOTION_EVENT_ACTION_DOWN:
							{
								Touch t = { AMotionEvent_getX(event, 0), AMotionEvent_getY(event, 0) };
								myapp->OnTouchDown(&t, 1);
								
								break;
							}
							case AMOTION_EVENT_ACTION_MOVE:
							{
								Touch t = { AMotionEvent_getX(event, 0), AMotionEvent_getY(event, 0) };
								myapp->OnTouchMoved(&t, 1);
								
								break;
							}
							case AMOTION_EVENT_ACTION_UP:
							{
								Touch t = { AMotionEvent_getX(event, 0), AMotionEvent_getY(event, 0) };
								myapp->OnTouchUp(&t, 1);
								
								break;
							}
						}
						return 1;
					}
				}
			}
			
			return 1;
		}
		
		static void HandleAndroid(struct android_app* app, int32_t cmd)
		{
			Android_App* myapp = (Android_App*) app->userData;
		
			switch (cmd)
			{
				case APP_CMD_INIT_WINDOW:
					DebugLog("APP_CMD_INIT_WINDOW");
					// The window is being shown, get it ready.
					if (myapp->m_pApp->window != NULL)
					{
						EGLint w, h;
						myapp->InitialiseEGL(w, h);
						
						// --- Create the app!
						myapp->Initialise(w, h, "Gravity Wars Redux");
					}
					break;
		
				case APP_CMD_TERM_WINDOW:
					DebugLog("APP_CMD_TERM_WINDOW");
					myapp->Destroy();
                	myapp->DestroyEGL();
					break;
				case APP_CMD_GAINED_FOCUS:
					DebugLog("APP_CMD_GAINED_FOCUS");
					myapp->m_bAnimating = true;
					break;
				case APP_CMD_LOST_FOCUS:
					DebugLog("APP_CMD_LOST_FOCUS");
					myapp->m_bAnimating = false;
					break;
			}
		}
	};

bool Android_App::m_bReadyToLoadRes = false;

static Android_App g_app;

App* GetApp()
	{
	return &g_app;
	}
	
void android_main(struct android_app* state)
{
    // Make sure glue isn't stripped.
    app_dummy();

    state->userData     = &g_app;
    state->onAppCmd     = Android_App::HandleAndroid;
	state->onInputEvent = Android_App::HandleInput;
    g_app.m_pApp        = state;
    g_app.m_bAnimating  = false;

    for(;;)
    {
        // Read all pending events.
        int ident;
        int events;
        struct android_poll_source* source;

        // If not animating, we will block forever waiting for events.
        // If animating, we loop until all events are read, then continue
        // to draw the next frame of animation.
        while((ident=ALooper_pollAll(g_app.m_bAnimating ? 0 : -1, NULL, &events, (void**)&source)) >= 0)
        {
            // Process this event.
            if (source != NULL)
            {
                source->process(state, source);
            }

            // Check if we are exiting.
            if (state->destroyRequested != 0)
            {
				DebugLog("Shutting down the application...");
                return;
            }
        }

        if(g_app.m_bAnimating)
        {
			g_app.OnUpdate();
			g_app.OnRender();

			g_app.SwapBuffers();
        }
    }
}
