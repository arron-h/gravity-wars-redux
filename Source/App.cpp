#include "stdafx.h"
#include "App.h"
#include "Graphics.h"
#include "Resources/Mdl_Planet.h"
#include <time.h>

//#define PARTICLE_TEST
//#define LIMIT_30FPS
//#define LOG_FPS

App::App() : m_pGraphicsSys(NULL), m_pViewMan(NULL), m_pResMan(NULL), m_pAudioEng(NULL), m_bResLoaded(false), m_eState(enumSTATE_Initialising)
	{
	m_eLanguage = enumLANGUAGE_English;
	m_pViewMan = new ViewManager();

	srand((Uint32)time(0));
	}

App::~App()
	{
	delete m_pGraphicsSys;
	m_pGraphicsSys = NULL;

	delete m_pViewMan;
	m_pViewMan = NULL;

	delete m_pAudioEng;
	m_pAudioEng = NULL;

	delete m_pResMan;
	m_pResMan = NULL;
	}

/*!***********************************************************************
 @Function		OnInitialise
 @Access		public 
 @Returns		void
 @Description	Called before the window is created, so set any properties
				here.
*************************************************************************/
void App::OnInitialise(Float32& fDesiredVW, Float32& fDesiredVH)
	{
	fDesiredVW = 960.0f;
	fDesiredVH = 640.0f;
	}


/*!***********************************************************************
 @Function		OnViewInitialised
 @Access		public 
 @Returns		void
 @Description	
*************************************************************************/
void App::OnViewInitialised()
	{
	// Configure OpenGL states
	Float32 fW, fH;
	if(GFX->IsRotated())
		{
		fW = GFX->GetDeviceHeight();
		fH = GFX->GetDeviceWidth();
		}
	else 
		{
		fW = GFX->GetDeviceWidth();
		fH = GFX->GetDeviceHeight();
		}

	glViewport(0, 0, fW, fH);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_CULL_FACE);			// TODO
	glEnable(GL_DEPTH_TEST);

	m_pViewMan->ShowView("AppLoader");			// Show loading screen
	}

/*!***********************************************************************
 @Function		ResourceLoader
 @Access		public 
 @Returns		void
 @Description	Load any resources here. They will be loaded on a thread.
*************************************************************************/
void App::ResourceLoader()
	{
	RESMAN->LoadTexture("glow");
	RESMAN->LoadTexture("projectile");
	RESMAN->LoadTexture("spaceship");
	RESMAN->LoadTexture("mercury");
	RESMAN->LoadTexture("venus");
	RESMAN->LoadTexture("earth");
	RESMAN->LoadTexture("mars");
	RESMAN->LoadTexture("jupiter");
	RESMAN->LoadTexture("playerindicator");

	RESMAN->LoadTexture("particle");
	RESMAN->LoadTexture("particle-cloud");
	RESMAN->LoadTexture("projectile-trail");
	RESMAN->LoadTexture("particle-cloud2");
	RESMAN->LoadTexture("explosiondisc");

	RESMAN->LoadModelFromMemory("planet", (const char*)_Mdl_Planet_pod, _Mdl_Planet_pod_size);

	RESMAN->LoadFont("courbd_18");
	RESMAN->LoadFont("courbd_36");
	RESMAN->LoadFont("courbd_64");

	if(GFX->GetDeviceResolution() == enumDEVRES_HVGA)
		{
		RESMAN->LoadTexture("keyboard-up-512");
		RESMAN->LoadTexture("keyboard-down-512");
		}
	else
		{
		RESMAN->LoadTexture("keyboard-up-1024");
		RESMAN->LoadTexture("keyboard-down-1024");
		}

	RESMAN->LoadTexture("menu-galaxy-bg");

	ShaderRef pShader;
	Uint32 uiVert, uiFrag;

	const char* pszMVP_V_T_CAttribs[] = {"inVertex", "inTexCoord", "inRGBA"};
	RESMAN->LoadShaderSource("MVP_V_T_C.vert", GL_VERTEX_SHADER, uiVert);
	RESMAN->LoadShaderSource("MVP_V_T_C.frag", GL_FRAGMENT_SHADER, uiFrag);
	pShader= RESMAN->CreateShaderProgram("MVP_V_T_C", uiVert, uiFrag, pszMVP_V_T_CAttribs, 3);
	pShader->BindUniform("mxMVP");

	const char* pszMVP_V_CAttribs[] = {"inVertex", "inTexCoord", "inRGBA"};
	RESMAN->LoadShaderSource("MVP_V_C.vert", GL_VERTEX_SHADER, uiVert);
	RESMAN->LoadShaderSource("MVP_V_C.frag", GL_FRAGMENT_SHADER, uiFrag);
	pShader= RESMAN->CreateShaderProgram("MVP_V_C", uiVert, uiFrag, pszMVP_V_CAttribs, 3);
	pShader->BindUniform("mxMVP");

	const char* pszPlanetAttribs[] = {"inVertex", "inTexCoord", "inRGBA", "inNormal"};
	RESMAN->LoadShaderSource("Planet.vert", GL_VERTEX_SHADER, uiVert);
	RESMAN->LoadShaderSource("Planet.frag", GL_FRAGMENT_SHADER, uiFrag);
	pShader= RESMAN->CreateShaderProgram("Planet", uiVert, uiFrag, pszPlanetAttribs, 4);
	pShader->BindUniform("mxVP");
	pShader->BindUniform("mxModel");
	pShader->BindUniform("vLightDir");
	pShader->BindUniform("fTime");
	pShader->BindUniform("fAmp");

	RESMAN->LoadAudioStream("menu.wav");
	RESMAN->LoadAudioStream("menumusic.mp3");
	RESMAN->LoadAudioStream("gamemusic.mp3");
	RESMAN->LoadAudioStream("gameinput.wav");
	
	m_bResLoaded = true;
	}

