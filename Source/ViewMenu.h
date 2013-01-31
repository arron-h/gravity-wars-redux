#ifndef VIEWMENU_H
#define VIEWMENU_H

#include "ViewGUI.h"

class ViewMenu : public ViewGUI
	{
	private:
		enum enumMENUITEM
			{
			// Main
			enumMENUITEM_Main_To1Player,
			enumMENUITEM_Main_To2Player,
			enumMENUITEM_Main_ToCredits,

			// Credits
			enumMENUITEM_Credits_BackToMain,

			// 1 Player
			enumMENUITEM_1Player_NumRounds,
			enumMENUITEM_1Player_Difficulty,
			enumMENUITEM_1Player_Launch,
			enumMENUITEM_1Player_Back,

			// 2 Player
			enumMENUITEM_2Player_NumRounds,
			enumMENUITEM_2Player_GameType,
			enumMENUITEM_2Player_Launch,
			enumMENUITEM_2Player_Back,

			enumMENUITEM_MAX,
			};

		enum enumMENUSTATE
			{
			enumMENUSTATE_ShowScreen		= 0x0,
			enumMENUSTATE_Tween				= 0x1,
			}							m_eState;

		enum enumMENUSCREEN
			{
			enumMENUSCREEN_Main,
			enumMENUSCREEN_Credits,
			enumMENUSCREEN_1PlayerSelect,
			enumMENUSCREEN_2PlayerSelect,

			enumMENUSCREEN_MAX,
			}							m_eScreen, m_eScreenNext;

		static Float32 ViewMenu_ViewOffsets[enumMENUSCREEN_MAX];
		static Sint32 ViewMenu_GUIOffsets[enumMENUSCREEN_MAX];
		typedef std::map<Uint32, Uint32> ControlMap;
		static ControlMap				m_ControlMap;

		Interpolator<Float32>			m_TweenInterp;

		Rectanglef						m_RectItems[enumMENUITEM_MAX];

		GameData						m_GameData;

		typedef std::vector<SMessage> MessageGroup;
		MessageGroup					m_Messages[enumMENUSCREEN_MAX];

		SMessage*						m_MsgRoot;

		AudioRef						m_AudioPress;
		AudioRef						m_AudioMusic;

	private:
		bool RectBelongsToScreen(Uint32 uiRectID);

	protected:
		virtual void RenderText();
		
	public:
		ViewMenu();
		~ViewMenu();

		virtual void Update(double dt);
		virtual void OnForeground();

		virtual void OnTouchUp(Touch* pTouches, Uint32 uiNum);
	};

#endif
