#include "stdafx.h"
#include "ViewGame.h"
#include "StarfieldGenerator.h"
#include "Planet.h"
#include "Spaceship.h"
#include "Projectile.h"
#include <stdlib.h>
#include <algorithm>

//#define DEBUG_INPUT

#define SIMULATE_STEPS 256

#define PLAYER_1 0
#define PLAYER_2 1

#define PLAYER_HUMAN PLAYER_1
#define PLAYER_AI PLAYER_2

#define MIN_POWER 60.0f
#define MAX_POWER 250.0f
#define TOUCH_IND_RAD 24.0f

const Uint32  c_uiNumSimsForDifficulty[] =
	{
	1,			// enumGWDIFFICULTY_Easy
	2,			// enumGWDIFFICULTY_Medium
	4,			// enumGWDIFFICULTY_Hard
	};

const Uint32 c_uiCalcIterationsForDifficulty[] =
	{
	20,			// enumGWDIFFICULTY_Easy
	50,			// enumGWDIFFICULTY_Medium
	100,		// enumGWDIFFICULTY_Hard
	};

const char* c_pszPlanetTextures[] =
	{
	"mercury",
	"venus",
	"earth",
	"mars",
	"jupiter",
	};
const int gc_NumPlantTex = ELEMS_IN_ARRAY(c_pszPlanetTextures);

const Uint32 c_uiMVP = HashMap_Hash("mxMVP");

const Float32 c_fGameVolLevel = 0.5f;

// Prototype
void PrecalcPhysics(double fDT, Planet* pPlanets, Uint32 uiNumPlanets, PVRTVec3& vVel, PVRTVec3& vPos);

GameView::GameView() : m_pPlanets(NULL), m_uiNumPlanets(0), m_bSpringBack(false), m_nPlayerTurn(-1), m_pSpaceships(NULL), m_bg(NULL), m_bShowQuitMenu(false)
	{
#ifdef INPUT_PRECALC_TEST
	m_debugtris.Initialise(SIMULATE_STEPS);
	m_debugcount = 0;
#endif

	// Device specific sizes
	if(GFX->GetDeviceResolution() == enumDEVRES_HVGA)
	{
		m_fButtonSize = 40.0f;
		m_fKeyboardY  = 7.0f;
	}
	else
	{
		m_fButtonSize = 80.0f;
		m_fKeyboardY  = 5.0;
	}

	m_fAILastResults[0] = m_fAILastResults[1] = 999999999.0f;

	m_ui16KeyMask = 0;
	GraphicsSystem* pGfx = GetApp()->GetGraphicsSystem();
	// We'll base our screen coords on Retina iPhone.
	Float32 fW, fH;
	Float32 fInvAspect = 1.0f / pGfx->GetAspectRatio();

	Float32 fHudW, fHudH;
	if(GFX->IsRotated())
		{
		fHudW = (Float32)pGfx->GetDeviceHeight();
		fHudH = (Float32)pGfx->GetDeviceWidth();
		fW = m_fViewCoordY = (ceilf(pGfx->GetVirtualWidth() * fInvAspect)) * 0.5f;		// Handle iPad.
		fH = m_fViewCoordX = pGfx->GetVirtualWidth() * 0.5f;
		}
	else
		{
		fHudW = (Float32)pGfx->GetDeviceWidth();
		fHudH = (Float32)pGfx->GetDeviceHeight();
		fW = m_fViewCoordX = pGfx->GetVirtualWidth() * 0.5f;
		fH = m_fViewCoordY = (ceilf(pGfx->GetVirtualWidth() * fInvAspect)) * 0.5f;		// Handle iPad.
		}

	m_cam.SetProjection(PVRTMat4::Ortho(-fW, fH, fW, -fH, -100.0f, 100.0f, PVRTMat4::OGL, pGfx->IsRotated()));
	SetCamera(&m_cam);

	// Create a matrix for the HUD. This is based on device coords
	m_camHUD.SetProjection(PVRTMat4::Ortho(0, fHudH, fHudW, 0, -1.0f, 1.0f, PVRTMat4::OGL, pGfx->IsRotated()));

	// --- Get Resources
	m_texPlayerIndicator = RESMAN->GetTexture("playerindicator");

	// Get font
	if(GFX->GetDeviceResolution() == enumDEVRES_HVGA)
		{
		m_FontBig = RESMAN->GetFont("courbd_18");
		m_FontSmall = RESMAN->GetFont("courbd_18");		// TODO. New small font
		}
	else
		{
		m_FontBig   = RESMAN->GetFont("courbd_36");
		m_FontSmall = RESMAN->GetFont("courbd_18");
		}

	m_2DShader = RESMAN->GetShader("MVP_V_T_C");
	m_VCShader = RESMAN->GetShader("MVP_V_C");

	// Get some textures
	if(GFX->GetDeviceResolution() == enumDEVRES_HVGA)
	    {
		m_texKeyboardUp   = RESMAN->GetTexture("keyboard-up-512");
		m_texKeyboardDown = RESMAN->GetTexture("keyboard-down-512");
	    }
	else
	    {
		m_texKeyboardUp   = RESMAN->GetTexture("keyboard-up-1024");
		m_texKeyboardDown = RESMAN->GetTexture("keyboard-down-1024");
	    }

	m_HUDFont = RESMAN->GetFont("courbd_36");

    m_Music = RESMAN->GetAudioStream("gamemusic.mp3");
	m_InputSfx = RESMAN->GetAudioStream("gameinput.wav");
	}

GameView::~GameView()
	{
	delete [] m_pPlanets;
	delete [] m_pSpaceships;
	delete m_bg;
	}