/*!***********************************************************************
 @Function		ToggleNextLanguage
 @Access		public
 @Returns		void
 @Description
 *************************************************************************/
void App::ToggleNextLanguage()
{
	if(m_eLanguage == enumLANGUAGE_LAST)
	{
		m_eLanguage = enumLANGUAGE_FIRST;
	}
	else
	{
		m_eLanguage = (enumLANGUAGE)(m_eLanguage + 1);
	}
}

/*!***********************************************************************
 @Function		OnUpdate
 @Access		public 
 @Returns		void
 @Description	
*************************************************************************/
void App::OnUpdate()
	{
	// Update current view
	double fThisTick = GetTicks();
	double fDT = fThisTick - m_fLastTick;
	m_fLastTick = fThisTick;

#ifdef LIMIT_30FPS
	const Float32 fTargetFPS = 30.0f;
	const Float32 fTargetFPSRecip = 1.0f / fTargetFPS;

	// Stick around while we reach our desired FPS
	do 
		{
		fThisTick = GetTicks();
		fDT = fThisTick - m_fLastTick;
		} while (fDT < fTargetFPSRecip);
	
	m_fLastTick = fThisTick;
#endif



#ifdef LOG_FPS
	static Float32 fFPSTick = 0.0f;
	static Uint32 uiTicks = 0;
	fFPSTick += (Float32)fDT;
	uiTicks++;
	if(fFPSTick > 1.0f)
		{
		Float32 fAvgFPS = uiTicks > 0 ? 1.0f / (Float32)uiTicks : 0;
		Float32 fFPS = fAvgFPS > 0 ? 1.0f / fAvgFPS : 0.0f;
		DebugLog("Avg. FPS: %f", fFPS);
		fFPSTick  = 0.0f;
		uiTicks   = 0;
		}
#endif

	if(m_eState == enumSTATE_ResourceLoading && m_bResLoaded)
		{
		m_eState = enumSTATE_Rendering;
#ifdef PARTICLE_TEST
		m_pViewMan->ShowView("Particles");
#else
		m_pViewMan->ShowView("Menu");		// Show default view
#endif
		}

	m_pViewMan->GetActiveView()->Update(fDT);
	}

/*!***********************************************************************
 @Function		OnRender
 @Access		public 
 @Returns		void
 @Description	
*************************************************************************/
void App::OnRender()
	{
	// Clear buffers
	glClearColor(0,0,0,1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Render current view
	Camera* pCam = m_pViewMan->GetActiveView()->GetCamera();
	if(pCam)
		pCam->Render();

	m_pViewMan->GetActiveView()->Render();
		
	// Swap buffers
	GFX->SwapBuffers();
	}

/*!***********************************************************************
 @Function		OnDestroy
 @Access		public 
 @Returns		void
 @Description	
*************************************************************************/
void App::OnDestroy()
	{
	// Clean up
	Player::CleanUp();
	}

/*!***********************************************************************
 @Function		OnTouchDown
 @Access		public 
 @Param			Touch * pTouches
 @Param			Uint32 uiNum
 @Returns		void
 @Description	
*************************************************************************/
void App::OnTouchDown(Touch* pTouches, Uint32 uiNum)
	{
	View* pView = m_pViewMan->GetActiveView();
	if(pView)
		pView->OnTouchDown(pTouches, uiNum);
	}

/*!***********************************************************************
 @Function		OnTouchMoved
 @Access		public 
 @Param			Touch * pTouches
 @Param			Uint32 uiNum
 @Returns		void
 @Description	
*************************************************************************/
void App::OnTouchMoved(Touch* pTouches, Uint32 uiNum)
	{
	View* pView = m_pViewMan->GetActiveView();
	if(pView)
		pView->OnTouchMoved(pTouches, uiNum);
	}

/*!***********************************************************************
 @Function		OnTouchUp
 @Access		public 
 @Param			Touch * pTouches
 @Param			Uint32 uiNum
 @Returns		void
 @Description	
*************************************************************************/
void App::OnTouchUp(Touch* pTouches, Uint32 uiNum)
	{
	View* pView = m_pViewMan->GetActiveView();
	if(pView)
		pView->OnTouchUp(pTouches, uiNum);
	}

/*!***********************************************************************
 @Function		OnKeyUp
 @Access		public 
 @Param			Uint32 uiKeyCode
 @Returns		void
 @Description	
*************************************************************************/
void App::OnKeyUp(Uint32 uiKeyCode)
	{
	switch(uiKeyCode)
		{
		case 'N':
			GameData data;
			data.m_uiNumPlayers = 1;
			m_pViewMan->ShowView("GameLoader", &data);
			break;
		}
	}