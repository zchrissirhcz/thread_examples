#include <stdio.h>
#include <stdbool.h>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

bool running = false;
SRWLOCK lock;

unsigned int __stdcall thread_func(void* arg)
{
    printf("hello from thread\n");
    int cnt = 0;

    while (true)
    {
        cnt++;
        AcquireSRWLockExclusive(&lock);
        if (!running)
        {
            ReleaseSRWLockExclusive(&lock);
            break;
        }
        printf("thread is running, cnt = %d\n", cnt);
        ReleaseSRWLockExclusive(&lock);
        Sleep(1000);
    }

    printf("bye from thread\n");
    return 0;
}

int main()
{
    printf("hello from main\n");

    running = true;
    InitializeSRWLock(&lock);
    HANDLE thread = (HANDLE)_beginthreadex(NULL, 0, thread_func, NULL, 0, NULL);

    Sleep(5000);
    running = false;
    WaitForSingleObject(thread, INFINITE);
    CloseHandle(thread);

    printf("bye from main\n");
}