/*!***********************************************************************
 @Function		GenerateGame
 @Access		public
 @Returns		bool
 @Description	Returns TRUE if successful, returns false if it failed.
*************************************************************************/
bool GameView::GenerateGame(LoaderFunctor& loaderProgress, const GameData& data)
	{
	m_GameData = data;		// Copy game data.

	// --- Initialise the light position
	Float32 fRnd = Randomf() * PVRT_PI;
	m_vLight = PVRTVec4(cos(fRnd), sin(fRnd), 1.0f, 0.0f);

	Uint32 starfieldW = std::min<Uint32>(GFX->GetDeviceWidth(),  2048);
	Uint32 starfieldH = std::min<Uint32>(GFX->GetDeviceHeight(), 1024);

	DebugLog("Generating starfield %dx%d", starfieldW, starfieldH);

	// --- Generate a background image
	m_bg = new StarfieldGenerator;
	m_bg->Generate(starfieldW, starfieldH, loaderProgress);

	loaderProgress.SetMessage(GWSTR(enumSTRING_PopulatingSystem));

	// Get a bunch of textures
	TextureRef Textures[gc_NumPlantTex];
	for(int i = 0; i < gc_NumPlantTex; i++)
		Textures[i] = RESMAN->GetTexture(c_pszPlanetTextures[i]);

	// --- Initialise some random planets
	m_uiNumPlanets = 3 + Randomi(3);
	m_pPlanets = new Planet[m_uiNumPlanets];
	Float32 fAvgPlanetSize = 0.0f;				// Keep an average so we can reduce the size if it seems like the RND generator is fcking up.

	const Float32 fBigPlanetThresh = 55.0f;		// If the average size becomes bigger than this, reduce planet size a little.
	const Float32 fPlanetMin = 35.0f;
	const Float32 fPlanetMax = 85.0f;
	for(Uint32 i = 0; i < m_uiNumPlanets; i++)
		{
		Uint8 u8Atmosphere = 75 + Randomi(180);
		Uint32 u8AtmosTint = 150 + Randomi(105);
		Float32 fModMax = fAvgPlanetSize > fBigPlanetThresh ? fPlanetMax * 0.5f : fPlanetMax;
		Float32 fRadius = fPlanetMin + (Float32)Randomi((Sint32)(fModMax - fPlanetMin));
		Float32 fMass   = fRadius * 650.0f;			// Should bigger planets be more massive? In game-terms I think yes. IRL no.
		Float32 fAxis   = Randomf() * PVRT_PI;
		Float32 fSpeed  = 0.5f + Randomf();
		TextureRef tex  = Textures[Randomi(gc_NumPlantTex)];
		PVRTVec3 vLoc;
		if(!FindPosition(vLoc, fRadius, 0.5f))
			return false;

		m_pPlanets[i].Initialise(vLoc, m_vLight, tex, RGBAUint(u8AtmosTint, u8AtmosTint, 255, u8Atmosphere), fRadius, fMass, fAxis, fSpeed);
		fAvgPlanetSize = fRadius / m_uiNumPlanets;
		}

	loaderProgress.SetMessage(GWSTR(enumSTRING_PositingShips));

	// --- Initialise player spaceships
	m_pSpaceships = new Spaceship[enumPLAYER_MAX];

	PVRTVec3 vPositions[enumPLAYER_MAX];

	// Player 1
	if(!FindPosition(vPositions[enumPLAYER_1], 32.0f, 0.85f))
		return false;

	// Player 2 should be on the opposite side of the screen to player 1, and we should try
	// intersect a planet so they can't just shoot straight.
	// Find the planet that is closest to the centre of the screen to intersect
	Planet* pPlanetClosest = NULL;
	Float32 fClosestDist = 999999.0f;
	for(Uint32 i = 0; i < m_uiNumPlanets; ++i)
		{
		if(m_pPlanets[i].GetPosition().lenSqr() < fClosestDist)
			pPlanetClosest = &m_pPlanets[i];
		}
	ASSERT(pPlanetClosest);

	// Intersect a ray from Player 1 through this planet to find a general position for Player 2
	// and use it as a hint to find a position.
	PVRTVec3 vRay = pPlanetClosest->GetPosition() + (pPlanetClosest->GetPosition() - vPositions[enumPLAYER_1]);
	if(!FindPosition(vPositions[enumPLAYER_2], 32.0f, 0.85f, &vRay))
		return false;

	m_pSpaceships[enumPLAYER_1].Initialise(vPositions[enumPLAYER_1], m_GameData.m_pPlayers[enumPLAYER_1]->GetColour());
	m_pSpaceships[enumPLAYER_2].Initialise(vPositions[enumPLAYER_2], m_GameData.m_pPlayers[enumPLAYER_2]->GetColour());

	// If this is a 1 player game, find the AI's angle to human player while we're here.
	if(m_GameData.m_uiNumPlayers == 1)
		{
		PVRTVec3 v1 = m_pSpaceships[PLAYER_HUMAN].GetPosition() - m_pSpaceships[PLAYER_AI].GetPosition();
		v1.normalize();
		m_fAIAngleToHuman = RadToDeg(acos(v1.dot(PVRTVec3(1,0,0))));		// Find angle to other ship
		if(m_pSpaceships[PLAYER_HUMAN].GetPosition().y < m_pSpaceships[PLAYER_AI].GetPosition().y)		// Add
			m_fAIAngleToHuman = 360.0f - m_fAIAngleToHuman;
		}

	return true;
	}

/*!***********************************************************************
 @Function		Initialise
 @Access		public
 @Returns		void
 @Description	Initialises the game with a GL context.
*************************************************************************/
void GameView::Initialise()
	{
	m_bg->GenerateTexture();

#ifdef WIN32
	const GLfloat fvAmb[]		= {0.075f, 0.075f, 0.075f, 1.0f};
	const GLfloat fvDiff[]		= {1.0f, 1.0f, 1.0f, 1.0f};
	glLightfv(GL_LIGHT0, GL_POSITION, m_vLight.ptr());
	glLightfv(GL_LIGHT0, GL_AMBIENT, fvAmb);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, fvDiff);
#endif

	SetState(enumACTION_CalcNextTurn);
	}

/*!***********************************************************************
 @Function		OnForeground
 @Access		public
 @Returns		void
 @Description
*************************************************************************/
void GameView::OnForeground()
	{
	// Start music and fade on.
#ifndef __QNX__
	m_intMusicFade.Open(0.0f, c_fGameVolLevel, enumINTERPOLATOR_Linear, 1.0f);
#endif

	AUDENG->Play(m_Music, true);

#ifdef __QNX__
	AUDENG->SetVolume(m_Music, 0.8f);
#else
	AUDENG->SetVolume(m_Music, 0.0f);

	// Pick a random part of the song to come in to between 0% and 50%
	Float32 fLen = AUDENG->GetPlayLength(m_Music);
	Float32 fRndPos = Randomf() * (fLen * 0.5f);
	AUDENG->SetPlayPosition(m_Music, fRndPos);
#endif
	}

