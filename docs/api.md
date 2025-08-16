## Critical Section

CRITICAL_SECTION cs
InitializeCriticalSection()
DeleteCriticalSection()
EnterCriticalSection()
LeaveCriticalSection()

## SRW

SRW Lock (Slim Reader/Writer Lock): usable since Windows Vista:

SRWLOCK srw_lock
InitializeSRWLock()
AcquireSRWLockExclusive()
ReleaseSRWLockExclusive()
WaitForSingleObject()
WaitForMultipleObjects()   最多等待 MAXIMUM_WAIT_OBJECTS (64) 个线程

## pthread

pthread_create()
pthread_join()
pthread_detach()

pthread_mutex_init()
pthread_mutex_lock()
pthread_mutex_unlock()
pthread_mutex_destroy()

## std::thread

TBD