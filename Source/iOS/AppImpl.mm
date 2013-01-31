#include "stdafx.h"
#include "App.h"

#include "GraphicsImpl.h"
#include "ResourceImpl.h"
#include "AudioImpl.h"

#import <UIKit/UIKit.h>
#import <CoreFoundation/CoreFoundation.h>

class iOS_App : public App
	{
	private:
		static bool		m_bReadyToLoadRes;
		double			m_fStart;

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
			//THREADID id;
			//Thread::Create(&id, ThreadFunc, this);
			ResourceLoader();

			// We can now load resources.
			m_bReadyToLoadRes = true;
			m_eState = enumSTATE_ResourceLoading;

			// Notify that OpenGL has initialised.
			OnViewInitialised();		// OpenGL initialised.
			DebugLog("View Initialised.");
			
			m_fStart = CFAbsoluteTimeGetCurrent();
	
			return bResult;
			}

		virtual void Destroy()
			{
			OnDestroy();
			m_pGraphicsSys->Destroy();
			}

		virtual double GetTicks()
			{
			return CFAbsoluteTimeGetCurrent() - m_fStart;
			}

		static void* ThreadFunc(void* pArg)
			{
			DebugLog("THREAD2: Loading thread spawned. Creating OGL Context.");
			// ITODO: Create an OpenGL context

			DebugLog("THREAD2: Loading resources.");
			((App*)pArg)->ResourceLoader();
			return 0;
			}
	};

bool iOS_App::m_bReadyToLoadRes = false;

static iOS_App g_app;

App* GetApp()
	{
	return &g_app;
	}
