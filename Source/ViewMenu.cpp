#include "stdafx.h"
#include "ViewMenu.h"

//#define DISPLAY_MENU_HITBOX

extern const Uint32 c_uiMVP;
const Float32 c_fScreenScroll = 200.0f;
const Float32 c_fScrollGUIMulti = 1000.0f / c_fScreenScroll;

Float32 ViewMenu::ViewMenu_ViewOffsets[ViewMenu::enumMENUSCREEN_MAX] =
	{
	 0.0f,					// enumMENUSCREEN_Main
	-c_fScreenScroll,		// enumMENUSCREEN_Credits
	 c_fScreenScroll,		// enumMENUSCREEN_1PlayerSelect
	 c_fScreenScroll,		// enumMENUSCREEN_2PlayerSelect
	};

Sint32 ViewMenu::ViewMenu_GUIOffsets[ViewMenu::enumMENUSCREEN_MAX] =
	{
	(Sint32)( ViewMenu_ViewOffsets[enumMENUSCREEN_Main]*c_fScrollGUIMulti),					// enumMENUSCREEN_Main
	(Sint32)(-ViewMenu_ViewOffsets[enumMENUSCREEN_Credits]*c_fScrollGUIMulti),				// enumMENUSCREEN_Credits
	(Sint32)(-ViewMenu_ViewOffsets[enumMENUSCREEN_1PlayerSelect]*c_fScrollGUIMulti),		// enumMENUSCREEN_1PlayerSelect
	(Sint32)(-ViewMenu_ViewOffsets[enumMENUSCREEN_2PlayerSelect]*c_fScrollGUIMulti),		// enumMENUSCREEN_2PlayerSelect
	};

ViewMenu::ControlMap ViewMenu::m_ControlMap;