/*!***********************************************************************
 @Function		SetState
 @Access		public
 @Returns		void
 @Param			enumSTATE eState
 @Description
*************************************************************************/
void GameView::SetState(enumACTION eAction, void* pData)
	{
	switch(eAction)
		{
		case enumACTION_CalcNextTurn:
			{
			m_nPlayerTurn++;
			if(m_nPlayerTurn >= MAX_PLAYERS)
				m_nPlayerTurn = 0;

			if(m_GameData.m_uiNumPlayers == 1 && m_nPlayerTurn == PLAYER_AI)
				{
				for(Uint32 Sims = 0; Sims < c_uiNumSimsForDifficulty[m_GameData.m_eDifficulty]; Sims++)
					{
					if(CalcAI(m_Entry))
						break;		// We're done. Solution found.

					m_LastEntry[PLAYER_AI] = m_Entry;
					}

				SetState(enumACTION_PlayerLaunchedProjectile);
				}
			else
				{
				// Construct message for input
				enumSTRING eStr = enumSTRING_Invalid;
				char szMsg[64];

				if(m_GameData.m_eInputType == enumGWINPUTTYPE_Classic)
				{
					if(m_nPlayerTurn == 0)		eStr = enumSTRING_Player1;
					else if(m_nPlayerTurn == 1) eStr = enumSTRING_Player2;

					sprintf(szMsg, "%s %s", GWSTR(eStr), GWSTR(enumSTRING_InputVel));
				}
				else
				{
					if(m_nPlayerTurn == 0)		eStr = enumSTRING_Player1TakeTurn;
					else if(m_nPlayerTurn == 1) eStr = enumSTRING_Player2TakeTurn;

					sprintf(szMsg, "%s", GWSTR(eStr));
				}

				m_Message[enumMESSAGELINE_1].Set(szMsg, -1.0f, true, true);

				// Construct 'last input' message
				if(m_LastEntry[m_nPlayerTurn].fAngle > -0.5f && m_LastEntry[m_nPlayerTurn].fPower > -0.5f)		// Initial vals are -1,-1
					{
					sprintf(szMsg, GWSTR(enumSTRING_LastEntry), m_LastEntry[m_nPlayerTurn].fAngle, m_LastEntry[m_nPlayerTurn].fPower);
					m_Message[enumMESSAGELINE_2].Set(szMsg, -1.0f, false, false);
					}

				m_Entry.Reset();
				if(m_GameData.m_eInputType == enumGWINPUTTYPE_Touch)
				{
					if(m_LastEntry[m_nPlayerTurn].fPower > 0.0f)
					{
						m_Entry.fPower = m_LastEntry[m_nPlayerTurn].fPower;
						m_Entry.fAngle = m_LastEntry[m_nPlayerTurn].fAngle;
					}
					else
					{
						PVRTVec3 vDiff = -m_pSpaceships[m_nPlayerTurn].GetPosition();
						m_Entry.fPower = 100.0f;
						m_Entry.fAngle = RadToDeg(atan2f(vDiff.x, vDiff.y));
					}
				}

				m_eState = enumSTATE_PlayerInput;
				// Request input by tweening on the keyboard
				TweenKeyboard(true);
				m_fPlayerIndTime = 0.0f;
				}

			m_fEntryBeginTime = (Float32)GetApp()->GetTicks();
			}
			break;
		case enumACTION_PlayerLaunchedProjectile:
			{
			Float32 fAngleRad = DegToRad(m_Entry.fAngle);
			PVRTVec3 vVelocity(cos(fAngleRad), sin(fAngleRad), 0.0f);
			vVelocity *= m_Entry.fPower;
			m_pSpaceships[m_nPlayerTurn].Launch(vVelocity);
			m_eState = enumSTATE_TurnInProgress;

			// Copy entries
			m_LastEntry[m_nPlayerTurn] = m_Entry;
			m_afTimesPerTurn[m_nPlayerTurn].push_back((Float32)GetApp()->GetTicks() - m_fEntryBeginTime);
			}
			break;
		case enumACTION_RoundFinished:
			{
			ASSERT(pData);

			m_eState = enumSTATE_RoundFinished;
			m_fGameEndTime = (Float32)GetApp()->GetTicks();

			Uint32 uiDestroySelf = *((Uint32*)pData);

			m_intMusicFade.Open(c_fGameVolLevel, 0.0f, enumINTERPOLATOR_Linear, 1.0f);

			// Increment active player score
			if(uiDestroySelf)		// Active player killed themself. Deduct score and set other player to rounds won++.
				{
				m_GameData.m_pPlayers[!m_nPlayerTurn]->IncrementRoundsWon();		// TODO: This can only work for 1-2 players
				m_GameData.m_pPlayers[m_nPlayerTurn]->IncrementScore(-50);			// TODO: Maybe move to config?
				}
			else
				{
				m_GameData.m_pPlayers[m_nPlayerTurn]->IncrementRoundsWon();

				// Calculate a score
				Sint32 nScore = 0;
				for(Uint32 i = 0; i < m_afTimesPerTurn[m_nPlayerTurn].size(); ++i)
					nScore += (Sint32)(m_afTimesPerTurn[m_nPlayerTurn][i]);

				Float32 fScore = (1.0f / nScore) * 1000;

				m_GameData.m_pPlayers[m_nPlayerTurn]->IncrementScore((Sint32)fScore);
				}
			}
			break;
		default:
			ASSERT(!"Unhandled action!");
			break;
		}
	}


/*!***********************************************************************
 @Function		CalcAI
 @Access		private
 @Param			PlayerEntry & entry
 @Returns		bool
 @Description	Returns true for collision success, false if not.
*************************************************************************/
bool GameView::CalcAI(PlayerEntry& entry)
	{
	const Float32 fDefaultPower = 500.0f;
	const Uint32 uiMaxIterations = c_uiCalcIterationsForDifficulty[m_GameData.m_eDifficulty];
	const Float32 fMinAngleDiff = 2.0f;
	m_fAILastResults[1] = m_fAILastResults[0];

	// Brute force AI calculation
	// Difficulty determines number of simulation iterations

	Float32 fWeight;
	Uint32 uiIterations = 0;		// Iterations try to find a better solution than the previous.
	while(uiIterations < uiMaxIterations)
		{
		// Calculate new weighting based on previous results. Weighting is used to determine the randomness of the next shot.
		Uint32 uiIdx = (uiIterations > 0 ? 1 : 0);
		fWeight = powf(1.0f - CapNorm(m_fAILastResults[uiIdx] / 100000.0f), 2);		// 1 = good. 0 = shit.

		Float32 fConeRad  = (1-fWeight) * 90.0f;
		Float32 fPowerGap = (1-fWeight) * 300.0f;
		Float32 fAng   = (m_LastEntry[PLAYER_AI].fAngle*fWeight + m_fAIAngleToHuman*(1-fWeight)) + (RandomfCentre() * fConeRad);
		Float32 fPower = (m_LastEntry[PLAYER_AI].fPower*fWeight + fDefaultPower*(1-fWeight)) + (RandomfCentre() * fPowerGap);

		// Add or subtract the minimum angle per turn.
		if(fabs(fAng - m_LastEntry[PLAYER_AI].fAngle) < fMinAngleDiff)
			fAng += (fAng < m_LastEntry[PLAYER_AI].fAngle ? -fMinAngleDiff : fMinAngleDiff);

		entry.fAngle = fAng;
		entry.fPower = fPower;

		// Simulate this shot to record the result.
		Float32 fAngleRad = DegToRad(m_Entry.fAngle);
		PVRTVec3 vVelocity(cos(fAngleRad), sin(fAngleRad), 0.0f);
		vVelocity *= m_Entry.fPower;
		m_fAILastResults[0] = SimulatePhysics(vVelocity);

		if(m_fAILastResults[0] < 1.0f)		// Hit the human!
			{
			DebugLog("Simulation succeeded!");
			return true;
			}


		if(m_fAILastResults[0] > m_fAILastResults[1])			// Previous shot was better! Don't want to get worse!
			{
			uiIterations++;
			continue;
			}
		else
			break;
		}

	DebugLog("AI Simulation result: %f. Weight: %f. Iterations: %d", m_fAILastResults[0], fWeight, uiIterations);
	return false;
	}

/*!***********************************************************************
 @Function		Render
 @Access		public
 @Returns		void
 @Description
*************************************************************************/
void GameView::Render()
	{
	m_bg->Render();

	Uint32 i;
	for(i = 0; i < m_uiNumPlanets; i++)
		m_pPlanets[i].DrawGeom();

	for(i = 0; i < enumPLAYER_MAX; ++i)
		m_pSpaceships[i].Render();

	// Render player indicator
	if(m_fPlayerIndTime < 3.0f)
		{
		const Float32 fTimeout = 2.5f;
		Float32 fIndScale = EaseTo(1.0f, 0.0f, (m_fPlayerIndTime > fTimeout ? 1-(m_fPlayerIndTime - fTimeout)*2 : m_fPlayerIndTime * 0.75f));
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		Shader::Use(m_2DShader);
		PVRTMat4 mxMVP = m_cam.GetProjection() * m_cam.GetView();
		glUniformMatrix4fv(m_2DShader->GetUniform(c_uiMVP), 1, GL_FALSE, mxMVP.f);
		PVRTMat4 mxTrans = PVRTMat4::Translation(m_pSpaceships[m_nPlayerTurn].GetPosition()) * PVRTMat4::RotationZ(m_fPlayerIndTime) * PVRTMat4::Scale(fIndScale+1, fIndScale+1, 1.0f);
		GFX->RenderQuad(m_texPlayerIndicator, mxTrans, RGBA(1,1,1,1-fIndScale));
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_BLEND);
		}

#ifdef INPUT_PRECALC_TEST
	Shader::Use(m_VCShader);
	PVRTMat4 mxMVP = m_cam.GetProjection() * m_cam.GetView();
	glUniformMatrix4fv(m_VCShader->GetUniform(c_uiMVP), 1, GL_FALSE, mxMVP.f);
	GFX->RenderPrimitiveList(m_debugtris.Pointer(), m_debugcount, FLAG_VRT|FLAG_RGB);
