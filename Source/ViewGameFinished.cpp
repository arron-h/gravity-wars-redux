#include "stdafx.h"
#include <algorithm>
#include "ViewGameFinished.h"

bool PlayerSort_Score(Player* pA, Player* pB)  { return (pA->GetScore() > pB->GetScore()); }
bool PlayerSort_Rounds(Player* pA, Player* pB) { return (pA->GetRoundsWon() > pB->GetRoundsWon()); }

typedef bool(*PlayerSortFunc)(Player*, Player*);

/*!***********************************************************************
 @Function		ViewGameFinished
 @Access		public 
 @Param			const GameData & gamedata
 @Returns		
 @Description	
*************************************************************************/
ViewGameFinished::ViewGameFinished(const GameData& gamedata)
	{
	// Copy game data
	m_GameData = gamedata;

	// Who won? Copy the player list and sort it.
	std::vector<Player*> PlayerListToSort;
	for(Uint32 i = 0; i < m_GameData.m_pPlayers.size(); ++i)
		PlayerListToSort.push_back(m_GameData.m_pPlayers[i]);

	PlayerSortFunc pFunc = NULL;
	switch(m_GameData.m_eGameType)
		{
		case enumGWGAMETYPE_Rounds:		pFunc = PlayerSort_Rounds;
		case enumGWGAMETYPE_Score:		pFunc = PlayerSort_Score;
		default:
			ASSERT(!"Invalid");
		}
	std::sort(PlayerListToSort.begin(), PlayerListToSort.end(), pFunc);
	
	m_pWinner = PlayerListToSort[0];
	}

/*!***********************************************************************
 @Function		OnForeground
 @Access		public 
 @Returns		void
 @Description	
*************************************************************************/
void ViewGameFinished::OnForeground()
	{
	// Screen coordinates for Menu are in Retina iPhone (i.e 960 x 640)
	Sint32 ViewW = GetScreenDimensions(false).x;
	Sint32 ViewH = GetScreenDimensions(false).y;
	Sint32 ViewHW = ViewW / 2;

	//GraphicsSystem* pGfx = GetApp()->GetGraphicsSystem();
	TXChar szBuffer[256];
	m_Messages.resize(5);

	m_Messages[0].Set(GWSTR(enumSTRING_GameOver), false, false, 0.001f);
	m_Messages[0].Layout(m_FontTitle, Vector2i(ViewHW, ViewH-60), enumTEXTJUSTIFY_Centre);

	sprintf(szBuffer, GWSTR(enumSTRING_XWins), m_pWinner->GetName());
	m_Messages[1].Set(szBuffer, false, false, 0.001f);
	m_Messages[1].Layout(m_FontMenu, Vector2i(ViewHW, ViewH-200), enumTEXTJUSTIFY_Centre);

	if(m_GameData.m_uiNumPlayers == 1)
		{
		sprintf(szBuffer, GWSTR(enumSTRING_ScoreX), m_GameData.m_pPlayers[enumPLAYER_1]->GetScore());
		m_Messages[2].Set(szBuffer, false, false, 0.001f);
		m_Messages[2].Layout(m_FontMenu, Vector2i(100, ViewH-280), enumTEXTJUSTIFY_Left);

		sprintf(szBuffer, GWSTR(enumSTRING_BestScoreX), m_GameData.m_pPlayers[enumPLAYER_1]->GetScore());
		m_Messages[3].Set(szBuffer, false, false, 0.001f);
		m_Messages[3].Layout(m_FontMenu, Vector2i(100, ViewH-320), enumTEXTJUSTIFY_Left);
		}
	else
		{
		char szSprintfBuf[256];
		if(m_GameData.m_eGameType == enumGWGAMETYPE_Rounds)		// Print Rounds Won for each player
			{
			sprintf(szSprintfBuf, "%s %s", GWSTR(enumSTRING_Player1), GWSTR(enumSTRING_RoundsWonX));
			sprintf(szBuffer, szSprintfBuf, m_GameData.m_pPlayers[enumPLAYER_1]->GetRoundsWon());
			m_Messages[2].Set(szBuffer, false, false, 0.001f);
			m_Messages[2].Layout(m_FontMenu, Vector2i(100, ViewH-280), enumTEXTJUSTIFY_Left);

			sprintf(szSprintfBuf, "%s %s", GWSTR(enumSTRING_Player2), GWSTR(enumSTRING_RoundsWonX));
			sprintf(szBuffer, szSprintfBuf, m_GameData.m_pPlayers[enumPLAYER_2]->GetRoundsWon());
			m_Messages[3].Set(szBuffer, false, false, 0.001f);
			m_Messages[3].Layout(m_FontMenu, Vector2i(100, ViewH-320), enumTEXTJUSTIFY_Left);
			}
		else														// Print Score for each player
			{
			sprintf(szSprintfBuf, "%s %s", GWSTR(enumSTRING_Player1), GWSTR(enumSTRING_ScoreX));
			sprintf(szBuffer, szSprintfBuf, m_GameData.m_pPlayers[enumPLAYER_1]->GetScore());
			m_Messages[2].Set(szBuffer, false, false, 0.001f);
			m_Messages[2].Layout(m_FontMenu, Vector2i(100, ViewH-280), enumTEXTJUSTIFY_Left);

			sprintf(szSprintfBuf, "%s %s", GWSTR(enumSTRING_Player2), GWSTR(enumSTRING_ScoreX));
			sprintf(szBuffer, szSprintfBuf, m_GameData.m_pPlayers[enumPLAYER_2]->GetScore());
			m_Messages[3].Set(szBuffer, false, false, 0.001f);
			m_Messages[3].Layout(m_FontMenu, Vector2i(100, ViewH-320), enumTEXTJUSTIFY_Left);
			}
		}

	m_Messages[4].Set(GWSTR(enumSTRING_MainMenu), true, false, 0.01f);
	m_Messages[4].Layout(m_FontMenu, Vector2i(100, 100), enumTEXTJUSTIFY_Left);

	m_Messages[0].pNext = &m_Messages[1];
	m_Messages[1].pNext = &m_Messages[2];
	m_Messages[2].pNext = &m_Messages[3];
	m_Messages[3].pNext = &m_Messages[4];

	m_MsgRoot = &m_Messages[0];

	// --- Set Main Menu button rect
	m_RectMainMenu.m_fX = 100.0f;		m_RectMainMenu.m_fW = m_RectMainMenu.m_fX + 250.0f;
	m_RectMainMenu.m_fY = 94.0f;		m_RectMainMenu.m_fH = m_RectMainMenu.m_fY + 36.0f;
	}	