/*!***********************************************************************
 @Function		ViewMenu
 @Access		public
 @Returns
 @Description
*************************************************************************/
ViewMenu::ViewMenu() : m_eScreen(enumMENUSCREEN_Main), m_eState(enumMENUSTATE_ShowScreen),
					   m_eScreenNext(enumMENUSCREEN_MAX)
	{
	// Screen coordinates for Menu are in Retina iPhone (i.e 960 x 640)
	Sint32 ViewH = GetScreenDimensions(false).y;


	// Reset all of the players
	Player::CleanUp();

	m_AudioPress = RESMAN->GetAudioStream("menu.wav");
	m_AudioMusic = RESMAN->GetAudioStream("menumusic.mp3");

	// --- Position collision menu items. Not the cleanest way to do this but I don't really give a shit about GUI, and it works.
	Uint32 idx;
	Uint32 sc;
	// --- Main Screen
	sc = enumMENUSCREEN_Main;
	idx = enumMENUITEM_Main_To1Player;
	m_RectItems[idx].m_fX = 100.0f;				m_RectItems[idx].m_fW = m_RectItems[idx].m_fX + 250.0f;
	m_RectItems[idx].m_fY = (Float32)ViewH-210;	m_RectItems[idx].m_fH = m_RectItems[idx].m_fY + 36.0f;
	m_ControlMap.insert(ControlMap::value_type(idx, sc));

	idx = enumMENUITEM_Main_To2Player;
	m_RectItems[idx].m_fX = 100.0f;				m_RectItems[idx].m_fW = m_RectItems[idx].m_fX + 250.0f;
	m_RectItems[idx].m_fY = (Float32)ViewH-290;	m_RectItems[idx].m_fH = m_RectItems[idx].m_fY + 36.0f;
	m_ControlMap.insert(ControlMap::value_type(idx, sc));

	idx = enumMENUITEM_Main_ToCredits;
	m_RectItems[idx].m_fX = 100.0f;				m_RectItems[idx].m_fW = m_RectItems[idx].m_fX + 250.0f;
	m_RectItems[idx].m_fY = (Float32)ViewH-370;	m_RectItems[idx].m_fH = m_RectItems[idx].m_fY + 36.0f;
	m_ControlMap.insert(ControlMap::value_type(idx, sc));

/*	idx = enumMENUITEM_Main_Language;
	m_RectItems[idx].m_fX = 100.0f;				m_RectItems[idx].m_fW = m_RectItems[idx].m_fX + 250.0f;
	m_RectItems[idx].m_fY = (Float32)ViewH-450;	m_RectItems[idx].m_fH = m_RectItems[idx].m_fY + 36.0f;
	m_ControlMap.insert(ControlMap::value_type(idx, sc));*/

	// --- Credits Screen
	sc  = enumMENUSCREEN_Credits;
	idx = enumMENUITEM_Credits_BackToMain;
	m_RectItems[idx].m_fX = ViewMenu_GUIOffsets[sc] + 100.0f;	m_RectItems[idx].m_fW = m_RectItems[idx].m_fX + 250.0f;
	m_RectItems[idx].m_fY = 94.0f;								m_RectItems[idx].m_fH = m_RectItems[idx].m_fY + 36.0f;
	m_ControlMap.insert(ControlMap::value_type(idx, sc));

	// --- 1 Player
	Float32 YMenuStart = ViewH-210.0f;
	Float32 YMenuDelta = (Sint32)m_FontMenu->GetHeight();
	sc  = enumMENUSCREEN_1PlayerSelect;
	idx = enumMENUITEM_1Player_NumRounds;
	m_RectItems[idx].m_fX = ViewMenu_GUIOffsets[sc] + 100.0f;	m_RectItems[idx].m_fW = m_RectItems[idx].m_fX + 400.0f;
	m_RectItems[idx].m_fY = YMenuStart-YMenuDelta*0;			m_RectItems[idx].m_fH = m_RectItems[idx].m_fY + 36.0f;
	m_ControlMap.insert(ControlMap::value_type(idx, sc));

	idx = enumMENUITEM_1Player_Difficulty;
	m_RectItems[idx].m_fX = ViewMenu_GUIOffsets[sc] + 100.0f;	m_RectItems[idx].m_fW = m_RectItems[idx].m_fX + 500.0f;
	m_RectItems[idx].m_fY = YMenuStart-YMenuDelta*1;			m_RectItems[idx].m_fH = m_RectItems[idx].m_fY + 36.0f;
	m_ControlMap.insert(ControlMap::value_type(idx, sc));

	idx = enumMENUITEM_1Player_Controls;
	m_RectItems[idx].m_fX = ViewMenu_GUIOffsets[sc] + 100.0f;	m_RectItems[idx].m_fW = m_RectItems[idx].m_fX + 550.0f;
	m_RectItems[idx].m_fY = YMenuStart-YMenuDelta*2;			m_RectItems[idx].m_fH = m_RectItems[idx].m_fY + 36.0f;
	m_ControlMap.insert(ControlMap::value_type(idx, sc));

	idx = enumMENUITEM_1Player_Launch;
	m_RectItems[idx].m_fX = ViewMenu_GUIOffsets[sc] + 100.0f;	m_RectItems[idx].m_fW = m_RectItems[idx].m_fX + 250.0f;
	m_RectItems[idx].m_fY = YMenuStart-YMenuDelta*5;			m_RectItems[idx].m_fH = m_RectItems[idx].m_fY + 36.0f;
	m_ControlMap.insert(ControlMap::value_type(idx, sc));

	idx = enumMENUITEM_1Player_Back;
	m_RectItems[idx].m_fX = ViewMenu_GUIOffsets[sc] + 100.0f;	m_RectItems[idx].m_fW = m_RectItems[idx].m_fX + 250.0f;
	m_RectItems[idx].m_fY = YMenuStart-YMenuDelta*7;			m_RectItems[idx].m_fH = m_RectItems[idx].m_fY + 36.0f;
	m_ControlMap.insert(ControlMap::value_type(idx, sc));

	// --- 2 Player
	sc  = enumMENUSCREEN_2PlayerSelect;
	idx = enumMENUITEM_2Player_NumRounds;
	m_RectItems[idx].m_fX = ViewMenu_GUIOffsets[sc] + 100.0f;	m_RectItems[idx].m_fW = m_RectItems[idx].m_fX + 400.0f;
	m_RectItems[idx].m_fY = YMenuStart-YMenuDelta*0;			m_RectItems[idx].m_fH = m_RectItems[idx].m_fY + 36.0f;
	m_ControlMap.insert(ControlMap::value_type(idx, sc));

	idx = enumMENUITEM_2Player_GameType;
	m_RectItems[idx].m_fX = ViewMenu_GUIOffsets[sc] + 100.0f;	m_RectItems[idx].m_fW = m_RectItems[idx].m_fX + 500.0f;
	m_RectItems[idx].m_fY = YMenuStart-YMenuDelta*1;			m_RectItems[idx].m_fH = m_RectItems[idx].m_fY + 36.0f;
	m_ControlMap.insert(ControlMap::value_type(idx, sc));

	idx = enumMENUITEM_2Player_Controls;
	m_RectItems[idx].m_fX = ViewMenu_GUIOffsets[sc] + 100.0f;	m_RectItems[idx].m_fW = m_RectItems[idx].m_fX + 550.0f;
	m_RectItems[idx].m_fY = YMenuStart-YMenuDelta*2;			m_RectItems[idx].m_fH = m_RectItems[idx].m_fY + 36.0f;
	m_ControlMap.insert(ControlMap::value_type(idx, sc));

	idx = enumMENUITEM_2Player_Launch;
	m_RectItems[idx].m_fX = ViewMenu_GUIOffsets[sc] + 100.0f;	m_RectItems[idx].m_fW = m_RectItems[idx].m_fX + 250.0f;
	m_RectItems[idx].m_fY = YMenuStart-YMenuDelta*5;			m_RectItems[idx].m_fH = m_RectItems[idx].m_fY + 36.0f;
	m_ControlMap.insert(ControlMap::value_type(idx, sc));

	idx = enumMENUITEM_2Player_Back;
	m_RectItems[idx].m_fX = ViewMenu_GUIOffsets[sc] + 100.0f;	m_RectItems[idx].m_fW = m_RectItems[idx].m_fX + 250.0f;
	m_RectItems[idx].m_fY = YMenuStart-YMenuDelta*7;			m_RectItems[idx].m_fH = m_RectItems[idx].m_fY + 36.0f;
	m_ControlMap.insert(ControlMap::value_type(idx, sc));
	}

