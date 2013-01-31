#include "stdafx.h"

ObjectPool<CRITICAL_SECTION> g_MutexPool(128);

/*!***********************************************************************
 @Function		Create
 @Access		public 
 @Param			THREADID * pThread
 @Param			void * 
 @Param			* pStartRoutine
 @Param			void * 
 @Param			void * pArg
 @Returns		Sint32
 @Description	
*************************************************************************/
Sint32 Thread::Create(THREADID *pThread, void *(*pStartRoutine)(void *), void *pArg)
	{
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)pStartRoutine, pArg, 0, (LPDWORD)pThread);
	return 0;
	}

/*!***********************************************************************
 @Function		Cancel
 @Access		public 
 @Param			THREADID pThread
 @Returns		Sint32
 @Description	
*************************************************************************/
Sint32 Thread::Cancel(THREADID pThread)
	{
	HANDLE hThread = OpenThread(THREAD_TERMINATE, FALSE, pThread);
	TerminateThread(hThread, 0);
	CloseHandle(hThread);
	return 0;
	}

/*!***********************************************************************
 @Function		Sleep
 @Access		public 
 @Param			Sint32 nMS
 @Returns		void
 @Description	
*************************************************************************/
void Thread::Sleep(Sint32 nMS)
	{
	::Sleep(nMS);
	}


/*!***********************************************************************
 @Function		Mutex
 @Access		public 
 @Returns		
 @Description	
*************************************************************************/
Mutex::Mutex() : m_pHandle(NULL)
	{
	CRITICAL_SECTION* pCritSection = g_MutexPool.Allocate();
	if(pCritSection)
		{
		InitializeCriticalSection(pCritSection);
		m_pHandle = reinterpret_cast<void*>(pCritSection);
		}
	}

/*!***********************************************************************
 @Function		~Mutex
 @Access		public 
 @Returns		
 @Description	
*************************************************************************/
Mutex::~Mutex()
	{
	if(m_pHandle)
		{
		CRITICAL_SECTION* pCritSection = reinterpret_cast<CRITICAL_SECTION*>(m_pHandle);
		DeleteCriticalSection(pCritSection);
		g_MutexPool.Free(pCritSection);
		}
	}

/*!***********************************************************************
 @Function		Lock
 @Access		public 
 @Returns		void
 @Description	
*************************************************************************/
void Mutex::Lock()
	{
	if(!m_pHandle)
		return;

	CRITICAL_SECTION* pCritSection = reinterpret_cast<CRITICAL_SECTION*>(m_pHandle);
	EnterCriticalSection(pCritSection);
	}

/*!***********************************************************************
 @Function		Release
 @Access		public 
 @Returns		void
 @Description	
*************************************************************************/
void Mutex::Release()
	{
	if(!m_pHandle)
		return;

	CRITICAL_SECTION* pCritSection = reinterpret_cast<CRITICAL_SECTION*>(m_pHandle);
	LeaveCriticalSection(pCritSection);
	}

/*!***********************************************************************
 @Function		TryLock
 @Access		public 
 @Returns		bool
 @Description	
*************************************************************************/
bool Mutex::TryLock()
	{
	if(!m_pHandle)
		return false;

	CRITICAL_SECTION* pCritSection = reinterpret_cast<CRITICAL_SECTION*>(m_pHandle);
	return (TryEnterCriticalSection(pCritSection) == TRUE ? true : false);
	}
