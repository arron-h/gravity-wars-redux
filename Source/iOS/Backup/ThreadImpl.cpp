#include "stdafx.h"

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
	return pthread_create(pThread, NULL, pStartRoutine, pArg);
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
	return pthread_cancel(pThread);
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
	sleep(ms);
	}