/*!***********************************************************************
 @Function		~ViewMenu
 @Access		public
 @Returns
 @Description
*************************************************************************/
ViewMenu::~ViewMenu()
	{
	}

/*!***********************************************************************
 @Function		CalcMessages
 @Access		private
 @Returns
 @Description
 *************************************************************************/
void ViewMenu::CalcMessages()
{
	// --- Set Messages
	// Main Menu
	{
		MessageGroup& Group = m_Messages[enumMENUSCREEN_Main];
		Group[0].Set(GWSTR(enumSTRING_Title), false, false, 0.001f);
		Group[1].Set(GWSTR(enumSTRING_MenuItems), false, true, 0.001f);
		Group[2].Set(GWSTR(enumSTRING_Copyright), false, false, 0.001f);
	}

	// Credits
	{
		MessageGroup& Group = m_Messages[enumMENUSCREEN_Credits];
		Group[0].Set(GWSTR(enumSTRING_Credits), false, false, 0.01f);
		Group[1].Set(GWSTR(enumSTRING_CreditsText1), false, false, 0.01f);
		Group[2].Set(GWSTR(enumSTRING_CreditsText2), false, false, 0.01f);
		Group[3].Set(GWSTR(enumSTRING_Back), true, false, 0.01f);
	}

	// 1 Player
	{
		MessageGroup& Group = m_Messages[enumMENUSCREEN_1PlayerSelect];
		Group[0].Set(GWSTR(enumSTRING_1Player), false, false,   0.01f);
		Group[1].Set(GWSTR(enumSTRING_NumRounds), true, false,  0.001f);
		Group[2].Set(RoundsToCString(m_GameData.m_uiRounds), false, false,  0.001f);
		Group[3].Set(GWSTR(enumSTRING_Difficulty), true, false, 0.001f);
		Group[4].Set(GWSTR(DifficultyToString(enumGWDIFFICULTY_Default)), false, false,  0.001f);
		Group[5].Set(GWSTR(enumSTRING_Controls),   true, false, 0.001f);
		Group[6].Set(GWSTR(InputTypeToString(enumGWINPUTTYPE_Default)), false, false,  0.001f);
		Group[7].Set(GWSTR(enumSTRING_PlayGame), true, false,   0.001f);
		Group[8].Set(GWSTR(enumSTRING_Back), true, false,       0.001f);
	}

	// 2 Player
	{
		MessageGroup& Group = m_Messages[enumMENUSCREEN_2PlayerSelect];
		Group[0].Set(GWSTR(enumSTRING_2Player), false, false,   0.01f);
		Group[1].Set(GWSTR(enumSTRING_NumRounds), true, false,  0.001f);
		Group[2].Set(RoundsToCString(m_GameData.m_uiRounds), false, false,  0.001f);
		Group[3].Set(GWSTR(enumSTRING_GameType), true, false, 0.001f);
		Group[4].Set(GWSTR(GameTypeToString(enumGWGAMETYPE_Default)), false, false,  0.001f);
		Group[5].Set(GWSTR(enumSTRING_Controls),   true, false, 0.001f);
		Group[6].Set(GWSTR(InputTypeToString(enumGWINPUTTYPE_Default)), false, false,  0.001f);
		Group[7].Set(GWSTR(enumSTRING_PlayGame), true, false,   0.001f);
		Group[8].Set(GWSTR(enumSTRING_Back), true, false,       0.001f);
	}
}

