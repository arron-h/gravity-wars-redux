#ifndef VIEWMANAGER_H
#define VIEWMANAGER_H

#include <map>
#include <string>

class ViewManager
	{
	private:
		typedef std::map<std::string, View*>	ViewList;
		
		ViewList		m_pViews;
		View*			m_pActiveView;

	public:
		ViewManager();

		void ShowView(const char* c_pszViewName, void* pData = NULL);
		void ShowView(View* pView);
		View* GetActiveView();
	};

#endif