#endif

	// Render HUD
	RenderHUD();
	}

/*!***********************************************************************
 @Function		RenderHUD
 @Access		private
 @Returns		void
 @Description
*************************************************************************/
void GameView::RenderHUD()
{
	if(m_eState == enumSTATE_PlayerInput || m_intKeyboard.IsActive())
	{
		if(m_GameData.m_eInputType == enumGWINPUTTYPE_Classic)
		{
			RenderClassicHUD();
		}
		else
		{
			RenderTouchHUD();
		}
	}

    if(m_bShowQuitMenu)
    {
        Float32 fHudW, fHudH;
        if(GFX->IsRotated())
        {
            fHudW = (Float32)GFX->GetDeviceHeight();
            fHudH = (Float32)GFX->GetDeviceWidth();
        }
        else
        {
            fHudW = (Float32)GFX->GetDeviceWidth();
            fHudH = (Float32)GFX->GetDeviceHeight();
        }

        PVRTMat4 mxProj = PVRTMat4::Ortho(-fHudW/2, fHudH/2, fHudW/2, -fHudH/2, -100.0f, 100.0f, PVRTMat4::OGL, GFX->IsRotated());

        Shader::Use(m_VCShader);
        glUniformMatrix4fv(m_VCShader->GetUniform(c_uiMVP), 1, GL_FALSE, mxProj.f);

        // Render vector indicator
        glEnable(GL_BLEND);
	    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        Rectanglef rect;
        rect.m_fX = -150.0f;
        rect.m_fW = 150.0f;
        rect.m_fY = -100.0f;
        rect.m_fH = 120.0f;

        PVRTMat4 mxTrans = PVRTMat4::Identity();
        GFX->RenderQuad(rect, mxTrans, FLAG_VRT|FLAG_RGB, 0xDD000000);

        Shader::Use(m_2DShader);
        glUniformMatrix4fv(m_2DShader->GetUniform(c_uiMVP), 1, GL_FALSE, mxProj.f);

        m_HUDFont->RenderString("Quit Game?", 0.0f, 80.0f, enumTEXTJUSTIFY_Centre, 1.0f, -2.0f);
        m_HUDFont->RenderString("> Yes", 0.0f,  10.0f, enumTEXTJUSTIFY_Centre, 1.0f, -2.0f);
        m_HUDFont->RenderString("> No",  0.0f, -50.0f, enumTEXTJUSTIFY_Centre, 1.0f, -2.0f);
        Shader::Use(0);
    }

	if((m_eState == enumSTATE_RoundFinished || m_eState == enumSTATE_GameQuit) && GetApp()->GetTicks() - m_fGameEndTime > 4.0f)
	{
		Float32 fTime = (Float32)GetApp()->GetTicks() - m_fGameEndTime - 4.0f;
		// Fade a black quad over the screen as a fade out
		glEnable(GL_BLEND);
		GraphicsSystem* pGFX = GetApp()->GetGraphicsSystem();
		Rectanglef quad(0, 0, (Float32)pGFX->GetDeviceWidth(), (Float32)pGFX->GetDeviceHeight());
		RGBA rgba(0,0,0,fTime);
		Shader::Use(m_VCShader);
		PVRTMat4 mxMVP = m_camHUD.GetProjection() * m_camHUD.GetView();
		glUniformMatrix4fv(m_2DShader->GetUniform(c_uiMVP), 1, GL_FALSE, mxMVP.f);
		pGFX->RenderQuad(quad, PVRTMat4::Identity(), FLAG_VRT | FLAG_RGB, rgba);
		Shader::Use(0);
		glDisable(GL_BLEND);
	}
}

/*!***********************************************************************
 @Function		RenderClassicHUD
 @Access		private
 @Returns		void
 @Description
 *************************************************************************/
void GameView::RenderClassicHUD()
{
	Shader::Use(m_2DShader);
	PVRTMat4 mxMVP = m_camHUD.GetProjection() * m_camHUD.GetView();
	glUniformMatrix4fv(m_2DShader->GetUniform(c_uiMVP), 1, GL_FALSE, mxMVP.f);

	if(m_Message[enumMESSAGELINE_1].bDisplay)
	{
		Float32 fScale = GFX->GetDeviceResolution() == enumDEVRES_HVGA ? 0.5f : 1.0f;
		m_FontBig->RenderString(m_Message[enumMESSAGELINE_1].szMsgToRender, 0, (Sint32)(GFX->GetDeviceHeight() - 36*fScale), enumTEXTJUSTIFY_Left, 1.0f, -2.0f*fScale);
	}
	if(m_Message[enumMESSAGELINE_2].bDisplay)
	{
		Float32 fScale = GFX->GetDeviceResolution() == enumDEVRES_HVGA ? 0.6f : 1.0f;
		m_FontSmall->RenderString(m_Message[enumMESSAGELINE_2].szMsgToRender, 46*fScale, (Sint32)(GFX->GetDeviceHeight() - 60*fScale), enumTEXTJUSTIFY_Left, fScale, -2.0f*fScale);
	}

	// Render a keyboard
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Render each key
	Rectanglef rectButton(0, 0, m_fButtonSize, m_fButtonSize);
	PVRTMat4 mxTrans;
	PVRTVec2 vUVs[2];
	const Float32 fWRecip = 1.0f / m_texKeyboardUp->GetWidth();
	const Float32 fHRecip = 1.0f / m_texKeyboardUp->GetHeight();
	const Float32 fYPos = m_intKeyboard.Value();

	for(Uint32 i = 0; i < 12; ++i)
	{
		TextureRef tex;
		if(m_ui16KeyMask & (1<<i))	tex = m_texKeyboardDown;
		else						tex = m_texKeyboardUp;

		mxTrans = PVRTMat4::Translation(i*m_fButtonSize, fYPos, 0.0f);
		vUVs[0] = PVRTVec2(((i+0)*m_fButtonSize)*fWRecip, 0.0f);
		vUVs[1] = PVRTVec2(((i+1)*m_fButtonSize)*fWRecip, m_fButtonSize*fHRecip);

		glBindTexture(GL_TEXTURE_2D, tex->GetHandle());
		GFX->RenderQuad(rectButton, mxTrans, FLAG_VRT | FLAG_TEX0 | FLAG_RGB, 0xFFFFFFFF, vUVs);
	}
	glDisable(GL_BLEND);
}

/*!***********************************************************************
 @Function		RenderTouchHUD
 @Access		private
 @Returns		void
 @Description
 *************************************************************************/