/*!***********************************************************************
 @Function		OnForeground
 @Access		public
 @Returns		void
 @Description
*************************************************************************/
void ViewMenu::OnForeground()
	{
	// Screen coordinates for Menu are in Retina iPhone (i.e 960 x 640)
	Sint32 ViewW = GetScreenDimensions(false).x;
	Sint32 ViewH = GetScreenDimensions(false).y;
	Sint32 ViewHW = ViewW / 2;

	GetApp()->GetAudioEngine()->Play(m_AudioMusic, true);
	GetApp()->GetAudioEngine()->SetVolume(m_AudioMusic, 1.0f);

	// --- Set Messages
	// Main Menu
		{
		MessageGroup& Group = m_Messages[enumMENUSCREEN_Main];
		Group.resize(3);
		Group[0].Layout(m_FontTitle, Vector2i(ViewHW, ViewH-60), enumTEXTJUSTIFY_Centre);
		Group[1].Layout(m_FontMenu,  Vector2i(100, ViewH-200), enumTEXTJUSTIFY_Left);
		Group[2].Layout(m_FontSmall, Vector2i(ViewHW, 10), enumTEXTJUSTIFY_Centre);
		Group[0].pNext = &Group[1];
		Group[1].pNext = &Group[2];
		}

	// Credits
		{
		MessageGroup& Group = m_Messages[enumMENUSCREEN_Credits];
		Group.resize(4);
		Group[0].Layout(m_FontTitle, Vector2i(ViewHW, ViewH-60), enumTEXTJUSTIFY_Centre);
		Group[1].Layout(m_FontMenu, Vector2i(100, ViewH-200), enumTEXTJUSTIFY_Left);
		Group[2].Layout(m_FontSmall, Vector2i(100, ViewH-240), enumTEXTJUSTIFY_Left);
		Group[3].Layout(m_FontMenu, Vector2i(100, 100), enumTEXTJUSTIFY_Left);
		Group[0].pNext = &Group[1];
		Group[1].pNext = &Group[2];
		Group[2].pNext = &Group[3];
		}

	// 1 Player
		{
		MessageGroup& Group = m_Messages[enumMENUSCREEN_1PlayerSelect];
		Sint32 YMenuStart = ViewH-200;
		Sint32 YMenuDelta = (Sint32)m_FontMenu->GetHeight();
		Group.resize(9);
		Group[0].Layout(m_FontTitle, Vector2i(ViewHW, ViewH-60), enumTEXTJUSTIFY_Centre);
		Group[1].Layout(m_FontMenu,  Vector2i(100, YMenuStart-YMenuDelta*0), enumTEXTJUSTIFY_Left);
		Group[2].Layout(m_FontMenu,  Vector2i(450, YMenuStart-YMenuDelta*0), enumTEXTJUSTIFY_Left);
		Group[3].Layout(m_FontMenu,  Vector2i(100, YMenuStart-YMenuDelta*1), enumTEXTJUSTIFY_Left);
		Group[4].Layout(m_FontMenu,  Vector2i(450, YMenuStart-YMenuDelta*1), enumTEXTJUSTIFY_Left);
		Group[5].Layout(m_FontMenu,  Vector2i(100, YMenuStart-YMenuDelta*2), enumTEXTJUSTIFY_Left);
		Group[6].Layout(m_FontMenu,  Vector2i(450, YMenuStart-YMenuDelta*2), enumTEXTJUSTIFY_Left);
		Group[7].Layout(m_FontMenu,  Vector2i(100, YMenuStart-YMenuDelta*5), enumTEXTJUSTIFY_Left);
		Group[8].Layout(m_FontMenu,  Vector2i(100, YMenuStart-YMenuDelta*7), enumTEXTJUSTIFY_Left);
		Group[0].pNext = &Group[1];
		Group[1].pNext = &Group[2];
		Group[2].pNext = &Group[3];
		Group[3].pNext = &Group[4];
		Group[4].pNext = &Group[5];
		Group[5].pNext = &Group[6];
		Group[6].pNext = &Group[7];
		Group[7].pNext = &Group[8];
		}

	// 2 Player
		{
		MessageGroup& Group = m_Messages[enumMENUSCREEN_2PlayerSelect];
		Sint32 YMenuStart = ViewH-200;
		Sint32 YMenuDelta = (Sint32)m_FontMenu->GetHeight();
		Group.resize(9);
		Group[0].Layout(m_FontTitle, Vector2i(ViewHW / 2, ViewH-60), enumTEXTJUSTIFY_Centre);
		Group[1].Layout(m_FontMenu,  Vector2i(100, YMenuStart-YMenuDelta*0), enumTEXTJUSTIFY_Left);
		Group[2].Layout(m_FontMenu,  Vector2i(450, YMenuStart-YMenuDelta*0), enumTEXTJUSTIFY_Left);
		Group[3].Layout(m_FontMenu,  Vector2i(100, YMenuStart-YMenuDelta*1), enumTEXTJUSTIFY_Left);
		Group[4].Layout(m_FontMenu,  Vector2i(450, YMenuStart-YMenuDelta*1), enumTEXTJUSTIFY_Left);
		Group[5].Layout(m_FontMenu,  Vector2i(100, YMenuStart-YMenuDelta*2), enumTEXTJUSTIFY_Left);
		Group[6].Layout(m_FontMenu,  Vector2i(450, YMenuStart-YMenuDelta*2), enumTEXTJUSTIFY_Left);
		Group[7].Layout(m_FontMenu,  Vector2i(100, YMenuStart-YMenuDelta*5), enumTEXTJUSTIFY_Left);
		Group[8].Layout(m_FontMenu,  Vector2i(100, YMenuStart-YMenuDelta*7), enumTEXTJUSTIFY_Left);
		Group[0].pNext = &Group[1];
		Group[1].pNext = &Group[2];
		Group[2].pNext = &Group[3];
		Group[3].pNext = &Group[4];
		Group[4].pNext = &Group[5];
		Group[5].pNext = &Group[6];
		Group[6].pNext = &Group[7];
		Group[7].pNext = &Group[8];
		}

	CalcMessages();

	m_MsgRoot = &m_Messages[enumMENUSCREEN_Main][0];
	}

