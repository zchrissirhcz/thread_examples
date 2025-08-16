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