void GameView::RenderTouchHUD()
{
	Shader::Use(m_2DShader);
	PVRTMat4 mxMVP = m_camHUD.GetProjection() * m_camHUD.GetView();
	glUniformMatrix4fv(m_2DShader->GetUniform(c_uiMVP), 1, GL_FALSE, mxMVP.f);

	if(m_Message[enumMESSAGELINE_1].bDisplay)
	{
		Float32 fScale = GFX->GetDeviceResolution() == enumDEVRES_HVGA ? 0.5f : 1.0f;
		m_FontBig->RenderString(m_Message[enumMESSAGELINE_1].szMsgToRender, 0, (Sint32)(GFX->GetDeviceHeight() - 36*fScale), enumTEXTJUSTIFY_Left, 1.0f, -2.0f*fScale);
	}
	if(m_Message[enumMESSAGELINE_2].bDisplay)
	{
		Float32 fScale = GFX->GetDeviceResolution() == enumDEVRES_HVGA ? 0.6f : 1.0f;
		m_FontSmall->RenderString(m_Message[enumMESSAGELINE_2].szMsgToRender, 46*fScale, (Sint32)(GFX->GetDeviceHeight() - 60*fScale), enumTEXTJUSTIFY_Left, fScale, -2.0f*fScale);
	}

	// Render vector indicator
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	mxMVP = m_cam.GetProjection() * m_cam.GetView();
	glUniformMatrix4fv(m_2DShader->GetUniform(c_uiMVP), 1, GL_FALSE, mxMVP.f);

	PVRTVec3 vPos;
	float fAngleRad = DegToRad(m_Entry.fAngle);
	vPos.x = cos(fAngleRad) * m_Entry.fPower;
	vPos.y = sin(fAngleRad) * m_Entry.fPower;
	vPos.z = 0.0f;

	PVRTMat4 mxTrans = PVRTMat4::Translation(vPos) *
	PVRTMat4::Translation(m_pSpaceships[m_nPlayerTurn].GetPosition()) *
	PVRTMat4::RotationZ(m_fPlayerIndTime) *
	PVRTMat4::Scale(0.5f, 0.5f, 1.0f);

	GFX->RenderQuad(m_texPlayerIndicator, mxTrans, RGBA(1,1,1,1));
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_BLEND);

	// --- Render interconnecting line
	float fLineWidth = 1.0f / (m_Entry.fPower / MAX_POWER);

	// BB is in world space. Translate vPos.
	vPos += m_pSpaceships[m_nPlayerTurn].GetPosition();
	PVRTVec2 vStartPos = m_pSpaceships[m_nPlayerTurn].GetBoundingBox().ClosestPointTo(PVRTVec2(vPos.x, vPos.y)) - m_pSpaceships[m_nPlayerTurn].GetPosition();

	Rectanglef rect;
	rect.m_fX = -(fLineWidth/2);
	rect.m_fW = +(fLineWidth/2);
	rect.m_fY = vStartPos.length();
	rect.m_fH = m_Entry.fPower-TOUCH_IND_RAD;

	mxTrans = PVRTMat4::Translation(m_pSpaceships[m_nPlayerTurn].GetPosition()) *
	PVRTMat4::RotationZ(DegToRad(m_Entry.fAngle)-PVRT_PI_OVER_TWO);

	GFX->RenderQuad(rect, mxTrans, FLAG_VRT|FLAG_RGB, 0xFF00FF00);
}

/*!***********************************************************************
 @Function		Update
 @Access		public
 @Param			double fDT
 @Returns		void
 @Description
*************************************************************************/
void GameView::Update(double fDT)
	{
	// Adjust BG music
#ifndef __QNX__
	if(m_intMusicFade.IsActive())
		{
		m_intMusicFade.Update(fDT);
		AUDENG->SetVolume(m_Music, m_intMusicFade.Value());
		}
#endif

	// Check timeout
	if(m_eState == enumSTATE_RoundFinished && GetApp()->GetTicks() - m_fGameEndTime > 5.0f)		// 5 second timeout
		{
		m_GameData.m_uiRoundsPlayed++;
		if(m_GameData.m_uiRoundsPlayed >= m_GameData.m_uiRounds)
			return VIEWMAN->ShowView("GameFinished", &m_GameData);
		else
			return VIEWMAN->ShowView("GameLoader", &m_GameData);
		}
    else if(m_eState == enumSTATE_GameQuit && GetApp()->GetTicks() - m_fGameEndTime > 5.0f)
    {
        return VIEWMAN->ShowView("Menu");
    }

    if(m_bShowQuitMenu)
        return;

	for(Uint32 i = 0; i < m_uiNumPlanets; i++)
		m_pPlanets[i].Update(fDT);

	for(Uint32 i = 0; i < MAX_PLAYERS; i++)
		m_pSpaceships[i].Update(fDT);

	// Rotate the player indicator
	m_fPlayerIndTime += (Float32)fDT;

	// Interpolate back view to 0, 0
	if(m_bSpringBack)
		{
		m_fSpringPerc += 1.0f * (float)fDT;

		PVRTVec3 vIntPos;
		vIntPos.x = EaseTo(m_vSpringFrom.x, 0.0f, m_fSpringPerc);
		vIntPos.y = EaseTo(m_vSpringFrom.y, 0.0f, m_fSpringPerc);
		vIntPos.z = 0.0f;

		if(FloatIsEqual(vIntPos.x, 0.0f) && FloatIsEqual(vIntPos.y, 0.0f))
			m_bSpringBack = false;		// Done

		m_cam.SetPosition(vIntPos);
		m_bg->SetView(vIntPos);
		}

	// Interpolate keyboard
	if(m_intKeyboard.IsActive())
		m_intKeyboard.Update(fDT);

	Projectile* pProjectile = m_pSpaceships[m_nPlayerTurn].GetProjectile();
	Uint32 uiResult = 0;
	// Update both projectiles, but we only care about the active player's result
	for(Sint32 i = 0; i < enumPLAYER_MAX; ++i)
		{
		Uint32 uiRes = m_pSpaceships[i].GetProjectile()->Update(fDT, m_pPlanets, m_uiNumPlanets, m_pSpaceships, MAX_PLAYERS);
		if(i == m_nPlayerTurn)
			uiResult = uiRes;
		}

	if(uiResult & Projectile::PROJRESULT_HITPLANET)
		{
		// What planet was hit?
		Uint32 uiPlanetID = (uiResult >> 16) & 0xF;
		Uint32 uiAngle    = (uiResult >> 20) & 0xFFF;

		// TODO: Do an explosion or summat.
		m_pPlanets[uiPlanetID].Hit(uiAngle);
		pProjectile->Hit();
		}
	else if(uiResult & Projectile::PROJRESULT_EXPIRED)
		{
		// Dissipate
		pProjectile->Dissipate();
		}
	else if(uiResult & Projectile::PROJRESULT_HITSPACESHIP)
		{
		// Who was hit?
		Uint32 uiPlayerID = (uiResult >> 16) & 0xFFFF;
		m_pSpaceships[uiPlayerID].Hit();
		pProjectile->Hit();

		Uint32 uiDestroyedSelf = (uiPlayerID == m_nPlayerTurn ? 1 : 0);

		SetState(enumACTION_RoundFinished, &uiDestroyedSelf);
		return;
		}

	if(uiResult != Projectile::PROJRESULT_MISS && uiResult != Projectile::PROJRESULT_INVALID)
		{
		// Switch turns
		SetState(enumACTION_CalcNextTurn);
		}

	for(Uint32 uiMsg = 0; uiMsg < enumMESSAGELINE_MAX; uiMsg++)
		if(m_Message[uiMsg].bDisplay)
			m_Message[uiMsg].Update((Float32)fDT);
	}