/*!***********************************************************************
 @Function		Render
 @Access		public
 @Returns		void
 @Description
*************************************************************************/
void ViewMenu::RenderText()
	{
	Uint32 uiPasses = (m_eState == enumMENUSTATE_Tween ? 2 : 1);
	enumMENUSCREEN eScreenToRender = m_eScreen;		// Always render the screen we are displaying
	for(Uint32 i = 0; i < uiPasses; ++i)
		{
		MessageGroup& Group = m_Messages[eScreenToRender];
		for(Uint32 uiMsg = 0; uiMsg < Group.size(); uiMsg++)
			Group[uiMsg].Font->RenderString(Group[uiMsg].szMsgToRender, ViewMenu_GUIOffsets[eScreenToRender] + Group[uiMsg].vPos.x,
											Group[uiMsg].vPos.y, Group[uiMsg].eJustify);

		eScreenToRender = m_eScreenNext;		// Set to the next screen such that if we're doing 2 passes, the tweening screen will also render.
		}

#ifdef DISPLAY_MENU_HITBOX
	glEnable(GL_BLEND);
	Shader::Use(m_VCShader);
	PVRTMat4 mxMVP = m_GUICam.GetProjection() * m_GUICam.GetView();
	glUniformMatrix4fv(m_VCShader->GetUniform(c_uiMVP), 1, GL_FALSE, mxMVP.f);
	for(Uint32 i = 0; i < enumMENUITEM_MAX; ++i)
		GFX->RenderQuad(m_RectItems[i], PVRTMat4::Identity(), FLAG_VRT | FLAG_TEX0 | FLAG_RGB, RGBA(1,0,0,0.5f));
	Shader::Use(0);
	glDisable(GL_BLEND);
#endif
	}

