#include "stdafx.h"

// --- Include Views
#include "ViewAppLoader.h"
#include "ViewGame.h"
#include "ViewGameLoader.h"
#include "ViewParticles.h"
#include "ViewMenu.h"
#include "ViewGameFinished.h"

enum enumVIEW
	{
	enumVIEW_AppLoad,
	enumVIEW_GameLoad,
	enumVIEW_Menu,
	enumVIEW_GameFinished,
	enumVIEW_Particles,
	enumVIEW_MAX,

	enumVIEW_Default = enumVIEW_GameLoad,
	};

const char* cg_pszViews[] =
	{
	"AppLoader",				// enumVIEW_AppLoad
	"GameLoader",				// enumVIEW_GameLoad
	"Menu",						// enumVIEW_Menu
	"GameFinished",				// enumVIEW_GameFinished
	"Particles",				// enumVIEW_Particles
	};


/*!***********************************************************************
 @Function		CreateView
 @Access		public 
 @Param			const char * c_pszViewName
 @Returns		View*
 @Description	
*************************************************************************/
View* CreateView(const char* c_pszViewName, void* pData)
	{
	enumVIEW eView = enumVIEW_Default;
	if(c_pszViewName)
		{
		eView = enumVIEW_MAX;
		for(Uint32 uiIndex = 0; uiIndex < enumVIEW_MAX; uiIndex++)
			{
			if(strcmp(cg_pszViews[uiIndex], c_pszViewName) == 0)
				eView = (enumVIEW)uiIndex;
			}
		}

	ASSERT(eView != enumVIEW_MAX);		// Not found

	switch(eView)
		{
		case enumVIEW_AppLoad:
			return new ViewAppLoader();
			break;
		case enumVIEW_GameLoad:
			ASSERT(pData);
			return new ViewGameLoader(*reinterpret_cast<GameData*>(pData));
			break;
		case enumVIEW_GameFinished:
			ASSERT(pData);
			return new ViewGameFinished(*reinterpret_cast<GameData*>(pData));
			break;
		case enumVIEW_Particles:
			return new ViewParticles();
			break;
		case enumVIEW_Menu:
			return new ViewMenu();
			break;
		default:
			ASSERT(!"Unhandled view!");
		}

	return NULL;
	}

ViewManager::ViewManager() : m_pActiveView(NULL)
	{
	ASSERT(sizeof(cg_pszViews) / sizeof(cg_pszViews[0]) == enumVIEW_MAX);
	
	// Add all the views the map, but don't actually allocate them.
//	for(Uint32 uiIndex = 0; uiIndex < enumVIEW_MAX; uiIndex++)
//		m_pViews.insert(ViewList::value_type(cg_pszViews[uiIndex], NULL));
	}

/*!***********************************************************************
 @Function		ShowView
 @Access		public 
 @Param			const char * c_pszViewName
 @Returns		void
 @Description	
*************************************************************************/
void ViewManager::ShowView(const char* c_pszViewName, void* pData)
	{
	View* pNewView = NULL;
		
	ASSERT(pNewView == NULL);
	pNewView = CreateView(c_pszViewName[0] == 0 ? NULL : c_pszViewName, pData);
	pNewView->OnForeground();

	// Delete old view
	if(m_pActiveView)
		delete m_pActiveView;
	m_pActiveView = NULL;
	
	// Set new view
	m_pActiveView = pNewView;	
	}

/*!***********************************************************************
 @Function		ShowView
 @Access		public 
 @Param			View * pView
 @Returns		void
 @Description	Shows a view that has been user-managed
*************************************************************************/
void ViewManager::ShowView(View* pView)
	{
	ASSERT(pView);
	pView->OnForeground();

	// Set new view
	m_pActiveView = pView;	
	}

/*!***********************************************************************
 @Function		GetActiveView
 @Access		public 
 @Returns		View*
 @Description	
*************************************************************************/
View* ViewManager::GetActiveView()
	{
	return m_pActiveView;
	}