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

pthread_cond_init()
pthread_cond_destroy()
pthread_cond_wait()
pthread_cond_timedwait()
pthread_cond_signal()
pthraed_cond_broadcast()

## std::thread

std::thread
std::mutex
std::condition_variable
std::lock_guard
std::unique_lock

### std::lock_guard

```cpp
std::mutex mtx;
{
    std::lock_guard<std::mutex> lock(mtx)
    std::this_thread::sleep_for(std::chrono::seconds(1)); // simulate some work
}
```
equivalent to
```cpp
std::mutex mtx;
{
    mtx.lock();
    std::this_thread::sleep_for(std::chrono::seconds(1)); // simulate some work
    mtx.unlock();
}
```

### std::condition_variable::wait(lock, pred)

```cpp
std::condition_variable cv;
std::mutex mtx;
{
    std::unique_lock<std::mutex> lock(mtx);
    auto pred = [&]() { return /* condition */; };
    cv.wait(lock, pred);
}
```
equivalent to
```cpp
std::condition_variable cv;
std::mutex mtx;
{
    std::unique_lock<std::mutex> lock(mtx);
    auto pred = [&]() { return /* condition */; };
    while (!pred)
        cv.wait(lock);
}
```