/*!***********************************************************************
 @Function		Update
 @Access		public
 @Param			double dt
 @Returns		void
 @Description
*************************************************************************/
void ViewMenu::Update(double dt)
	{
	ViewGUI::Update(dt);

	// Update messages
	if(m_eState == enumMENUSTATE_ShowScreen)
		m_MsgRoot->Update((Float32)dt);

	// Update screen
	if(m_TweenInterp.IsActive())
		{
		m_TweenInterp.Update(dt);
		Float32 fXVal = m_TweenInterp.Value();
		m_mxCam = PVRTMat4::Translation(fXVal, 0.0f, 0.0f);
		m_mxCamGUI = PVRTMat4::Translation(fXVal*c_fScrollGUIMulti, 0.0f, 0.0f);	// Move to/from 0 > 1000
		m_cam.Set(m_mxCam);
		m_GUICam.Set(m_mxCamGUI);
		if(!m_TweenInterp.IsActive())
			{
			m_eScreen = m_eScreenNext;
			m_eState  = enumMENUSTATE_ShowScreen;

			m_eScreenNext = enumMENUSCREEN_MAX;
			}
		}

	// Switch view.
	if(m_eGUIState == enumGUISTATE_Off_Done)
		return VIEWMAN->ShowView("GameLoader", &m_GameData);
	}

