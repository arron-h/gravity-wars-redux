#include "stdafx.h"
#include "App.h"

#include "GraphicsImpl.h"
#include "ResourceImpl.h"

#import <UIKit/UIKit.h>

class iOS_App : public App
	{
	private:
		static bool		m_bReadyToLoadRes;

	public:
		virtual bool Initialise(Uint32 uiWidth, Uint32 uiHeight, const char* c_pszWindowTitle)
			{
			m_bReadyToLoadRes = false;

			m_pGraphicsSys = new GraphicsSystemImpl();
			m_pResMan	   = new ResourceManagerImpl();

			Float32 fVWidth = 1024.0f;
			Float32 fVHeight = 768.0f;
			OnInitialise(fVWidth, fVHeight);		// Callback to initialise.

			bool bResult = m_pGraphicsSys->Initialise(uiWidth, uiHeight, fVWidth, fVHeight, false, c_pszWindowTitle);
			if(!bResult)
				{
				delete m_pGraphicsSys;
				m_pGraphicsSys = NULL;
				return false;
				}

			// Create a thread to load background resources
			THREADID id;
			Thread::Create(&id, ThreadFunc, this);

			// We can now load resources.
			m_bReadyToLoadRes = true;
			m_eState = enumSTATE_ResourceLoading;

			// Notify that OpenGL has initialised.
			OnViewInitialised();		// OpenGL initialised.

			return bResult;
			}

		virtual void Destroy()
			{
			OnDestroy();
			m_pGraphicsSys->Destroy();
			}

		virtual double GetTicks()
			{
			return CFGetAbsoluteTime();
			}

		static void* ThreadFunc(void* pArg)
			{
			GraphicsSystemImpl* pGFX = (GraphicsSystemImpl*)(GetApp()->GetGraphicsSystem());
			// Create an OpenGL context
	
			((App*)pArg)->ResourceLoader();
			return 0;
			}
	};

static iOS_App g_app;

App* GetApp()
	{
	return &g_app;
	}

int main(int argc, char *argv[])
	{
	NSAutoreleasePool*		pool = [NSAutoreleasePool new];
	UIApplicationMain(argc, argv, nil, @"AppController");

	[pool release];
	return 0;
	}