/*!***********************************************************************
 @Function		FindPosition
 @Access		private
 @Param			PVRTVec3 & vPosOut
 @Param			Float32 fRadius
 @Param			Float32 fWeight
 @Param			PVRTVec3 * vHint
 @Returns		void
 @Description
*************************************************************************/
bool GameView::FindPosition(PVRTVec3& vPosOut, Float32 fRadius, Float32 fWeight, PVRTVec3* vHint)
	{
	bool bFound = false;
	Float32 fSpacing = 50.0f;
	Float32 fHintDist = 1.5f;
	Float32 fMinHintDist = 0.4f;
	Uint32 uiItCount = 0;
	while(!bFound)
		{
		if(vHint)		// We were provided with a hint where to place the object. See if it will fit.
			vPosOut = *vHint * (fMinHintDist + Randomf() * fHintDist);
		else
			vPosOut = PVRTVec3(RandomfCentre() * m_fViewCoordX, RandomfCentre() * m_fViewCoordY, 0.0f) * fWeight;

		bFound = true;		// Assume we've found it

		if(uiItCount > 100)
			{
			fSpacing = fSpacing - 10.0f;		// Too many iterations. Reduce the spacing. Don't want to end up in an infinite loop.
			if(fSpacing < 0.0f)			// Fuck this. Bail.
				return false;

			DebugLog("WARNING: FindPosition() Iteration count exceeded 100. Reducing gap to %f", fSpacing);
			uiItCount = 0;
			}


		// Check other planets position
		PVRTVec3 vTest;
		Float32 fTestRadius = 0;
		for(Uint32 i = 0; i < m_uiNumPlanets; i++)
			{
			// Even if a planet isn't initialised this will still work as Get() funcs return 0.
			vTest = m_pPlanets[i].GetPosition() - vPosOut;
			fTestRadius = m_pPlanets[i].GetRadius();
			if(FloatIsEqual(fTestRadius, 0.0f))		// Bleh, this planet isn't initialised yet.
				continue;

			Float32 fDist = vTest.length();
			Float32 fMinDistance = fRadius + fTestRadius + fSpacing;
			if(fDist < fMinDistance)
				{
				bFound = false;
				break;		// Start again
				}
			}

		// Check screen edges
		if(   vPosOut.x + fRadius > m_fViewCoordX || vPosOut.x - fRadius < -m_fViewCoordX
		   || vPosOut.y + fRadius > m_fViewCoordY || vPosOut.y - fRadius < -m_fViewCoordY)
			bFound = false;

		uiItCount++;
		}

	return true;
	}

/*!***********************************************************************
 @Function		TouchedKeyboard
 @Access		private
 @Returns		Sint32
 @Description	Gets the value of the touch, or -1 if it's out of bounds.
*************************************************************************/
Sint32 GameView::TouchedKeyboard(const Touch& c_Touch)
	{
	// Touch is in device coords
	const Float32 fYPos = GFX->GetDeviceHeight() - (m_fKeyboardY * (GFX->GetDeviceHeight() / 320.0f));
	if(c_Touch.fY > fYPos - m_fButtonSize && c_Touch.fY < fYPos)
		{
		Sint32 nIndex = (Sint32)(c_Touch.fX / m_fButtonSize);
		return nIndex;
		}

	return -1;
	}

/*!***********************************************************************
 @Function		InputComplete
 @Access		private
 @Param			Float32 fInput
 @Returns		void
 @Description
*************************************************************************/
void GameView::InputComplete(Float32 fInput)
	{
	// User has given us their input. Perform an action depending on the state of the game.
	if(m_Entry.fAngle < 0.0f)
		{
		// Set the value
		m_Entry.fAngle = fInput;

		// Reset 'Current'
		m_Entry.szCurrent[0] = 0;

		// Switch to power
		enumSTRING eStr = enumSTRING_Invalid;
		if(m_nPlayerTurn == 0)		eStr = enumSTRING_Player1;
		else if(m_nPlayerTurn == 1) eStr = enumSTRING_Player2;
		char szMsg[64];
		sprintf(szMsg, "%s %s", GWSTR(eStr), GWSTR(enumSTRING_InputPwr));
		m_Message[enumMESSAGELINE_1].Set(szMsg, -1.0f, true, true);
		}
	else
		{
		m_Entry.fPower = fInput;
		// Fire!
		SetState(enumACTION_PlayerLaunchedProjectile);
		TweenKeyboard(false);
		m_Message[enumMESSAGELINE_1].bDisplay = false;
		m_Message[enumMESSAGELINE_2].bDisplay = false;
		}
	}

/*!***********************************************************************
 @Function		OnTouchDown
 @Access		public
 @Param			Touch * pTouches
 @Param			Uint32 uiNum
 @Returns		void
 @Description
*************************************************************************/
void GameView::OnTouchDown(Touch* pTouches, Uint32 uiNum)
{
	// Calculate the game coords
	PVRTVec3 vTouchPos;
	vTouchPos.x = ((pTouches[0].fX / (Float32)GFX->GetDeviceWidth())  * 2.0f) - 1.0f;
	vTouchPos.y = ((pTouches[0].fY / (Float32)GFX->GetDeviceHeight()) * 2.0f) - 1.0f;
	vTouchPos.x *= m_fViewCoordX;
	vTouchPos.y *= m_fViewCoordY;
	vTouchPos.y *= -1.0f;
	vTouchPos.z  = 0.0f;

#ifdef DEBUG_INPUT
	// Work out velocity based on touch - ship pos						// TODO: THIS HAS TO BE REMOVED (DEBUG PURPOSES ONLY)
	PVRTVec3 vVelocity = vTouchPos - m_pSpaceships[m_nPlayerTurn].GetPosition();
	m_pSpaceships[m_nPlayerTurn].Launch(vVelocity);
#endif

	m_bValidTouchControlDown = false;
	bool bHandled			 = false;
    if(m_bShowQuitMenu)
    {
        Rectanglef yesBtn(  GFX->GetDeviceWidth()/2 - 75.0,
                            GFX->GetDeviceHeight()/2 - 30.0,
                            GFX->GetDeviceWidth()/2 + 75.0,
                            GFX->GetDeviceHeight()/2 + 30.0);
        Rectanglef noBtn(  GFX->GetDeviceWidth()/2 - 75.0,
                            GFX->GetDeviceHeight()/2 + 35.0,
                            GFX->GetDeviceWidth()/2 + 75.0,
                            GFX->GetDeviceHeight()/2 + 90.0);
        if(yesBtn.Contains(PVRTVec2(pTouches[0].fX, pTouches[0].fY)))
        {
            m_eState = enumSTATE_GameQuit;
            m_fGameEndTime = (Float32)GetApp()->GetTicks() - 4.0f; // Dirty hack :)
            m_intMusicFade.Open(c_fGameVolLevel, 0.0f, enumINTERPOLATOR_Linear, 1.0f);
        }
        else if(noBtn.Contains(PVRTVec2(pTouches[0].fX, pTouches[0].fY)))
        {
            m_bShowQuitMenu = false;
        }
    }
	else if(m_GameData.m_eInputType == enumGWINPUTTYPE_Classic)
	{
		// Check keyboard
		if(m_eState == enumSTATE_PlayerInput)
		{
			Sint32 nResult = TouchedKeyboard(pTouches[0]);
			if(nResult >= 0)
			{
				AUDENG->Play(m_InputSfx, false);

				bHandled = true;
				m_ui16KeyMask = (1<<nResult);
				if(nResult < 10 && strlen(m_Entry.szCurrent) < 3)
				{
					Sint32 nIndex = nResult;
					nIndex += 1;
					nIndex %= 10;
					char szInput[2];	szInput[0] = 0;
					sprintf(szInput, "%d", nIndex);
					strcat(m_Entry.szCurrent, szInput);		// Append to the 'current' string
					m_Message[enumMESSAGELINE_1].Append(szInput);					// Append to the viewable string
				}
				else if(nResult == 10 && m_Entry.szCurrent[0] != 0)			// Backspace
				{
					m_Entry.szCurrent[strlen(m_Entry.szCurrent)-1] = 0;
					m_Message[enumMESSAGELINE_1].RemoveLast();
				}
				else if(nResult == 11)						// Enter
				{
					// Figure out what this was supposed to be
					Float32 fVal = (Float32)atof(m_Entry.szCurrent);
					InputComplete(fVal);
				}
			}
		}
	}
	else
	{
		// Detect if the touch down was with inside the indicator bounds
		PVRTVec3 vPos;
		float fAngleRad = DegToRad(m_Entry.fAngle);
		vPos.x = cos(fAngleRad) * m_Entry.fPower;
		vPos.y = sin(fAngleRad) * m_Entry.fPower;
		vPos.z = 0.0f;
		vPos = vPos + m_pSpaceships[m_nPlayerTurn].GetPosition();

		if((vTouchPos - vPos).length() < TOUCH_IND_RAD)
			m_bValidTouchControlDown = true;
	}

	if(!bHandled)
		m_vTouchLast = vTouchPos;
}