/*!***********************************************************************
 @Function		OnTouchUp
 @Access		public
 @Param			Touch * pTouches
 @Param			Uint32 uiNum
 @Returns		void
 @Description
*************************************************************************/
void ViewMenu::OnTouchUp(Touch* pTouches, Uint32 uiNum)
	{
	if(m_eState == enumMENUSTATE_Tween || m_eGUIState & enumGUISTATE_Off_Mask)
		return;

	m_MsgRoot->Update(-1.0f);		// Force messages to render completely.

	// Scale touch to our local view coords (iPad or Retina)
	Float32 fCoordX = (Float32)GetScreenDimensions(false).x;
	Float32 fCoordY = (Float32)GetScreenDimensions(false).y;
	PVRTVec2 vTouchWorld((pTouches[0].fX / (Float32)GFX->GetDeviceWidth()) * fCoordX, (pTouches[0].fY / (Float32)GFX->GetDeviceHeight()) * fCoordY);

	PVRTVec2 vTouch = PVRTVec2(ViewMenu_GUIOffsets[m_eScreen] + vTouchWorld.x,	fCoordY - vTouchWorld.y);
	for(Uint32 i = 0; i < enumMENUITEM_MAX; i++)
		{
		if(RectBelongsToScreen(i) && m_RectItems[i].Contains(vTouch))
			{
			GetApp()->GetAudioEngine()->Play(m_AudioPress, false);
			switch(i)
				{
				// Main
				case enumMENUITEM_Main_ToCredits:
					m_eScreenNext = enumMENUSCREEN_Credits;
					break;
				case enumMENUITEM_Main_To1Player:
					m_GameData.m_uiNumPlayers = 1;
					m_eScreenNext = enumMENUSCREEN_1PlayerSelect;
					break;
				case enumMENUITEM_Main_To2Player:
					m_GameData.m_uiNumPlayers = 2;
					m_eScreenNext = enumMENUSCREEN_2PlayerSelect;
					break;
				case enumMENUITEM_Main_Language:
					GetApp()->ToggleNextLanguage();
					CalcMessages();
					break;

				// Credits
				case enumMENUITEM_Credits_BackToMain:
					m_eScreenNext = enumMENUSCREEN_Main;
					break;

				// 1 Player
				case enumMENUITEM_1Player_NumRounds:
					m_GameData.m_uiRounds += 2;
					if(m_GameData.m_uiRounds > MAX_NUM_ROUNDS)	m_GameData.m_uiRounds = 1;
					m_Messages[enumMENUSCREEN_1PlayerSelect][2].Set(RoundsToCString(m_GameData.m_uiRounds), false, false,  0.001f);
					break;
				case enumMENUITEM_1Player_Difficulty:
					// Cycle difficulty
					m_GameData.m_eDifficulty = (enumGWDIFFICULTY)(m_GameData.m_eDifficulty+1);
					if(m_GameData.m_eDifficulty == enumGWDIFFICULTY_MAX)
						m_GameData.m_eDifficulty = (enumGWDIFFICULTY)0;
					m_Messages[enumMENUSCREEN_1PlayerSelect][4].Set(GWSTR(DifficultyToString(m_GameData.m_eDifficulty)), false, false,  0.001f);
					break;
				case enumMENUITEM_1Player_Controls:
					// Cycle controls
					m_GameData.m_eInputType = (enumGWINPUTTYPE)(m_GameData.m_eInputType+1);
					if(m_GameData.m_eInputType == enumGWINPUTTYPE_MAX)
						m_GameData.m_eInputType = (enumGWINPUTTYPE)0;
					m_Messages[enumMENUSCREEN_1PlayerSelect][6].Set(GWSTR(InputTypeToString(m_GameData.m_eInputType)), false, false,  0.001f);
					break;
				case enumMENUITEM_1Player_Launch:
					// Create some players
					m_GameData.m_pPlayers.push_back(Player::NewPlayer(GWSTR(enumSTRING_Player1), RGBA(1,0.75f,0.75f,1)));
					m_GameData.m_pPlayers.push_back(Player::NewPlayer(GWSTR(enumSTRING_AI), RGBA(0.75f,0.75f,1,1)));
					m_GameData.m_eGameType = enumGWGAMETYPE_Rounds;		// 1Player is based on rounds but best score recorded.
					Close();
					break;
				case enumMENUITEM_1Player_Back:
					m_eScreenNext = enumMENUSCREEN_Main;
					break;


				// 2 Player
				case enumMENUITEM_2Player_NumRounds:
					m_GameData.m_uiRounds += 2;
					if(m_GameData.m_uiRounds > MAX_NUM_ROUNDS)	m_GameData.m_uiRounds = 1;
					m_Messages[enumMENUSCREEN_2PlayerSelect][2].Set(RoundsToCString(m_GameData.m_uiRounds), false, false,  0.001f);
					break;
				case enumMENUITEM_2Player_GameType:
					// Cycle game types
					m_GameData.m_eGameType = (enumGWGAMETYPE)(m_GameData.m_eGameType+1);
					if(m_GameData.m_eGameType == enumGWGAMETYPE_MAX)
						m_GameData.m_eGameType = (enumGWGAMETYPE)0;
					m_Messages[enumMENUSCREEN_2PlayerSelect][4].Set(GWSTR(GameTypeToString(m_GameData.m_eGameType)), false, false,  0.001f);
					break;
				case enumMENUITEM_2Player_Controls:
					// Cycle controls
					m_GameData.m_eInputType = (enumGWINPUTTYPE)(m_GameData.m_eInputType+1);
					if(m_GameData.m_eInputType == enumGWINPUTTYPE_MAX)
						m_GameData.m_eInputType = (enumGWINPUTTYPE)0;
					m_Messages[enumMENUSCREEN_2PlayerSelect][6].Set(GWSTR(InputTypeToString(m_GameData.m_eInputType)), false, false,  0.001f);
					break;
				case enumMENUITEM_2Player_Launch:
					// Create some players
					m_GameData.m_pPlayers.push_back(Player::NewPlayer(GWSTR(enumSTRING_Player1), RGBA(1,0.75f,0.75f,1)));
					m_GameData.m_pPlayers.push_back(Player::NewPlayer(GWSTR(enumSTRING_Player2), RGBA(0.75f,0.75f,1,1)));
					Close();
					break;
				case enumMENUITEM_2Player_Back:
					m_eScreenNext = enumMENUSCREEN_Main;
					break;

				default:
					ASSERT(!"Unhandled item!");
				}
			}
		}

	if(m_eScreenNext != enumMENUSCREEN_MAX)
		{
		m_eState  = enumMENUSTATE_Tween;
		m_MsgRoot = &m_Messages[m_eScreenNext][0];
		m_TweenInterp.Open(ViewMenu_ViewOffsets[m_eScreen], ViewMenu_ViewOffsets[m_eScreenNext], enumINTERPOLATOR_CubicEaseOut, 1.0f);	// AH: FS. Interpolator func won't go to 0.
		}
	}

