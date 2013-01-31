#include <pthread.h>
#include <unistd.h>
#include "stdafx.h"


std::vector<pthread_t> Threads;

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
	THREADID index = Threads.size();
	pthread_t newthread;
	return pthread_create(&newthread, NULL, pStartRoutine, pArg);
	
	Threads.push_back(newthread);
	
	*pThread = index;
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
	pthread_t threadid = Threads[pThread];	
	return pthread_cancel(threadid);
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
	sleep(nMS);
	}


/*!***********************************************************************
 @Function		Mutex
 @Access		public 
 @Returns		
 @Description	
*************************************************************************/
Mutex::Mutex() : m_pHandle(NULL)
	{
	// ITODO
	}

/*!***********************************************************************
 @Function		~Mutex
 @Access		public 
 @Returns		
 @Description	
*************************************************************************/
Mutex::~Mutex()
	{
	// ITODO
	}

/*!***********************************************************************
 @Function		Lock
 @Access		public 
 @Returns		void
 @Description	
*************************************************************************/
void Mutex::Lock()
	{
	// ITODO
	}

/*!***********************************************************************
 @Function		Release
 @Access		public 
 @Returns		void
 @Description	
*************************************************************************/
void Mutex::Release()
	{
	// ITODO
	}

/*!***********************************************************************
 @Function		TryLock
 @Access		public 
 @Returns		bool
 @Description	
*************************************************************************/
bool Mutex::TryLock()
	{
	// ITODO
	return false;
	}
