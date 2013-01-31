#ifndef THREAD_H
#define THREAD_H

typedef Uint32 THREADID;
typedef Uint32 MUTEXID;

class Thread
	{
	public:
		static Sint32 Create(THREADID *pThread, void*(*pStartRoutine)(void*), void* pArg);
		static Sint32 Cancel(THREADID pThread);
		static void Sleep(Sint32 nMS);
		
		static void Initialise();
	};

class Mutex
	{
	protected:
		void*		m_pHandle;			// This could be an #ifdef for WIN32 etc. Neater this way though.

	public:
		Mutex();
		~Mutex();

		void Lock();
		void Release();
		bool TryLock();
	};

#endif
