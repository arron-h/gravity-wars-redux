#include "stdafx.h"
#include "ViewGame.h"
#include "ViewGameLoader.h"
#include <time.h>

bool g_bLoaded;
extern const Uint32 c_uiMVP = HashMap_Hash("mxMVP");

ViewGameLoader::ViewGameLoader(const GameData& gamedata) : m_fMusicFade(1.0f)
	{
	g_bLoaded = false;

	m_Font = RESMAN->GetFont("courbd_36");
	m_shaderFont = RESMAN->GetShader("MVP_V_T_C");

	GraphicsSystem* pGfx = GetApp()->GetGraphicsSystem();
	// We'll base our screen coords on Retina iPhone.
	Float32 fW, fH;
	if(GFX->IsRotated())
		{
		fW = (Float32)pGfx->GetDeviceHeight();
		fH = (Float32)pGfx->GetDeviceWidth();		// Handle iPad.		
		}
	else
		{
		fW = (Float32)pGfx->GetDeviceWidth();
		fH = (Float32)pGfx->GetDeviceHeight();		// Handle iPad.
		}
	m_cam.SetProjection(PVRTMat4::Ortho(0, fH, fW, 0, -1.0f, 1.0f, PVRTMat4::OGL, pGfx->IsRotated()));
	SetCamera(&m_cam);

	m_LoaderData.Data = gamedata;		// Copy the game data

	m_MenuMusic = RESMAN->GetAudioStream("menumusic.mp3");

	THREADID threadID;
	Thread::Create(&threadID, LoadGame, &m_LoaderData);
	}

/*!***********************************************************************
 @Function		LoadGame
 @Access		private static 
 @Param			void * pArg
 @Returns		void*
 @Description	
*************************************************************************/
void* ViewGameLoader::LoadGame(void* pArg)
	{
	srand((Uint32)(time((time_t)0)));
	GameLoaderData* pNewGameData = (GameLoaderData*)pArg;
	pNewGameData->pGameView = new GameView();
	while(!(pNewGameData->pGameView->GenerateGame(pNewGameData->ProgressFunc, pNewGameData->Data)));		// Keep trying until it fookin' works. (99% of the time it will generate OK, just need to protect ourselves).
	g_bLoaded = true;

	return 0;
	}

/*!***********************************************************************
 @Function		Render
 @Access		public 
 @Returns		void
 @Description	
*************************************************************************/
void ViewGameLoader::Render()
	{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	TXChar szMessage[256], szRound[256];
	m_LoaderData.ProgressFunc.GetMessage(szMessage, 256);

	sprintf(szRound, GWSTR(enumSTRING_RoundXofY), m_LoaderData.Data.m_uiRoundsPlayed+1, m_LoaderData.Data.m_uiRounds);

	Shader::Use(m_shaderFont);
	PVRTMat4 mxMVP = m_cam.GetProjection();
	glUniformMatrix4fv(m_shaderFont->GetUniform(c_uiMVP), 1, GL_FALSE, mxMVP.f);
	m_Font->RenderString(GWSTR(enumSTRING_Loading), GFX->GetDeviceWidth()/2, GFX->GetDeviceHeight()/2, enumTEXTJUSTIFY_Centre, 1.0f, -2.0f);
	m_Font->RenderString(szRound, GFX->GetDeviceWidth()/2, GFX->GetDeviceHeight()/2 - 50, enumTEXTJUSTIFY_Centre, 0.5f, -2.0f);
	m_Font->RenderString(szMessage, GFX->GetDeviceWidth()/2, GFX->GetDeviceHeight()/2 - 75, enumTEXTJUSTIFY_Centre, 0.5f, -2.0f);
	Shader::Use(0);
	}

/*!***********************************************************************
 @Function		Update
 @Access		public 
 @Param			double dt
 @Returns		void
 @Description	
*************************************************************************/
void ViewGameLoader::Update(double dt)
	{
	if(m_fMusicFade > 0.0f)
		{
		m_fMusicFade -= (Float32)dt;
		AUDENG->SetVolume(m_MenuMusic, m_fMusicFade);
		}
	
	if(g_bLoaded)
		{
		AUDENG->Stop(m_MenuMusic);
		m_LoaderData.pGameView->Initialise();
		GetApp()->GetViewManager()->ShowView(m_LoaderData.pGameView);
		}
	}