/*!***********************************************************************
 @Function		RenderText
 @Access		protected 
 @Returns		void
 @Description	
*************************************************************************/
void ViewGameFinished::RenderText()
	{
	for(Uint32 uiMsg = 0; uiMsg < m_Messages.size(); uiMsg++)
		m_Messages[uiMsg].Font->RenderString(m_Messages[uiMsg].szMsgToRender, m_Messages[uiMsg].vPos.x, 
											 m_Messages[uiMsg].vPos.y, m_Messages[uiMsg].eJustify);
	}

/*!***********************************************************************
 @Function		Update
 @Access		virtual public 
 @Param			double fDT
 @Returns		void
 @Description	
*************************************************************************/
void ViewGameFinished::Update(double fDT)
	{
	ViewGUI::Update(fDT);

	m_MsgRoot->Update((Float32)fDT);

	if(m_eGUIState == enumGUISTATE_Off_Done)
		return VIEWMAN->ShowView("Menu");
	}

/*!***********************************************************************
 @Function		OnTouchUp
 @Access		public 
 @Param			Touch * pTouches
 @Param			Uint32 uiNum
 @Returns		void
 @Description	
*************************************************************************/
void ViewGameFinished::OnTouchUp(Touch* pTouches, Uint32 uiNum)
	{
	if(m_eGUIState & enumGUISTATE_Off_Mask)
		return;
		
	// Scale touch to our local view coords (iPad or Retina)
	Float32 fCoordX = (Float32)GetScreenDimensions(false).x;
	Float32 fCoordY = (Float32)GetScreenDimensions(false).y;
	PVRTVec2 vTouchWorld((pTouches[0].fX / (Float32)GFX->GetDeviceWidth()) * fCoordX, (pTouches[0].fY / (Float32)GFX->GetDeviceHeight()) * fCoordY);

	PVRTVec2 vTouch = PVRTVec2(vTouchWorld.x,	fCoordY - vTouchWorld.y);

	if(m_RectMainMenu.Contains(vTouch))
		Close();
	}