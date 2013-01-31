#ifndef VIEWGAMEFINISHED_H
#define VIEWGAMEFINISHED_H

#include "ViewGUI.h"

class ViewGameFinished : public ViewGUI
	{
	private:
		GameData					m_GameData;
		std::vector<SMessage>		m_Messages;
		SMessage*					m_MsgRoot;

		Player*						m_pWinner;

		Rectanglef					m_RectMainMenu;

	protected:
		void RenderText();

	public:
		ViewGameFinished(const GameData& gamedata);

		virtual void Update(double fDT);
		virtual void OnForeground();

		virtual void OnTouchUp(Touch* pTouches, Uint32 uiNum);
	};

#endif