/*!***********************************************************************
 @Function		OnTouchMoved
 @Access		public
 @Param			Touch * pTouches
 @Param			Uint32 uiNum
 @Returns		void
 @Description
*************************************************************************/
void GameView::OnTouchMoved(Touch* pTouches, Uint32 uiNum)
	{
	if(m_ui16KeyMask)		// Bail if there's a key down.
		return;

	PVRTVec3 vTouchPos, vTouchDiff;
	vTouchPos.x = ((pTouches[0].fX / (Float32)GFX->GetDeviceWidth())  * 2.0f) - 1.0f;
	vTouchPos.y = ((pTouches[0].fY / (Float32)GFX->GetDeviceHeight()) * 2.0f) - 1.0f;
	vTouchPos.x *= m_fViewCoordX;
	vTouchPos.y *= m_fViewCoordY;
	vTouchPos.y *= -1.0f;
	vTouchPos.z = 0.0f;

	// Move the touch control
	if(m_bValidTouchControlDown)
	{
		PVRTVec3 vVelocity = vTouchPos - m_pSpaceships[m_nPlayerTurn].GetPosition();
		float fPower       = vVelocity.length();
		if(fPower < MIN_POWER) fPower = MIN_POWER;
		if(fPower > MAX_POWER) fPower = MAX_POWER;
		vVelocity.normalize();

		float fAngle = atan2f(vVelocity.y, vVelocity.x);
		m_Entry.fAngle = RadToDeg(fAngle);
		m_Entry.fPower = fPower;
	}
	else
	{
		vTouchDiff = vTouchPos - m_vTouchLast;
		m_vTouchLast = vTouchPos;

		if(m_bSpringBack)		// Don't want to translate if we're interpolating back.
			return;

		Uint32 uiTransResult = m_bg->TranslateView(vTouchDiff);
		if(uiTransResult & StarfieldGenerator::TRANSRETURN_NOX)
			vTouchDiff.x = 0.0f;
		if(uiTransResult & StarfieldGenerator::TRANSRETURN_NOY)
			vTouchDiff.y = 0.0f;

		m_cam.Translate(vTouchDiff);
	}

#ifdef INPUT_PRECALC_TEST
	// Work out velocity based on touch - ship pos						// TODO: THIS HAS TO BE REMOVED (DEBUG PURPOSES ONLY)
	PVRTVec3 vVelocity = vTouchPos - m_pSpaceships[m_nPlayerTurn].GetPosition();
	float fPower       = vVelocity.length();
	if(fPower < MIN_POWER) fPower  = MIN_POWER;
	if(fPower > MAX_POWER) fPower = MAX_POWER;
	vVelocity.normalize();

	// Remap range. 60 - 250 = 0.0 - 1.0
	fPower -= MIN_POWER;
	fPower /= MAX_POWER-MIN_POWER;
	fPower *= 1000.0f;

	vVelocity *= fPower;
	SimulatePhysics(vVelocity);
#endif
	}

/*!***********************************************************************
 @Function		OnTouchUp
 @Access		public
 @Param			Touch * pTouches
 @Param			Uint32 uiNum
 @Returns		void
 @Description
*************************************************************************/
void GameView::OnTouchUp(Touch* pTouches, Uint32 uiNum)
	{
	Uint16 ui16PrevKeyMask = m_ui16KeyMask;
	m_ui16KeyMask = 0;

	m_bValidTouchControlDown = false;

	if(ui16PrevKeyMask)
		return;

	if(m_bSpringBack)
		return;

	m_vTouchLast.x = m_vTouchLast.y = m_vTouchLast.z = 0.0f;

	m_bSpringBack = true;
	m_fSpringPerc = 0.0f;

	m_vSpringFrom.x = m_cam.GetView().f[12];
	m_vSpringFrom.y = m_cam.GetView().f[13];
	m_vSpringFrom.z = 0.0f;
	m_vSpringFromLast = m_vSpringFrom;
	}

/*!***********************************************************************
 @Function		OnKeyPress
 @Access		protected
 @Param			key
 @Returns		void
 @Description
*************************************************************************/
void GameView::OnKeyPress(Uint32 key)
{
    switch(key)
    {
        case App::enumKEY_Back:
            m_bShowQuitMenu = !m_bShowQuitMenu;
            break;
    }
}

/*!***********************************************************************
 @Function		TweenKeyboard
 @Access		private
 @Param			bool bOn
 @Returns		void
 @Description
*************************************************************************/
void GameView::TweenKeyboard(bool bOn)
	{
	Float32 fDevScale = GFX->GetDeviceHeight() / 320.0f;
	if(bOn)
		{
		m_intKeyboard.Open(-m_fButtonSize, m_fKeyboardY*fDevScale, enumINTERPOLATOR_QuadraticEaseOut, 0.5f);
		}
	else
		{
		m_intKeyboard.Open(m_fKeyboardY*fDevScale, -m_fButtonSize, enumINTERPOLATOR_QuadraticEaseOut, 0.5f);
		}
	}


/*!***********************************************************************
 @Function		LoaderFunctor
 @Access		public
 @Returns
 @Description
*************************************************************************/
GameView::LoaderFunctor::LoaderFunctor()
	{
	m_szMessage[0] = 0;
	}

/*!***********************************************************************
 @Function		SetMessage
 @Access		public
 @Param			const TXChar * pszMessage
 @Returns		void
 @Description
*************************************************************************/
void GameView::LoaderFunctor::SetMessage(const TXChar* pszMessage)
	{
	m_Mutex.Lock();
	strcpy(m_szMessage, pszMessage);
	m_Mutex.Release();
	}

/*!***********************************************************************
 @Function		GetMessage
 @Access		public
 @Returns		TXChar*
 @Description
*************************************************************************/
void GameView::LoaderFunctor::GetMessage(TXChar* pszMessage, Uint32 uiSize)
	{
	m_Mutex.Lock();
	strncpy(pszMessage, m_szMessage, uiSize);
	m_Mutex.Release();
	}


/*!***********************************************************************
 @Function		Message
 @Access		public
 @Returns
 @Description
*************************************************************************/
GameView::SMessage::SMessage()
	{
	bDisplay  = false;
	bInsertion = true;
	bCaret     = true;
	}

/*!***********************************************************************
 @Function		Append
 @Access		public
 @Param			const char * c_pszMsg
 @Returns		void
 @Description
*************************************************************************/
void GameView::SMessage::Append(const char* c_pszMsg)
	{
	ASSERT(c_pszMsg);
	Uint32 uiThis = strlen(c_pszMsg);
	uiMsgLen += uiThis;
	strcat(szMessage, c_pszMsg);
	}

