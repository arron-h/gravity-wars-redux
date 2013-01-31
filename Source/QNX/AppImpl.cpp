#include <screen/screen.h>
#include <bps/navigator.h>
#include <bps/screen.h>
#include <bps/bps.h>
#include <bps/event.h>

#include <input/event_types.h>
#include <input/screen_helpers.h>

#include <EGL/egl.h>
#include <GLES2/gl2.h>

#include <sys/time.h>

#include "3rdparty/bbutil.h"

#include "stdafx.h"
#include "App.h"

#include "GraphicsImpl.h"
#include "ResourceImpl.h"
#include "AudioImpl.h"


class QNX_App : public App
	{
	private:
		timeval			m_timeStart;
		static bool		m_bReadyToLoadRes;

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

			// QNX TEMP
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

		void HandleScreenEvents(bps_event_t *event)
		{
			screen_event_t screen_event = screen_event_get_event(event);

			int screen_val, buttons;
			int pair[2];

			static bool mouse_pressed = false;

			//Query type of screen event and its location on the screen
			screen_get_event_property_iv(screen_event, SCREEN_PROPERTY_TYPE, &screen_val);
			screen_get_event_property_iv(screen_event, SCREEN_PROPERTY_SOURCE_POSITION, pair);

			Touch t = {pair[0], pair[1]};

			//There is a difference between touch screen events and mouse events
			if (screen_val == SCREEN_EVENT_MTOUCH_RELEASE)
			{
				OnTouchUp(&t,1);
			}
			else if (screen_val == SCREEN_EVENT_MTOUCH_TOUCH)
			{
				OnTouchDown(&t,1);
			}
			else if (screen_val == SCREEN_EVENT_MTOUCH_MOVE)
			{
				OnTouchMoved(&t,1);
			}
			else if (screen_val == SCREEN_EVENT_POINTER)
			{
				//This is a mouse move event, it is applicable to a device with a usb mouse or simulator
				screen_get_event_property_iv(screen_event, SCREEN_PROPERTY_BUTTONS, &buttons);

				//Left mouse button is pressed
				if (buttons == SCREEN_LEFT_MOUSE_BUTTON)
				{
					if (mouse_pressed)
					{
						OnTouchMoved(&t,1);
					}
					else
					{
						OnTouchDown(&t,1);
					}

					mouse_pressed = true;
				}
				else
				{
					if (mouse_pressed)
					{
						// Left mouse button was released, handle left click
						OnTouchUp(&t,1);
						mouse_pressed = false;
					}
				}
			}
		}
	};

bool QNX_App::m_bReadyToLoadRes = false;
//HGLRC Win32_App::m_rcSecContext = 0;

static QNX_App g_app;

App* GetApp()
	{
	return &g_app;
	}

int main(int argc, char *argv[])
	{
	static screen_context_t screen_cxt;
	
	//Create a screen context that will be used to create an EGL surface to to receive libscreen events
    screen_create_context(&screen_cxt, 0);
	
	 //Initialize BPS library
    bps_initialize();
	
	// EGL init
	if(bbutil_init_egl(screen_cxt) != EXIT_SUCCESS)
	{
		fprintf(stderr, "bbutil_init_egl failed\n");
        bbutil_terminate();
        screen_destroy_context(screen_cxt);
		return 0;
	}
	
	//Query width and height of the window surface created by utility code
    EGLint surface_width, surface_height;
    eglQuerySurface(egl_disp, egl_surf, EGL_WIDTH, &surface_width);
    eglQuerySurface(egl_disp, egl_surf, EGL_HEIGHT, &surface_height);
	
	// --- Create the app!
	if(!g_app.Initialise(surface_width, surface_height, "Gravity Wars Redux"))
		return 0;
		
	//Signal BPS library that navigator and screen events will be requested
	screen_request_events(screen_cxt);
	navigator_request_events(0);		// TODO ?
	
	//Signal BPS library that navigator orientation is to be locked
	navigator_rotation_lock(true);
	
	GraphicsSystemImpl* pGFXImp = (GraphicsSystemImpl*)g_app.GetGraphicsSystem();

	DebugLog("Starting Render loop");
	int exit_application = 0;
	int rc;
	while (!exit_application) 
	{
        //Request and process all available BPS events
        bps_event_t *event = NULL;
        for(;;) 
		{
            rc = bps_get_event(&event, 0);
            assert(rc == BPS_SUCCESS);

            if (event) 
			{
                int domain = bps_event_get_domain(event);

                if (domain == screen_get_domain()) 
				{
                    g_app.HandleScreenEvents(event);
                } 
				else if ((domain == navigator_get_domain()) && (NAVIGATOR_EXIT == bps_event_get_code(event)))
				{
                    exit_application = 1;
                }
            }
			else 
			{
                break;
            }
        }
        g_app.OnUpdate();
		g_app.OnRender();
    }
	
	g_app.Destroy();
	
	//Stop requesting events from libscreen
    screen_stop_events(screen_cxt);

    //Shut down BPS library for this process
    bps_shutdown();

    //Use utility code to terminate EGL setup
    bbutil_terminate();

    //Destroy libscreen context
    screen_destroy_context(screen_cxt);
    return 0;
}