/*!***********************************************************************
 @Function		OnKeyPress
 @Access		proteced
 @Param			eKey
 @Returns		void
 @Description
*************************************************************************/
void ViewMenu::OnKeyPress(Uint32 eKey)
{
	if(m_eState == enumMENUSTATE_Tween || m_eGUIState & enumGUISTATE_Off_Mask)
		return;

    switch(eKey)
    {
        case App::enumKEY_Back:
            {
                if(m_eScreen != enumMENUSCREEN_Main)
                {
                    m_eScreenNext = enumMENUSCREEN_Main;
                }
                break;
            }
    }

	if(m_eScreenNext != enumMENUSCREEN_MAX)
		{
		m_eState  = enumMENUSTATE_Tween;
		m_MsgRoot = &m_Messages[m_eScreenNext][0];
		m_TweenInterp.Open(ViewMenu_ViewOffsets[m_eScreen], ViewMenu_ViewOffsets[m_eScreenNext], enumINTERPOLATOR_CubicEaseOut, 1.0f);	// AH: FS. Interpolator func won't go to 0.
		}
}

/*!***********************************************************************
 @Function		RectBelongsToScreen
 @Access		private
 @Param			Uint32 uiRectID
 @Returns		bool
 @Description
*************************************************************************/
bool ViewMenu::RectBelongsToScreen(Uint32 uiRectID)
	{
	if(m_ControlMap[uiRectID] == m_eScreen)
		return true;
	else
		return false;
	}