/*!***********************************************************************
 @Function		RemoveLast
 @Access		public
 @Returns		void
 @Description
*************************************************************************/
void GameView::SMessage::RemoveLast()
	{
	uiMsgLen--;
	szMessage[uiMsgLen] = 0;
	if(uiDisplayCount > uiMsgLen)
		{
		uiDisplayCount--;
		szMsgToRender[uiMsgLen] = 0;
		}
	}

/*!***********************************************************************
 @Function		Set
 @Access		public
 @Param			const char * c_pszMsg
 @Param			Float32 fUpTime
 @Returns		void
 @Description
*************************************************************************/
void GameView::SMessage::Set(const char* c_pszMsg, Float32 fUpTime, bool bShowCaret, bool bShowInsertionMarker)
	{
	ASSERT(c_pszMsg);
	uiMsgLen = strlen(c_pszMsg) + 2;
	ASSERT(uiMsgLen > 0);
	sprintf(szMessage, "%s%s", (bShowInsertionMarker ? "> " : ""), c_pszMsg);
	szMsgToRender[0]	= 0;
	uiDisplayCount		= 0;
	fCurrUpTime			= 0;
	fMaxUpTime			= fUpTime;
	bDisplay			= true;
	fCharDelta			= ((fUpTime > 0.0f ? fUpTime : 3.0f) * 0.25f) / uiMsgLen;
	fCharTick			= 0;
	bCaret				= bShowCaret;
	bInsertion			= bShowInsertionMarker;
	}

/*!***********************************************************************
 @Function		Update
 @Access		public
 @Param			Float32 fDT
 @Returns		void
 @Description
*************************************************************************/
void GameView::SMessage::Update(Float32 fDT)
	{
	fCurrUpTime += fDT;
	if(fMaxUpTime > 0.0f && fCurrUpTime >= fMaxUpTime)
		bDisplay = false;

	if(fCurrUpTime > uiDisplayCount * fCharDelta && uiDisplayCount < uiMsgLen)
		{
		uiDisplayCount++;
		strncpy(szMsgToRender, szMessage, uiDisplayCount);
		szMsgToRender[uiDisplayCount] = 0;
		}

	if(uiDisplayCount == uiMsgLen && bCaret)
		{
		if(fCurrUpTime - 0.5f > floorf(fCurrUpTime))
			{
			szMsgToRender[uiDisplayCount]   = '_';
			szMsgToRender[uiDisplayCount+1] = 0;
			}
		else
			{
			szMsgToRender[uiDisplayCount]   = 0;
			}
		}
	}

void PrecalcPhysics(double fDT, Planet* pPlanets, Uint32 uiNumPlanets, PVRTVec3& vVel, PVRTVec3& vPos)
	{
	// Calculate acceleration
	PVRTVec3 vAccel = PVRTVec3(0,0,0);
	for(Uint32 i = 0; i < uiNumPlanets; ++i)
		{
		Float32 fPlanetDistance = PVRTVec3(pPlanets[i].GetPosition() - vPos).length();
		Float32 fPlanetForce = pPlanets[i].GetMass() / powf(fPlanetDistance, 2);
		vAccel += (pPlanets[i].GetPosition() - vPos) * fPlanetForce;
		}

	// Calculate velocity
	vVel = vVel + vAccel * (Float32)fDT;													// v0 = v1 + a*dt

	// Calculate new position
	PVRTVec3 vDist = (vVel * (Float32)fDT) + (0.5f * vAccel * powf((Float32)fDT, 2));		// d = v*t + 1/2at^2
	vPos = vPos + vDist;																	// s1 = s0 + d
	}

/*!***********************************************************************
 @Function		CalcAI
 @Access		private
 @Param			PlayerEntry & entry
 @Returns		void
 @Description
*************************************************************************/
Float32 GameView::SimulatePhysics(PVRTVec3& vVel)
	{
	PVRTVec3 vLaunchDir = m_pSpaceships[m_nPlayerTurn].GetPosition() + vVel;	// Code for when we remove the touch.
	PVRTVec2 v2Pos = m_pSpaceships[m_nPlayerTurn].GetBoundingBox().ClosestPointTo(PVRTVec2(vLaunchDir.x, vLaunchDir.y));
	PVRTVec3 vPos = PVRTVec3(v2Pos.x, v2Pos.y, 0.0f);

	Float32 fGameX = GFX->GetVirtualWidth()  * 0.5f;
	Float32 fGameY = GFX->GetVirtualHeight() * 0.5f;

	static int nResult = 0;
	int thisresult = 0;
#ifdef INPUT_PRECALC_TEST
	m_debugcount = 0;
#endif

	Float32 fClosestDist = 9999999.f;
	for(Uint32 i = 0; i < SIMULATE_STEPS; ++i)
		{
		PrecalcPhysics(1/60.0f, m_pPlanets, m_uiNumPlanets, vVel, vPos);

		// Check screen edges
		if(vPos.x > fGameX || vPos.y < -fGameX || vPos.y > fGameY || vPos.y < -fGameX)
			thisresult = 1;

		// Check planets
		Uint32 j;
		for(j = 0; j < m_uiNumPlanets; ++j)
			{
			Float32 fDistSqrd = PVRTVec3(vPos - m_pPlanets[j].GetPosition()).lenSqr();
			if(fDistSqrd < powf(m_pPlanets[j].GetRadius(), 2.0f))
				{
				thisresult = 2;
				break;
				}
			}

		if(thisresult)
			break;

		// Check other ship
		Float32 fDist = PVRTVec3(m_pSpaceships[PLAYER_HUMAN].GetPosition() - vPos).lenSqr();
		if(fDist < fClosestDist)
			fClosestDist = fDist;
		if(m_pSpaceships[PLAYER_HUMAN].GetBoundingBox().Contains(PVRTVec2(vPos.x, vPos.y)))		// Quick test
			{
			// Ok, now test the collision bitmap for proper collisions
			PVRTVec2 vLocalPoint = PVRTVec2(vPos.x, vPos.y) - PVRTVec2(m_pSpaceships[PLAYER_HUMAN].GetPosition().x, m_pSpaceships[PLAYER_HUMAN].GetPosition().y);
			if(m_pSpaceships[PLAYER_HUMAN].GetCollisionBitmap()->Check(vLocalPoint, CollisionBitmap::enumSOURCE_Centre))
				{
				// Hit!
				return 0.0f;
				}
			}

#ifdef INPUT_PRECALC_TEST
		m_debugtris[i].vert[0].v = vPos;
		m_debugtris[i].vert[1].v = PVRTVec3(vPos.x - 5.0f, vPos.y - 5.0f, 0.0f);
		m_debugtris[i].vert[2].v = PVRTVec3(vPos.x + 5.0f, vPos.y - 5.0f, 0.0f);

		memset(m_debugtris[i].vert[0].rgba, 0xFF, 4);
		memset(m_debugtris[i].vert[1].rgba, 0xFF, 4);
		memset(m_debugtris[i].vert[2].rgba, 0xFF, 4);

		if(nResult == 1)
			{
			m_debugtris[i].vert[0].rgba[0] = 0;
			m_debugtris[i].vert[1].rgba[0] = 0;
			m_debugtris[i].vert[2].rgba[0] = 0;
			}
		else if(nResult == 2)
			{
			m_debugtris[i].vert[0].rgba[1] = 0;
			m_debugtris[i].vert[1].rgba[1] = 0;
			m_debugtris[i].vert[2].rgba[1] = 0;
			}

		m_debugcount++;
#endif
		}

	nResult = thisresult;
	return fClosestDist;
	}
